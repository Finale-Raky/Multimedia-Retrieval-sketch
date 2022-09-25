// rakyproj.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include<iostream>
#include<fstream>
#include<stdio.h>
#include<string.h>
#include "MeshProcessingViewer.h"
#include "pmp/BoundingBox.h"

using namespace pmp;
using namespace std;

int main()
{
    // read mesh from file 
    char fileName[10];
    fstream fp;
    cout << "****************************************Raky Studio. Inc***********************************************\n";
    cout << "Please enter the Name of File: ";
    cin >> fileName;
    fp.open(fileName, fstream::in);
    if (!fp)
    {
        cout << "\nError Occurred!";
        return 0;
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

    // create bounding box and get the size
    BoundingBox bb = mesh.bounds();
    auto size = mesh.bounds().size();

    // cout info
    cout << "\nContent of " << fileName << ":\n";
    // get the filename and find the tag
    int filenum = atoi(strtok(fileName, "."));
    cout << "Mesh theme: This is a ";
    if (filenum < 21) printf("Human.\n");
    else if (filenum < 41) printf("Cup.\n");
    else if (filenum < 61) printf("Glasses.\n");
    else if (filenum < 81)printf("Airplane.\n");
    else if (filenum < 101)printf("Ant.\n");
    else if (filenum < 121)printf("Chair.\n");
    else if (filenum < 141)printf("Octopus.\n");
    else if (filenum < 161)printf("Table.\n");
    else if (filenum < 181)printf("Teddy.\n");
    else if (filenum < 201)printf("Hand.\n");
    else if (filenum < 221) printf("Plier.\n");
    else if (filenum < 241)printf("Fish.\n");
    else if (filenum < 281)printf("Bird.\n");
    else if (filenum < 301)printf("Armadillo.\n");
    else if (filenum < 321) printf("Bust.\n");
    else if (filenum < 341)printf("Mech.\n");
    else if (filenum < 361)printf("Bearing.\n");
    else if (filenum < 381)printf("Vase.\n");
    else if (filenum < 401)printf("Fourleg.\n");

    // calculate the mean valence to determine the face type
    cout << "The face type: ";
    if (mean_valence == 6)
        std::cout << "only triangles." << std::endl;
    else if (mean_valence == 4)
        std::cout << "only quads." << std::endl;
    else if (mean_valence >5.9)
        std::cout << "Overwhelming triangles with only a few quads." << std::endl;
    else
        std::cout << "mixes of triangles and quads." << std::endl;

    // calculate the number of vertices and faces
    cout << "The number of vertices and faces:\n";
    std::cout << "vertices: " << mesh.n_vertices() << std::endl;
    std::cout << "faces: " << mesh.n_faces() << std::endl;

    // detect the bounding box and calculate the size of bounding box
    cout << "Bounding box: ";
    (bb.is_empty()) ? std::cout << "N/A" << std::endl: std::cout << "created successful, the size is ";
    if (!bb.is_empty())
        std::cout << size << "." << std::endl;

    // Activate the viewer
    MeshProcessingViewer window("MeshProcessingViewer", 800, 600);
    window.load_mesh("output.off");
    //printf("\n\n\n");
    return window.run();
}
