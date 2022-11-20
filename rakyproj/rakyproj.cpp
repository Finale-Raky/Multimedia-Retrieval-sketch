// rakyproj.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <iomanip>
#include "emd1.h"
#include "nanoflann.hpp"
#include "utils.h"

using Eigen::MatrixXd;
using namespace pmp;
using namespace std;
using namespace nanoflann;

string* Type = new string[20]{
    "Human",     "Cup",   "Glass", "Airplane", "Ant",  "Chair",  "Octopus",
    "Table",     "Teddy", "Hand",  "Plier",    "Fish", "Bird",   " ",
    "Armadillo", "Bust",  "Mech",  "Bearing",  "Vase", "Fourleg"};
//cup =2, glass = 2, bearing = 1
int* Typecnt = new int[20]{
    20, 18, 18, 20, 20, 20, 20, 20, 20, 20,
    20, 20, 20, 0,  20, 20, 20, 19, 20, 20,
};

char examinput, filepath[11] = "LabeledDB/", filetype[5] = ".off",
                csvpath[15] = "csv/report.csv";

//KNN tool
template <typename num_t>
void kdtree_demo(const size_t N, vector<vector<string>> array)
{
    using std::cout;
    using std::endl;

    PointCloud<float> cloud;
    //generateRandomPointCloud(cloud, N);
    //PointCloud<T>& point;
    cloud.pts.resize(N);
    for (size_t i = 0; i < N; i++)
    {
        
        cloud.pts[i].x = stod(array[i][0]);
        cloud.pts[i].y = stod(array[i][1]);
        cloud.pts[i].z = 0;
    }

    // construct a kd-tree index:
    using my_kd_tree_t = nanoflann::KDTreeSingleIndexAdaptor<
        nanoflann::L2_Simple_Adaptor<num_t, PointCloud<num_t>>,
        PointCloud<num_t>, 3 /* dim */
        >;

    my_kd_tree_t index(3 /*dim*/, cloud, {10 /* max leaf */});
    index.buildIndex();

#if 0
    // Test resize of dataset and rebuild of index:
    cloud.pts.resize(cloud.pts.size() * 0.5);
    index.buildIndex();
#endif
    std::cout << "Notice: 261.off - 280.off is not available\n";
    std::cout << "please give a mesh No. (1-400): \n";
    int selectednot;
    cin >> selectednot;
    int selectedno = selectednot - 1;
    const num_t query_pt[3] = {stof(array[selectedno][0]), stof(array[selectedno][1]),
                               0}; //查找的位置
    char** target = new char*[10];
    for (int i = 0; i < 10; ++i)
        target[i] = new char[20];
    bool gui = true;

    std::sprintf(target[9], "%s%d%s", filepath, stoi(array[selectedno][2]),
                 filetype);
    MeshViewer viewer0("MeshViewer: selected mesh", 800, 600, gui);
    viewer0.load_mesh(target[9]);
    viewer0.run();
    viewer0.~MeshViewer();
    // ----------------------------------------------------------------
    // knnSearch():  Perform a search for the N closest points
    // ----------------------------------------------------------------
    {
        size_t num_results = 5; //Knn的k
        std::vector<uint32_t> ret_index(num_results);
        std::vector<num_t> out_dist_sqr(num_results);

        num_results = index.knnSearch(&query_pt[0], num_results, &ret_index[0],
                                      &out_dist_sqr[0]);

        // In case of less points in the tree than requested:
        ret_index.resize(num_results);
        out_dist_sqr.resize(num_results);

        cout << "knnSearch(): num_results=" << num_results << "\n";
        for (size_t i = 0; i < num_results; i++)
            cout << "idx[" << i << "]=" << array[ret_index[i]][2] << " dist[" << i
                 << "]=" << out_dist_sqr[i] << " "
                 << Type[(stoi(array[ret_index[i]][2]) - 1) / 20] << endl;
        cout << "\n";

        
        for (int i = 0; i < num_results; i++)
        {
            std::sprintf(target[i], "%s%d%s", filepath, stoi(array[ret_index[i]][2]),
                         filetype);
        }

        MeshViewer viewer("MeshViewer: 1st", 800, 600, gui);
        viewer.load_mesh(target[0]);
        viewer.run();
        viewer.~MeshViewer();

        MeshViewer viewer2("MeshViewer: 2nd", 800, 600, gui);
        viewer2.load_mesh(target[1]);
        viewer2.run();
        viewer2.~MeshViewer();

        MeshViewer viewer3("MeshViewer: 3rd", 800, 600, gui);
        viewer3.load_mesh(target[2]);
        viewer3.run();
        viewer3.~MeshViewer();

        MeshViewer viewer4("MeshViewer: 4th", 800, 600, gui);
        viewer4.load_mesh(target[3]);
        viewer4.run();
        viewer4.~MeshViewer();

        MeshViewer viewer5("MeshViewer: 5th", 800, 600, gui);
        viewer5.load_mesh(target[4]);
        viewer5.run();
        viewer5.~MeshViewer();
        exit(1);
    }
}

void clearFile(string filePath)
{
    fstream fs;
    fs.open(filePath, ios::out); //这里写ios::trunc不能达到清空文件的效果
    fs.close();                  
}

//for holefilling
Halfedge find_boundary(const SurfaceMesh& mesh)
{
    for (auto h : mesh.halfedges())
        if (mesh.is_boundary(h))
            return h;
    return Halfedge();
}

bool sortcol(const vector<double>& v1, const vector<double>& v2)
{
    return v1[1] < v2[1];
}

