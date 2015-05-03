#pragma once
#ifndef _vector
#define _vector
#include <vector>
#endif

using namespace std;
class Arm;
class Vector4 {
    public:
        Vector4();
        Vector4(double px, double py, double pz, double pw);
        double xc();
        double yc();
        double zc();
        double wc();
        double dot4(Vector4 v);
        void unit();
        Vector4 sub(Vector4 v);
        Vector4 cross(Vector4 v);
    private:
        double x;
        double y;
        double z;
        double w;
};

class Arm {
    public:
        Arm();
        Arm(double l);
        Arm(double l, double rot[3]);
        double length;
        vector<double> rotation;
        Arm* getNext();
        void setNext(Arm* toSet);
    private:
        Arm* nextArm;
};

class Bezier{
    /*
    * Bezier is a class that holds the bezier data for a given surface. 
    */
    public:
    Bezier(double patch[][3]);
    Bezier(){};
    vector<double> getPoint(double u);
    vector<double> cx;
    vector<double> cy;
    vector<double> cz;
    vector<vector<double> > patch_store;
    
};

class matrix {
//0: translation, 1: scale, 2: rotation
//Warning: don't use the invmult on a matrix created by transposeInverse
public:
    matrix();
    matrix(double a, double b, double c, int mtype);
    matrix(double x1, double x2, double x3, double x4,
           double y1, double y2, double y3, double y4,
           double z1, double z2, double z3, double z4,
           double w1, double w2, double w3, double w4);
    Vector4 multiplyv(Vector4 v);
    void multiplym(matrix m);
    matrix multiplymRet(matrix m);
    void printMatrix();
private:
    vector<Vector4> mtrx;
};

class Scene {
	/*
	* Scene is a class that serves as a container for objects to be written to the screen.
	*/
	public:
	Scene(Arm* a, double pt[3]);
	Scene(){};
	Arm* rootArm;
	vector<double> point;
};

