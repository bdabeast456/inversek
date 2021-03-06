#include <string.h>
#include <fstream>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <ostream>
#include <numeric>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string.h>

#include "inversek_util.h"
#ifndef _Eigen_System
#define _Eigen_System
#include "Eigen/SVD"
#include "Eigen/Dense"
#endif

#define PI 3.14159265  // Should be used from mathlib
inline float sqr(float x) { return x*x; }
inline float min(float x, float y) {if (x < y) { return x;} else {return y;}}



using namespace std;

//****************************************************
// Some Classes
//****************************************************
class Viewport;

class Viewport {
    public:
        int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport    viewport;
float numCurves = 0;
int curFrame = 0;
int frameStepSize = 0;
int counter = 1;
double ustep = .04;
double errorBound = 1e-9; 
double sError = 1e-4;
double epsilon = 1e-9;
vector<Bezier> curves;
vector<Scene*> frames;

//****************************************************
// Simple init function
//****************************************************
void initScene(){
    GLfloat mat_specular[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat mat_shininess[] = { 350 };
    GLfloat mat_amb_diff[] = { 0.1, 0.5, 0.8, 1.0 };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_FLAT);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_amb_diff);


    GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };

    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHTING | GL_DEPTH_TEST);
    glEnable(GL_LIGHT0);


}

void myFrameMove() {
#ifdef _WIN32
  Sleep(10);                                   
#endif
  glutPostRedisplay(); 
}

void myKey(unsigned char key, int x, int y) {
    /*
     * General input key handling.
     */
    if(key==32) {
        frameStepSize = 1 - frameStepSize;
    } else if (key==27) {
    	exit(0);
    }
}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
    viewport.w = w;
    viewport.h = h;

    glViewport (0,0,viewport.w,viewport.h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ((float)w)/((float)h), .1f, 1500.0f);
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);              
    glMatrixMode(GL_MODELVIEW);                 
    glLoadIdentity();
    gluLookAt(10, 10, 10, 0, 0, 0, -1, 1, -1);
    glLineWidth(1.5); 
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    
    Arm curArm = *(frames[curFrame]->rootArm);
    Arm a2 = *(curArm.getNext());
    Arm a3 = *(a2.getNext());
    Arm a4 = *(a3.getNext());

    matrix rootT = matrix(a4.rotation[0], a4.rotation[1], a4.rotation[2], 2).multiplymRet(
    			   matrix(a4.length, 0, 0, 0));
    matrix a2T = rootT.multiplymRet(matrix(a3.rotation[0], a3.rotation[1], a3.rotation[2], 2).multiplymRet(
    		     matrix(a3.length, 0, 0, 0)));
    matrix a3T = a2T.multiplymRet(matrix(a2.rotation[0], a2.rotation[1], a2.rotation[2], 2).multiplymRet(
    		     matrix(a2.length, 0, 0, 0)));
    matrix a4T = a3T.multiplymRet(matrix(curArm.rotation[0], curArm.rotation[1], curArm.rotation[2], 2).multiplymRet(
    		     matrix(curArm.length, 0, 0, 0)));
    Vector4* start = new Vector4(0, 0, 0, 1);
    Vector4 p1 = rootT.multiplyv(*start);
    Vector4 p2 = a2T.multiplyv(*start);
    Vector4 p3 = a3T.multiplyv(*start);
    Vector4 p4 = a4T.multiplyv(*start);
    delete start;

    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(p1.xc(), p1.yc(), p1.zc());
    
    glEnd();
    glBegin(GL_LINES);

    glVertex3f(p1.xc(), p1.yc(), p1.zc()); 
    glVertex3f(p2.xc(), p2.yc(), p2.zc());
    
    glEnd();
    glBegin(GL_LINES);

    glVertex3f(p2.xc(), p2.yc(), p2.zc());
    glVertex3f(p3.xc(), p3.yc(), p3.zc());

    glEnd();
    glBegin(GL_LINES);

    glVertex3f(p3.xc(), p3.yc(), p3.zc());
	glVertex3f(p4.xc(), p4.yc(), p4.zc());

    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.2f);
    glVertex3f(8.0f, -4.0f, -8.0f);
    glVertex3f(-8.0f, -4.0f, -8.0f);
    glVertex3f(-8.0f, -4.0f, 8.0f);
    glVertex3f(8.0f, -4.0f, 8.0f);
    glEnd();
    glPushMatrix();
    glTranslatef(frames[curFrame]->point[0], frames[curFrame]->point[1], frames[curFrame]->point[2]);
   	glBegin(GL_LINES);
   	glColor3f(0.0f, 1.0f, 0.0f);
   	GLUquadric *quad;
   	GLdouble radius = 0.2;
   	GLint slices, stacks;
   	quad = gluNewQuadric();
   	slices = stacks = 10;
   	gluSphere(quad, radius, slices, stacks);
   	glPopMatrix();
   	glEnd();
         
    glFlush();
    glutSwapBuffers();                  // swap buffers (we earlier set double buffer)
    
    if (counter % 10 == 0) {
    	curFrame = (curFrame + frameStepSize) % frames.size();
    	counter = 0;
    }
    counter++;
}