int main(int argc, char** argv)
{
    std::cout << "(bonus)Do you want to try KNN? (y/n)\n";
    char knninput;
    std::cin >> knninput;
    
    if (knninput == 'y')
    {
        ifstream tsnefile("csv/t-sne-nolabel.csv", ios::in);
        vector<vector<string>> tsnecontent;
        vector<string> tsnerow;
        string tsneline, tsneword;
        if (tsnefile.is_open())
        {
            while (getline(tsnefile, tsneline))
            {
                tsnerow.clear();
                stringstream tsnestr(tsneline);
                while (getline(tsnestr, tsneword, ','))
                {
                    tsnerow.push_back(tsneword);
                }
                tsnecontent.push_back(tsnerow);
            } 
        }
        else
            std::cout << "Could not open the file\n";
        //knn search
        kdtree_demo<float>(375, tsnecontent);
        exit(1);
    }
    std::cout << "Readme: Please use the mesh in Labeled PSB folder only or the type classifying will not work properly\n";
    std::cout << "Notice: 261.off - 280.off is not avaliable\n";
    std::cout << "Do you want to skip examine before querying? (y/n)\n";
    

    // Clear csv file and open
    fstream csvout;
    void clearFile(string(csvpath));
    csvout.open("csv/report.csv", ios::out);

    std::cin >> examinput;

    if (examinput == 'y')
    {
        int itera, iterend;
        char** target = new char*[10];
        for (int i = 0; i < 10; ++i)
            target[i] = new char[20];
        std::cout << "*************Querying started*************" << std::endl;
        std::cout << "where to start(min:1, max:400)?" << std::endl;
        cin >> itera;
        std::cout << "where to stop?(min:1, max:400)" << std::endl;
        std::cout << "If you selected 1 mesh, comparison window will be offered" << std::endl;
        cin >> iterend;
        bool awake_viewer = false;
        if (itera == iterend)
        {
            awake_viewer = true;
        }
        int numberofquery;
        ;
        std::cout << "Please enter K(number of query): " << std::endl;
        std::cin >> numberofquery;
        for (itera; itera <= iterend;
             itera++)
        {
            //added
            int meshinfo_size = 57;
            ifstream file("csv/reportdots3000NOLABEL.csv", ios::in);
            vector<vector<string>> content;
            vector<string> row;
            string line, word, selected_meshclass;
            vector<double> selected_mesh;
            vector<double> Eucstore;
            vector<vector<double>> Euclideandistancearray;
            vector<double> Cosstore;
            vector<vector<double>> Cosdistancearray;
            std::vector<double> av, bv, aw, bw, a3v, d1v, d2v, d3v, d4v, a3w,
                d1w, d2w, d3w, d4w, ba3v, bd1v, bd2v, bd3v, bd4v, ba3w, bd1w,
                bd2w, bd3w, bd4w;
            vector<double> EMDstore;
            vector<vector<double>> EMDdistancearray;
            double Euclideandistance;
            bool findno = false;
            selected_mesh.push_back(0);
            selected_mesh.push_back(0);

            //std::cout << "iter" << itera << "started" << std::endl;
            csvout << itera << ",";
            //int wordcnt = 0, rowcnt = 0;
            if (file.is_open())
            {

                while (getline(file, line))
                {
                    row.clear();
                    stringstream str(line);
                    while (getline(str, word, ','))
                    {
                        if (!findno && word == std::to_string(itera))
                        {
                            getline(str, word, ',');
                            selected_meshclass = word;
                            for (int i = 0; i < meshinfo_size; i++)
                            {
                                getline(str, word, ',');
                                selected_mesh.push_back(stod(word));
                            }
                            findno = true;
                        }
                        else
                        {
                            row.push_back(word);
                            //wordcnt++;
                        }
                    }
                    content.push_back(row);
                    //rowcnt++;
                    //cout << "iter " << rowcnt << " wordcnt is " << wordcnt << endl;
                }
            }
            else
                std::cout << "Could not open the file\n";
            
            std::cout << "You have selected " << itera << ".off, it is a " << selected_meshclass << std::endl;
            csvout << selected_meshclass << ",";

            double Cossum = 0;

            //added
            for (int i = 0; i < content.size(); i++)
            {
                /*std::cout << "Pointer is " << i << " ";*/
                if (content[i].size() == 0)
                    continue;
                double Eucfactor = 0, Eucfactor3 = 0;
                double EMDEucfactor = 0;
                double Coschild = 0, Cosparent = 1, Coschildm = 0,
                       Cosparentm = 1, Coschildda = 0, Cosparentdda = 1,
                       Coschilddb = 0, Cosparentddb = 1, Coschilddc = 0,
                       Cosparentddc = 1, Coschilddd = 0, Cosparentddd =1;
                double Cosfactor1 = 0, Cosfactor2 = 0, Cosfactor3 = 0, Cosfactorm1 = 0, Cosfactorm2 = 0, Cosfactorm3 = 0, Cosfactorda1 = 0, Cosfactorda2 = 0, Cosfactorda3 = 0;
                double Cosfactordb1 = 0, Cosfactordb2 = 0, Cosfactordb3 = 0,
                       Cosfactordc1 = 0, Cosfactordc2 = 0, Cosfactordc3 = 0,
                       Cosfactordd1 = 0, Cosfactordd2 = 0, Cosfactordd3 = 0;
                Eucstore.clear();
                Cosstore.clear();
                EMDstore.clear();
                a3v.clear();
                a3w.clear();
                ba3v.clear();
                ba3w.clear();
                d1v.clear();
                d1w.clear();
                bd1v.clear();
                bd1w.clear();
                d2v.clear();
                d2w.clear();
                bd2v.clear();
                bd2w.clear();
                d3v.clear();
                d3w.clear();
                bd3v.clear();
                bd3w.clear();
                d4v.clear();
                d4w.clear();
                bd4v.clear();
                bd4w.clear();
                Eucstore.push_back(stod(content[i][0]));
                Cosstore.push_back(stod(content[i][0]));
                EMDstore.push_back(stod(content[i][0]));
                for (int j = 2; j < meshinfo_size; j++)
                {
                    //added
                    //EuclideanDistance
                    if (j >= 7 && j < 17)
                    {
                        Eucfactor3 +=pow((selected_mesh[j] - stod(content[i][j])), 2);
                    }
                    else
                    {
                        Eucfactor += pow((selected_mesh[j] - stod(content[i][j])), 2);
                    }
                        
                    /*Eucfactor +=
                        pow((selected_mesh[j] - stod(content[i][j])), 2);*/
                    //CosDistance
                    if (j >= 7 && j < 17)
                    {
                        Cosfactor1 = selected_mesh[j] * stod(content[i][j]);
                        Cosfactor2 += pow(selected_mesh[j], 2);
                        Cosfactor3 += pow(stod(content[i][j]), 2);
                        Coschild += Cosfactor1;
                    }
                    else if (j >= 17 && j < 27)
                    {
                        Cosfactorda1 = selected_mesh[j] * stod(content[i][j]);
                        Cosfactorda2 += pow(selected_mesh[j], 2);
                        Cosfactorda3 += pow(stod(content[i][j]), 2);
                        Coschildda += Cosfactorda1;
                    }
                    else if (j >= 27 && j < 37)
                    {
                        Cosfactordb1 = selected_mesh[j] * stod(content[i][j]);
                        Cosfactordb2 += pow(selected_mesh[j], 2);
                        Cosfactordb3 += pow(stod(content[i][j]), 2);
                        Coschilddb += Cosfactordb1;
                    }
                    else if (j >= 37 && j < 47)
                    {
                        Cosfactordc1 = selected_mesh[j] * stod(content[i][j]);
                        Cosfactordc2 += pow(selected_mesh[j], 2);
                        Cosfactordc3 += pow(stod(content[i][j]), 2);
                        Coschilddc += Cosfactordc1;
                    }
                    else if (j >= 47 && j < 57)
                    {
                        Cosfactordd1 = selected_mesh[j] * stod(content[i][j]);
                        Cosfactordd2 += pow(selected_mesh[j], 2);
                        Cosfactordd3 += pow(stod(content[i][j]), 2);
                        Coschilddd += Cosfactordd1;
                    }
                    else
                    {
                        Cosfactorm1 = selected_mesh[j] * stod(content[i][j]);
                        Cosfactorm2 += pow(selected_mesh[j], 2);
                        Cosfactorm3 += pow(stod(content[i][j]), 2);
                        Coschildm += Cosfactorm1;
                    }
                    //if (Cosfactor2 != 0) Cosparent *= Cosfactor2;
                    //cout << content[i][j] << " ";
                    //EMD
                    if (j < 7)
                        EMDEucfactor += Eucfactor;
                    else if (j >= 7 && j < 17)
                    {
                        a3v.push_back(selected_mesh[j]);
                        ba3v.push_back(stod(content[i][j]));
                    }
                    else if (j >= 17 && j < 27)
                    {
                        d1v.push_back(selected_mesh[j]);
                        bd1v.push_back(stod(content[i][j]));
                    }
                    else if (j >= 27 && j < 37)
                    {
                        d2v.push_back(selected_mesh[j]);
                        bd2v.push_back(stod(content[i][j]));
                    }
                    else if (j >= 37 && j < 47)
                    {
                        d3v.push_back(selected_mesh[j]);
                        bd3v.push_back(stod(content[i][j]));
                    }
                    else
                    {
                        d4v.push_back(selected_mesh[j]);
                        bd4v.push_back(stod(content[i][j]));
                    }
                }
                //Euc with weight
                Euclideandistance = 1*sqrt(Eucfactor) + 0*sqrt(Eucfactor3);
                //std::cout << "the Euclidean distance to " << content[i][0] << ".off is" << Euclideandistance << std::endl;
                
                Eucstore.push_back(Euclideandistance);
                Euclideandistancearray.push_back(Eucstore);
                
                //Cos
                Cosparent = sqrt(Cosfactor2) * sqrt(Cosfactor3);
                Cosparentm = sqrt(Cosfactorm2) * sqrt(Cosfactorm3);
                Cosparentdda = sqrt(Cosfactorda2) * sqrt(Cosfactorda3);
                Cosparentddb = sqrt(Cosfactordb2) * sqrt(Cosfactordb3);
                Cosparentddc = sqrt(Cosfactordc2) * sqrt(Cosfactordc3);
                Cosparentddd = sqrt(Cosfactordd2) * sqrt(Cosfactordd3);
                float cos_wa3 = 0.02, cos_wd1 = 0.35, cos_wd2 = 0.15, cos_wd3 = 0.04, cos_wd4 = 0.04, cos_wg = 1 - cos_wa3 - cos_wd1 - cos_wd2 - cos_wd3 -cos_wd4;
                Cosstore.push_back(cos_wa3 * abs(1 - (Coschild / Cosparent)) +
                    cos_wg * abs(1 - (Coschildm / Cosparentm)) +
                    cos_wd1 * abs(1 - (Coschildda / Cosparentdda)) +
                    cos_wd2 * abs(1 - (Coschilddb / Cosparentddb)) +
                    cos_wd3 * abs(1 - (Coschilddc / Cosparentddc)) +
                    cos_wd4 * abs(1 - (Coschilddd / Cosparentddd)));
                //std::cout << "Cosparent: " << Cosparent<< " Cosresult : " << 1 - (Coschild / Cosparent)<< std::endl;
                Cosdistancearray.push_back(Cosstore);
                
                //EMD here
                for (int j = 0; j < 10; j++)
                {
                    a3w.push_back(1);
                    ba3w.push_back(1);
                    d1w.push_back(1);
                    bd1w.push_back(1);
                    d2w.push_back(3);
                    bd2w.push_back(3);
                    d3w.push_back(3);
                    bd3w.push_back(3);
                    d4w.push_back(2);
                    bd4w.push_back(2);
                }
                EMDstore.push_back(log10(sqrt(EMDEucfactor)) +
                                   log10(wasserstein(a3v, a3w, ba3v, ba3w)) +
                                   log10(wasserstein(d1v, d1w, bd1v, bd1w)) +
                                   log10(wasserstein(d2v, d2w, bd2v, bd2w)) +
                                   log10(wasserstein(d3v, d3w, bd3v, bd3w)) +
                                   log10(wasserstein(d4v, d4w, bd4v, bd4w)));
                EMDdistancearray.push_back(EMDstore);
            }

            //querying result
            
            //Euclideandistance querying
            
            std::sort(Euclideandistancearray.begin(), Euclideandistancearray.end(), sortcol);
            std::cout << "The most similar " << numberofquery 
                      << " shapes to this " << selected_meshclass
                      << " at Euclidean Distance are : " << std::endl;
            std::cout << "     Class         No.       Distance " << std::endl;
            //d = content.size(), c = Typecnt[?/20], s = numberofquery
            double TPcnt = 0 ,FPcnt = 0, FNcnt = 0, TNcnt = 0;
            for (int i = 0; i < numberofquery; i++)
            {
                std::cout << setw(10)
                     << Type[(int(Euclideandistancearray[i][0]) - 1) / 20]
                     << " ";
                //added
                //csvout << Type[(int(Euclideandistancearray[i][0]) - 1) / 20]<< ",";
                
                for (int j = 0; j < Euclideandistancearray[i].size(); j++)
                {
                    std::cout << setw(15) << Euclideandistancearray[i][j];
                    //added
                    //csvout << Euclideandistancearray[i][j] << ",";
                }
                //TP
                if (selected_meshclass ==
                    Type[(int(Euclideandistancearray[i][0]) - 1) / 20])
                {
                    std::cout << " TP";
                    TPcnt++;
                }
                std::cout << endl;
            }
            //d = content.size(), c = Typecnt[(int(distancearray[i][0]) - 1) / 20], s = numberofquery
            FPcnt = numberofquery - TPcnt;
            //FN = c - TP
            FNcnt = Typecnt[(itera - 1) / 20] - TPcnt;
            //TN = d - c - FP
            TNcnt = content.size() - Typecnt[(itera - 1) / 20] -
                    FPcnt;
            if ((TPcnt + FPcnt + TNcnt + FNcnt) != content.size())
                std::cout << "error" ;
            double EucPre = TPcnt / (TPcnt + FPcnt);
            double EucNPV = TNcnt / (TNcnt + FNcnt);
            double EucSen = TPcnt / (TPcnt + FNcnt);
            double EucSpe = TNcnt / (TNcnt + FPcnt);
            double EucAccuracy = (TPcnt + TNcnt) / content.size();
            /*std::cout << TPcnt << ", " << FPcnt << ", " << FNcnt << ", " << TNcnt << ", "<< EucAccuracy<< std::endl;
            std::cout << EucPre << ", " << EucNPV << std::endl;
            std::cout << EucSen << ", " << EucSpe << std::endl;*/
            csvout << EucAccuracy << "," << EucPre << "," << EucNPV << ","
                   << EucSen << "," << EucSpe << ",";
                   
            //Cosine distance querying
            std::sort(Cosdistancearray.begin(), Cosdistancearray.end(),
                      sortcol);
            std::cout << "The most similar " << numberofquery
                      << " shapes to this " << selected_meshclass
                      << " at Cosine Distance are : " << std::endl;
            std::cout << "     Class              No.      Distance "
                      << std::endl;
            TPcnt = 0, FPcnt = 0, FNcnt = 0, TNcnt = 0;
            //for (int i = Cosdistancearray.size() - 1;i > Cosdistancearray.size() - numberofquery - 1; i--)
            for (int i = 0;i < numberofquery; i++)
            {
                std::cout << setw(10)
                          << Type[(int(Cosdistancearray[i][0]) - 1) / 20]
                     << " ";
                //added
                //csvout << Type[(int(Cosdistancearray[i][0]) - 1) / 20] << ",";
                
                for (int j = 0; j < Cosdistancearray[i].size(); j++)
                {
                    std::cout << setw(15) << Cosdistancearray[i][j];
                    //added
                    //csvout << Cosdistancearray[i][j] << ",";
                }
                //TP
                if (selected_meshclass == Type[(int(Cosdistancearray[i][0]) - 1) / 20])
                {
                    std::cout << " TP";
                    TPcnt++;
                }
                std::cout << endl;
            }
            //d = content.size(), c = Typecnt[(int(distancearray[i][0]) - 1) / 20], s = numberofquery
            FPcnt = numberofquery - TPcnt;
            //FN = c - TP
            FNcnt = Typecnt[(itera - 1) / 20] - TPcnt;
            //TN = d - c - FP
            TNcnt = content.size() - Typecnt[(itera - 1) / 20] - FPcnt;
            if (TPcnt + FPcnt + TNcnt + FNcnt != content.size())
                std::cout << "error";
            double CosPre = TPcnt / (TPcnt + FPcnt);
            double CosNPV = TNcnt / (TNcnt + FNcnt);
            double CosSen = TPcnt / (TPcnt + FNcnt);
            double CosSpe = TNcnt / (TNcnt + FPcnt);
            double CosAccuracy = (TPcnt + TNcnt) / content.size();
            /*std::cout << TPcnt << ", " << CosAccuracy << std::endl;
            std::cout << CosPre << ", " << CosNPV << std::endl;
            std::cout << CosSen << ", " << CosSpe << std::endl;*/
            csvout << CosAccuracy << "," << CosPre << "," << CosNPV << ","
                   << CosSen << "," << CosSpe << ",";

            //Euclidean + EMD distance querying
            std::sort(EMDdistancearray.begin(),
                      EMDdistancearray.end(), sortcol);
            std::cout << "The most similar " << numberofquery
                      << " shapes to this " << selected_meshclass
                      << " at EMD Distance are : " << std::endl;
            std::cout << "     Class         No. Distance " << std::endl;
            TPcnt = 0, FPcnt = 0, FNcnt = 0, TNcnt = 0;
            for (int i = 0; i < numberofquery; i++)
            {
                std::cout << setw(10)
                          << Type[(int(EMDdistancearray[i][0]) - 1) / 20]
                          << " ";
                //added
                //csvout << Type[(int(EMDdistancearray[i][0]) - 1) / 20] << ",";
                
                for (int j = 0; j < EMDdistancearray[i].size(); j++)
                {
                    std::cout << setw(10) << EMDdistancearray[i][j];
                    //added
                    //csvout << EMDdistancearray[i][j] << ",";
                }
                //TP
                if (selected_meshclass ==
                    Type[(int(EMDdistancearray[i][0]) - 1) / 20])
                {
                    std::cout << " TP";
                    TPcnt++;
                }
                std::cout << endl;
            }
            //d = content.size(), c = Typecnt[(int(distancearray[i][0]) - 1) / 20], s = numberofquery
            FPcnt = numberofquery - TPcnt;
            //FN = c - TP
            FNcnt = Typecnt[(itera - 1) / 20] - TPcnt;
            //TN = d - c - FP
            TNcnt = content.size() - Typecnt[(itera - 1) / 20] - FPcnt;
            if (TPcnt + FPcnt + TNcnt + FNcnt != content.size())
                std::cout << "error";
            double EMDPre = TPcnt / (TPcnt + FPcnt);
            double EMDNPV = TNcnt / (TNcnt + FNcnt);
            double EMDSen = TPcnt / (TPcnt + FNcnt);
            double EMDSpe = TNcnt / (TNcnt + FPcnt);
            double EMDAccuracy = (TPcnt + TNcnt) / content.size();
            /*std::cout << TPcnt << ", " << EMDAccuracy << std::endl;
            std::cout << EMDPre << ", " << EMDNPV << std::endl;
            std::cout << EMDSen << ", " << EMDSpe << std::endl;*/
            csvout << EMDAccuracy << "," << EMDPre << "," << EMDNPV << ","
                   << EMDSen << "," << EMDSpe << ",";

            //when the number of mesh equals to 1, select preferred distance
            if (awake_viewer)
            {
                std::cout << "Which distance do you prefer? (e/c): "
                          << std::endl;
                std::cout << "e for Euclidean; c for Cosine, m for EMD"
                          << std::endl;
                std::cin >> examinput;
                if (examinput == 'e')
                {
                    for (int i = 0; i < numberofquery; i++)
                    {
                        std::sprintf(target[i], "%s%d%s", filepath, int(Euclideandistancearray[i][0]), filetype);
                    }
                }
                else if (examinput == 'c')
                {
                    for (int i = 0; i < numberofquery; i++)
                    {
                        std::sprintf(target[i], "%s%d%s", filepath, int(Cosdistancearray[i][0]), filetype);
                        //std::cout << "target is " << target[meshtotal - i] << std::endl;
                    }
                }
                else if (examinput == 'm')
                {
                    //EMD here if avaliable
                    for (int i = 0; i < numberofquery; i++)
                    {
                        std::sprintf(target[i], "%s%d%s", filepath,
                                     int(EMDdistancearray[i][0]),
                                     filetype);
                    }     
                }
                else
                {
                    std::cout << "illegal input!" << std::endl;
                    return 0;
                }       
            }  

            //added
            content.clear();
            row.clear();
            selected_mesh.clear();
            Eucstore.clear();
            Cosstore.clear();
            EMDstore.clear();
            Euclideandistancearray.clear();
            Cosdistancearray.clear();
            EMDdistancearray.clear();
            csvout << std::endl;
            file.close();
            //std::cout << "iter" << itera << "ended" << std::endl;
            //csvout << "\n";
            if (itera == 24 || itera == 38 || itera == 57 || itera == 46 ||
                itera == 356)
                itera++;
            else if (itera == 260)
                itera = 280;
        }
        if (awake_viewer)
        {
            bool gui = true;
            MeshViewer viewer("MeshViewer: 1st", 800, 600, gui);
            viewer.load_mesh(target[0]);
            viewer.run();
            viewer.~MeshViewer();

            MeshViewer viewer2("MeshViewer: 2nd", 800, 600, gui);
            viewer2.load_mesh(target[1]);
            viewer2.run();
            viewer2.~MeshViewer();

            MeshViewer viewer3("MeshViewer: 3rd", 800, 600, gui);
            viewer3.load_mesh(target[2]);
            viewer3.run();
            viewer3.~MeshViewer();
            exit(1);
        }
        cin.get();
        return 0;
    }
    else
    {
        std::cout << "Enter y to scan all meshes for basic info / n for querying one mesh? (y/n)  ";
        char modeinput;
        int cycle;
        bool scan = true, autoscan = true;

        // tool mode select(single/all)
        std::cin >> modeinput;
        if (modeinput == 'y')
        {
            autoscan = true;
        }
        else
        {
            autoscan = false;
        }
        char filerealn[10];
        std::cout << "Please enter the mesh No.(min:1,max:400): ";
        std::cin >> cycle;
        int itera;
        while (scan)
        {
            // select scantype
            std::cout << "****************************************Raky Studio. "
                         "Inc***********************************************\n";

            // initialize
            char fileName[10];
            std::sprintf(fileName, "%s%d%s", filepath, cycle, filetype);
            std::sprintf(filerealn, "%s%d%s", filepath, cycle, filetype);
            std::puts(fileName);
            SurfaceMesh mesh;
            mesh.read(fileName);

            /************************Step 2********************************/
            //the class of the shape
            
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

            //holefilling(water-proof)
            //find boundary halfedge
            //Halfedge h = find_boundary(mesh);
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

            /***********************Step 3.2*******************************/
            auto S = surface_area(mesh);
            std::cout << "surface area: " << S << endl;
            csvout << S << ",";

            auto V = volume(mesh);
            auto C = (S * S * S) / (36 * M_PI * V * V);
            std::cout << "compactness: " << C << endl;
            csvout << C << ",";

            auto R = V / mesh.bounds().size();
            std::cout << "3D rectangularity: " << R << endl;
            csvout << R << ",";

            /*double diameter = 0;
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
            csvout << diameter << ",";*/

            Eigen::VectorXcd eigenval = afes.eigenvalues();
            std::vector<double> com_eigenval = {
                eigenval(0).real(), eigenval(1).real(), eigenval(2).real()};
            int bigid = std::distance(
                com_eigenval.begin(),
                std::max_element(com_eigenval.begin(), com_eigenval.end()));
            int smallid = std::distance(
                com_eigenval.begin(),
                std::min_element(com_eigenval.begin(), com_eigenval.end()));
            /*cout << "x: " << eigenval(0).real() << "y: " << eigenval(1).real()<< "z: "<< eigenval(2).real() << endl;
            cout << "biggest eigenval: " << bigid << endl;
            cout << "smallest eigenval: " << smallid << endl;*/
            double eccentricity =
                eigenval(bigid).real() / eigenval(smallid).real();
            std::cout << "eccentricity: " << eccentricity << endl;
            csvout << eccentricity << ",";

            //Vector<Scalar, 1> ax = normalize(Vector<Scalar, 1>(eccentricity));
            //cout << "ax: "<< ax << endl;

            //other descriptor
            std::vector<Point> p_arr;
            for (auto v : mesh.vertices())
            {
                p_arr.push_back(points[v]);
            }

            int data_count = 3000; // 这里要是平方数不然bin算出来不对
            int bin = 10;
                //sqrt(data_count);

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
                d1_descriptor.push_back(d1_distance(p_arr, index_D1, p)); //p is baryceter
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
            //knn-test
            // Randomize Seed
            //srand(static_cast<unsigned int>(time(nullptr)));
            //kdtree_demo<float>(20);

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

            std::cout << a3_hist << endl;
            std::cout << d1_hist << endl;
            std::cout << d2_hist << endl;
            std::cout << d3_hist << endl;
            std::cout << d4_hist << endl;
            csvout << a3_hist;
            csvout << d1_hist;
            csvout << d2_hist;
            csvout << d3_hist;
            csvout << d4_hist;
            csvout << "\n";

            //cycle control
            itera = cycle;
            cycle++;
            if (cycle == 261)
                cycle = 281;
            if (!autoscan || cycle == 401)
                scan = false;
        }
        if (!autoscan)
        {
            // Activate the viewer, display input mesh
            bool gui = true;
            MeshViewer viewer3("MeshViewer", 800, 600, gui);
            viewer3.load_mesh(filerealn);
            viewer3.run();
            viewer3.~MeshViewer();
            csvout.close();
            int iterend = itera;
            char** target = new char*[10];
            for (int i = 0; i < 10; ++i)
                target[i] = new char[20];
            bool awake_viewer = true;
            int numberofquery;
            std::cout << "Please enter K (How many meshes do you want to check for each query?): " << std::endl;
            std::cin >> numberofquery;
            for (itera; itera <= iterend; itera++)
            {
                //added
                int meshinfo_size = 57;
                ifstream file("csv/report1115dots1000.csv", ios::in);
                vector<vector<string>> content;
                vector<string> row;
                vector<double> selected_mesh;
                vector<double> Eucstore;
                vector<vector<double>> Euclideandistancearray;
                vector<double> Cosstore;
                vector<vector<double>> Cosdistancearray;
                vector<double> av, bv, aw, bw, a3v, d1v, d2v, d3v, d4v,
                    a3w, d1w, d2w, d3w, d4w, ba3v, bd1v, bd2v, bd3v, bd4v, ba3w,
                    bd1w, bd2w, bd3w, bd4w;
                vector<double> EMDstore;
                vector<vector<double>> EMDdistancearray;
                string line, word, selected_meshclass;

                double Euclideandistance;
                bool findno = false;
                selected_mesh.push_back(0);
                selected_mesh.push_back(0);

                //std::cout << "iter" << itera << "started" << std::endl;
                csvout << itera << ",";
                //int wordcnt = 0, rowcnt = 0;
                if (file.is_open())
                {
                    while (getline(file, line))
                    {
                        row.clear();
                        stringstream str(line);
                        while (getline(str, word, ','))
                        {
                            if (!findno && word == std::to_string(itera))
                            {
                                getline(str, word, ',');
                                selected_meshclass = word;
                                for (int i = 0; i < meshinfo_size; i++)
                                {
                                    getline(str, word, ',');
                                    selected_mesh.push_back(stod(word));
                                }
                                findno = true;
                            }
                            else
                            {
                                row.push_back(word);
                                //wordcnt++;
                            }
                        }
                        content.push_back(row);
                        //rowcnt++;
                        //cout << "iter " << rowcnt << " wordcnt is " << wordcnt << endl;
                    }
                }
                else
                    std::cout << "Could not open the file\n";

                std::cout << "You have selected " << itera << ".off, it is a "
                          << selected_meshclass << std::endl;
                csvout << selected_meshclass << ",";

                    /*for (int i = 0; i < content.size(); i++)
                    cout << "content size, content[i] size: " << i
                         << ":" << content[i].size() << endl;*/
                    /*for (int i = 0; i < content.size(); i++)
                {
                    if (content[pointer].size() == 0)
                        continue;
                    for (int j = 0; j < content[i].size(); j++)
                        std::cout << content[i][j] << ",";
                }*/

                //std::cout << "Mesh info: " << selected_mesh << std::endl;
                double Cossum = 0;

                //added
                for (int i = 0; i < content.size(); i++)
                {
                    /*std::cout << "Pointer is " << i << " ";*/
                    if (content[i].size() == 0)
                        continue;
                    double Eucfactor = 0, Eucfactor3 = 0;
                    double EMDEucfactor = 0;
                    double Coschild = 0, Cosparent = 1, Coschildm = 0,
                           Cosparentm = 1, Coschildda = 0, Cosparentdda = 1,
                           Coschilddb = 0, Cosparentddb = 1, Coschilddc = 0,
                           Cosparentddc = 1, Coschilddd = 0, Cosparentddd = 1;
                    double Cosfactor1 = 0, Cosfactor2 = 0, Cosfactor3 = 0,
                           Cosfactorm1 = 0, Cosfactorm2 = 0, Cosfactorm3 = 0,
                           Cosfactorda1 = 0, Cosfactorda2 = 0, Cosfactorda3 = 0;
                    double Cosfactordb1 = 0, Cosfactordb2 = 0, Cosfactordb3 = 0,
                           Cosfactordc1 = 0, Cosfactordc2 = 0, Cosfactordc3 = 0,
                           Cosfactordd1 = 0, Cosfactordd2 = 0, Cosfactordd3 = 0;
                    Eucstore.clear();
                    Cosstore.clear();
                    EMDstore.clear();
                    a3v.clear();
                    a3w.clear();
                    ba3v.clear();
                    ba3w.clear();
                    d1v.clear();
                    d1w.clear();
                    bd1v.clear();
                    bd1w.clear();
                    d2v.clear();
                    d2w.clear();
                    bd2v.clear();
                    bd2w.clear();
                    d3v.clear();
                    d3w.clear();
                    bd3v.clear();
                    bd3w.clear();
                    d4v.clear();
                    d4w.clear();
                    bd4v.clear();
                    bd4w.clear();
                    Eucstore.push_back(stod(content[i][0]));
                    Cosstore.push_back(stod(content[i][0]));
                    EMDstore.push_back(stod(content[i][0]));
                    for (int j = 2; j < meshinfo_size; j++)
                    {
                        //added
                        //EuclideanDistance
                        if (j >= 7 && j < 17)
                        {
                            Eucfactor3 += pow(
                                (selected_mesh[j] - stod(content[i][j])), 2);
                        }
                        else
                        {
                            Eucfactor += pow(
                                (selected_mesh[j] - stod(content[i][j])), 2);
                        }

                        /*Eucfactor +=
                        pow((selected_mesh[j] - stod(content[i][j])), 2);*/
                        //CosDistance
                        if (j >= 7 && j < 17)
                        {
                            Cosfactor1 = selected_mesh[j] * stod(content[i][j]);
                            Cosfactor2 += pow(selected_mesh[j], 2);
                            Cosfactor3 += pow(stod(content[i][j]), 2);
                            Coschild += Cosfactor1;
                        }
                        else if (j >= 17 && j < 27)
                        {
                            Cosfactorda1 =
                                selected_mesh[j] * stod(content[i][j]);
                            Cosfactorda2 += pow(selected_mesh[j], 2);
                            Cosfactorda3 += pow(stod(content[i][j]), 2);
                            Coschildda += Cosfactorda1;
                        }
                        else if (j >= 27 && j < 37)
                        {
                            Cosfactordb1 =
                                selected_mesh[j] * stod(content[i][j]);
                            Cosfactordb2 += pow(selected_mesh[j], 2);
                            Cosfactordb3 += pow(stod(content[i][j]), 2);
                            Coschilddb += Cosfactordb1;
                        }
                        else if (j >= 37 && j < 47)
                        {
                            Cosfactordc1 =
                                selected_mesh[j] * stod(content[i][j]);
                            Cosfactordc2 += pow(selected_mesh[j], 2);
                            Cosfactordc3 += pow(stod(content[i][j]), 2);
                            Coschilddc += Cosfactordc1;
                        }
                        else if (j >= 47 && j < 57)
                        {
                            Cosfactordd1 =
                                selected_mesh[j] * stod(content[i][j]);
                            Cosfactordd2 += pow(selected_mesh[j], 2);
                            Cosfactordd3 += pow(stod(content[i][j]), 2);
                            Coschilddd += Cosfactordd1;
                        }
                        else
                        {
                            Cosfactorm1 =
                                selected_mesh[j] * stod(content[i][j]);
                            Cosfactorm2 += pow(selected_mesh[j], 2);
                            Cosfactorm3 += pow(stod(content[i][j]), 2);
                            Coschildm += Cosfactorm1;
                        }
                        //if (Cosfactor2 != 0) Cosparent *= Cosfactor2;
                        //cout << content[i][j] << " ";
                        //EMD
                        if (j < 7)
                            EMDEucfactor += Eucfactor;
                        else if (j >= 7 && j < 17)
                        {
                            a3v.push_back(selected_mesh[j]);
                            ba3v.push_back(stod(content[i][j]));
                        }
                        else if (j >= 17 && j < 27)
                        {
                            d1v.push_back(selected_mesh[j]);
                            bd1v.push_back(stod(content[i][j]));
                        }
                        else if (j >= 27 && j < 37)
                        {
                            d2v.push_back(selected_mesh[j]);
                            bd2v.push_back(stod(content[i][j]));
                        }
                        else if (j >= 37 && j < 47)
                        {
                            d3v.push_back(selected_mesh[j]);
                            bd3v.push_back(stod(content[i][j]));
                        }
                        else
                        {
                            d4v.push_back(selected_mesh[j]);
                            bd4v.push_back(stod(content[i][j]));
                        }
                    }
                    //Euc with weight
                    Euclideandistance =
                        1 * sqrt(Eucfactor) + 0 * sqrt(Eucfactor3);
                    //std::cout << "the Euclidean distance to " << content[i][0] << ".off is" << Euclideandistance << std::endl;

                    Eucstore.push_back(Euclideandistance);
                    Euclideandistancearray.push_back(Eucstore);

                    //Cos
                    Cosparent = sqrt(Cosfactor2) * sqrt(Cosfactor3);
                    Cosparentm = sqrt(Cosfactorm2) * sqrt(Cosfactorm3);
                    Cosparentdda = sqrt(Cosfactorda2) * sqrt(Cosfactorda3);
                    Cosparentddb = sqrt(Cosfactordb2) * sqrt(Cosfactordb3);
                    Cosparentddc = sqrt(Cosfactordc2) * sqrt(Cosfactordc3);
                    Cosparentddd = sqrt(Cosfactordd2) * sqrt(Cosfactordd3);
                    float cos_wa3 = 0.02, cos_wd1 = 0.35, cos_wd2 = 0.15,
                          cos_wd3 = 0.04, cos_wd4 = 0.04,
                          cos_wg = 1 - cos_wa3 - cos_wd1 - cos_wd2 - cos_wd3 -
                                   cos_wd4;
                    Cosstore.push_back(
                        cos_wa3 * abs(1 - (Coschild / Cosparent)) +
                        cos_wg * abs(1 - (Coschildm / Cosparentm)) +
                        cos_wd1 * abs(1 - (Coschildda / Cosparentdda)) +
                        cos_wd2 * abs(1 - (Coschilddb / Cosparentddb)) +
                        cos_wd3 * abs(1 - (Coschilddc / Cosparentddc)) +
                        cos_wd4 * abs(1 - (Coschilddd / Cosparentddd)));
                    //std::cout << "Cosparent: " << Cosparent<< " Cosresult : " << 1 - (Coschild / Cosparent)<< std::endl;
                    Cosdistancearray.push_back(Cosstore);
                
                    //EMD here
                    for (int j = 0; j < 10; j++)
                    {
                        a3w.push_back(1);
                        ba3w.push_back(1);
                        d1w.push_back(1);
                        bd1w.push_back(1);
                        d2w.push_back(3);
                        bd2w.push_back(3);
                        d3w.push_back(3);
                        bd3w.push_back(3);
                        d4w.push_back(2);
                        bd4w.push_back(2);
                    }

                    //std::cout << "calculating EMD" << std::endl;
                    //std::cout << "calculating EMD" << std::endl;
                    EMDstore.push_back(
                        log10(sqrt(EMDEucfactor)) +
                        log10(wasserstein(a3v, a3w, ba3v, ba3w)) +
                        log10(wasserstein(d1v, d1w, bd1v, bd1w)) +
                        log10(wasserstein(d2v, d2w, bd2v, bd2w)) +
                        log10(wasserstein(d3v, d3w, bd3v, bd3w)) +
                        log10(wasserstein(d4v, d4w, bd4v, bd4w)));
                    EMDdistancearray.push_back(EMDstore);
                }

                //querying result

                //Euclideandistance querying

                std::sort(Euclideandistancearray.begin(),
                          Euclideandistancearray.end(), sortcol);
                std::cout << "The most similar " << numberofquery
                          << " shapes to this " << selected_meshclass
                          << " at Euclidean Distance are : " << std::endl;
                std::cout << "     Class         No.       Distance " << std::endl;
                //d = content.size(), c = Typecnt[?/20], s = numberofquery
                double TPcnt = 0, FPcnt = 0, FNcnt = 0, TNcnt = 0;
                for (int i = 0; i < numberofquery; i++)
                {
                    std::cout
                        << setw(10)
                        << Type[(int(Euclideandistancearray[i][0]) - 1) / 20]
                        << " ";
                    //added
                    //csvout << Type[(int(Euclideandistancearray[i][0]) - 1) / 20]<< ",";

                    for (int j = 0; j < Euclideandistancearray[i].size(); j++)
                    {
                        std::cout << setw(15) << Euclideandistancearray[i][j];
                        //added
                        //csvout << Euclideandistancearray[i][j] << ",";
                    }
                    //TP
                    if (selected_meshclass ==
                        Type[(int(Euclideandistancearray[i][0]) - 1) / 20])
                    {
                        std::cout << " TP";
                        TPcnt++;
                    }
                    std::cout << endl;
                }
                //d = content.size(), c = Typecnt[(int(distancearray[i][0]) - 1) / 20], s = numberofquery
                FPcnt = numberofquery - TPcnt;
                //FN = c - TP
                FNcnt = Typecnt[(itera - 1) / 20] - TPcnt;
                //TN = d - c - FP
                TNcnt = content.size() - Typecnt[(itera - 1) / 20] - FPcnt;
                if ((TPcnt + FPcnt + TNcnt + FNcnt) != content.size())
                    std::cout << "error";
                double EucPre = TPcnt / (TPcnt + FPcnt);
                double EucNPV = TNcnt / (TNcnt + FNcnt);
                double EucSen = TPcnt / (TPcnt + FNcnt);
                double EucSpe = TNcnt / (TNcnt + FPcnt);
                double EucAccuracy = (TPcnt + TNcnt) / content.size();
                /*std::cout << TPcnt << ", " << FPcnt << ", " << FNcnt << ", " << TNcnt << ", "<< EucAccuracy<< std::endl;
            std::cout << EucPre << ", " << EucNPV << std::endl;
            std::cout << EucSen << ", " << EucSpe << std::endl;*/
                csvout << EucAccuracy << "," << EucPre << "," << EucNPV << ","
                       << EucSen << "," << EucSpe << ",";

                //Cosine distance querying

                std::sort(Cosdistancearray.begin(), Cosdistancearray.end(),
                          sortcol);
                std::cout << "The most similar " << numberofquery
                          << " shapes to this " << selected_meshclass
                          << " at Cosine Distance are : " << std::endl;
                std::cout << "     Class              No.      Distance "
                          << std::endl;
                TPcnt = 0, FPcnt = 0, FNcnt = 0, TNcnt = 0;
                //for (int i = Cosdistancearray.size() - 1;i > Cosdistancearray.size() - numberofquery - 1; i--)
                for (int i = 0; i < numberofquery; i++)
                {
                    std::cout << setw(10)
                              << Type[(int(Cosdistancearray[i][0]) - 1) / 20]
                              << " ";
                    //added
                    //csvout << Type[(int(Cosdistancearray[i][0]) - 1) / 20] << ",";

                    for (int j = 0; j < Cosdistancearray[i].size(); j++)
                    {
                        std::cout << setw(15) << Cosdistancearray[i][j];
                        //added
                        //csvout << Cosdistancearray[i][j] << ",";
                    }
                    //TP
                    if (selected_meshclass ==
                        Type[(int(Cosdistancearray[i][0]) - 1) / 20])
                    {
                        std::cout << " TP";
                        TPcnt++;
                    }
                    std::cout << endl;
                }
                //d = content.size(), c = Typecnt[(int(distancearray[i][0]) - 1) / 20], s = numberofquery
                FPcnt = numberofquery - TPcnt;
                //FN = c - TP
                FNcnt = Typecnt[(itera - 1) / 20] - TPcnt;
                //TN = d - c - FP
                TNcnt = content.size() - Typecnt[(itera - 1) / 20] - FPcnt;
                if (TPcnt + FPcnt + TNcnt + FNcnt != content.size())
                    std::cout << "error";
                double CosPre = TPcnt / (TPcnt + FPcnt);
                double CosNPV = TNcnt / (TNcnt + FNcnt);
                double CosSen = TPcnt / (TPcnt + FNcnt);
                double CosSpe = TNcnt / (TNcnt + FPcnt);
                double CosAccuracy = (TPcnt + TNcnt) / content.size();
                /*std::cout << TPcnt << ", " << CosAccuracy << std::endl;
            std::cout << CosPre << ", " << CosNPV << std::endl;
            std::cout << CosSen << ", " << CosSpe << std::endl;*/
                csvout << CosAccuracy << "," << CosPre << "," << CosNPV << ","
                       << CosSen << "," << CosSpe << ",";

                //Euclidean + EMD distance querying
                std::sort(EMDdistancearray.begin(), EMDdistancearray.end(),
                          sortcol);
                std::cout << "The most similar " << numberofquery
                          << " shapes to this " << selected_meshclass
                          << " at EMD Distance are : " << std::endl;
                std::cout << "     Class         No. Distance " << std::endl;
                TPcnt = 0, FPcnt = 0, FNcnt = 0, TNcnt = 0;
                for (int i = 0; i < numberofquery; i++)
                {
                    std::cout << setw(10)
                              << Type[(int(EMDdistancearray[i][0]) - 1) / 20]
                              << " ";
                    //added
                    //csvout << Type[(int(EMDdistancearray[i][0]) - 1) / 20] << ",";
                    
                    for (int j = 0; j < EMDdistancearray[i].size(); j++)
                    {
                        std::cout << setw(10) << EMDdistancearray[i][j];
                        //added
                        //csvout << EMDdistancearray[i][j] << ",";
                    }
                    //TP
                    if (selected_meshclass == Type[(int(EMDdistancearray[i][0]) - 1) / 20])
                    {
                        std::cout << " TP";
                        TPcnt++;
                    }
                    std::cout << endl;
                }

                //d = content.size(), c = Typecnt[(int(distancearray[i][0]) - 1) / 20], s = numberofquery
                FPcnt = numberofquery - TPcnt;
                //FN = c - TP
                FNcnt = Typecnt[(itera - 1) / 20] - TPcnt;
                //TN = d - c - FP
                TNcnt = content.size() - Typecnt[(itera - 1) / 20] - FPcnt;
                if (TPcnt + FPcnt + TNcnt + FNcnt != content.size())
                    std::cout << "error";
                double EMDPre = TPcnt / (TPcnt + FPcnt);
                double EMDNPV = TNcnt / (TNcnt + FNcnt);
                double EMDSen = TPcnt / (TPcnt + FNcnt);
                double EMDSpe = TNcnt / (TNcnt + FPcnt);
                double EMDAccuracy = (TPcnt + TNcnt) / content.size();
                std::cout << "TP count:"<< TPcnt <<std::endl;
                /*std::cout << TPcnt << ", " << EMDAccuracy << std::endl;
                std::cout << EMDPre << ", " << EMDNPV << std::endl;
                std::cout << EMDSen << ", " << EMDSpe << std::endl;*/
                csvout << EMDAccuracy << "," << EMDPre << "," << EMDNPV << ","
                       << EMDSen << "," << EMDSpe << ",";

                //when the number of mesh equals to 1, ask user to select preferred distance
                if (awake_viewer)
                {
                    std::cout << "Which distance do you prefer? (e/c/m): "
                              << std::endl;
                    std::cout << "e for Euclidean; c for Cosine, m for EMD"
                              << std::endl;
                    std::cin >> examinput;
                    if (examinput == 'e')
                    {
                        for (int i = 0; i < numberofquery; i++)
                        {
                            std::sprintf(target[i], "%s%d%s", filepath,
                                         int(Euclideandistancearray[i][0]),
                                         filetype);
                        }
                    }
                    else if (examinput == 'c')
                    {
                        //int meshtotal = Cosdistancearray.size() - 1;
                        //for (int i = meshtotal;i > meshtotal - numberofquery - 1; i--)
                        for (int i = 0; i < numberofquery; i++)
                        {
                            //std::sprintf(target[meshtotal - i], "%s%d%s",filepath, int(Cosdistancearray[i][0]),filetype);
                            std::sprintf(target[i], "%s%d%s",filepath, int(Cosdistancearray[i][0]),filetype);
                            //std::cout << "target is " << target[meshtotal - i] << std::endl;
                        }
                    }
                    else if (examinput == 'm')
                    {
                        //EMD here if avaliable
                        for (int i = 0; i < numberofquery; i++)
                        {
                            std::sprintf(target[i], "%s%d%s", filepath,
                                         int(EMDdistancearray[i][0]), filetype);
                        }
                    }
                    else
                    {
                        std::cout << "illegal input!" << std::endl;
                        return 0;
                    }
                }

                //added
                content.clear();
                row.clear();
                selected_mesh.clear();
                Eucstore.clear();
                Cosstore.clear();
                EMDstore.clear();
                Euclideandistancearray.clear();
                Cosdistancearray.clear();
                EMDdistancearray.clear();
                csvout << std::endl;
                file.close();
                //std::cout << "iter" << itera << "ended" << std::endl;
                //csvout << "\n";
                if (itera == 24 || itera == 38 || itera == 57 || itera == 46 ||
                    itera == 356)
                    itera++;
                else if (itera == 260)
                    itera = 280;
            }

            // if user only select 1 mesh, display 3 best-matching meshes in the viewer

            if (awake_viewer)
            {
                bool gui = true;
                MeshViewer viewer("MeshViewer: 1st", 800, 600, gui);
                viewer.load_mesh(target[0]);
                viewer.run();
                viewer.~MeshViewer();

                MeshViewer viewer2("MeshViewer: 2nd", 800, 600, gui);
                viewer2.load_mesh(target[1]);
                viewer2.run();
                viewer2.~MeshViewer();

                MeshViewer viewer3("MeshViewer: 3rd", 800, 600, gui);
                viewer3.load_mesh(target[2]);
                viewer3.run();
                viewer3.~MeshViewer();
                exit(1);
            }
        }
        
        cin.get();
        return 0;
    }
}
