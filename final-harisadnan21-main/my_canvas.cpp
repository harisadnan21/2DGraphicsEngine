/*
 *  Copyright 2023 <Haris Adnan>
 */


using namespace std;
#include <stack>
#include <stdexcept>
#include "include/GCanvas.h"
#include "include/GRect.h"
#include "include/GColor.h"
#include "include/GBitmap.h"
#include "include/GPaint.h"
#include "include/GBlendMode.h"
#include "include/GPixel.h"
#include "BlendModes.h"
#include "include/GShader.h"
#include "Edge.h"
#include "include/GPath.h"
#include "Curves.h"
#include "ColorTexShader.h"
#include "TexShader.h"
#include "ColorShader.h"


// typedef GPixel (*BLENDFUNC)(const GPixel, const GPixel)

class MyCanvas : public GCanvas {
public:
    MyCanvas(const GBitmap& device) : fDevice(device) {
        ctm = GMatrix();
        //stack<GMatrix> matrixStack;

        //initialise the stack?
        matrixStack.push(ctm);
    }

    /**
     *  Save off a copy of the canvas state (CTM), to be later used if the balancing call to
     *  restore() is made. Calls to save/restore can be nested:
     *  save();
     *      save();
     *          concat(...);    // this modifies the CTM
     *          .. draw         // these are drawn with the modified CTM
     *      restore();          // now the CTM is as it was when the 2nd save() call was made
     *      ..
     *  restore();              // now the CTM is as it was when the 1st save() call was made
     */
    void save() override{
    // Get a reference to the top matrix on the stack
        assert(!this->matrixStack.empty());
        GMatrix topMatrix = this->matrixStack.top();
  
  // Create a copy of the top matrix and add it to the top of the stack
        matrixStack.push(topMatrix);
}


    /**
     *  Copy the canvas state (CTM) that was record in the correspnding call to save() back into
     *  the canvas. It is an error to call restore() if there has been no previous call to save().
     */
    void restore() override{
    // Check if there are any matrices on the stack
        if (matrixStack.empty()) {
            // Throw an error if there are no matrices on the stack
            throw std::runtime_error("Error: Cannot call restore() without a previous call to save().");
        }
        
        // Pop the top matrix from the stack
        GMatrix topMatrix = matrixStack.top();
        matrixStack.pop();
        
        // Assign the popped matrix to the ctm matrix
        ctm = topMatrix;
    }

    /**
     *  Modifies the CTM by preconcatenating the specified matrix with the CTM. The canvas
     *  is constructed with an identity CTM.
     *
     *  CTM' = CTM * matrix
     */
    void concat(const GMatrix& matrix) override {
  // Pre-concatenate the specified matrix with the current CTM
        matrixStack.top() = GMatrix::Concat(matrixStack.top(), matrix);
    }

 //typedef GPixel (*BLENDFUNC)(const GPixel, const GPixel)

