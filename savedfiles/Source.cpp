// rakyproj.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <pmp/SurfaceMesh.h>
using namespace pmp;

int main(int argc, char** argv)
{
    // read mesh from file
    SurfaceMesh mesh;
    mesh.read("33.off");

    // add custom vertex property: new vertex position
    auto new_pos = mesh.add_vertex_property<Point>("new position");

    // perform 10 iterations
    for (int iter = 0; iter < 10; ++iter)
    {
        //iterate over all vertices
        for (auto v : mesh.vertices())
        {
            //compute barycenter of neighbors
            Point c(0, 0, 0);
            for (auto neighbor : mesh.vertices(v))
            {
                c += mesh.position(neighbor);
            }
            c /= mesh.valence(v);
            // assign to vertex property
            new_pos[v] = c;
        }

        //update vertex positions
        for (auto v : mesh.vertices())
        {
            mesh.position(v) = new_pos[v];
        }
    }

    //write mesh to file
    mesh.write(argv[2]);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started:
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
