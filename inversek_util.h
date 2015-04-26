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
        Arm(double l, double angle, Arm next);
        Arm(double l, double angle, Arm next, vector<double> coordinate);
        double length;
        double angleFromVert;
        //boost::recursive_wrapper<Arm>;
        //Arm nextArm;
        //Arm firstArm;
        vector<double> location;
        Arm getNext();
        void setNext(Arm toSet);
    private:
        vector<Arm> nextArm;
};