    static inline GPixel colorWithBlendMode(GPixel src, GPixel dest, GBlendMode blendmode){


        switch (blendmode){
            case GBlendMode::kClear:
                return kClearFunc(src, dest);;
                break;
            case GBlendMode::kSrc:
                return kSrcFunc(src, dest);
                break;
            case GBlendMode::kDst:
                return kDstFunc(src, dest);
                break;
            case GBlendMode::kSrcOver:
                return kSrcOverFunc(src, dest);
                break;
            case GBlendMode::kDstOver:
                return kDstOverFunc(src, dest);
                break;
            case GBlendMode::kSrcIn:
                return kSrcInFunc(src, dest);
                break;
            case GBlendMode::kDstIn:
                return kDstInFunc(src, dest);
                break;
            case GBlendMode::kSrcOut:
                return kSrcOutFunc(src, dest);
                break;
            case GBlendMode::kDstOut:
                return kDstOutFunc(src, dest);
                break;
            case GBlendMode::kSrcATop:
                return kSrcATopFunc(src, dest);
                break;
            case GBlendMode::kDstATop:
                return kDstATopFunc(src, dest);
                break;
            case GBlendMode::kXor:
                return kXorFunc(src, dest);
                break;
            
        }
    }
    //call function before for loop
    GBlendMode optimizedBlendMode(GBlendMode bm, int srca){
        if (bm == GBlendMode::kSrcOver && srca == 255) {
            return GBlendMode::kSrc;

        }
        if (bm == GBlendMode::kDstIn && srca == 0 ){
            return GBlendMode::kClear;
        }
        if(bm == GBlendMode::kDstOut && srca == 0){
            return GBlendMode::kDst;
        
        }
        if(bm == GBlendMode::kDstOut && srca == 255){
            return GBlendMode::kClear;
        }
        if(bm == GBlendMode::kSrcATop && srca == 255){
            return GBlendMode::kSrcIn;
        }
        if(bm == GBlendMode::kDstATop && srca == 0){
            return GBlendMode::kSrcOut;
        }
        if(bm == GBlendMode::kXor && srca == 255){
            return GBlendMode::kSrcOut;
        }

        //ADD more
        return bm;

    }
    typedef GPixel (*BlendFunc)(const GPixel, const GPixel);

    BlendFunc getBlendFunc(const GBlendMode blendmode) {
    switch (blendmode) {
        case GBlendMode::kClear:
            return kClearFunc;
        case GBlendMode::kSrc:
            return kSrcFunc;
        case GBlendMode::kDst:
            return kDstFunc;
        case GBlendMode::kSrcOver:
            return kSrcOverFunc;
        case GBlendMode::kDstOver:
            return kDstOverFunc;
        case GBlendMode::kSrcIn:
            return kSrcInFunc;

        case GBlendMode::kDstIn:
            return kDstInFunc;

        case GBlendMode::kSrcOut:
            return kSrcOutFunc;
 
        case GBlendMode::kDstOut:
            return kDstOutFunc;

        case GBlendMode::kSrcATop:
            return kSrcATopFunc;
    
        case GBlendMode::kDstATop:
            return kDstATopFunc;
  
        case GBlendMode::kXor:
            return kXorFunc;
   
    }
}

    void drawPaint(const GPaint& Paint) override {
        //check if paint has shader, if it does then make 4 corners for the bitmap and pass into drawconvexPoly,
        int height =fDevice.height();
        int width = fDevice.width();
        if (Paint.getShader() != nullptr){
            GPoint points[] = {
                {0, 0},
                {0, height},
                {width, height},
                {width, 0}
            };
            drawConvexPolygon(points, 4, Paint);
            return;
        }
        
        GColor color = Paint.getColor().pinToUnit();
        GBlendMode blendMode = Paint.getBlendMode();

        float mulr =  color.r * color.a;
        float mulg =  color.g * color.a;
        float mulb =  color.b * color.a;
        int hexValr = GRoundToInt(mulr * 255);
        int hexValg = GRoundToInt(mulg * 255);
        int hexValb = GRoundToInt(mulb * 255);
        int hexVala = GRoundToInt(color.a * 255);
        GPixel src = GPixel_PackARGB(hexVala, hexValr, hexValg, hexValb);
        int srca = GPixel_GetA(src);
        blendMode = optimizedBlendMode(blendMode, srca);
        //based on the blend mode, do the correct funciton
        const auto blendFunc = getBlendFunc(blendMode);

        for (int y = 0; y<height ; y++){
            for(int x = 0 ; x < width; x++){
                GPixel *address = fDevice.getAddr(x, y);
                *address = blendFunc(src,*address);

            }
        }

    }
    

