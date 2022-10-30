#pragma once
#include <cstdlib>
#include<stdio.h>
#include<string.h>
#include <iostream>

#include<math.h>
#include <string>
#include <vector>
#include "pmp/visualization/Window.h"
#include "pmp/MatVec.h"

using namespace std;
using namespace pmp;

double Distance(const vec3& V1, const vec3& V2)
{
    double a = (V1[0] - V2[0]) * (V1[0] - V2[0]);
    double b = (V1[1] - V2[1]) * (V1[1] - V2[1]);
    double c = (V1[2] - V2[2]) * (V1[2] - V2[2]);
    double distance = sqrt(a + b + c);
    return distance;
}

int sign(double x)
{
    int s;
    if (x >= 0)
        s = 1;
    else
        s = -1;
    return s;
}
