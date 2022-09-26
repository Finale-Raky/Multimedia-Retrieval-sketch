#include <GLUT/glut.h>									//GLUT library
#include <iostream>
#include "UnstructuredGrid3D.h"							//the 3D unstructured grid class
#include "MeshRenderer.h"								//a simple renderer for 3D unstructured grids, demonstrating flat/smooth shading
#include "PlyReader.h"									//a reader that initializes UnstructuredGrid3D objects with meshes stored in the PLY file format
#include "zpr.h"										//library for interactively manipulating the OpenGL camera (viewpoint) using the mouse
#include <map>



float fov	 = 120;										//Perspective projection parameters
float z_near = -100;
float z_far  = 100;

PlyReader           rdr;
MeshRenderer		renderer;
UnstructuredGrid3D *grid = 0, *grid2 = 0;
int					drawing_style = 0;
float               fraction = 0.5;                     //Cells larger than the average-area * fraction will be subdivided
float               avg_area = 0;                       //Average cell area in input mesh

void viewing(int W, int H)								//Window resize function, sets up viewing parameters (GLUT callback function)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glMatrixMode(GL_PROJECTION);						//1. Set the projection matrix
	glLoadIdentity();	
	gluPerspective(fov,float(W)/H,z_near,z_far);

	glViewport(0,0,W,H);								//2. Set the viewport to the entire size of the rendering window
}


float computeAverageCell(UnstructuredGrid3D* grid)      //Compute the average cell area
{
    float ret = 0;
    
    for(int i=0;i<grid->numCells();++i)
    {
        int c[3];
        grid->getCell(i,c);
        
        Point3d pts[3];
        grid->getPoint(c[0],pts[0]);
        grid->getPoint(c[1],pts[1]);
        grid->getPoint(c[2],pts[2]);

        Point3d cross_prod = (pts[1]-pts[0]).cross(pts[2]-pts[0]);
        float area = cross_prod.norm()/2;               //A cell's area is half the cross-product of two edge vectors
        
        ret += area;
    }

    return ret/grid->numCells();
}


