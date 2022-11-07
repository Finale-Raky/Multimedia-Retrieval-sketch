#include <GLUT/glut.h>
#include <vector>
#include <map>
#include <math.h>
#include <iostream>
#include "emd.h"


using namespace std;



typedef map<int,float> FeatureVec;              //Used to make sure we don't draw two points
                                                //with the same x coordinate

FeatureVec    features1,features2;              //The two distributions
float         f1Sum = 0, f2Sum = 0;             //The sums of values in the two distributions (used for normalization)
int           grid_size = 10;                   //Coarseness of drawing grid (in pixels)
int           state = GLUT_UP;
float         emdValue = -1;                    //Currently computed EMD value (for display); -1 = not computed yet
int           button = 0;
int           winX = 640, winY = 480;



void glutDrawString(const char* s)              //Draw a character string on screen
{
    for (int i=0; i < strlen(s); i++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
}

int round(int x)                                //Round pixel coords to grid coords
{
    return (x + grid_size - 1) / grid_size * grid_size;
}

void addPoint(FeatureVec& fv,float& fvSum,int x,int y)       //Add one more point to given distribution
{
    y = glutGet(GLUT_WINDOW_HEIGHT) - y;
    fv.insert(make_pair(round(x),round(y)));
    fvSum += round(y);
}

void myMouse(int button_, int state_, int x, int y)
{                                               //Draw: add points to either of the two
    state = state_;                             //distributions
    button = button_;
    
    if(state != GLUT_DOWN) return;

    if (button==GLUT_LEFT_BUTTON)
        addPoint(features1,f1Sum,x,y);
    else
        addPoint(features2,f2Sum,x,y);

    glutPostRedisplay();
}

void move(int x,int y)                          //Draw: add points to either of the two
{                                               //distributions
    if (button==GLUT_LEFT_BUTTON)
        addPoint(features1,f1Sum,x,y);
    else
        addPoint(features2,f2Sum,x,y);
    
    glutPostRedisplay();
}


void myDisplay()                                //Draw the two distributions as histograms
{
    char buf[100];
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, winX, 0.0, winY);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPointSize(grid_size);

    glColor3f(1,0,0);
    for(FeatureVec::iterator it=features1.begin();it!=features1.end();++it)
        glRecti(it->first,it->second,it->first+grid_size-1,0);

    glColor3f(0,1,0);
    for(FeatureVec::iterator it=features2.begin();it!=features2.end();++it)
        glRecti(it->first+1,it->second,it->first+grid_size-2,0);
    
    glColor3f(0,0,0);
    glRasterPos2i(10,winY-30);
    sprintf(buf,"Red histogram: %d points",features1.size());
    glutDrawString(buf);
    
    glRasterPos2i(10,winY-60);
    sprintf(buf,"Green histogram: %d points",features2.size());
    glutDrawString(buf);

    if (emdValue>=0)
    {
        glRasterPos2i(10,winY-90);
        sprintf(buf,"EMD: %f",emdValue);
        glutDrawString(buf);
    }

    glFlush();
}



float dist(feature_t *F1, feature_t *F2)                    //Distance of two histogram bars
{
    return fabs(*F1 - *F2);
}


void computeEMD()                                           //Compute the EMD of the drawn
{                                                           //distributions
    int N1 = features1.size(), N2 = features2.size();
    
    feature_t *f1 = new feature_t[N1], *f2 = new feature_t[N2];
    float     *w1 = new float[N1],     *w2 = new float[N2];
    
    int i=0;                                                //Convert the data into emd()'s format
    for(FeatureVec::iterator it=features1.begin();it!=features1.end();++it,++i)
    {
        f1[i] = it->first;
        w1[i] = it->second / f1Sum;
    }

    i=0;                                                    //Convert the 2nd distribution
    for(FeatureVec::iterator it=features2.begin();it!=features2.end();++it,++i)
    {
        f2[i] = it->first;
        w2[i] = it->second / f2Sum;
    }
    
    signature_t s1 = { N1, f1, w1}, s2 = { N2, f2, w2};
    
    emdValue = emd(&s1, &s2, dist, 0, 0);                    //Compute the EMD
    
    cout<<"Distribution 1 (red): "<<N1<<" points"<<endl;    //Print results
    cout<<"Distribution 2 (green): "<<N2<<" points"<<endl;
    cout<<"EMD = "<<emdValue<<endl;
    
    delete[] f1; delete[] f2; delete[] w1; delete[] w2;
    
    glutPostRedisplay();
}

void commands(unsigned char c,int,int)
{
    switch(c)
    {
        case 'e':
            computeEMD();
            break;
        case 'c':
            features1.clear();
            features2.clear();
            f1Sum = f2Sum = 0; emdValue = -1;
            break;
        case 'd':
            features2 = features1;
            break;
        case 'q':
            exit(0);
            break;
    }
    
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    cout<<"Earth Mover's Distance Demo"<<endl<<endl;
    
    cout<<" left-click/drag:  draw 1st distribution (red)"<<endl;
    cout<<" right-click/drag: draw 1st distribution (green)"<<endl;
    cout<<" space:            delete everything"<<endl;
    cout<<" e:                compute EMD"<<endl;
    cout<<" d:                copy 1st distribution to 2nd one"<<endl;
    cout<<" q:                exit program"<<endl<<endl;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winX, winY);
    glutInitWindowPosition(100, 150);
    glutCreateWindow("Draw Pixel");
    glutDisplayFunc(myDisplay);
    glutMouseFunc(myMouse);
    glutKeyboardFunc(commands);
    glutMotionFunc(move);

    glutMainLoop();
    return 0;
}


