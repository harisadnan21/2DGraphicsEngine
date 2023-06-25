/**
 *  Copyright Haris Adnan
 */

#include <vector>
#include "include/GMatrix.h"
#include "include/GPoint.h"
#include "include/GRect.h"
#include "include/GPath.h"


    // GPath::GPath(){

    // };

    
    /**
     *  Append a new contour to this path, made up of the 4 points of the specified rect,
     *  in the specified direction.
     *
     *  In either direction the contour must begin at the top-left corner of the rect.
     *
     *  Returns a reference to this path.
     */
    void GPath::addRect(const GRect& rect, Direction dir){
        GPoint pts[4];
    if (dir == kCW_Direction) {
        pts[0] = GPoint{rect.left(), rect.top()};
        pts[1] = GPoint{rect.right(), rect.top()};
        pts[2] = GPoint{rect.right(), rect.bottom()};
        pts[3] = GPoint{rect.left(), rect.bottom()};
    } else {
        pts[0] = GPoint{rect.left(), rect.top()};
        pts[1] = GPoint{rect.left(), rect.bottom()};
        pts[2] = GPoint{rect.right(), rect.bottom()};
        pts[3] = GPoint{rect.right(), rect.top()};
    }
        this->addPolygon(pts, 4);
    };
    /**
     *  Append a new contour to this path with the specified polygon.
     *  Calling this is equivalent to calling moveTo(pts[0]), lineTo(pts[1..count-1]).
     *
     *  Returns a reference to this path.
     */
    void GPath::addPolygon(const GPoint pts[], int count){

        //implemented correctly?

        if (count < 2) {
            return;
        }
        this->moveTo(pts[0]);
        for (int i = 1; i < count; ++i) {
            this->lineTo(pts[i]);
        }

    };

    /**
     *  Return the bounds of all of the control-points in the path.
     *
     *  If there are no points, returns an empty rect (all zeros)
     */
    GRect GPath::bounds() const{
        if (fPts.empty()) {
            return GRect::WH(0, 0);
        }
        //initialise left and right
        float left = fPts[0].x();
        float top = fPts[0].y();
        float right = fPts[0].x();
        float bottom = fPts[0].y();

        for (size_t i = 1; i < fPts.size(); i++) {
            const GPoint& pt = fPts[i];
            if (pt.x() < left) {
                left = pt.x();
            } else if (pt.x() > right) {
                right = pt.x();
            }
            if (pt.y() < top) {
                top = pt.y();
            } else if (pt.y() > bottom) {
                bottom = pt.y();
            }
        }

        return GRect::LTRB(left, top, right,bottom);
    };

    /**
     *  Transform the path in-place by the specified matrix.
     */
    void GPath::transform(const GMatrix& matrix){
        GPoint dst[fPts.size()];
        const GPoint* src = fPts.data();
        int count = fPts.size();
        matrix.mapPoints(dst, src, count);
        for (int i = 0; i < count; ++i) {
            fPts[i] = dst[i];
        }
    

    };
     /**
     *  Append a new contour respecting the Direction. The contour should be an approximate
     *  circle (8 quadratic curves will suffice) with the specified center and radius.
     */
    void GPath::addCircle(GPoint center, float radius, Direction dir){
        float sqrt = 0.707106781186548; 
        float tan = 0.414213562373095; 

        GMatrix tMat = GMatrix::Translate(center.x(), center.y()) * GMatrix::Scale(radius, radius);
        GPoint pt = {1, 0};

        tMat.mapPoints(&pt, 1);
        this->moveTo(pt);

        if (dir == GPath::Direction::kCW_Direction){
            const GPoint start[] = {{1, tan},{tan, 1}, {-tan, 1},{-1, tan}, {-1, -tan},{-tan, -1}, {tan, -1},{1, -tan}};
            const GPoint end[] = {  {sqrt, sqrt}, {0, 1},{-sqrt, sqrt}, {-1, 0},{-sqrt, -sqrt}, {0, -1},{sqrt, -sqrt}, {1, 0}};
            for (int i = 0; i < 8; ++i) {
                GPoint section[] = {start[i], end[i]}; 
                tMat.mapPoints(section, 2); 
                quadTo(section[0], section[1]); 
            }
        }
        else {
            const GPoint start[] = 

            {{1, -tan}, {tan, -1}, {-tan, -1},{-1, -tan}, {-1, tan}, {-tan, 1}, {tan, 1}, {1, tan}};
            const GPoint end[] = {  {sqrt, -sqrt}, {0, -1},{-sqrt, -sqrt}, {-1, 0}, {-sqrt, sqrt}, {0, 1}, {sqrt, sqrt}, {1, 0}};
            for (int i = 0; i < 8; ++i) {
                GPoint section[] = {start[i], end[i]}; 

                tMat.mapPoints(section, 2);
                quadTo(section[0], section[1]); 
            }
        }
    
    }

 /**
     *  Given 0 < t < 1, subdivide the src[] quadratic bezier at t into two new quadratics in dst[]
     *  such that
     *  0...t is stored in dst[0..2]
     *  t...1 is stored in dst[2..4]
     */
    void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t){
         // Compute the split point on the quadratic Bezier curve
        GPoint split = src[0] * (1 - t) * (1 - t) + src[1] * 2 * t * (1 - t) + src[2] * t * t;

        // Compute the new control points for the left quadratic Bezier curve
        GPoint left1 = src[0];
        GPoint left2 = src[0] * (1 - t) + src[1] * t;
        GPoint left3 = split;

        // Compute the new control points for the right quadratic Bezier curve
        GPoint right1 = split;
        GPoint right2 = src[1] * (1 - t) + src[2] * t;
        GPoint right3 = src[2];

        // Store the new quadratic Bezier curves in the output array
        dst[0] = left1;
        dst[1] = left2;
        dst[2] = left3;
        dst[3] = right2;
        dst[4] = right3;

    }

    /**
     *  Given 0 < t < 1, subdivide the src[] cubic bezier at t into two new cubics in dst[]
     *  such that
     *  0...t is stored in dst[0..3]
     *  t...1 is stored in dst[3..6]
     */
    void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t){
        // Compute the split point on the cubic Bezier curve
        GPoint split = src[0] * pow(1 - t, 3) + src[1] * 3 * t * pow(1 - t, 2) + src[2] * 3 * pow(t, 2) * (1 - t) + src[3] * pow(t, 3);

        // Compute the new control points for the left cubic Bezier curve
        GPoint left1 = src[0];
        GPoint left2 = src[0] * (1 - t) + src[1] * t;
        GPoint left3 = src[0] * (1 - t) * (1 - t) + src[1] * 2 * t * (1 - t) + src[2] * pow(t, 2) ;
        GPoint left4 = split;

        // Compute the new control points for the right cubic Bezier curve
        GPoint right1 = split;
        GPoint right2 = src[1] * pow(1 - t, 2) + src[2] * 2 * t * (1 - t) + src[3] * t * t;
        GPoint right3 =  src[2] * (1 - t) + src[3] * t;
        GPoint right4 = src[3];

        // Store the new cubic Bezier curves in the output array
        dst[0] = left1;
        dst[1] = left2;
        dst[2] = left3;
        dst[3] = left4;
        dst[4] = right2;
        dst[5] = right3;
        dst[6] = right4;
    }

