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
#include "MeshProcessingViewer.h"
#include "pmp/BoundingBox.h"

using namespace pmp;
using namespace std;

void clearFile(string filePath)
{
    fstream fs;
    fs.open(filePath, ios::out); //这里写ios::trunc不能达到清空文件的效果
    fs.close();                  
}

int main()
{
    std::cout << "Readme: Please use the mesh in Labeled PSB folder only or the type classifying will not work properly\n";
    std::cout << "Please enter the number of mesh you would like to check:";
    int cycle;
    bool autoscan;
    fstream meshfile, csvout;
    char filepath[11] = "LabeledDB/", filetype[5] = ".off",
         csvpath[15] = "csv/report.csv";

    // Create a new file to store updated data
    void clearFile(string(csvpath));
    
    csvout.open("csv/report.csv", ios::out); 

    // select mesh
    cin >> cycle;
    (cycle == 1) ? autoscan = true : autoscan = false;

    while (cycle <= 260 || cycle > 280)
    {
        
        std::cout << "****************************************Raky Studio. Inc***********************************************\n";
        
        //open files in folder one by one
        char fileName[10];
        sprintf(fileName, "%s%d%s", filepath, cycle, filetype);
        puts(fileName);

        meshfile.open(fileName, fstream::in);
        if (!meshfile)
        {
            std::cout << "\nError Occurred!";
            break;
        }

        //file open successfully
        SurfaceMesh mesh;
        mesh.read(fileName);
        mesh.write("output.off");

        //calculate the mean valence of the mesh
        float mean_valence = 0.0f;

        // loop over all vertices
        for (auto v : mesh.vertices())
        {
            // sum up vertex valences
            mean_valence += mesh.valence(v);
        }

        mean_valence /= mesh.n_vertices();

        // calculate the barycenter
        // get pre-defined property storing vertex positions
        auto points = mesh.get_vertex_property<Point>("v:point");

        Point p(0, 0, 0), bbcenter, pmin(0, 0, 0), pmax(1, 1, 1);

        for (auto v : mesh.vertices())
        {
            // access point property like an array
            p += points[v];
        }

        p /= mesh.n_vertices();

        std::cout << "barycenter: " << p << std::endl;

        // create bounding box and get the size
        BoundingBox bb(pmin, pmax);
        //= mesh.bounds();
        auto size = bb.size();
        //mesh.bounds().size();
        bbcenter = bb.center();
        //mesh.bounds().center();
        std::cout << "bbcenter: " << p << std::endl;
        
        // cout info
        
        // get the filename and find the tag
        char*tok;
        tok = strtok(strtok(fileName, "."), "/");
        tok = strtok(NULL, "/");
        int filenum = atoi(tok);
        std::cout << "\nContent of " << cycle << filetype << ":\n";
        csvout << cycle << filetype << ": ";
        std::cout << "Mesh theme: This is a "; 
        csvout << "Mesh theme: ";
        if (filenum < 21)
        {
            printf("Human.\n");
            csvout << "Human; ";
        }
        else if (filenum < 41)
        {   printf("Cup.\n");
            csvout << "Cup; ";
        }  
        else if (filenum < 61)
        {
            printf("Glasses.\n");
            csvout << "Glasses; ";
        }  
        else if (filenum < 81)
        {
            printf("Aimeshfilelane.\n");
            csvout << "Aimeshfilelane; ";
        }  
        else if (filenum < 101)
        {
            printf("Ant.\n");
            csvout << "Ant; ";
        }  
        else if (filenum < 121)
        {
            printf("Chair.\n");
            csvout << "Chair; ";
        }  
        else if (filenum < 141)
        {
            printf("Octopus.\n");
            csvout << "Octopus; ";
        }  
        else if (filenum < 161)
        {
            printf("Table.\n");
            csvout << "Table; ";
        }  
        else if (filenum < 181)
        {
            printf("Teddy.\n");
            csvout << "Teddy; ";
        }  
        else if (filenum < 201)
        {
            printf("Hand.\n");
            csvout << "Hand; ";
        }  
        else if (filenum < 221)
        {
            printf("Plier.\n");
            csvout << "Plier; ";
        }  
        else if (filenum < 241)
        {
            printf("Fish.\n");
            csvout << "Fish; ";
        }  
        else if (filenum < 281)
        {
            printf("Bird.\n");
            csvout << "Bird; ";
        }  
        else if (filenum < 301)
        {
            printf("Armadillo.\n");
            csvout << "Armadillo; ";
        }  
        else if (filenum < 321)
        {
            printf("Bust.\n");
            csvout << "Bust; ";
        }  
        else if (filenum < 341)
        {
            printf("Mech.\n");
            csvout << "Mech; ";
        }  
        else if (filenum < 361)
        {
            printf("Bearing.\n");
            csvout << "Bearing; ";
        }  
        else if (filenum < 381)
        {
            printf("Vase.\n");
            csvout << "Vase; ";
        }  
        else if (filenum < 401)
        {
            printf("Fourleg.\n");
            csvout << "Fourleg; ";
        }  

        // calculate the mean valence to determine the face type
        std::cout << "The face type: ";
        csvout << "The face type: ";
        if (mean_valence == 6)
        {
            std::cout << "only triangles." << std::endl;
            csvout << "only triangles; ";
        }
        else if (mean_valence == 4)
        {
            std::cout << "only quads." << std::endl;
            csvout << "only quads; ";
        }
        else if (mean_valence > 5.9)
        {
            std::cout << "Overwhelming triangles with only a few quads."
                      << std::endl;
            csvout << "Overwhelming triangles with only a few quads; ";
        }
        else
        {
            std::cout << "mixes of triangles and quads." << std::endl;
            csvout << "mixes of triangles and quads; ";
        }

        // calculate the number of vertices and faces
        std::cout << "The number of vertices and faces:\n";
        csvout << "The number of vertices and faces: ";
        std::cout << "vertices: " << mesh.n_vertices() << ";" << std::endl;
        csvout << "vertices: " << mesh.n_vertices() << "; ";
        std::cout << "faces: " << mesh.n_faces() << ";" << std::endl;
        csvout << "faces: " << mesh.n_faces() << "; ";

        // detect the bounding box and calculate the size of bounding box
        std::cout << "Bounding box: ";
        csvout << "Bounding box ";
        if (bb.is_empty())
        {
            std::cout << "N/A" << std::endl;
            csvout << "N/A, ";
        }
        else
        {
            std::cout << "created successful, the size is ";
            csvout << "size: ";
            std::cout << size << "." << std::endl;
            csvout << size << "\n ";
        }

        //// Activate the viewer
        //MeshProcessingViewer window("MeshProcessingViewer", 800, 600);
        //window.load_mesh("output.off");
        ////printf("\n\n\n");
        //return window.run();
        cycle++;
        if (cycle == 261)
            cycle = 281;
        if (!autoscan)
            break;
        meshfile.close();
    }
    csvout.close();
    return 0;
}
