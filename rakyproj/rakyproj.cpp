// rakyproj.cpp : This file contains the 'main' function. Program execution begins and ends there.
//#include <cstdlib>
//#include <fstream>
//#include <stdio.h>
//#include <string.h>
//#include <iostream>
//#include "MeshProcessingViewer.h"
//#include <pmp/BoundingBox.h>

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
#include <pmp/SurfaceMesh.h>
#include <pmp/algorithms/Remeshing.h>
#include "MeshProcessingViewer.h"
#include "pmp/BoundingBox.h"
#include <string>
#include <vector>
#include "pmp/visualization/Window.h"
#include "pmp/algorithms/HoleFilling.h"
#include "pmp/MatVec.h"
#include <Eigen/Dense>
#include <pmp/algorithms/Triangulation.h>
#include "pmp/algorithms/DifferentialGeometry.h"
#include "Common_Func.h"
#include "shapedescriptor.h"
using Eigen::MatrixXd;
using namespace pmp;
using namespace std;

void clearFile(string filePath)
{
    fstream fs;
    fs.open(filePath, ios::out); //这里写ios::trunc不能达到清空文件的效果
    fs.close();                  
}

Halfedge find_boundary(const SurfaceMesh& mesh)
{
    for (auto h : mesh.halfedges())
        if (mesh.is_boundary(h))
            return h;
    return Halfedge();
}

