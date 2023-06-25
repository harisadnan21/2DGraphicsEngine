#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GMatrix.h"
#include "include/GColor.h"
#include "SimpleShader.h"
#include "Tile.h"
using namespace std;
#include <cmath>

class ColorTexShader : public GShader {
public:
    ColorTexShader(GShader* cshader, GShader* tshader) {
        color_shader = cshader;
        tex_shader= tshader;
        
    }

 
    bool isOpaque() override {
        // A bitmap shader is opaque if the bitmap is opaque.
        return false;
        //return fBitmap.isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        return (color_shader -> setContext(ctm) && tex_shader -> setContext(ctm));
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
        //how to get the pixels from shaderow??
        //initialise an array
        GPixel color_row[count];
        GPixel tex_row[count];
        color_shader -> shadeRow(x,y, count, color_row);
        tex_shader->shadeRow(x,y,count,tex_row);
        for (int i = 0; i < count; ++i) {
            int a = GPixel_GetA(color_row[i]) * GPixel_GetA(tex_row[i]);
            int r = GPixel_GetR(color_row[i]) * GPixel_GetR(tex_row[i]);
            int g = GPixel_GetG(color_row[i]) * GPixel_GetG(tex_row[i]);
            int b = GPixel_GetB(color_row[i]) * GPixel_GetB(tex_row[i]);

            row[i] = GPixel_PackARGB(GDiv255(a), GDiv255(r), GDiv255(g), GDiv255(b));

        }
    }

private:
    GShader* color_shader;
    GShader* tex_shader;
    GPixel colorPixels;
    GPixel texPixels;
};


std::unique_ptr<GShader> GCreateColorTexGradient(GShader* colorshader, GShader* texshader){
    
    // is the shader with &   ? 
    return std::unique_ptr<GShader>( new ColorTexShader(colorshader, texshader));
    
}
