#include <GLUT/glut.h>									//GLUT library
#include <iostream>
#include "UnstructuredGrid3D.h"							//the 3D unstructured grid class
#include "MeshRenderer.h"								//a simple renderer for 3D unstructured grids, demonstrating flat/smooth shading
#include "PlyReader.h"									//a reader that initializes UnstructuredGrid3D objects with meshes stored in the PLY file format
#include "zpr.h" from zpr.cpp										//library for interactively manipulating the OpenGL camera (viewpoint) using the mouse
#include "MeshDecimator.h"
#include <iostream>





using namespace std;
using namespace MeshDecimation;


float fov    = 60;										//Perspective projection parameters
float z_near = -100; 
float z_far  = 100;


MeshRenderer		renderer;
UnstructuredGrid3D*	grid = 0;
int			drawing_style = 0;


void viewing(int W, int H)								//Window resize function, sets up viewing parameters (GLUT callback function)
{
	glMatrixMode(GL_PROJECTION);						//1. Set the projection matrix
	glLoadIdentity();	
	gluPerspective(fov,float(W)/H,z_near,z_far);

	glViewport(0,0,W,H);								//2. Set the viewport to the entire size of the rendering window
}



void mouseclick(int button, int state, int x, int y)	//Callback for mouse click events. We use these to control the viewpoint interactively.
{
  int keys = glutGetModifiers();						//The mouse events are interpreted as follows:	
  if (keys & GLUT_ACTIVE_CTRL)							// 
    button = GLUT_MIDDLE_BUTTON;						// -left button + move:                         rotate viewpoint
  if (keys & GLUT_ACTIVE_SHIFT)							// -middle button (or left button+Control key): zoom in / out
    button = GLUT_RIGHT_BUTTON;							// -right buttom (or left button+Shift key):    translate viewpoint
  
  zprMouse(button,state,x,y);							//Use the ZPR library to manipulate the viewpoint. The library sets the modelview  
														//OpenGL matrix based on the mouse events, thereby changing the current viewpoint.
}

void mousemotion(int x, int y)							//Callback for mouse move events. We use these to control the viewpoint interactively.
{
  zprMotion(x,y);										//Pass the current location of the mouse to the ZPR library, to change the viewpoint.

  glutPostRedisplay();									//After each mouse move event, ask GLUT to redraw our window, so we see the viewpoint change.
}

void decimationCallback(const char * msg)					//Progress callback for decimation
{
    cout<<msg;
}


void keyboard(unsigned char c,int,int)					//Callback for keyboard events:
{
	switch(c)	
	{
	case ' ':							// space:   Toggle through the various drawing styles of the mesh renderer
		{
		  drawing_style = (++drawing_style) % 4;		
		  renderer.setDrawingStyle((MeshRenderer::DRAW_STYLE)drawing_style);
		  break;
		}

	case 'd':							// decimate the grid with some constant factor
	        {
	          int decPoints  = grid->numPoints() * 0.8;		// the decimation factors (vertices, cells)
                  int decCells = grid->numCells() * 0.4;		// amd max decimation error are given here; change as desired
		  float maxDecError = 1.0;				
		  
	          MeshDecimator dec;
	          dec.SetCallBack(&decimationCallback);
	          dec.Initialize(*grid);
	          dec.Decimate(decPoints,decCells,maxDecError);

	          delete grid;						// remake the grid (from decimation) for display
	          dec.GetDecimatedOutput(grid);	                   
	        }  
	}	

	glutPostRedisplay();			
}	

void draw()									//Render the 3D mesh (GLUT callback function)
{
	renderer.draw(*grid);
}


int main(int argc,char* argv[])							//Main program
{
   cout<<"Decimating 3D unstructured grids."<<endl;
   cout<<endl;
   cout<<" d: Do one decimation step"<<endl;
   cout<<" space: Toggle through rendering modes"<<endl<<endl;

   const char* filename = (argc<2)? "DATA/scapula.ply" : argv[1];  //Read the PLY file given as 1st argument. If no arguments given, use a default file.

   glutInit(&argc, argv);								//1.  Initialize the GLUT toolkit
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
														//2.  Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
   glutInitWindowSize(500,500);							//3.  Tell GLUT how large are the windows we want to create next
   glutCreateWindow("Decimating 3D unstructured grids");			//4.  Create our window
   zprInit(0,0,0);										//5.  Initialize the viewpoint interaction-tool to look at the point (0,0,0)
   
   PlyReader rdr;										//6.  Read a 3D mesh stored in a file in the PLY format
   grid = rdr.read(filename);
   grid->normalize();									//7.  Normalize the mesh in the [-1,1] cube. This makes setting the OpenGL projection easier.
   grid->computeFaceNormals();							//8.  Compute face and vertex normals for the mesh. This allows us to shade the mesh next.
   grid->computeVertexNormals();
   
   glutMouseFunc(mouseclick);							//9.  Bind the mouse click and mouse drag (click-and-move) events to callbacks. This allows us
   glutMotionFunc(mousemotion);							//    next to control the viewpoint interactively.
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(draw);								//10. Add a drawing callback to the window	
   glutReshapeFunc(viewing);							//11. Add a resize callback to the window
   glutMainLoop();										//12. Start the event loop that displays the graph and handles window-resize events
   
   return 0;
}