    void drawRect(const GRect& rect, const GPaint& Paint) override {
        int height =fDevice.height();
        int width = fDevice.width();
        
        //if paint has shader, pass coord of rect into poly
        if (Paint.getShader() != nullptr){
            GPoint points[] = {
                {rect.fLeft,rect.fTop},
                {rect.fLeft,rect.fBottom},
                {rect.fRight,rect.fBottom},
                {rect.fRight,rect.fTop},
            };
            drawConvexPolygon(points, 4, Paint);
            return;
        }

        GRect mappedRect;
        matrixStack.top().mapPoints((GPoint*)&mappedRect, (GPoint*)&rect,2);

        GIRect newrect = mappedRect.round();
        GColor color = Paint.getColor().pinToUnit();
        GBlendMode blendMode = Paint.getBlendMode();

        float mulr =  color.r * color.a;
        float mulg =  color.g * color.a;
        float mulb =  color.b * color.a;

        int hexValr = GRoundToInt(mulr * 255);
        int hexValg = GRoundToInt(mulg * 255);
        int hexValb = GRoundToInt(mulb * 255);
        int hexVala = GRoundToInt(color.a * 255);

        GPixel src = GPixel_PackARGB(hexVala, hexValr, hexValg, hexValb);
        int srca = GPixel_GetA(src);
        blendMode = optimizedBlendMode(blendMode, srca);
        const auto blendFunc = getBlendFunc(blendMode);


        
        int minx = std::max(0, newrect.left());
        int miny = std::max(0, newrect.top());
        int maxx = std::min(fDevice.width(), newrect.right());
        int maxy = std::min(fDevice.height(), newrect.bottom());  

        
        
        for (int y = miny; y<maxy ; y++){
            for(int x = minx ; x < maxx; x++){
                GPixel *address = fDevice.getAddr(x, y);
                //*address = colorWithBlendMode(src,*address, blendMode);
                *address = blendFunc(src,*address);

            }
        } 
        

    }
    
    //blit function that writes the pixel accordign to blendmode
    inline void blit(int x, int y , int n, GPixel src, GBlendMode blendMode ){
        const auto blendFunc = getBlendFunc(blendMode);

        for (int i = x; i < x+n; i++){
            if (i > 0 && i < fDevice.width() && y > 0 && y < fDevice.height()){
    
                GPixel *address = fDevice.getAddr(i, y);
                //*address = colorWithBlendMode(src,*address, blendMode);
                *address = blendFunc(src,*address);

            }
        }
    }

