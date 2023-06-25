#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GMatrix.h"
#include "include/GColor.h"
#include "SimpleShader.h"
#include "Tile.h"
using namespace std;
#include <cmath>
#include "include/GBitmap.h"

class BilinearShader : public GShader {
public:
    BilinearShader(const GBitmap& bm , const GMatrix& localMatrix) {
        
        //currently has the same template as color shader 
        
        // fColors = (GColor*)malloc(sizeof(GColor) * (count + 1));
        // memcpy(fColors, colors, count * sizeof(GColor));
        // fColors[count] = colors[count -1];
        //is_opaque = (fColors[0].a == 1) && (fColors[1].a == 1) &&(fColors[2].a == 1);
        fBitmap = bm;
        loc_mat = localMatrix;
        transformation_matrix = GMatrix();

    }

 
    bool isOpaque() override {
        // A bitmap shader is opaque if the bitmap is opaque.
        return is_opaque;
        //return fBitmap.isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        bool val = GMatrix::Concat(ctm, loc_mat).invert(&transformation_matrix);
        return val;
    }
    GPixel retSRCPixel(GColor color){
            float mulr =  color.r * color.a;
            float mulg =  color.g * color.a;
            float mulb =  color.b * color.a;
            int hexValr = GRoundToInt(mulr * 255);
            int hexValg = GRoundToInt(mulg * 255);
            int hexValb = GRoundToInt(mulb * 255);
            int hexVala = GRoundToInt(color.a * 255);
            GPixel src = GPixel_PackARGB(hexVala, hexValr, hexValg, hexValb);
            return src;
    }
    void shadeRow(int x, int y, int count, GPixel row[]) override {
        GPoint pt = {x , y };
        GPoint ptout = transformation_matrix * pt; 
         // the central point
        GPoint newpt = {ptout.fX + 0.5f, ptout.fY +0.5};
        GPoint C = {GFloorToInt(newpt.fX), GFloorToInt(newpt.fY)};
        GPoint A= {C.fX -1, C.fY -1};
        GPoint B = {C.fX, C.fY - 1};
        GPoint D = {C.fX -1, C.fY};
        float u= newpt.fX - GFloorToInt(newpt.fX);
        float v= newpt.fY - GFloorToInt(newpt.fY);

        

        // float nX = ptout.fX;
        // float nY = ptout.fY;
        // GColor DC1 = fColors[1] - fColors[0];
        // GColor DC2 = fColors[2] - fColors[0];
        // GColor C;
        // C = nX * DC1 + nY * DC2 + fColors[0];
        // GColor DC = transformation_matrix[0] * DC1 + transformation_matrix[3] * DC2;

        
        for (int i = 0 ; i < count ; ++i){
            // the central point
        GPoint newpt = {ptout.fX + 0.5f + i, ptout.fY +0.5};

        //ABCD are colors
        


        GPoint C = {GFloorToInt(newpt.fX), GFloorToInt(newpt.fY)};
        GPoint A= {C.fX -1, C.fY -1};
        GPoint B = {C.fX, C.fY - 1};
        GPoint D = {C.fX -1, C.fY};


        // GPixel d = *fBitmap.getAddr(colIdx, rowIdx);
        // GPixel c = *fBitmap.getAddr(colIdx- 1 , rowIdx);
        // GPixel b = *fBitmap.getAddr(colIdx , rowIdx-1);
        // GPixel a = *fBitmap.getAddr(colIdx-1, rowIdx-1);

    //     GPixel d = *fBitmap.getAddr(colIdx, rowIdx);
    //     GPixel c = *fBitmap.getAddr(clampInt(colIdx- 1 , fBitmap.width()), rowIdx);
    // GPixel b = *fBitmap.getAddr(colIdx , clampInt(rowIdx-1, fBitmap.height()));
    // GPixel a = *fBitmap.getAddr(clampInt(colIdx- 1 , fBitmap.width()), clampInt(rowIdx-1, fBitmap.height()));

        GPixel A_Pixel = *fBitmap.getAddr((int)A.fX, (int)A.fY);
        GPixel B_Pixel = *fBitmap.getAddr((int)B.fX,(int) B.fY);
        GPixel C_Pixel = *fBitmap.getAddr((int)C.fX, (int)C.fY);
        GPixel D_Pixel = *fBitmap.getAddr((int)D.fX, (int)D.fY);



        float u= newpt.fX - GFloorToInt(newpt.fX);
        float v= newpt.fY - GFloorToInt(newpt.fY);
            //change types



            // GColor my_color = ((1 - u) * (1 - v) * A_) + 
            //         (u * (1 - v) * B_) +
            //         ((1 - u) * v * C) + 
            //         (u * v * D);
            // row[i] = retSRCPixel(my_color);
          
            // GColor my_color = nX * DC1 + nY * DC2 + fColors[0];
            // //GColor my_color= fColors[1] * (nX) + fColors[2] * nY + fColors[0] * (1 - nX - nY);
            // GPixel ret_pixel = retSRCPixel(my_color);
            // row[i] = ret_pixel;
            // nX += transformation_matrix[0];
            // nY+= transformation_matrix[3];

        }
    }

//     int clampInt(int num, int length){
//     if (num < 0){
//         num = 0;
//     }
//     if (num >= length){
//         num = length-1;
//     }
//    return num;
// }

private:

    GBitmap fBitmap;
    bool is_opaque;
    GMatrix loc_mat;
    GMatrix transformation_matrix;
    //GColor* fColors;
    //int fCount;
    
};
