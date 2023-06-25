#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GMatrix.h"
#include "include/GColor.h"
#include "SimpleShader.h"
#include "Tile.h"
using namespace std;
#include <cmath>
#include "include/GBitmap.h"

class RadialShader : public GShader {
public:
    RadialShader(GPoint center, float radius,const GColor colors[], int count, GShader::TileMode mode) {
        
        //currently has the same template as color shader 
        
        // fColors = (GColor*)malloc(sizeof(GColor) * (count + 1));
        // memcpy(fColors, colors, count * sizeof(GColor));
        // fColors[count] = colors[count -1];
        //is_opaque = (fColors[0].a == 1) && (fColors[1].a == 1) &&(fColors[2].a == 1);
       fColors = (GColor*)malloc(sizeof(GColor) * (count + 1));
        memcpy(fColors, colors, count * sizeof(GColor));
        fColors[count] = colors[count -1];

         c = center;
         rad = radius;
        //const GColor[] col_arr = colors;
         tm = mode;
         fCount = count;

    }

 
    bool isOpaque() override {
        // A bitmap shader is opaque if the bitmap is opaque.
        return false;
        //return fBitmap.isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        // GMatrix m = {fP1.x() - fP0.x(), 
        //             fP0.y() - fP1.y() , 
        //             fP0.x(), fP1.y() - fP0.y(), 
        //             fP1.x() - fP0.x(), fP0.y()};
        // GMatrix inv_m; 
        // bool val_m = m.invert(&inv_m);


        // GMatrix ctm_inverse; 
        // bool val_ctm = ctm.invert(&ctm_inverse);
        
        // if (!(val_ctm && val_m)) {
        //     return false;
        // }
        // fCTM = GMatrix::Concat(inv_m, ctm_inverse);
        // //we multiply with count here?
        // //fCTM = GMatrix::Scale(fCount - 1, fCount -1) * fCTM;
        // return true;


    //GPoint transformedCenter = ctm.mapPt(fCenter);
    GPoint transformedCenter = ctm.operator*(c);
    float transformedRadius = 2;
   //float transformedRadius = ctm.mapRadius(fRadius);

    GMatrix shaderMatrix;
    shaderMatrix.Scale(1 / transformedRadius, 1 / transformedRadius);
    shaderMatrix.Translate(-transformedCenter.fX, -transformedCenter.fY);

    fCTM = GMatrix::Concat(shaderMatrix, ctm);

    return true;

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
        switch (tm){
            case(kClamp):
                for (int i = 0; i < count; ++i) {
                    GPoint pt = {x + 0.5f + i, y + 0.5f};
                    GPoint ptout = fCTM * pt;
                    float xval = ptout.x();
                    xval = xval * (fCount -1);
                    if (xval <= 0){
                        GColor color = fColors[0];
                        GPixel src = retSRCPixel(color);
                        row[i] = src;
                    }
                    else if (xval>= fCount -1){
                        GColor color = fColors[fCount -1];
                        GPixel src = retSRCPixel(color);
                        row[i] = src;
                    }
                    else{
                        int index = GFloorToInt(xval);
                        float frac = xval - index;
                        GColor color1 = fColors[index];
                        GColor color2 = fColors[index + 1];
                        GColor color = (color1 * (1 - frac )) + (color2*(frac));
                        GPixel src = retSRCPixel(color);
                        row[i] = src;
                    }
                }
                break;
            case(kRepeat):
            //double check
                for (int i = 0; i < count; ++i) {
                    GPoint pt = {x + 0.5f + i, y + 0.5f};
                    GPoint ptout = fCTM * pt;
                    float xval = ptout.x();

                    xval = xval - GFloorToInt(xval); 
                    xval = xval * (fCount - 1);
                    int index = GFloorToInt(xval);
                    float frac = xval - index;
                    GColor color1 = fColors[index];
                    GColor color2 = fColors[(index + 1)]; // wrap around
                    GColor color = (color1 * (1 - frac )) + (color2*(frac));
                    GPixel src = retSRCPixel(color);
                    row[i] = src;
                }
                break;
            case(kMirror):
                //double check
                for (int i = 0; i < count; ++i) {
                    GPoint pt = {x + 0.5f + i, y + 0.5f};
                    GPoint ptout = fCTM * pt;
                    float xval = ptout.x();
                    xval = xval * 0.5;
            
                    xval = xval - GFloorToInt(xval); // mirror
                    if (xval > 0.5){
                        xval = 1 - xval;
                    }
                    xval = xval * (fCount - 1) * 2;
                    int index = GFloorToInt(xval);
                    float frac = xval - index;
                    GColor color1 = fColors[index];
                    GColor color2 = fColors[(index + 1) ]; // wrap around
                    GColor color = (color1 * (1 - frac )) + (color2*(frac));
                    GPixel src = retSRCPixel(color);
                    row[i] = src;
                }
                break;
        }
    }


private:
    GPoint c ;
    float rad ;
    TileMode tm ;
    int fCount;
    GColor* fColors;
    GMatrix fCTM;

    //GColor* fColors;
    //int fCount;
    
};
