#include <math.h>
#include <cmath>
#ifndef _vector
#define _vector
#include <vector>
#endif

#include <iostream>
#include "inversek_util.h"
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

using namespace std;
const double PI_rad = 3.141592653589793/180;
const double nearzero = 0.00000000000000000000000000001;


Vector4::Vector4() {
    /*
     * Constructs the 0 Vector4.
     */
    x = 0;
    y = 0;
    z = 0;
    w = 0;
}

Vector4::Vector4(double px, double py, double pz, double pw) {
    /*
     * Constructs new Vector4 with specified components.
     */
    x = px;
    y = py;
    z = pz;
    w = pw;
}

double Vector4::xc() {
    /*
     * Returns x component of this Vector4.
     */
    return x;
}

double Vector4::yc() {
    /*
     * Returns y component of this Vector4.
     */
    return y;
}

double Vector4::zc() {
    /*
     * Returns z component of this Vector4.
     */
    return z;
}

double Vector4::wc() {
    /*
     * Returns w component of this Vector4.
     */
    return w;
}

double Vector4::dot4(Vector4 v) {
    /*
     * Returns the dot product of this Vector4 and v.
     */
    return x*v.xc() + y*v.yc() + z*v.zc() + w*v.wc();
}

void Vector4::unit() {
    if (w == 1) {
        x = x/w;
        y = y/w;
        z = z/w;
        w = 1.0;
        return;
    } else {
        double length = pow(x*x + y*y + z*z, .5);
        x = x/length;
        y = y/length;
        z = z/length;
        return;
    }
}

Vector4 Vector4::sub(Vector4 v) {
    Vector4 newV = Vector4(x-v.xc(), y-v.yc(), z-v.zc(), 0);
    return newV;
}

Vector4 Vector4::cross(Vector4 v) {
    return Vector4(y*v.zc()-z*v.yc(), -(x*v.zc()-z*v.xc()), x*v.yc()-y*v.xc(), 0);
}



Arm::Arm(){
    length = 0;
    angleFromVert = 0;
}

Arm::Arm(double l, double angle, Arm next){
    length = l;
    angleFromVert = angle;
    nextArm.push_back(next);
}

Arm::Arm(double l, double angle, Arm next, vector<double> coordinate){
    length = l;
    angleFromVert = angle;
    nextArm.push_back(next);
    location = coordinate;
}
Arm Arm::getNext(){
    return nextArm[0];
}
void Arm::setNext(Arm toSet){
    nextArm[0] = toSet;
}




Bezier::Bezier(double patch[][3]){
    /*
     * argument in the form of one = {{x,y,z},{x,y,z},{x,y,z},{x,y,z}};
     */
    // X COMPONENT
    cx.push_back(patch[0][0]);
    cx.push_back(-3*patch[0][0] + 3*patch[1][0]);
    cx.push_back(3*patch[0][0] -6*patch[1][0] + 3*patch[2][0]);
    cx.push_back(-patch[0][0] + 3*patch[1][0] -3*patch[2][0] + patch[3][0]);
    vector<double> x_comp;
    x_comp.push_back(patch[0][0]);
    x_comp.push_back(patch[0][1]);
    x_comp.push_back(patch[0][2]);

    // Y COMPONENT
    cy.push_back(patch[0][1]);
    cy.push_back(-3*patch[0][1] + 3*patch[1][1]);
    cy.push_back(3*patch[0][1] -6*patch[1][1] + 3*patch[2][1]);
    cy.push_back(-patch[0][1] + 3*patch[1][1] -3*patch[2][1] + patch[3][1]);
    vector<double> y_comp;
    y_comp.push_back(patch[1][0]);
    y_comp.push_back(patch[1][1]);
    y_comp.push_back(patch[1][2]);


    // Z COMPONENT
    cz.push_back(patch[0][2]);
    cz.push_back(-3*patch[0][2] + 3*patch[1][2]);
    cz.push_back(3*patch[0][2] -6*patch[1][2] + 3*patch[2][2]);
    cz.push_back(-patch[0][2] + 3*patch[1][2] -3*patch[2][2] + patch[3][2]);
    vector<double> z_comp;
    z_comp.push_back(patch[2][0]);
    z_comp.push_back(patch[2][1]);
    z_comp.push_back(patch[2][2]);

    vector<double> fourth_comp;
    fourth_comp.push_back(patch[3][0]);
    fourth_comp.push_back(patch[3][1]);
    fourth_comp.push_back(patch[3][2]);



    patch_store.push_back(x_comp);
    patch_store.push_back(y_comp);
    patch_store.push_back(z_comp);
    patch_store.push_back(fourth_comp);



}

vector<double> Bezier::getPoint(double u){
    /*
     * returns {x,y,z} from the bezier formula with degree up to u^3
     */
    double x = cx[0] + cx[1]*u + cx[2]*pow(u, 2) + cx[3]*pow(u, 3);
    double y = cy[0] + cy[1]*u + cy[2]*pow(u, 2) + cy[3]*pow(u, 3);
    double z = cz[0] + cz[1]*u + cz[2]*pow(u, 2) + cz[3]*pow(u, 3);
    vector<double> rv;
    rv.push_back(x);
    rv.push_back(y);
    rv.push_back(z);
    return rv;
}