int main(int argc, char** argv)
{
    std::cout << "Readme: Please use the mesh in Labeled PSB folder only or the type classifying will not work properly\n";
    std::cout << "Readme: Notice 261 - 280 is not avaliable\n";
    std::cout << "Do you want to scan all? (y/n)  ";
    char modeinput;
    int cycle;
    bool scan= true, autoscan = true;
    fstream csvout;
    char filepath[11] = "LabeledDB/", filetype[5] = ".off",
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
        std::cout << "****************************************Raky Studio. "
                     "Inc***********************************************\n";

        // initialize
        char fileName[10];
        sprintf(fileName, "%s%d%s", filepath, cycle, filetype);
        puts(fileName);

        // file open successfully, read mesh
        SurfaceMesh mesh;
        /************************Step 2********************************/
        mesh.read(fileName);
        //the class of the shape
        string Type[20] = {"Human", "Cup",     "Glass",     "Airplane",
                           "Ant",   "Chair",   "Octopus",   "Table",
                           "Teddy", "Hand",    "Plier",     "Fish",
                           "Bird",  " ",       "Armadillo", "Bust",
                           "Mech",  "Bearing", "Vase",      "Fourleg"};
        char* tok;
        tok = strtok(strtok(fileName, "."), "/");
        tok = strtok(NULL, "/");
        int filenum = atoi(tok);
        std::cout << "\nContent of " << cycle << filetype << ":\n";
        csvout << cycle << filetype << ",";
        std::cout << "Mesh theme: This is a " << Type[(cycle - 1) / 20]
                  << std::endl;
        csvout << Type[(cycle - 1) / 20] << ",";

        //
        size_t num_v = mesh.n_vertices();
        size_t num_f = mesh.n_faces();
        // get pre-defined property storing vertex positions
        auto points = mesh.get_vertex_property<Point>("v:point");
        MatrixXd A_o(num_v, 3); //Get the matrix A_o
        int i = 0;
        for (auto v : mesh.vertices())
        {
            A_o(i, 0) = points[v][0]; //x_coordinate
            A_o(i, 1) = points[v][1]; //y_coordinate
            A_o(i, 2) = points[v][2]; //z_coordinate
            i++;
        }
        Point p = A_o.colwise().mean(); //! [barycenter]
        std::cout << "barycenter: " << A_o.colwise().mean() << std::endl;

        //translation
        //MatrixXd A_trans = A_o.rowwise() - A_o.colwise().mean();

        //Computing posscalar
        MatrixXd becentered = A_o.rowwise() - A_o.colwise().mean();
        MatrixXd becov = becentered.adjoint() * becentered;
        Eigen::EigenSolver<MatrixXd> bees(becov);

        MatrixXd bee_v = bees.pseudoEigenvectors();
        Eigen::Vector3d bee1 = bee_v.row(0);

        /*double beforeposscalar = abs(bee1[0]) / abs(Distance(bee1, vec3(0, 0, 0)));
        std::cout << "before posscalar: " << beforeposscalar << std::endl;
        csvout << beforeposscalar << "; ";*/

        //Computing PCA in C++
        MatrixXd A_trans = A_o.rowwise() - A_o.colwise().mean();
        MatrixXd centered = A_trans.rowwise() - A_trans.colwise().mean();
        MatrixXd cov = centered.adjoint() * centered;
        Eigen::EigenSolver<MatrixXd> es(cov);

        MatrixXd e_v = es.pseudoEigenvectors();
        Eigen::Vector3d e1 = e_v.col(1);
        Eigen::Vector3d e2 = e_v.col(0);
        Eigen::Vector3d e3 = e1.cross(e2);

        //Alignment
        i = 0;
        for (auto v : mesh.vertices())
        {
            Eigen::Vector3d p_update = A_trans.row(i);
            points[v][0] = p_update.dot(e1);
            points[v][1] = p_update.dot(e2);
            points[v][2] = p_update.dot(e3);
            i++;
        }
        //mesh.write("align.off");

        //flipping
        double f0 = 0, f1 = 0, f2 = 0;
        for (auto f : mesh.faces()) // loop over all faces
        {
            Point c(0, 0, 0);
            Scalar n(0);
            for (auto v : mesh.vertices(f))
            {
                c += mesh.position(v);
                ++n;
            }
            c /= n;
            //computes a value fi along each axis
            double c_x, c_y, c_z;
            c_x = c[0];
            c_y = c[1];
            c_z = c[2];
            f0 += sign(c_x) * c_x * c_x;
            f1 += sign(c_y) * c_y * c_y;
            f2 += sign(c_z) * c_z * c_z;
        }

        //Size
        // compute the axis-aligned bounding box sizes
        /* double scale = -1;
    for (auto v : mesh.vertices())
    {
        double d = Distance(points[v], vec3(0, 0, 0));
        if (d >= scale)
            scale = d;
    }
    cout << "scale: " << scale << endl;*/

        //Scale
        Eigen::Vector3d b_min = mesh.bounds().min();
        Eigen::Vector3d b_max = mesh.bounds().max();
        Eigen::Vector3d D = b_max - b_min;
        double scale = 1 / D.maxCoeff();

        for (auto v : mesh.vertices())
        {
            points[v][0] = points[v][0] * scale * sign(f0);
            points[v][1] = points[v][1] * scale * sign(f1);
            points[v][2] = points[v][2] * scale * sign(f2);
        }

        ////holefilling(water-proof)
        //// find boundary halfedge
        //Halfedge h = find_boundary(mesh);

        //// fill hole
        //HoleFilling hf(mesh);
        //hf.fill_hole(h);

        //compute afterposscalar
        MatrixXd A_af(num_v, 3); //Get the matrix A_o
        i = 0;
        for (auto v : mesh.vertices())
        {
            A_af(i, 0) = points[v][0]; //x_coordinate
            A_af(i, 1) = points[v][1]; //y_coordinate
            A_af(i, 2) = points[v][2]; //z_coordinate
            i++;
        }
        Point afp = A_af.colwise().mean(); //! [barycenter]
        std::cout << "barycenter: " << A_af.colwise().mean() << std::endl;

        //translation
        //MatrixXd A_trans = A_af.rowwise() - A_af.colwise().mean();

        //Computing posscalar
        MatrixXd afcentered = A_af.rowwise() - A_af.colwise().mean();
        MatrixXd afcov = afcentered.adjoint() * afcentered;
        Eigen::EigenSolver<MatrixXd> afes(afcov);

        //MatrixXd afe_v = afes.pseudoEigenvectors();
        //Eigen::Vector3d afe1 = afe_v.row(0);

        //std::cout << "after vertices: " << mesh.n_vertices() << std::endl;
        //csvout << mesh.n_vertices() << "; ";
        //std::cout << "after-aabb size:" << mesh.bounds().size() << std::endl;
        //csvout << mesh.bounds().size() << ", ";
        std::cout << "after-aabb center:" << mesh.bounds().center()
                  << std::endl;
        double afterd = Distance(mesh.bounds().center(), vec3(0, 0, 0));
        std::cout << "after-aabb center distance:" << afterd << std::endl;
        //csvout << afterd << "; \n";
        //double afterposscalar =
        //    abs(afe1[0]) / abs(Distance(afe1, vec3(0, 0, 0)));
        //std::cout << "after posscalar: " << afterposscalar << std::endl;
        //csvout << afterposscalar << "; ";

        //mesh.write("scale.off");
        /***********************Step 3.2*******************************/
        auto S = surface_area(mesh);
        cout << "surface area: " << S << endl;
        csvout << S << ",";

        auto V = volume(mesh);
        auto C = (S * S * S) / (36 * M_PI * V * V);
        cout << "compactness: " << C << endl;
        csvout << C << ",";

        auto R = V / mesh.bounds().size();
        cout << "3D rectangularity: " << R << endl;
        csvout << R << ",";

        double diameter = 0;
        for (auto v1 : mesh.vertices())
        {
            for (auto v2 : mesh.vertices())
            {
                double dist = Distance(points[v1], points[v2]);
                if (dist > diameter)
                {
                    diameter = dist;
                }
            }
        }
        cout << "diameter: " << diameter << endl;
        csvout << diameter << ",";

        Eigen::VectorXcd eigenval = afes.eigenvalues();
        std::vector<double> com_eigenval = {eigenval(0).real(), eigenval(1).real(), eigenval(2).real()};
        int bigid = std::distance( com_eigenval.begin(), std::max_element(com_eigenval.begin(), com_eigenval.end()));
        int smallid = std::distance(com_eigenval.begin(), std::min_element(com_eigenval.begin(), com_eigenval.end()));
        /*cout << "x: " << eigenval(0).real() << "y: " << eigenval(1).real()
             << "z: "
             <<
            eigenval(2).real() << endl;
        cout << "biggest eigenval: " << bigid << endl;
        cout << "smallest eigenval: " << smallid << endl;*/
        double eccentricity = eigenval(bigid).real() / eigenval(smallid).real();
        cout << "eccentricity: " << eccentricity << endl;
        csvout << eccentricity << ",";

        //Vector<Scalar, 1> ax = normalize(Vector<Scalar, 1>(eccentricity));
        //cout << "ax: "<< ax << endl;

        //other descriptor
        std::vector<Point> p_arr;
        for (auto v : mesh.vertices())
        {
            p_arr.push_back(points[v]);
        }
        
        int data_count =
            100; // 这里要是平方数不然bin算出来不对
        int bin = sqrt(data_count);

        std::vector<double> a3_descriptor;
        std::vector<double> d1_descriptor; 
        std::vector<double> d2_descriptor;
        std::vector<double> d3_descriptor;
        std::vector<double> d4_descriptor;
        for (int i = 0; i < data_count; i++)
        {
            //A3
            vector<int> index_A3 = G_random(3, num_v);
            a3_descriptor.push_back(A3_angle(p_arr, index_A3));
            // D1
            vector<int> index_D1 = G_random(1, num_v);
            d1_descriptor.push_back(
                d1_distance(p_arr, index_D1, p)); //p is baryceter
            //D2
            vector<int> index_D2 = G_random(2, num_v);
            d2_descriptor.push_back(d2_distance(p_arr, index_D2));
            //D3
            vector<int> index_D3 = G_random(3, num_v);
            d3_descriptor.push_back(d3_distance(p_arr, index_D3));
            //D4
            vector<int> index_D4 = G_random(4, num_v);
            d4_descriptor.push_back(d4_distance(p_arr, index_D4));
        }

        std::vector<double> a3_hist;
        a3_hist = hist(bin, a3_descriptor);
        std::vector<double> d1_hist;
        d1_hist = hist(bin, d1_descriptor);
        std::vector<double> d2_hist;
        d2_hist = hist(bin, d2_descriptor);
        std::vector<double> d3_hist;
        d3_hist = hist(bin, d3_descriptor);
        std::vector<double> d4_hist;
        d4_hist = hist(bin, d4_descriptor);

        cout << a3_hist << endl;
        cout << d1_hist << endl;
        cout << d2_hist << endl;
        cout << d3_hist << endl;
        cout << d4_hist << endl;
        csvout << a3_hist ;
        csvout << d1_hist ;
        csvout << d2_hist ;
        csvout << d3_hist ;
        csvout << d4_hist ;
        csvout << "\n";


        //cycle control
        cycle++;
        if (cycle == 261)
            cycle = 281;
        if (!autoscan || cycle == 401)
            scan = false;

        //// Activate the viewer
        //MeshProcessingViewer window("MeshProcessingViewer", 800, 600);
        //window.load_mesh(fileName);
        ////printf("\n\n\n");
        //return window.run();
    }
    csvout.close();
    return 0;
}
