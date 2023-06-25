

#ifndef SimpleShader_DEFINED
#define SimpleShader_DEFINED
#include "include/GPixel.h"
#include "include/GShader.h"


class SimpleShader : public GShader {
    const GPixel fPixel;
    
public:
    GPixel getSrcPixel2(GColor color){
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
    SimpleShader (const GColor& c) : fPixel(getSrcPixel2(c)) {}

    bool isOpaque() override { 
        return GPixel_GetA(fPixel) == 0xFF;
    }

    bool setContext(const GMatrix&) override {
         return true;
    }
    void shadeRow(int, int, int count, GPixel row[]) override {

        for (int i = 0; i < count; ++i) {
            row[i] = fPixel;
            
        }
    }
};

#endif