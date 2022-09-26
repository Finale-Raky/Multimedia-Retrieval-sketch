#pragma once


class ScalarAttributes;


class Grid
{
public: 

virtual			         	  ~Grid();
virtual int				  numPoints() const		=0; 
virtual int				  numCells() const		=0; 
virtual void			  	  getPoint(int,float*)		=0; 
virtual int				  getCell(int,int*) const	=0; 
virtual int				  findCell(float*)		=0;
virtual ScalarAttributes& 		  pointScalars()		=0;

protected:
						  Grid() { }
};



