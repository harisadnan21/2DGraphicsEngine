#include "include/GShader.h"
#include "include/GPoint.h"
#include "include/GMatrix.h"
#include "include/GBitmap.h"
#include "Tile.h"
using namespace std;


class GBitmapShader : public GShader {
public:
    GBitmapShader(const GBitmap& bitmap, const GMatrix& localInverse, TileMode tilemode) 
        : fBitmap(bitmap), fLocalInverse(localInverse) , tm(tilemode){}

    bool isOpaque() override {
        // A bitmap shader is opaque if the bitmap is opaque.
        return fBitmap.isOpaque();
    }

    bool setContext(const GMatrix& ctm) override {
        // Calculate the inverse of the CTM and multiply it by the local inverse to get the total inverse matrix.
        GMatrix inverse; 
        bool val = ctm.invert(&inverse);
        
        if (!val) {
            return false;
        }
        fTotalInverse = GMatrix::Concat(fLocalInverse, inverse);
        return true;
    }

    void clamp(int &row, int &col, int width, int height){
        //write
        if (row < 0){
            row = 0;

        }
        if (row >= height){
            row = height-1;
        }
        if (col < 0){
            col = 0;
        }
        if(col >= width){
            col = width-1;
        }

}
// void repeatFunc(int &row, int &col, int width, int height){
        
//     }

    void shadeRow(int x, int y, int count, GPixel row[]) override {
        
        switch (tm){
            case(kClamp):
            {
                GPoint pt = {x + 0.5f, y + 0.5f};
            GPoint ptout = fTotalInverse * pt;
            // Calculate the corresponding row and column indices in the bitmap.
            int rowIdx = (int)ptout.fY;
            int colIdx = (int)ptout.fX;
            for (int i = 0; i < count; ++i) {
                GPoint pt = {x + 0.5f + i, y + 0.5f};
                GPoint ptout = fTotalInverse * pt;
            // Calculate the corresponding row and column indices in the bitmap.
                int rowIdx = (int)ptout.fY;
                int colIdx = (int)ptout.fX;
                clamp(rowIdx,colIdx, fBitmap.width(), fBitmap.height());
                // Get the pixel at the current row and column index and store it in the row[] array.
                row[i] = *fBitmap.getAddr(colIdx, rowIdx);
                colIdx++;
            }
            break;


            }
            
            case (kRepeat):
            {
                for (int i = 0; i < count; ++i) {
                    GPoint pt = {x + 0.5f + i, y + 0.5f};
                    GPoint ptout = fTotalInverse * pt;
                    float ptx = ptout.fX;
                    float pty = ptout.fY;
                    float xval = ptx / fBitmap.width();
                    float yval = pty / fBitmap.height();
                    float tiled_x = tile_repeat(xval);
                    float tiled_y = tile_repeat(yval);
                    row[i] = *fBitmap.getAddr((int)(tiled_x * fBitmap.width()),(int)(tiled_y * fBitmap.height()));

                    //mod

                    //treat bitmap like it is gradient
                    // divide x and y number with width and height
                    // Then you can do floating pt repeat, tile like gradient
                    // multiply by width, cast it to int, get addr
                

            }
            break;

            }

            case(kMirror):
            {
                for (int i = 0; i < count; ++i) {
                    GPoint pt = {x + 0.5f + i, y + 0.5f};
                    GPoint ptout = fTotalInverse * pt;
                    float ptx = ptout.fX;
                    float pty = ptout.fY;
                    float xval = ptx / fBitmap.width();
                    float yval = pty / fBitmap.height();
                    float tiled_x = tile_mirror(xval);
                    float tiled_y = tile_mirror(yval);
                    row[i] = *fBitmap.getAddr((int)(tiled_x * fBitmap.width()),(int)(tiled_y * fBitmap.height()));
            }
            break;
        }
    }
}
private:
    const GBitmap fBitmap;
    GMatrix fLocalInverse;
    GMatrix fInverse;
    GMatrix fTotalInverse;
    TileMode tm;
};

std::unique_ptr<GShader> GCreateBitmapShader(const GBitmap& bitmap, const GMatrix& localInverse, GShader::TileMode tm) {
    if (bitmap.width() <= 0 || bitmap.height() <= 0) {
        return nullptr;
    }
    //where do we get the localInverse from
    return std::unique_ptr<GShader>( new GBitmapShader(bitmap, localInverse, tm));
}
