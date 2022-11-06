#pragma once
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <vector>
#include <numeric>


std::random_device rd;
std::mt19937 gen(rd());

vector<int> G_random(int n, int range)//generate n different number in the range of [0,R]
{
    std::vector<int> numbers;
    for (int i = 0; i < range; i++)       // add 0-range to the vector
        numbers.push_back(i);
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(numbers.begin(), numbers.end(), std::default_random_engine(seed));
    std::vector<int> index;
    for (int i = 0; i < n; i++)
    {
        index.push_back(numbers[i]);
    }
    
    return index;
}

//shape descriptor
double A3_angle(vector<Point> p_arr, vector<int> index)
{
    Point A = p_arr[index[0]]; //Get 3 random point
    Point B = p_arr[index[1]];
    Point C = p_arr[index[2]];
    //calculate angle
    /* cosA = (AB*AB + AC*AC - BC*BC ) / 2*AB*AC */
    Point AB = B - A;
    Point AC = C - A;
    Point BC = C - B;
    double A3_cos =
        (dot(AB, AB) + dot(AC, AC) - dot(BC, BC)) / (2 * dot(AB, AC));
    double a3_cos = fminl(fmaxl(A3_cos, -1.0), 1.0);
    double a3 = acos(a3_cos);
    //double a3 = acos(A3_cos) / M_PI * 180;
    return a3;
}

double d1_distance(vector<Point> p_arr, vector<int> index, Point barycenter)
{
    Point R = p_arr[index[0]];
    double d = Distance(R, barycenter);
    return d;
}

double d2_distance(vector<Point> p_arr, vector<int> index)
{
    Point A = p_arr[index[0]]; //Get 2 random point
    Point B = p_arr[index[1]];
    double d = Distance(A,B);
    return d;
}


double d3_distance(vector<Point> p_arr, vector<int> index)
{
    Point A = p_arr[index[0]]; //Get 3 random point
    Point B = p_arr[index[1]];
    Point C = p_arr[index[2]];
   // calculate ABC  (S = sqrt(p*(p-a)*(p-b)*(p-c)))
    double a = Distance(A, B);
    double b = Distance(A, C);
    double c = Distance(B, C);
    double p = (a + b + c) / 2;
    double S = sqrt(p * (p - a) * (p - b) * (p - c));
    //square root 
    double d3 = sqrt(S);
    return d3;

}

double d4_distance(vector<Point> p_arr, vector<int> index)
{
    Point a = p_arr[index[0]]; //Get 4 random point
    Point b = p_arr[index[1]];
    Point c = p_arr[index[2]];
    Point d = p_arr[index[3]];
    // calculate ABCD
    Point ab = a - b;
    Point bd = b - d;
    Point cd = c - d;
    double v = cbrt((dot(ab, cross(bd, cd)) / 6));
    return v;
}

vector<double> hist(int bin, vector<double> descriptor) //calculate the hist after normalization
{
    double max = *max_element(descriptor.begin(), descriptor.end());
    double min = *min_element(descriptor.begin(), descriptor.end());
    double inter = (max - min) / bin;
    
    std::vector<int> hist(bin, 0);
    for (int i = 0; i < descriptor.size(); i++)
    {
        int ind = (descriptor[i]-min) / inter;
        if (ind == bin)
        {
            ind = ind - 1;
        }
        hist[ind] = hist[ind]+1;
    }

    std::vector<double> hist_nor;
    //Normalizing a histogram
    double sum = std::accumulate(hist.begin(), hist.end(), 0);
    for (int k = 0; k < hist.size(); k++)
    {
        hist_nor.push_back(hist[k] / sum);
    }
    return hist_nor;
}

