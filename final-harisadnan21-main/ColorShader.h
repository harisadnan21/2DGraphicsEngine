#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GMatrix.h"
#include "include/GColor.h"
#include "SimpleShader.h"
#include "Tile.h"
using namespace std;
#include <cmath>

class ColorShader : public GShader {
public:
    ColorShader(const GColor* colors, GPoint points[], int count) {
        
        //not points but save colors
        // Color0 = points[0];
        // Color1= points[1];
        // Color2= points[2];
        fColors = (GColor*)malloc(sizeof(GColor) * (count + 1));
        memcpy(fColors, colors, count * sizeof(GColor));
        fColors[count] = colors[count -1];
        is_opaque = (fColors[0].a == 1) && (fColors[1].a == 1) &&(fColors[2].a == 1);

        loc_mat = GMatrix((points[1].fX - points[0].fX), (points[2].fX -points[0].fX), (points[0].fX),
        (points[1].fY - points[0].fY), (points[2].fY - points[0].fY), points[0].fY);
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
        GPoint pt = {x + 0.5f, y + 0.5f};
        GPoint ptout = transformation_matrix * pt;
        float nX = ptout.fX;
        float nY = ptout.fY;
        GColor DC1 = fColors[1] - fColors[0];
        GColor DC2 = fColors[2] - fColors[0];
        GColor C;
        C = nX * DC1 + nY * DC2 + fColors[0];
        GColor DC = transformation_matrix[0] * DC1 + transformation_matrix[3] * DC2;

        for (int i = 0 ; i < count ; ++i){
            row[i] = retSRCPixel(C);
            C += DC;


            // GColor my_color = nX * DC1 + nY * DC2 + fColors[0];
            // //GColor my_color= fColors[1] * (nX) + fColors[2] * nY + fColors[0] * (1 - nX - nY);
            // GPixel ret_pixel = retSRCPixel(my_color);
            // row[i] = ret_pixel;
            // nX += transformation_matrix[0];
            // nY+= transformation_matrix[3];

        }
    }

private:


    bool is_opaque;
    GMatrix loc_mat;
    GMatrix transformation_matrix;
    GColor* fColors;
    int fCount;
    
};
