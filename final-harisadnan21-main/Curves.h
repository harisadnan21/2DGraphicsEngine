#ifndef Curves_DEFINED
#define Curves_DEFINED

#include "include/GPixel.h"
#include "include/GPoint.h"
#include "include/GRect.h"

static int numSegmentQuad(GPoint a, GPoint b, GPoint c){
    GPoint E = 0.25*a - 0.5 *b + 0.25*c;
    float tol = 0.25;
    int num_segs = (int)ceil(sqrt(E.length()/tol));
    return num_segs;

};

static int numSegmentCubic(GPoint a, GPoint b, GPoint c, GPoint d){
    GPoint E0 = a + 2*b + c;
    GPoint E1 = b + 2*c + d;
    GPoint E = {max(abs(E0.x()), abs(E1.x())), max(abs(E0.y()), abs(E1.y()))};
    float tol = 0.25;
    int num_segs = (int)ceil(sqrt((3*E.length())/(4*tol)));
    return num_segs;
}

//Are these two fucntios correct?
static GPoint evalQuad(GPoint a, GPoint b, GPoint c, float t ){
    
    return a + t*(2.0f*(b - a) + t * ( a - 2.0f* b + c));
};

static GPoint evalCubic(GPoint a, GPoint b, GPoint c, GPoint d, float t){
    float oneMinusT = 1.0f - t;
    // return a * oneMinusT * oneMinusT * oneMinusT
    //      + b * 3.0f * t * oneMinusT * oneMinusT
    //      + c * 3.0f * t * t * oneMinusT
    //      + d * t * t * t;
    return a + t*(3.0f*(b-a) + t*(3.0f*(a-2.0f*b+c) + t*(3.0f*(b-c) - a +d)));
};
#endif