vector<double> getEndPoint(double u_val){
    double u = u_val;
    bool assigned = false;
    Bezier curve;
    for(int i = 0; i < curves.size(); i++){
        if(u <= 1){
            assigned = true;
            curve = curves[i];
            break;
        }
        u -=1;
    }
    if(assigned){
        return curve.getPoint(u);
    }
    vector<double> rv;
    rv.push_back(INFINITY);
    rv.push_back(INFINITY);
    rv.push_back(INFINITY);
    return rv;
}

double distance(double x, double y, double z, vector<double> point2) {
	return pow(pow(x-point2[0], 2)+pow(y-point2[1], 2)+pow(z-point2[2], 2), .5);
}

vector<Vector4> partial(vector<double> rotations, double length[4]) {
	vector<Vector4> retVector;
	Vector4 tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   		   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
					   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
					    matrix(rotations[0]+epsilon, rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
					    Vector4(0, 0, 0, 1));	
	Vector4 tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   		   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
					   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
					    matrix(rotations[0]-epsilon, rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
					    Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1]+epsilon, rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1]-epsilon, rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2]+epsilon, 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2]-epsilon, 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3]+epsilon, rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
			    Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3]-epsilon, rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4]+epsilon, rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
			    Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4]-epsilon, rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5]+epsilon, 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5]-epsilon, 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6]+epsilon, rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6]-epsilon, rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7]+epsilon, rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7]-epsilon, rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8]+epsilon, 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8]-epsilon, 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9]+epsilon, rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9]-epsilon, rotations[10], rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10]+epsilon, rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
			    Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10]-epsilon, rotations[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	tempPe1 = ((matrix(rotations[9], rotations[10], rotations[11]+epsilon, 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));	
	tempPe2 = ((matrix(rotations[9], rotations[10], rotations[11]-epsilon, 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[6], rotations[7], rotations[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
			   (matrix(rotations[3], rotations[4], rotations[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
				matrix(rotations[0], rotations[1], rotations[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
				Vector4(0, 0, 0, 1));
	retVector.push_back((tempPe1.sub(tempPe2)).scale(1/(2*epsilon)));
	return retVector;
}

void generateFrames() {
	int steps = (int)(numCurves/ustep);
	for (int i=0; i<steps; i++) {
		vector<double> goal = getEndPoint(i*ustep);
		cout << "goal is: " << goal[0] << ", " << goal[1] << ", " << goal[2] << endl;
		Arm* beforeArm;
		if (frames.size()==0) {
			beforeArm = new Arm();
		} else {
			beforeArm = frames[frames.size()-1]->rootArm;
		}
		Arm* a2 = beforeArm->getNext();
		Arm* a3 = a2->getNext();
		Arm* a4 = a3->getNext();
		vector<double> rotations;
		vector<double> rotationsTemp;
		rotations.push_back(beforeArm->rotation[0]);
		rotations.push_back(beforeArm->rotation[1]);
		rotations.push_back(beforeArm->rotation[2]);
		rotations.push_back(a2->rotation[0]);
		rotations.push_back(a2->rotation[1]);
		rotations.push_back(a2->rotation[2]);
		rotations.push_back(a3->rotation[0]);
		rotations.push_back(a3->rotation[1]);
		rotations.push_back(a3->rotation[2]);
		rotations.push_back(a4->rotation[0]);
		rotations.push_back(a4->rotation[1]);
		rotations.push_back(a4->rotation[2]);
		if (goal[0] == 0 && goal[1] == 0 && goal[2] == 0) {
			for (int r=0; r<12; r++) {
				rotations[r] = rotations[r] + .1;
			}
		}
		for (int j=0; j<12; j++) {
			rotationsTemp.push_back(rotations[j]);
		}
		double length[4] = {beforeArm->length, a2->length, a3->length, a4->length};
		double alpha = 1;
		Vector4 Pe = ((matrix(rotationsTemp[9], rotationsTemp[10], rotationsTemp[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
					 (matrix(rotationsTemp[6], rotationsTemp[7], rotationsTemp[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
					 (matrix(rotationsTemp[3], rotationsTemp[4], rotationsTemp[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
					  matrix(rotationsTemp[0], rotationsTemp[1], rotationsTemp[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
					  Vector4(0, 0, 0, 1));
		double prevDist = distance(Pe.xc(), Pe.yc(), Pe.zc(), goal);
		int iterations = 0;
		Eigen::VectorXd dr;
        bool dontWhile = false;
		while (true && !dontWhile) {
			Vector4 tempPe = ((matrix(rotationsTemp[9], rotationsTemp[10], rotationsTemp[11], 2).multiplymRet(matrix(length[3], 0, 0, 0))).multiplymRet(
						 (matrix(rotationsTemp[6], rotationsTemp[7], rotationsTemp[8], 2).multiplymRet(matrix(length[2], 0, 0, 0))).multiplymRet(
						 (matrix(rotationsTemp[3], rotationsTemp[4], rotationsTemp[5], 2).multiplymRet(matrix(length[1], 0, 0, 0))).multiplymRet(
						  matrix(rotationsTemp[0], rotationsTemp[1], rotationsTemp[2], 2).multiplymRet(matrix(length[0], 0, 0, 0)))))).multiplyv(
						  Vector4(0, 0, 0, 1));
		    double currDist = distance(tempPe.xc(), tempPe.yc(), tempPe.zc(), goal);
           			
			if (currDist <= errorBound || alpha < errorBound) {
				rotations = rotationsTemp;
				break;
			} else if (currDist >= prevDist) {
				alpha = alpha / 2;
			} else {
				rotations = rotationsTemp;
				alpha = 1;
				prevDist = currDist;
				Pe = tempPe;
			}
			Eigen::MatrixXd jacobian(3, 12);
			vector<Vector4> p = partial(rotations, length);
			jacobian << p[0].xc(), p[1].xc(), p[2].xc(), p[3].xc(), p[4].xc(), p[5].xc(),
					    p[6].xc(), p[7].xc(), p[8].xc(), p[9].xc(),p[10].xc(),p[11].xc(),
					    p[0].yc(), p[1].yc(), p[2].yc(), p[3].yc(), p[4].yc(), p[5].yc(),
					    p[6].yc(), p[7].yc(), p[8].yc(), p[9].yc(),p[10].yc(),p[11].yc(),
					    p[0].zc(), p[1].zc(), p[2].zc(), p[3].zc(), p[4].zc(), p[5].zc(),
					    p[6].zc(), p[7].zc(), p[8].zc(), p[9].zc(),p[10].zc(),p[11].zc();
            Eigen::VectorXd dp(3,1);
            dp << alpha*(goal[0] - Pe.xc()), 
                  alpha*(goal[1] - Pe.yc()), 
                  alpha*(goal[2] - Pe.zc());
			Eigen::JacobiSVD<Eigen::MatrixXd> svd(jacobian, Eigen::ComputeFullU | Eigen::ComputeFullV);
			Eigen::MatrixXd uMat = svd.matrixU().transpose();
			Eigen::MatrixXd vMat = svd.matrixV();
			Eigen::VectorXd sVals = svd.singularValues();
			for (int j=0; j<3; j++) {
				if (abs(sVals(j)) > sError) {
					sVals(j) = 1/sVals(j);
				} else {
					sVals(j) = 0;
				}
			}
			Eigen::MatrixXd sMat(12, 3);
			sMat << sVals(0), 0, 0,
					0, sVals(1), 0,
					0, 0, sVals(2),
					0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					0, 0, 0,
					0, 0, 0;
			Eigen::MatrixXd pseudoInverse = vMat*sMat*uMat;
			dr = pseudoInverse*dp;
			for (int k=0; k<12; k++) {
				rotationsTemp[k] = rotations[k] + dr(k);
			}
			iterations++;
		}
		double rot1[3] = {rotations[0], rotations[1], rotations[2]};
		double rot2[3] = {rotations[3], rotations[4], rotations[5]};
		double rot3[3] = {rotations[6], rotations[7], rotations[8]};
		double rot4[3] = {rotations[9], rotations[10], rotations[11]};
		beforeArm = new Arm(length[0], rot1);
		a2 = new Arm(length[1], rot2);
		a3 = new Arm(length[2], rot3);
		a4 = new Arm(length[3], rot4);
		beforeArm->setNext(a2);
		a2->setNext(a3);
		a3->setNext(a4);
		double endPoint[3] = {goal[0], goal[1], goal[2]};
		frames.push_back(new Scene(beforeArm, endPoint));
	}
}

int mod(int n, int m) {
	if (n < m && n >=0) {
		return n;
	} else if (n>=m) {
		return n%m;
	} else {
		while (n<0) {
			n = n+m;
		}
		return n;
	}
}

void specialKey(int key, int x, int y){
	if (frameStepSize == 0) {
    	if(key == GLUT_KEY_LEFT){
        	curFrame = mod(curFrame - 1, frames.size());
        	myDisplay();
    	}
    	if(key == GLUT_KEY_RIGHT){
    		curFrame = mod(curFrame+1, frames.size());
        	myDisplay();
   		}
   	}
}


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
    const int MAX_CHARS_PER_LINE = 512;
    const int MAX_TOKENS_PER_LINE = 17;
    const char* const DELIMITER = " ";
    string readFile;
    if (argc < 2) {
        cout << "No input file specified." << endl;
        exit(0);
    }
    string arg1 = string(argv[1]);
    if(strlen(arg1.c_str()) >= 4){
        string last4 = arg1.substr(strlen(arg1.c_str())-4,string::npos);
        if(last4 == ".bez"){
            readFile = arg1;
        }
        else{
            cout << "input file not in .bez format" << endl;
            exit(0);
        }
    }
    else {
        cout << "Unrecognized argument. Please review usage." << endl;
        exit(0);
    }
    //start parsing readFile
    ifstream myFile;
    myFile.open(readFile);
    if(readFile == ""){
        cout << "No input provided. Please review usage." << endl;
        exit(0);
    }
    else{
        int lineNumber= 1;
        vector<int> patchNum; // when == 4, parse current set of patches into surfaces
        patchNum.push_back(0);
        double patchOne[4][3];
        cout << "Parsing BEZ file" << endl;
        std::string str;
        string to_add = " ";
        std::vector<std::string> vec, result_vec;
        while (std::getline(myFile, str)) {
            vec.push_back(str);
        }
        for(std::vector<string>::iterator it = vec.begin(); it != vec.end(); ++it) {
            string s = *it;
            s.insert(s.length(),1, to_add[0]);
            s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
            result_vec.push_back(s);
        }
        std::ofstream out_file(readFile);
        std::copy(result_vec.begin(), result_vec.end(), std::ostream_iterator<std::string>(out_file, "\n"));
        out_file.clear();
        out_file.close();
        
        myFile.clear();
        myFile.seekg(0, ios::beg);
        
        while (!myFile.eof()){
            char buf[MAX_CHARS_PER_LINE];
            myFile.getline(buf, MAX_CHARS_PER_LINE);
            const char* token[MAX_TOKENS_PER_LINE] = {}; 
            token[0] = strtok(buf, DELIMITER); // first token

            if (token[0]){
                int length = 0;
                for (int n = 1; n < MAX_TOKENS_PER_LINE; n++) {
                    token[n] = strtok(0,DELIMITER);
                    length +=1;
                    if (!token[n]){
                        break;
                    }
                }

                string first = string(token[0]).c_str();
                if(lineNumber == 1){
                    numCurves = atof(string(token[0]).c_str());
                }
                else{
                    double totalPatch[4][3];
                    totalPatch[0][0] = atof(string(token[0]).c_str());
                    totalPatch[0][1] = atof(string(token[1]).c_str());
                    totalPatch[0][2] = atof(string(token[2]).c_str());

                    totalPatch[1][0] = atof(string(token[3]).c_str());
                    totalPatch[1][1] = atof(string(token[4]).c_str());
                    totalPatch[1][2] = atof(string(token[5]).c_str());

                    totalPatch[2][0] = atof(string(token[6]).c_str());
                    totalPatch[2][1] = atof(string(token[7]).c_str());
                    totalPatch[2][2] = atof(string(token[8]).c_str());

                    totalPatch[3][0] = atof(string(token[9]).c_str());
                    totalPatch[3][1] = atof(string(token[10]).c_str());
                    totalPatch[3][2] = atof(string(token[11]).c_str());
                    //if(patchNum[0] == 0){
                    for(int i = 0; i < 4; i ++){
                        for (int j = 0; j < 3; j++){
                            patchOne[i][j] =  totalPatch[i][j];
                        }
                    }
                    Bezier curBez = Bezier(patchOne);
                    curves.push_back(curBez);
                }
                lineNumber+=1;
            } // end of if(token[0])
        } // end of while(!myFile.eof())
        
    } //end of parsing
    cout << "Done parsing" << endl;

    generateFrames();

    //This initializes glut
    glutInit(&argc, argv);
    //This tells glut to use a double-buffered window with red, green, and blue channels 

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Initalize theviewport size
    viewport.w = 400;
    viewport.h = 400;

    //The size and position of the window
    glutInitWindowSize(viewport.w, viewport.h);
    glutInitWindowPosition(0,0);
    glutCreateWindow(argv[0]);

    initScene();                            // quick function to set up scene
    glEnable(GL_LIGHTING | GL_DEPTH_TEST);

    glutDisplayFunc(myDisplay);             // function to run when its time to draw something
    glutReshapeFunc(myReshape);             // function to run when the window gets resized
    glutKeyboardFunc(myKey);
    glutSpecialFunc(specialKey);
    glEnable(GL_LIGHTING | GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glutIdleFunc(myFrameMove);
    glutMainLoop();                         // infinite loop that will keep drawing and resizing


    return 0;
}