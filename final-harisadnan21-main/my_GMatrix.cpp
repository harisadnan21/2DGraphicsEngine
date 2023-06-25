/**
 *  Copyright Haris Adnan
 */


#include "include/GColor.h"
#include "include/GMath.h"
#include "include/GPoint.h"
#include "include/GRect.h"
#include "include/GMatrix.h"

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // These methods must be implemented by the student.

    GMatrix::GMatrix(){
        fMat[0] = 1;    fMat[1] = 0;    fMat[2] = 0;
        fMat[3] = 0;    fMat[4] = 1;    fMat[5] = 0;
    };  // initialize to identity
    //translate by tx and tx. 
    // make a matrix 3 * 3 such that when multipled by [x returns a 3*1 matrix that is correctly transformed
    //                                                  y
    //                                                  1]
    GMatrix GMatrix::Translate(float tx, float ty){
        return GMatrix(1 , 0, tx, 0, 1, ty);

    };
    GMatrix GMatrix::Scale(float sx, float sy){
        return GMatrix(sx, 0, 0 , 0, sy, 0);
    };
    GMatrix GMatrix::Rotate(float radians){
        float cosR = cos(radians);
        float sinR = sin(radians);
        return GMatrix(cosR, -1 *sinR, 0, sinR,cosR, 0);
    };


    /**
     *  Return the product of two matrices: a * b
     */
    GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b){
        float f0 = (a.fMat[0] * b.fMat[0]) + (a.fMat[1] * b.fMat[3]);
        float f1 = (a.fMat[0] * b.fMat[1]) + (a.fMat[1] * b.fMat[4]);
        float f2 = (a.fMat[0] * b.fMat[2]) + (a.fMat[1] * b.fMat[5]) + a.fMat[2];

        float f3 = (a.fMat[3] * b.fMat[0]) + (a.fMat[4] * b.fMat[3]);
        float f4 = (a.fMat[3] * b.fMat[1]) + (a.fMat[4] * b.fMat[4]);
        float f5 = (a.fMat[3] * b.fMat[2]) + (a.fMat[4] * b.fMat[5]) + a.fMat[5];

        return GMatrix(f0, f1, f2, f3, f4, f5);
        
    };


    /*
     *  Compute the inverse of this matrix, and store it in the "inverse" parameter, being
     *  careful to handle the case where 'inverse' might alias this matrix.
     *
     *  If this matrix is invertible, return true. If not, return false, and ignore the
     *  'inverse' parameter.
     */

    bool GMatrix::invert(GMatrix* inverse) const {
        float a = fMat[0], b = fMat[1], c = fMat[2];
        float d = fMat[3], e = fMat[4], f = fMat[5];
        float det = a * e - b * d;
        if (det == 0) {
            return false;
        }
        float inv_det = 1.0f / det;
        inverse->fMat[0] =  e * inv_det;
        inverse->fMat[1] = -b * inv_det;
        inverse->fMat[2] = (b * f - c * e) * inv_det;
        inverse->fMat[3] = -d * inv_det;
        inverse->fMat[4] =  a * inv_det;
        inverse->fMat[5] = (c * d - a * f) * inv_det;
        return true;
    }

    /**
     *  Transform the set of points in src, storing the resulting points in dst, by applying this
     *  matrix. It is the caller's responsibility to allocate dst to be at least as large as src.
     *
     *  [ a  b  c ] [ x ]     x' = ax + by + c
     *  [ d  e  f ] [ y ]     y' = dx + ey + f
     *  [ 0  0  1 ] [ 1 ]
     *
     *  Note: It is legal for src and dst to point to the same memory (however, they may not
     *  partially overlap). Thus the following is supported.
     *
     *  GPoint pts[] = { ... };
     *  matrix.mapPoints(pts, pts, count);
     */
    void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const{
        for (int i = 0; i < count; i++) {
            float x = src[i].x();
            float y = src[i].y();
            float x_new = fMat[0] * x + fMat[1] * y + fMat[2];
            float y_new = fMat[3] * x + fMat[4] * y + fMat[5];
            dst[i] = {x_new, y_new};
        }

    };




