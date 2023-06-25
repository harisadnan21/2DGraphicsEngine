/**
 *  Copyright Haris Adnan
 */


#include "include/GColor.h"
#include "include/GMath.h"
#include "include/GPoint.h"
#include "include/GRect.h"
#include "include/GMatrix.h"
#include "include/GFinal.h"
#include "RadialShader.h"
#include "BilinearShader.h"

class MyFinal : public GFinal {
public:
        MyFinal() {
        
    }


    /**
     *  Return a radial-gradient shader.
     *
     *  This is a shader defined by a circle with center and a radius.
     *  The array of colors are evenly distributed between the center (color[0]) out to
     *  the radius (color[count-1]). Beyond the radius, it respects the TileMode.
     */
    std::unique_ptr<GShader> createRadialGradient(GPoint center, float radius,
                                                          const GColor colors[], int count,
                                                          GShader::TileMode mode) override {
        return std::unique_ptr<GShader>( new RadialShader( center,  radius,  colors,  count, mode));
        //return nullptr;
    }
/**
     * Return a bitmap shader that performs kClamp tiling with a bilinear filter on each sample, respecting the
     * localMatrix.
     *
     * This is in contrast to the existing GCreateBitmapShader, which performs "nearest neightbor" sampling
     * when it fetches a pixel from the src bitmap.
     */
    std::unique_ptr<GShader> createBilerpShader(const GBitmap& bm ,
                                                        const GMatrix& localMatrix) override {
        return nullptr;

        //return std::unique_ptr<GShader>( new BilinearShader(  bm , localMatrix));
    }
  

  
    void addLine(GPath* path, GPoint p0, GPoint p1, float width, CapType ct)  override {
        //path -> addCircle({2,2}, 20, GPath::kCW_Direction);
        //path -> addPolygon(G)
        GPoint pts_a[4];
        pts_a[0] = {1,2};
        pts_a[1] = {3,4};
        pts_a[2] = {3,5};
        pts_a[3] = {1,6};
        //add p4 before p3
        //make polygon
        //call like canvas->drawConvexPolgon({p1, p2 , p4, p3} , 4, paint);
        
        //path->addPolygon(pts_a, 4);

      

        float dx =  p1.fX - p0.fX;
        float dy = p1.fY - p0.fY;
        //may use std::sqrt
        float len = sqrt((dx * dx) + (dy * dy));
        float rad = width / 2;
        
        float x_1 = (dx * rad) / len;
        float y_1 = (dy * rad) / len;



        GPoint normalised_vector = {dx/len , dy/ len};


        GPoint transverse = {-y_1,x_1};


        // GPoint p1 = {pt_A.fX - y_1, pt_A.fY + x_1};
        // GPoint p2 = {pt_A.fX + y_1, pt_A.fY - x_1};
        // GPoint p3 = {pt_B.fX - y_1, pt_B.fY + x_1};
        // GPoint p4 = {pt_B.fX + y_1, pt_B.fY - x_1};
        GPoint point1 = p0 + transverse;
        GPoint point2 = p0 - transverse;
        GPoint point3 = p1+ transverse;
        GPoint point4 = p1 - transverse;

        GPoint pts[4];
        pts[0] = point1;
        pts[1] = point3;
        pts[2] = point4;
        pts[3] = point2;
        //add p4 before p3
        //make polygon
        //call like canvas->drawConvexPolgon({p1, p2 , p4, p3} , 4, paint);
        
        path->addPolygon(pts, 4);

        //also add on this the cap type:
        if (ct == kSquare){
            GPoint dist_line = normalised_vector * (width/2);
                GPoint squareA_1 = pts[0] - dist_line;
                GPoint squareA_2 = pts[0] + dist_line;
                GPoint squareA_3 = pts[3] + dist_line;
                GPoint squareA_4 = pts[3] - dist_line;


                // GPoint squareA_1 = pts[3] - dist_line;
                // GPoint squareA_2 = pts[3] + dist_line;
                // GPoint squareA_3 = pts[0] + dist_line;
                // GPoint squareA_4 = pts[0] - dist_line;

                // GPoint squareA_1 = pts[3] - dist_line;
                // GPoint squareA_2 = pts[0] + dist_line;
                // GPoint squareA_3 =pts[3] - dist_line;
                // GPoint squareA_4 =  pts[0] + dist_line;

                // GPoint squareA_1 = pts[0] - dist_line;
                // GPoint squareA_2 = pts[3] - dist_line;
                // GPoint squareA_3 = pts[3] + dist_line;
                // GPoint squareA_4 = pts[1] + dist_line;


                GPoint squareB_1 = pts[1] - dist_line;
                GPoint squareB_2 = pts[1] + dist_line;
                GPoint squareB_3 = pts[2] + dist_line;
                GPoint squareB_4 = pts[2] - dist_line;

                  
                GPoint sqA_lst[4];
                sqA_lst[0] = squareA_1;
                sqA_lst[1] = squareA_3;
                sqA_lst[2] = squareA_4;
                sqA_lst[3] = squareA_2;
                
                path->addPolygon(sqA_lst, 4);

                GPoint sqB_lst[4];
                sqB_lst[0] = squareB_1;
                sqB_lst[1] = squareB_3;
                sqB_lst[2] = squareB_4;
                sqB_lst[3] = squareB_2;
                
                path->addPolygon(sqB_lst, 4);
        }
        else if (ct == kRound){
            path->addCircle(p0 , width / 2, GPath::Direction::kCCW_Direction);
            path->addCircle(p1 , width / 2, GPath::Direction::kCCW_Direction);

        }
        else{

        }
        // switch(ct){
        //     case kButt:
        //         break;
        //     case kSquare:
        //         GPoint dist_line = normalised_vector * (width/2);
        //         GPoint squareA_1 = pts[0] - dist_line;
        //         GPoint squareA_2 = pts[0] + dist_line;
        //         GPoint squareA_3 = pts[3] + dist_line;
        //         GPoint squareA_4 = pts[3] - dist_line;

        //         GPoint squareB_1 = pts[1] - dist_line;
        //         GPoint squareB_2 = pts[1] + dist_line;
        //         GPoint squareB_3 = pts[2] + dist_line;
        //         GPoint squareB_4 = pts[2] - dist_line;

                  
        //         GPoint sqA_lst[4];
        //         sqA_lst[0] = squareA_1;
        //         sqA_lst[1] = squareA_3;
        //         sqA_lst[2] = squareA_4;
        //         sqA_lst[3] = squareA_2;
                
        //         path->addPolygon(sqA_lst, 4);

        //         GPoint sqB_lst[4];
        //         sqB_lst[0] = squareB_1;
        //         sqB_lst[1] = squareB_3;
        //         sqB_lst[2] = squareB_4;
        //         sqB_lst[3] = squareB_2;
                
        //         path->addPolygon(sqB_lst, 4);
        //         break;
        //     case (kRound):
        //         path->addCircle(p0 , width / 2, GPath::Direction::kCW_Direction);
        //         path->addCircle(p1 , width / 2, GPath::Direction::kCW_Direction);

        //         break;
            
        // }


        return;

        };

    /*
     *  Draw the corresponding mesh constructed from a quad with each side defined by a
     *  quadratic bezier, evaluating them to produce "level" interior lines (same convention
     *  as drawQuad().
     *
     *  pts[0]    pts[1]    pts[2]
     *  pts[7]              pts[3]
     *  pts[6]    pts[5]    pts[4]
     *
     *  Evaluate values within the mesh using the Coons Patch formulation:
     *
     *  value(u,v) = TB(u,v) + LR(u,v) - Corners(u,v)
     *
     *  Where
     *      TB is computed by first evaluating the Top and Bottom curves at (u), and then
     *      linearly interpolating those points by (v)
     *      LR is computed by first evaluating the Left and Right curves at (v), and then
     *      linearly interpolating those points by (u)
     *      Corners is computed by our standard "drawQuad" evaluation using the 4 corners 0,2,4,6
     */
    void drawQuadraticCoons(GCanvas*, const GPoint pts[8], const GPoint tex[4],
                                    int level, const GPaint&) override {}



};

/**
 *  Implement this to return ain instance of your subclass of GFinal.
 */
std::unique_ptr<GFinal> GCreateFinal(){
    return std::unique_ptr<GFinal>(new MyFinal());
}