    inline void blitArray(int x, int y , int n, GPixel srcarr[], GBlendMode blendMode ){
        const auto blendFunc = getBlendFunc(blendMode);

        for (int i = x; i < x+n; i++){
            if (i > 0 && i < fDevice.width() && y > 0 && y < fDevice.height()){
    
                GPixel *address = fDevice.getAddr(i, y);
                //*address = colorWithBlendMode(srcarr[i - x],*address, blendMode);
                *address = blendFunc(srcarr[i - x],*address);

            }
        }
    }
    /**
     *  Fill the convex polygon with the color and blendmode,
     *  following the same "containment" rule as rectangles.
     */
    GPixel getSrcPixel(GColor color){
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

    void drawConvexPolygon(const GPoint newarr[], int count, const GPaint& p) override{
        if(count < 3){
            return;
        }
        int height =fDevice.height();
        int width = fDevice.width();
        //fix these GPaint errors
        GColor color = p.getColor().pinToUnit();
        GBlendMode blendMode = p.getBlendMode();
        
        float mulr =  color.r * color.a;
        float mulg =  color.g * color.a;
        float mulb =  color.b * color.a;

        int hexValr = GRoundToInt(mulr * 255);
        int hexValg = GRoundToInt(mulg * 255);
        int hexValb = GRoundToInt(mulb * 255);
        int hexVala = GRoundToInt(color.a * 255);

        GPixel src = getSrcPixel(color);
        //check i
        if(p.getShader() != nullptr){
            GShader* shader = p.getShader();
            bool val = shader->setContext(matrixStack.top());
            if (!val){
                return;
            }
        }

        GPoint pointarr[1000];
        assert(!matrixStack.empty());
        matrixStack.top().mapPoints(pointarr, newarr,count);

        //change blendmode here
        int srca = GPixel_GetA(src);
        
        blendMode = optimizedBlendMode(blendMode, srca);
    
        const GRect Rect = GRect::LTRB(0, 0,fDevice.width(),fDevice.height());
        


        std::vector<Edge> clipped_arr;
        clipped_arr.reserve(sizeof(Edge));
        
        for (int i = 0; i < count -1; i++){
            GPoint p1 = pointarr[i];
            GPoint p2 = pointarr[i +1];
            clipPointsArrayAndReturnEdgeArray(p1, p2, Rect,clipped_arr);
            
        }
        //if count 0?

        clipPointsArrayAndReturnEdgeArray(pointarr[count-1], pointarr[0], Rect,clipped_arr);
        if (clipped_arr.size()== 0){
            return;
        }
        //sort such that biggest bottom on top
        int total= sizeof(clipped_arr);
        std::sort(clipped_arr.begin(), clipped_arr.end(), 
            [](const Edge a, const Edge b){
                return a.bottom > b.bottom;
            });
        int biggest_bottom = clipped_arr[0].getBottom();

        

        // sort array
        std::sort(clipped_arr.begin(), clipped_arr.end(), 
            [](const Edge a, const Edge b){
                return a.top < b.top;
            });
        

        int smallest_top = clipped_arr[0].getTop();

        int n = 2;
        Edge e0 = clipped_arr[0];
        Edge e1 = clipped_arr[1];
        
        for (int y = smallest_top; y < biggest_bottom; y++){
            //check if edge e0 and e1 should be 0.
            
            // Edge e0 = clipped_arr[0];
            // Edge e1 = clipped_arr[1];

            float x0 = e0.eval(y);
            float x1 = e1.eval(y);

            if(x0>x1){
                std::swap(x0, x1);

            }
            int L = GRoundToInt(x0);
            int R = GRoundToInt(x1);
            if(p.getShader()!= nullptr){
                //initialise size outside
                GPixel srcpixel[1000];
                p.getShader()->shadeRow(L,y, R-L, srcpixel);
                blitArray(L, y, R-L, srcpixel, p.getBlendMode());

            }
            else{
                blit(L, y, R-L, src, blendMode);
            }

            if(e0.lastY(y)){
                //clipped_arr.erase(clipped_arr.begin());
                if (n-1 <= total){
                    e0 = clipped_arr[n];
                    n++;
                }
                
            }
            if (e1.lastY(y)){
                //clipped_arr.erase(clipped_arr.begin()+1);
                if (n-1 <= total){
                    e1 = clipped_arr[n];
                    n++;
                }
                
            }
            
        }

    };

    /**
     *  Fill the path with the paint, interpreting the path using winding-fill (non-zero winding).
     */
    void drawPath(const GPath& path, const GPaint& p){
            //updatetee
        GPoint pts[GPath::kMaxNextPoints];
        GPath::Edger iter(path);
        GPath::Verb v;

        int height =fDevice.height();
        int width = fDevice.width();
        GColor color = p.getColor().pinToUnit();
        GBlendMode blendMode = p.getBlendMode();
        
        float mulr =  color.r * color.a;
        float mulg =  color.g * color.a;
        float mulb =  color.b * color.a;

        int hexValr = GRoundToInt(mulr * 255);
        int hexValg = GRoundToInt(mulg * 255);
        int hexValb = GRoundToInt(mulb * 255);
        int hexVala = GRoundToInt(color.a * 255);

        GPixel src = getSrcPixel(color);

        //check i
        if(p.getShader() != nullptr){
            GShader* shader = p.getShader();
            bool val = shader->setContext(matrixStack.top());
            if (!val){
                return;
            }
        }

         //change blendmode here
        int srca = GPixel_GetA(src);
        
        blendMode = optimizedBlendMode(blendMode, srca);
    
        const GRect Rect = GRect::LTRB(0, 0,fDevice.width(),fDevice.height());

        std::vector<Edge> clipped_arr;
        clipped_arr.reserve(sizeof(Edge));

        GPath copy = path;
        assert(!matrixStack.empty());
        copy.transform(matrixStack.top());

        //copy is now the transformed path
        GPath::Edger edger(copy);
        while ((v = edger.next(pts)) != GPath::kDone) {
            switch (v) {
                case GPath::kLine:
                    clipPointsArrayAndReturnEdgeArray(pts[0], pts[1], Rect,clipped_arr);    
                    break;
                case GPath::kQuad:
                    {
                        int k = numSegmentQuad(pts[0], pts[1], pts[2]);
                        float dt = 1.0f / k;
                        float t = dt;
                        GPoint p_0 = pts[0];
                        GPoint p_1;
                        for (int i = 1 ; i < k; ++i){
                            
                            p_1 = evalQuad(pts[0], pts[1], pts[2], t);
                            clipPointsArrayAndReturnEdgeArray(p_0, p_1, Rect, clipped_arr);
                            t+= dt;
                            p_0 = p_1;

                        }
                        p_1 = pts[2];
                        clipPointsArrayAndReturnEdgeArray(p_0, p_1, Rect, clipped_arr);
                        break;

                    }
                    
                case GPath::kCubic:
                    //Check if this works
                    {
                    int k = numSegmentCubic(pts[0], pts[1], pts[2], pts[3]);
                    float dt = 1.0f / k;
                    float t = dt;
                    GPoint p_0 = pts[0];
                    GPoint p_1;
                    for (int i = 1 ; i < k; ++i){
                        p_1 = evalCubic(pts[0], pts[1], pts[2], pts[3], t); 
                        clipPointsArrayAndReturnEdgeArray(p_0, p_1, Rect, clipped_arr);
                        t+= dt;
                        p_0 = p_1;

                    }
                    p_1 = pts[3];
                    clipPointsArrayAndReturnEdgeArray(p_0, p_1, Rect, clipped_arr);
                    }
                    break;
                default:
                    break;
            }
        }


        if (clipped_arr.size()== 0){
            return;
        }

        //sort by the x values
        //sort such that biggest bottom on top
        std::sort(clipped_arr.begin(), clipped_arr.end(), 
            [](const Edge a, const Edge b){
                return a.bottom > b.bottom;
            });
        int biggest_bottom = clipped_arr[0].getBottom();

        // sort array
        std::sort(clipped_arr.begin(), clipped_arr.end(), 
            [](const Edge a, const Edge b){
                return a.top < b.top;
            });
        

        int smallest_top = clipped_arr[0].getTop();


        for (int y = smallest_top; y < biggest_bottom; y++){
            int w = 0; 
            int i = 0;
            int L; 
            int R;
            while (i < clipped_arr.size() && clipped_arr[i].active(y)){
                float x = clipped_arr[i].eval(y);
                if (w == 0){
                    L = GRoundToInt(x);
                }
                w += clipped_arr[i].getWind();
                if(w == 0){
                    R = GRoundToInt(x);
                    //assert(L is initialised)

                    //blit
                    if(p.getShader()!= nullptr){
                    //initialise size outside
                    GPixel srcpixel[1000];
                    //assert(L < R);

                    //assert((R-L)> 500);
                    p.getShader()->shadeRow(L,y, R-L, srcpixel);
                    blitArray(L, y, R-L, srcpixel, p.getBlendMode());
                    }
                    else{
                        blit(L, y, R-L, src, blendMode);
                    }
                }
                if (clipped_arr[i].lastY(y)){
                    //erase
                    clipped_arr.erase(clipped_arr.begin() + i);     // remove element at index i
                }
                else{
                    i++;
                }

            }

            //pickup new edges
            while(i<clipped_arr.size() && clipped_arr[i].active(y + 1)){
                i++;
            }
            std::sort(clipped_arr.begin(), clipped_arr.begin() + i, [y](const Edge& a, const Edge& b) {
                return a.eval(y) < b.eval(y);
            });

        }

    }; 

       /**
     *  Draw a mesh of triangles, with optional colors and/or texture-coordinates at each vertex.
     *
     *  The triangles are specified by successive triples of indices.
     *      int n = 0;
     *      for (i = 0; i < count; ++i) {
     *          point0 = vertx[indices[n+0]]
     *          point1 = verts[indices[n+1]]
     *          point2 = verts[indices[n+2]]
     *          ...
     *          n += 3
     *      }
     *
     *  If colors is not null, then each vertex has an associated color, to be interpolated
     *  across the triangle. The colors are referenced in the same way as the verts.
     *          color0 = colors[indices[n+0]]
     *          color1 = colors[indices[n+1]]
     *          color2 = colors[indices[n+2]]
     *
     *  If texs is not null, then each vertex has an associated texture coordinate, to be used
     *  to specify a coordinate in the paint's shader's space. If there is no shader on the
     *  paint, then texs[] should be ignored. It is referenced in the same way as verts and colors.
     *          texs0 = texs[indices[n+0]]
     *          texs1 = texs[indices[n+1]]
     *          texs2 = texs[indices[n+2]]
     *
     *  If both colors and texs[] are specified, then at each pixel their values are multiplied
     *  together, component by component.
     */
    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[], int count, const int indices[], const GPaint& paint){
        
        int n= 0;
        GPoint my_vertices[3];
        GColor my_colors[3];
        GPoint my_texs[3];
        GShader *shader;
        bool color_val;
        bool texs_val;
        if (colors == nullptr){
            color_val = false;
        }
        else{
            color_val = true;

        }
        if (texs == nullptr){
            texs_val = false;
        }
        else{
            texs_val = true;
            
        }
        //use different for loops for different cases
        if ((!color_val) && (!texs_val)){
            return;
        }
        for (int i = 0 ; i < count ; ++i)
        {
            my_vertices[0]=verts[indices[3*i]];
            my_vertices[1]=verts[indices[3*i+1]];
            my_vertices[2]=verts[indices[3*i+2]];   

    
            if (color_val){
            
                my_colors[0]=colors[indices[3*i]];
                my_colors[1]=colors[indices[3*i+1]];
                my_colors[2]=colors[indices[3*i+2]]; 
            }
            if (texs_val){
                my_texs[0]=texs[indices[3*i]];
                my_texs[1]=texs[indices[3*i+1]];
                my_texs[2]=texs[indices[3*i+2]]; 
                }
            
            if (texs_val && color_val){
                ColorShader tri_color_shader = ColorShader(my_colors, my_vertices, sizeof(my_colors));
                TexShader tri_tex_shader = TexShader(my_vertices,my_texs,paint.getShader());
                shader = new ColorTexShader(&tri_color_shader,&tri_tex_shader);
                drawConvexPolygon( my_vertices, 3,GPaint(shader));

            }
            else if (color_val){
                shader= new ColorShader(my_colors, my_vertices, sizeof(my_colors));
                drawConvexPolygon(my_vertices, 3, GPaint(shader));
            }
            else if (texs_val){
                shader = new TexShader(my_vertices,my_texs,paint.getShader());
                drawConvexPolygon(my_vertices, 3, GPaint(shader) );
            }
            else{
                continue;
            }
        }
                     
    };


