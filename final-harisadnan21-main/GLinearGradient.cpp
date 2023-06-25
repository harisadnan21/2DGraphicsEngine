#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GMatrix.h"
#include "include/GColor.h"
#include "SimpleShader.h"
#include "Tile.h"
using namespace std;
#include <cmath>

class GLinearGradient : public GShader {
public:
    GLinearGradient(GPoint p0, GPoint p1, const GColor* colors, int count, TileMode tilemode) {
        fColors = (GColor*)malloc(sizeof(GColor) * (count + 1));
        memcpy(fColors, colors, count * sizeof(GColor));
        fColors[count] = colors[count -1];

        fP0 = p0;
        fP1 = p1;
        fCount = count;
        fCTM = GMatrix();
        tm = tilemode;
    }

 
    bool isOpaque() override {
        // A bitmap shader is opaque if the bitmap is opaque.
        return false;
        //return fBitmap.isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        GMatrix m = {fP1.x() - fP0.x(), 
        fP0.y() - fP1.y() , 
        fP0.x(), fP1.y() - fP0.y(), 
        fP1.x() - fP0.x(), fP0.y()};
        GMatrix inv_m; 
        bool val_m = m.invert(&inv_m);


        GMatrix ctm_inverse; 
        bool val_ctm = ctm.invert(&ctm_inverse);
        
        if (!(val_ctm && val_m)) {
            return false;
        }
        fCTM = GMatrix::Concat(inv_m, ctm_inverse);
        //we multiply with count here?
        //fCTM = GMatrix::Scale(fCount - 1, fCount -1) * fCTM;
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
    GPoint fP0;
    GPoint fP1;
    GColor* fColors;
    GMatrix fCTM;
    int fCount;
    TileMode tm;
};

/**
 *  Return a subclass of GShader that draws the specified gradient of [count] colors between
 *  the two points. Color[0] corresponds to p0, and Color[count-1] corresponds to p1, and all
 *  intermediate colors are evenly spaced between.
 *
 *  The gradient colors are GColors, and therefore unpremul. The output colors (in shadeRow)
 *  are GPixel, and therefore premul. The gradient has to interpolate between pairs of GColors
 *  before "pre" multiplying them into GPixels.
 *
 *  If count < 1, this should return nullptr.
 */
std::unique_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor* colorlst, 
    int count, GShader::TileMode tm){
    if (count < 1){
        return nullptr;
    }
    if (count == 1){
        return std::unique_ptr<GShader>( new SimpleShader(colorlst[0]));
    }
    return std::unique_ptr<GShader>( new GLinearGradient(p0 , p1, colorlst, count, tm));
    
}
