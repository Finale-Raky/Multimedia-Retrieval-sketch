#pragma once

#include "Grid.h"
#include "ScalarAttributes.h"
#include <vector>

using namespace std;




class UnstructuredGrid : public Grid
{
public: 
					UnstructuredGrid(int P,int C): scalars(P)
					{
						pointsX.resize(P);
						pointsY.resize(P);
						cells.resize(3*C);
					}	 
					 	
int					numPoints() const
					{ return pointsX.size(); }				 

int					numCells() const	
					{ return cells.size()/3; }

void					getPoint(int i,float* p);

void					setPoint(int i,float* p);		

int					getCell(int cell,int* vertices) const; 

void					setCell(int cell,int* vertices);

int					findCell(float* p);

ScalarAttributes&	pointScalars()
					{ return scalars; }

protected:

ScalarAttributes	scalars;
vector<float>		pointsX,pointsY;
vector<int>			cells;
};