    /**
     *  Draw the quad, with optional color and/or texture coordinate at each corner. Tesselate
     *  the quad based on "level":
     *      level == 0 --> 1 quad  -->  2 triangles
     *      level == 1 --> 4 quads -->  8 triangles
     *      level == 2 --> 9 quads --> 18 triangles
     *      ...
     *  The 4 corners of the quad are specified in this order:
     *      top-left --> top-right --> bottom-right --> bottom-left
     *  Each quad is triangulated on the diagonal top-right --> bottom-left
     *      0---1
     *      |  /|
     *      | / |
     *      |/  |
     *      3---2
     *
     *  colors and/or texs can be null. The resulting triangles should be passed to drawMesh(...).
     */
    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],int level, const GPaint& paint){
            //initialise storage
            GPoint ** all_points = new GPoint*[level + 2];
            GPoint ** all_texs; 
            GColor ** all_colors;
            for (int i = 0 ; i < level + 2; i ++){
               all_points[i] = new GPoint[level + 2];
            }
            bool color_val;
            bool texs_val;
            if (colors == nullptr){
                color_val = false;
            }
            else{
                color_val = true;

            }
            if (texs == nullptr){
                texs_val = false;
            }
            else{
                texs_val = true;
                
            }
            
            if (color_val){
                all_colors = new GColor*[level + 2];
                for (int i = 0 ; i < level + 2; i ++){
                all_colors[i] = new GColor[level + 2];
                }
                
            }
            if (texs != nullptr){
                all_texs = new GPoint*[level + 2];
                for (int i = 0 ; i < level + 2; i ++){
                    all_texs[i] = new GPoint[level + 2];
                }

            }
            float dx = 1.0f / (level + 1);
            GPoint a = verts[0];
            GPoint b = verts[1];
            GPoint c = verts[2];
            GPoint d = verts[3];

            GPoint a_tex;
            GPoint b_tex; 
            GPoint c_tex;
            GPoint d_tex; 

            GColor b_color;
            GColor a_color;
            GColor c_color;
            GColor d_color;

            if (texs_val){
                 a_tex = texs[0];
                 b_tex = texs[1];
                 c_tex = texs[2];
                 d_tex = texs[3]; 
            }

            if (color_val){
                a_color = colors[0];
                 b_color = colors[1];
             
                c_color = colors[2];
                d_color = colors[3]; 
            }

            //handle edge cases
            for (int i = 0; i < level + 2; i++){
                GPoint point1;
                GPoint point2;

                GPoint tex1;
                GPoint tex2;

                GColor color1;
                GColor color2;
                point1 = a + ((i * dx) * (b - a ));
                point2 = d + ((i * dx) * (c - d));
                if (texs_val){
                    tex1 = a_tex + ((i * dx) * (b_tex - a_tex ));
                    tex2 = d_tex + ((i * dx) * (c_tex - d_tex));

                }

                if (color_val){
                    color1 = a_color + ((i * dx) * (b_color - a_color ));
                    color2 = d_color + ((i * dx) * (c_color - d_color)) ;
                }
                

                //}
                
                for (int j = 0; j < level + 2; j++ ){
                    all_points[i][j] = point1 + ((j * dx) * (point2- point1)) ;
                    if (texs_val){
                        all_texs[i][j] = tex1 + ((j * dx) * (tex2- tex1));
                    }
                    if (color_val){
                        all_colors[i][j] = color1 + ((j * dx) * (color2- color1));
                    }


                    //}
                }

            }
            //now to draw quads:
            int indices[] = {0, 1, 3, 3,2,1};
            GPoint points[4];
            GPoint i_texs[4];
            GColor i_colors[4];

            for (int i = 0; i < level + 1; i++){
                for (int j = 0 ; j < level +1; j++){
                    points[0] = all_points[i][j];
                    points[1] = all_points[i + 1][j];
                    points[2] = all_points[i + 1][j +1];
                    points[3] = all_points[i][j +1];
                    // GPoint i_texs[4];
                    // GColor i_colors[4];
                    if (texs_val)
                    {
                        i_texs[0] = all_texs[i][j];
                        i_texs[1] = all_texs[i + 1][j];
                        i_texs[2] = all_texs[i + 1][j + 1];
                        i_texs[3] = all_texs[i][j +1];  
                    }
                    if(color_val){
                        i_colors[0] = all_colors[i][j];
                        i_colors[1] = all_colors[i + 1][j];
                        i_colors[2] = all_colors[i + 1][j + 1];
                        i_colors[3] = all_colors[i][j +1];  
                    }

                    drawMesh(points, colors ? i_colors:nullptr,texs ? i_texs:nullptr, 2,indices,paint);
                }
            }
        
        };

        

 




    

        // Helpers
    //maybe use my_GMatrix
    void translate(float x, float y) {
        this->concat(GMatrix::Translate(x, y));
    }

    void scale(float x, float y) {
        this->concat(GMatrix::Scale(x, y));
    }

    void rotate(float radians) {
        this->concat(GMatrix::Rotate(radians));
    }

    void clear(const GColor& color) {
        GPaint paint(color);
        paint.setBlendMode(GBlendMode::kSrc);
        this->drawPaint(paint);
    }

    void fillRect(const GRect& rect, const GColor& color) {
        this->drawRect(rect, GPaint(color));
    }
