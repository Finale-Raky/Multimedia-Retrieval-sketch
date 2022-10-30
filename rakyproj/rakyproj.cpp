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
using Eigen::MatrixXd;
using namespace pmp;
using namespace std;

void clearFile(string filePath)
{
    fstream fs;
    fs.open(filePath, ios::out); //这里写ios::trunc不能达到清空文件的效果
    fs.close();                  
}

//double Distance(const vec3& V1, const vec3& V2)
//{
//    double a = (V1[0] - V2[0]) * (V1[0] - V2[0]);
//    double b = (V1[1] - V2[1]) * (V1[1] - V2[1]);
//    double c = (V1[2] - V2[2]) * (V1[2] - V2[2]);
//    double distance = sqrt(a + b + c);
//    return distance;
//}

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
    std::cout << "Do you want to scan all? (y/n)  ";
    char modeinput;
    int cycle;
    bool scan= true, autoscan = true;
    fstream meshfile, csvout;
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
        std::cout << "original vertices: " << mesh.n_vertices() << std::endl;
        csvout << mesh.n_vertices() << "; ";

        ////std::cout << "edges: " << mesh.n_edges() << std::endl;
        //std::cout << "faces: " << mesh.n_faces() << std::endl;
        //csvout << mesh.n_faces() << "; ";

        ////the type of faces
        //if (mesh.is_triangle_mesh())
        //{
        //    std::cout << "triangles" << std::endl;
        //    csvout << "triangles; ";
        //}
        //    
        //else if (mesh.is_quad_mesh())
        //{
        //    std::cout << "quads" << std::endl;
        //    csvout << "quads; ";
        //}
        //else
        //{
        //    std::cout << "mixed" << std::endl;
        //    csvout << "mixed; ";
        //}

        //the axis-aligned 3D bounding box of the shapes
        std::cout << "before-aabb size:" << mesh.bounds().size() << std::endl;
        csvout << mesh.bounds().size() << "; ";
        //std::cout << "before-aabb center:" << mesh.bounds().center() << std::endl;
        double befored = Distance(mesh.bounds().center(), vec3(0, 0, 0));
        std::cout << "before-aabb center distance:" << befored << std::endl;
        csvout << befored << "; ";
        //csvout << mesh.bounds().min() << "; ";
        //std::cout << mesh.bounds().max() << std::endl;
        //csvout << mesh.bounds().max() << "\n" ;

        //// Activate the viewer
        //MeshProcessingViewer window("MeshProcessingViewer", 800, 600);
        //window.load_mesh(fileName);
        ////printf("\n\n\n");
        //return window.run();
        // 
        int num_v = mesh.n_vertices();
        int num_f = mesh.n_faces();
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

        double beforeposscalar = abs(bee1[0]) / abs(Distance(bee1, vec3(0, 0, 0)));
        std::cout << "before posscalar: " << beforeposscalar << std::endl;
        csvout << beforeposscalar << "; ";
        
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

        /*MatrixXd afe_v = afes.pseudoEigenvectors();
        Eigen::Vector3d afe1 = afe_v.row(0);

        std::cout << "after vertices: " << mesh.n_vertices() << std::endl;
        csvout << mesh.n_vertices() << "; ";
        std::cout << "after-aabb size:" << mesh.bounds().size() << std::endl;
        csvout << mesh.bounds().size() << "; ";
        std::cout << "after-aabb center:" << mesh.bounds().center() << std::endl;
        double afterd = Distance(mesh.bounds().center(), vec3(0, 0, 0));
        std::cout << "after-aabb center distance:" << afterd << std::endl;
        csvout << afterd << "; \n";
        double afterposscalar =
            abs(afe1[0]) / abs(Distance(afe1, vec3(0, 0, 0)));
        std::cout << "after posscalar: " << afterposscalar << std::endl;
        csvout << afterposscalar << "; ";*/

        //mesh.write("scale.off");
        /***********************Step 3.2*******************************/
        auto S = surface_area(mesh);
        cout << "surface area: " << S << endl;

        auto V = volume(mesh);
        auto C = (S * S * S) / (36 * M_PI * V * V);
        cout << "compactness: " << C << endl;

        auto R = V / mesh.bounds().size();
        cout << "3D rectangularity: " << R << endl;
        /*float diameter = 0;
        for (auto v1 : mesh.vertices())
        {
            for (auto v2 : mesh.vertices())
            {
                float dist = Distance(points[v1], points[v2]);
                if (dist > diameter)
                {
                    diameter = dist;
                }
            }
        }
        cout << "diameter: " << diameter << endl;*/

        Eigen::VectorXcd eigenval = afes.eigenvalues();
        std::vector<double> com_eigenval = {eigenval(0).real(), eigenval(1).real(),
                                     eigenval(2).real()};
        int bigid = std::distance(
            com_eigenval.begin(), std::max_element(com_eigenval.begin(), com_eigenval.end()));
        int smallid = std::distance(
            com_eigenval.begin(), std::min_element(com_eigenval.begin(), com_eigenval.end()));
        /*cout << "x: " << eigenval(0).real() << "y: " << eigenval(1).real()
             << "z: "
             <<
            eigenval(2).real() << endl;
        cout << "biggest eigenval: " << bigid << endl;
        cout << "smallest eigenval: " << smallid << endl;*/
        double eccentricity = eigenval(bigid).real() / eigenval(smallid).real();
        cout << "eccentricity: " << eccentricity << endl;

        //other descriptor
        std::vector<Point> p_arr;
        for (auto v : mesh.vertices())
        {
            p_arr.push_back(points[v]);
        }
        
        int bin_count = 8;
        int data_count = 100000;
        size_t n_vertices = mesh.n_vertices();

        std::vector<float> a3_hist(bin_count, 0);
        std::vector<float> d1_hist(bin_count, 0);
        std::vector<float> d2_hist(bin_count, 0);
        std::vector<float> d3_hist(bin_count, 0);
        std::vector<float> d4_hist(bin_count, 0);
        srand(time(0));
        for (int i = 0; i < data_count; i++)
        {
            int id_0 = rand() % n_vertices;
            int id_1 = rand() % n_vertices;
            while (id_1 == id_0)
            {
                id_1 = rand() % n_vertices;
            }
            int id_2 = rand() % n_vertices;
            while (id_2 == id_0 || id_2 == id_1)
            {
                id_2 = rand() % n_vertices;
            }
            Point e0 = p_arr[id_1] - p_arr[id_0];
            Point e1 = p_arr[id_2] - p_arr[id_0];
            float a3_cos = std::clamp(dot(e0, e1) / distance(e0, Point(0)) / distance(e1, Point(0)), -1.f, 1.f);
            float a3 = acos(a3_cos) / M_PI * 180;
            int bin_id = std::clamp(int(a3 / 180 * bin_count), 0, bin_count - 1);
            a3_hist[bin_id]++;

            int id_3 = rand() % n_vertices;
            float d1 = distance(p_arr[id_3], Point(0));
            float dist_max = sqrt(3) / 2.f;
            bin_id = int(d1 / dist_max * bin_count);
            d1_hist[bin_id]++;

            int id_4 = rand() % n_vertices;
            int id_5 = rand() % n_vertices;
            while (id_5 == id_4)
            {
                id_5 = rand() % n_vertices;
            }
            float d2 = distance(p_arr[id_4], p_arr[id_5]);
            dist_max = sqrt(3);
            bin_id = int(d2 / dist_max * bin_count);
            d2_hist[bin_id]++;

            int id_6 = rand() % n_vertices;
            int id_7 = rand() % n_vertices;
            while (id_7 == id_6)
            {
                id_7 = rand() % n_vertices;
            }
            int id_8 = rand() % n_vertices;
            while (id_8 == id_6 || id_8 == id_7)
            {
                id_8 = rand() % n_vertices;
            }
            float d3 = sqrt(0.5 * norm(cross(p_arr[id_7] - p_arr[id_6],
                                             p_arr[id_8] - p_arr[id_6])));
            float area_max = sqrt(sqrt(3) / 2.f);
            bin_id = int(d3 / area_max * bin_count);
            d3_hist[bin_id]++;

            int id_9 = rand() % n_vertices;
            int id_10 = rand() % n_vertices;
            while (id_10 == id_9)
            {
                id_10 = rand() % n_vertices;
            }
            int id_11 = rand() % n_vertices;
            while (id_11 == id_9 || id_11 == id_10)
            {
                id_11 = rand() % n_vertices;
            }
            int id_12 = rand() % n_vertices;
            while (id_12 == id_9 || id_11 == id_10 || id_12 == id_11)
            {
                id_12 = rand() % n_vertices;
            }
            float d4 = cbrt(abs(dot(p_arr[id_12] - p_arr[id_9],
                                    cross(p_arr[id_10] - p_arr[id_9],
                                          p_arr[id_11] - p_arr[id_9]))) / 6);
            float volume_max = sqrt(1.f / 3);
            bin_id = int(d4 / volume_max * bin_count);
            d4_hist[bin_id]++;
        }

        for (int i = 0; i < bin_count; i++)
        {
            cout << a3_hist[i] / data_count << ",";
        }
        for (int i = 0; i < bin_count; i++)
        {
            cout << d1_hist[i] / data_count << ",";
        }
        for (int i = 0; i < bin_count; i++)
        {
            cout << d2_hist[i] / data_count << ",";
        }
        for (int i = 0; i < bin_count; i++)
        {
            cout << d3_hist[i] / data_count << ",";
        }
        for (int i = 0; i < bin_count; i++)
        {
            cout << d4_hist[i] / data_count << ",";
        }
        cout << "\n";

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
