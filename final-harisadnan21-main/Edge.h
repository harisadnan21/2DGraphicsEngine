#ifndef Edge_DEFINED
#define Edge_DEFINED

#include "include/GPixel.h"
#include "include/GPoint.h"
#include "include/GRect.h"

class Edge{
public:
    int top;
    int bottom;
    float m; 
    float b;
    int w;

    int getTop() const { return top; }
    int getBottom() const { return bottom; }
    float getM() const { return m; }
    float getB() const { return b; }
    int getWind() const {return w;}

    float eval(int y) const{
        return (m * (y + 0.5 )) + b;
    }

    bool lastY(int y){
        //assert(y >= top && y < bottom);
        return y == bottom -1;
    }
    bool active(int y){
        return y >= top && y < bottom;
    }

     


    static Edge makeEdge( int top,  int bottom, float m , float b, int w) {
        return {top, bottom, m, b, w};
    }

    static bool makeEdgeFromPoints(GPoint p1, GPoint p2, Edge& edge, int windVal){
    
    if (p1.y() > p2.y()){
        //implementing w value over here, w = 1 when the edge is in up direction as given
        std::swap(p1, p2);
        windVal *= -1;

    }
        float m = (p1.x() - p2.x())/(p1.y() - p2.y());
       
        float b = p1.x() - (m * p1.y());
        edge = Edge::makeEdge(GRoundToInt(p1.y()), GRoundToInt(p2.y()), m , b, windVal);
        if (GRoundToInt(p1.y()) == GRoundToInt(p2.y())){
            return false;
        }
        if (edge.getTop() == edge.getBottom()){
            return false;
        }
        return true;
    }
};



//static inline function to build edges array from an input of array of points, sort the edges after making them
//return the aray of edges
static inline std::vector<Edge> buildEdgesArray(std::vector<GPoint> pointsArray){
    std::vector<Edge> returnEdgeArr;
    int n = pointsArray.size();

    for (int i = 0; i < n - 1; i++){
        GPoint p1 = pointsArray[i];
        GPoint p2 = pointsArray[i+ 1];
        Edge e;
        if (Edge::makeEdgeFromPoints(p1, p2, e, -1)){
            returnEdgeArr.push_back(e);
        }
        
    }
    Edge e;
    if (Edge::makeEdgeFromPoints(pointsArray[n-1], pointsArray[0], e, -1)){
            returnEdgeArr.push_back(e);
        }

    return returnEdgeArr;
};


// return an edge list after appending to edge in every if statement
static void clipPointsArrayAndReturnEdgeArray(GPoint p1, GPoint p2, GRect rect, std::vector<Edge>& edgearr){
    int wind = -1;

    float m = (p1.x() - p2.x())/(p1.y() - p2.y());
    float b = p1.x() - (m * p1.y());
    if (p1.y() == p2.y()){
        return;
    }

    if(p1.y()> p2.y()){
        std::swap(p1, p2);
        wind *= -1;
    }
    if (p2.y() <= rect.top() || p1.y() >= rect.bottom()){
        //do nothing, dont add to array
        return ;
    }
    //p1 out of bounds
    if (p1.y() < rect.top()){
    
        p1.fX = m * (rect.fTop- p1.y()) + p1.x();
        p1.fY = rect.top();
    }
    //p2 out of bounds
    if (p2.y() > rect.bottom()){
        p2.fX += m * (rect.fBottom- p2.y());
        p2.fY = rect.bottom();
    }
    //Horizontal 
    if (p1.fX > p2.fX) {
        std::swap(p1, p2);
        wind *= -1;
    }
    //all to left
    if (p2.fX <= rect.left()) {   
        p1.fX = p2.fX = rect.left();
        Edge e;
        if (Edge::makeEdgeFromPoints(p1, p2, e, wind)){
            edgearr.push_back(e);
        }

    return ; 
    }
    // all to right
    if (p1.fX >= rect.right()) { 
        p1.fX = p2.fX = rect.right();
        Edge e;
        if (Edge::makeEdgeFromPoints(p1, p2, e, wind)){
            edgearr.push_back(e);
        }

    return ;
    }
    if (p1.fX < rect.left()) {
        float y = p1.fY + (rect.left() - p1.fX) / m; 
        Edge e;
        if (Edge::makeEdgeFromPoints({rect.left(), p1.fY}, {rect.left(), y}, e, wind)){
            edgearr.push_back(e);
        } 
        p1 = {rect.left(), y};
    

    }
    if (p2.fX > rect.right()) {
        float y = p1.fY + (rect.right() - p1.fX) / m;
        Edge e;
        if (Edge::makeEdgeFromPoints(p1, {rect.right(), y}, e, wind)){
            edgearr.push_back(e);
        } 
        Edge e1;
        if (Edge::makeEdgeFromPoints({rect.right(), y}, {rect.right(), p2.fY}, e1, wind)){
            edgearr.push_back(e1);
        } 

    }
    else {
        Edge e;
        if (Edge::makeEdgeFromPoints(p1, p2, e, wind)){
            edgearr.push_back(e);
        } 


    }
    return ;
};


#endif