private:
    // Note: we store a copy of the bitmap
    const GBitmap fDevice;
    GMatrix ctm;
    std::stack<GMatrix> matrixStack;


};


/**
 *  If the bitmap is valid for drawing into, this returns a subclass that can perform the
 *  drawing. If bitmap is invalid, this returns NULL.
 */
std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& bitmap){
        return std::unique_ptr<GCanvas>(new MyCanvas(bitmap));

};

static void make_star(GPath* path, int count, float anglePhase) {
    assert(count & 1);
    float da = 2 * M_PI * (count >> 1) / count;
    float angle = anglePhase;
    for (int i = 0; i < count; ++i) {
        GPoint p = { cosf(angle), sinf(angle) };
        i == 0 ? path->moveTo(p) : path->lineTo(p);
        angle += da;
    }
}

static void add_star(GPath* path, int count) {
    if (count & 1) {
        make_star(path, count, 0);
    } else {
        count >>= 1;
        make_star(path, count, 0);
        make_star(path, count, M_PI);
    }
}

/**
 *  Implement this, drawing into the provided canvas, and returning the title of your artwork.
 */
std::string GDrawSomething(GCanvas* canvas, GISize dim){

    GPath path;;
    add_star(&path, 7);
    
    canvas->clear({1, 1, 1, 1});
    
    GMatrix scale = GMatrix::Scale(50, 50);

    path.transform(scale);
    canvas->translate(60, 60);
    canvas->drawPath(path, GPaint({1,1,0,1}));

    path.reset();
    canvas->translate(60, 60);
    add_star(&path, 6);
    scale =  GMatrix::Scale(75, 75);
    path.transform(scale);
    canvas->drawPath(path, GPaint({0.5,0.5,0.5,0.5}));
   
    path.reset();
     canvas->translate(60, 60);
    add_star(&path, 15);
    scale =  GMatrix::Scale(75, 75);
    path.transform(scale);
    canvas->drawPath(path, GPaint({0,0.24,0.6,1}));

    path.reset();
    canvas->translate(-120, 0);
    add_star(&path, 7);
    path.transform(scale);
    canvas->drawPath(path, GPaint({1,1,0,1}));

    path.reset();
    canvas->translate(60, -60);
    add_star(&path, 6);
    scale =  GMatrix::Scale(75, 75);
    path.transform(scale);
    canvas->drawPath(path, GPaint({0.5,0.5,0.5,0.5}));
   
    path.reset();
     canvas->translate(60, -60);
    add_star(&path, 15);
    scale =  GMatrix::Scale(75, 75);
    path.transform(scale);
    canvas->drawPath(path, GPaint({0,0.24,0.6,1}));

    return "starsss";

}