void subdivide(UnstructuredGrid3D*& grid,UnstructuredGrid3D* grid2,float min_area)
{
    int NP = grid->numPoints(), NC = grid->numCells();
    
    vector<Point3d> pts(NP);                            //Copy all vertices; we will reuse all of them
    for(int i=0;i<NP;++i)
    {
        Point3d& pi = pts[i];
        grid->getPoint(i,&pi.x);
    }
    
    multimap<float,int> area_sorted_cells;              //Used to sort cells from largest to smallest
    for(int i=0;i<NC;++i)                               //Sort cells from largest to smallest in 'area_sorted_cells'
    {
        int c[3];
        grid->getCell(i,c);
        
        Point3d cross_prod = (pts[c[1]]-pts[c[0]]).cross(pts[c[2]]-pts[c[0]]);
        float area = cross_prod.norm()/2;
                                                        //Since multimap<> is ascendingly-sorted by definition, add
        area_sorted_cells.insert(make_pair(-area,i));   //cells with their negative areas so largest cells come first
    }
    
    int NP2 = NP;
    int NC2 = 0;
    int Nsubdivided = 0;
    vector<int> cells;
    
    for(map<float,int>::const_iterator it=area_sorted_cells.begin();it!=area_sorted_cells.end();++it)
    {                                                   //Treat cells from largest to smallest
        float area = -(*it).first;
        int      i = (*it).second;
        
        int c[3];
        grid->getCell(i,c);
        if (area >= min_area)                           //Cell larger than the min-area allowed for subdivision?
        {                                               //Then subdivide the cell into 3 cells using its barycenter
            Point3d center = (pts[c[0]]+pts[c[1]]+pts[c[2]])/3;
            pts.push_back(center);
            
            cells.push_back(c[0]); cells.push_back(c[1]); cells.push_back(NP2);
            cells.push_back(NP2); cells.push_back(c[1]); cells.push_back(c[2]);
            cells.push_back(c[2]); cells.push_back(c[0]); cells.push_back(NP2);
            
            Nsubdivided++;                              //One more cell was subdivided
            NP2++;
            NC2+=3;
        }
        else                                            //Cell smaller than subdivision threshold?
        {                                               //Then simply copy it
            cells.push_back(c[0]);
            cells.push_back(c[1]);
            cells.push_back(c[2]);
            NC2++;
        }
    }
    
    grid2 = new UnstructuredGrid3D(NP2,NC2);            //Now that we have all vertices and cells of the refined grid,
    for(int i=0;i<NP2;++i)                              //make the new grid
        grid2->setPoint(i,(float*)pts[i]);
    
    for(int i=0;i<NC2;++i)
        grid2->setCell(i,&(cells[3*i]));
    
    grid2->normalize();                                 //Renormalize the grid since we changed it
    grid2->computeFaceNormals();                        //Recompute face normals since we made new cells
    grid2->computeVertexNormals();                      //Recompute vertex normals since we made new vertices and cells
    
    delete grid;                                        //Delete the old grid and
    grid = grid2;                                       //replace it by the new, subdivided, one
    
    cout<<"Subdivided cells: "<<Nsubdivided<<endl;
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

void keyboard(unsigned char c,int,int)					//Callback for keyboard events:
{
	switch(c)	
	{
	case ' ':											// space:   Toggle through the various drawing styles of the mesh renderer
		{
		  drawing_style = (++drawing_style) % 4;		
		  renderer.setDrawingStyle((MeshRenderer::DRAW_STYLE)drawing_style);
		  break;
		}
	case 'R':											// 'r','R': Reset the viewpoint
	case 'r':
    {
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		zprInit(0,0,0);
		break;
    }
    case 's':
    {
        subdivide(grid,grid2,fraction*avg_area);
        break;
    }
    case 'f':
    {
        fraction /= 2;
        cout<<"Current subdivision fraction: "<<fraction<<endl;
        break;
    }
    case 'F':
    {
        fraction *= 2;
        cout<<"Current subdivision fraction: "<<fraction<<endl;
        break;
    }
    case 'w':
    {
        rdr.write("refined.ply",grid);
        cout<<"Subdivided grid written to refined.ply"<<endl;
    }
    }
	
	glutPostRedisplay();			
}	

void draw()												//Render the 3D mesh (GLUT callback function)
{
	renderer.draw(*grid);
}


int main(int argc,char* argv[])							//Main program
{
   cout<<"3D unstructured grid (mesh) visualization."<<endl;
   cout<<"Interaction options: "<<endl<<endl;
   cout<<"  Mouse: changes the viewpoint:"<<endl;
   cout<<"      -rotate:     left-click and drag"<<endl;
   cout<<"      -translate:  right-click (or Shift-left-click) and drag"<<endl;
   cout<<"      -scale:      middle-click (or Control-left-click) and drag"<<endl;
   cout<<"  Keyboard:"<<endl;
   cout<<"       -s:         subdivide all mesh cells larger than fraction * average-cell"<<endl;
   cout<<"       -f:         decrease fraction (subdivide more cells"<<endl;
   cout<<"       -F:         increase fraction (subdivide fewer cells)"<<endl;
   cout<<"      -r,R:        reset the viewpoint"<<endl;
   cout<<"      -space:      cycle through mesh rendering styles"<<endl;

   const char* filename = (argc<2)? "DATA/scapula.ply" : argv[1];  //Read the PLY file given as 1st argument. If no arguments given, use a default file.

   glutInit(&argc, argv);								//1.  Initialize the GLUT toolkit
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
														//2.  Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
   glutInitWindowSize(500,500);							//3.  Tell GLUT how large are the windows we want to create next
   glutCreateWindow("6. 3D mesh (unstructured grid)");	//4.  Create our window
   zprInit(0,0,0);										//5.  Initialize the viewpoint interaction-tool to look at the point (0,0,0)
   
   grid = rdr.read(filename);                           //6.  Read a 3D mesh stored in a file in the PLY format
   grid->normalize();									//7.  Normalize the mesh in the [-1,1] cube. This makes setting the OpenGL projection easier.
   grid->computeFaceNormals();							//8.  Compute face and vertex normals for the mesh. This allows us to shade the mesh next.
   grid->computeVertexNormals();
    
   avg_area = computeAverageCell(grid);
   cout<<"Average cell area: "<<avg_area<<endl;
   cout<<"Refining cells smaller than: "<<fraction<<" of average cell"<<endl;
    
   glutMouseFunc(mouseclick);							//9.  Bind the mouse click and mouse drag (click-and-move) events to callbacks. This allows us
   glutMotionFunc(mousemotion);							//    next to control the viewpoint interactively.
   glutKeyboardFunc(keyboard);
   glutDisplayFunc(draw);								//10. Add a drawing callback to the window	
   glutReshapeFunc(viewing);							//11. Add a resize callback to the window
   glutMainLoop();										//12. Start the event loop that displays the graph and handles window-resize events
   
   return 0;
}




