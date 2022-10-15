// rakyproj.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <cstdlib>
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<string.h>
#include <pmp/algorithms/Remeshing.h>
#include "MeshProcessingViewer.h"
#include "pmp/BoundingBox.h"
#include "pmp/algorithms/HoleFilling.h"

using namespace pmp;
using namespace std;

void clearFile(string filePath)
{
    fstream fs;
    fs.open(filePath, ios::out); //这里写ios::trunc不能达到清空文件的效果
    fs.close();                  
}

double Distance(const vec3& V1, const vec3& V2)
{
    double a = (V1[0] - V2[0]) * (V1[0] - V2[0]);
    double b = (V1[1] - V2[1]) * (V1[1] - V2[1]);
    double c = (V1[2] - V2[2]) * (V1[2] - V2[2]);
    double distance = sqrt(a + b + c);
    return distance;
}

Halfedge find_boundary(const SurfaceMesh& mesh)
{
    for (auto h : mesh.halfedges())
        if (mesh.is_boundary(h))
            return h;
    return Halfedge();
}

int main()
{
    std::cout << "Readme: Please use the mesh in Labeled PSB folder only or the type classifying will not work properly\n";
    std::cout << "Do you want to scan all? (y/n)  ";
    char modeinput;
    int cycle;
    bool scan= true, autoscan = true;
    fstream meshfile, csvout;
    char fileRName, filepath[11] = "LabeledDB/", filetype[5] = ".off",
         csvpath[15] = "csv/report.csv";

    // Clear csv file and open
    void clearFile(string(csvpath));
    csvout.open("csv/report.csv", ios::out); 

    // tool mode select(single/all)
    cin >> modeinput;
    if (modeinput == 'y')
    {
        autoscan = true;
    }
    else
        autoscan = false;
    std::cout << "Where do you want to start? (give a number): ";
    cin >> cycle;

    while (scan)
    {
        // select scantype
        std::cout << "****************************************Raky Studio. Inc***********************************************\n";
        
        // initialize
        char fileName[10];
        sprintf(fileName, "%s%d%s", filepath, cycle, filetype);
        puts(fileName);
        meshfile.open(fileName, fstream::in);
        if (!meshfile)
        {
            std::cout << "\nError Occurred!";
            break;
        }

        // file open successfully, read mesh
        SurfaceMesh mesh;
        /************************Step 2********************************/
        mesh.read(fileName);
        //the class of the shape
        string Type[20] = {"Human", "Cup",     "Glass", "Airplane",  "Ant",
                           "Chair", "Octopus", "Table", "Teddy",     "Hand",
                           "Plier", "Fish",    "Bird",   " ","Armadillo", 
                           "Bust", "Mech",  "Bearing", "Vase",  "Fourleg"};
        char* tok;
        tok = strtok(strtok(fileName, "."), "/");
        tok = strtok(NULL, "/");
        int filenum = atoi(tok);
        std::cout << "\nContent of " << cycle << filetype << ":\n";
        csvout << cycle << filetype << ": ";
        std::cout << "Mesh theme: This is a " << Type[(cycle -1)/ 20]
                  << std::endl;
        csvout << Type[(cycle - 1)/ 20] << "; ";

        //the number of faces and vertices of the shape
        std::cout << "vertices: " << mesh.n_vertices() << std::endl;
        csvout << mesh.n_vertices() << "; ";
        //std::cout << "edges: " << mesh.n_edges() << std::endl;
        std::cout << "faces: " << mesh.n_faces() << std::endl;
        csvout << mesh.n_faces() << "; ";

        //the type of faces
        if (mesh.is_triangle_mesh())
        {
            std::cout << "triangles" << std::endl;
            csvout << "triangles; ";
        }
            
        else if (mesh.is_quad_mesh())
        {
            std::cout << "quads" << std::endl;
            csvout << "quads; ";
        }
        else
        {
            std::cout << "mixed" << std::endl;
            csvout << "mixed; ";
        }

        //the axis-aligned 3D bounding box of the shapes
        std::cout << mesh.bounds().size() << std::endl;
        std::cout << mesh.bounds().min() << std::endl;
        csvout << mesh.bounds().min() << "; ";
        std::cout << mesh.bounds().max() << std::endl;
        csvout << mesh.bounds().max() << "\n" ;

        //// Activate the viewer
        //MeshProcessingViewer window("MeshProcessingViewer", 800, 600);
        //window.load_mesh(fileName);
        ////printf("\n\n\n");
        //return window.run();
        //! [barycenter]
        // get pre-defined property storing vertex positions
        auto points = mesh.get_vertex_property<Point>("v:point");
        Point p(0, 0, 0);
        for (auto v : mesh.vertices())
        {
            // access point property like an array
            p += points[v];
        }
        p /= mesh.n_vertices();
        std::cout << "barycenter: " << p << std::endl;

        //translation
        for (auto v : mesh.vertices())
        {
            points[v] = points[v] - p;
        }

        //scale
        int signx = 1, signy = 1, signz = 1;
        double scale, maxdistance = 0;
        for (auto v : mesh.vertices())
        {
            double d = Distance(points[v], vec3(0, 0, 0));
            if (d >= maxdistance)
                maxdistance = d;
        }
        scale = maxdistance / 0.5;

        Point ps(0, 0, 0);
        for (auto v : mesh.vertices())
        {
            ps += points[v];
        }
        if (ps[0] < 0)
            signx = -1;
        if (ps[1] < 0)
            signy = -1;
        if (ps[2] < 0)
            signz = -1;
        std::cout << "scale: " << signx << std::endl;
        std::cout << "scale: " << signy << std::endl;
        std::cout << "scale: " << signz << std::endl;

        for (auto v : mesh.vertices())
        {
            points[v][0] = points[v][0] / scale * signx;
            points[v][1] = points[v][1] / scale * signy;
            points[v][2] = points[v][2] / scale * signz;
        }

        //holefilling(water-proof)
        // find boundary halfedge
        Halfedge h = find_boundary(mesh);

        // fill hole
        HoleFilling hf(mesh);
        hf.fill_hole(h);

        mesh.write("scale.off");


        //cycle control
        cycle++;
        if (cycle == 261)
            cycle = 281;
        if (!autoscan || cycle == 401)
            scan = false;
        meshfile.close();
    }
    csvout.close();
    return 0;
}
