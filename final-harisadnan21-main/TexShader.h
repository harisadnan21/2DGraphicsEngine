#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GMatrix.h"
#include "include/GColor.h"
#include "SimpleShader.h"
#include "Tile.h"
using namespace std;
#include <cmath>

class TexShader : public GShader {
public:
    TexShader(GPoint points[], GPoint texs[], GShader* shader) {
        points_matrix=  GMatrix((points[1].fX - points[0].fX), (points[2].fX - points[0].fX), points[0].fX, (points[1].fY - points[0].fY), (points[2].fY - points[0].fY), points[0].fY);
        tex_matrix = GMatrix((texs[1].fX - texs[0].fX), (texs[2].fX - texs[0].fX), texs[0].fX, (texs[1].fY - texs[0].fY), (texs[2].fY - texs[0].fY), texs[0].fY);
        tex_matrix.invert(&tex_invert);
        
        local_matrix = GMatrix::Concat(points_matrix, tex_invert);
        my_shader = shader;
       
    }

 
    bool isOpaque() override {
        // A bitmap shader is opaque if the bitmap is opaque.
        return false;
    }

    bool setContext(const GMatrix& ctm) override {
        return my_shader-> setContext(GMatrix::Concat(ctm, local_matrix));
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
        my_shader->shadeRow(x,y,count,row);
        
    }

private:

    GMatrix points_matrix;
    GMatrix tex_matrix;
    GMatrix tex_invert;
    GMatrix local_matrix;
    GShader* my_shader;
 
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
std::unique_ptr<GShader> GCreateTexGradient( GPoint points[], GPoint texs[], GShader* shader){
    // is the shader with &   ? 
    return std::unique_ptr<GShader>( new TexShader(points, texs, shader));
    
}
