#ifndef BlendModes_DEFINED
#define BlendModes_DEFINED

#include "include/GBlendMode.h"
#include "include/GPixel.h"

//src is the color thats given to you 
//dest is the color that will be overwritten

// typedef GPixel (*BLENDFUNC)(const GPixel, const GPixel)



static inline uint8_t GDiv255(unsigned prod){
        prod += 0x80;
        prod += prod >>8;
        return prod >> 8;
    }

//!<     0

static inline GPixel kClearFunc (const GPixel src, const GPixel dest){
    return GPixel_PackARGB(0,0,0,0);
}
//!<     S
static inline GPixel kSrcFunc (const GPixel src, const GPixel dest){
    return src;
}

//!<     D
static inline GPixel kDstFunc (const GPixel src, const GPixel dest){
    return dest;
}

//!<     S + (1 - Sa)*D
static inline GPixel kSrcOverFunc (const GPixel src, const GPixel dest){
    
    int srca = GPixel_GetA(src); //from 0-255

    int srcr = GPixel_GetR(src);
    int srcg = GPixel_GetG(src);
    int srcb = GPixel_GetB(src);
    int isrca = 255 - srca;

    int desta = GPixel_GetA(dest);
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);

    unsigned a = srca + GDiv255(isrca * desta);
    unsigned r = srcr + GDiv255(isrca * destr);
    unsigned g = srcg + GDiv255(isrca * destg);
    unsigned b = srcb + GDiv255(isrca * destb);
    return GPixel_PackARGB(a,r,g,b);

}
//!<     D + (1 - Da)*S
static inline GPixel kDstOverFunc (const GPixel src, const GPixel dest){
    int desta = GPixel_GetA(dest);
    // if (desta == 255 ){
    //     return kDstFunc(src, dest);
    // }
    // if(desta == 0){
    //     return kSrcFunc(src, dest) + kDstFunc(src, dest);
    // }
    int srca = GPixel_GetA(src);
    
    int srcr = GPixel_GetR(src);
    int srcg = GPixel_GetG(src);
    int srcb = GPixel_GetB(src);

    
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);

    int idesta = 255 - desta;
    unsigned a = desta + GDiv255(idesta * srca);
    unsigned r = destr + GDiv255(idesta * srcr);
    unsigned g = destg + GDiv255(idesta * srcg);
    unsigned b = destb + GDiv255(idesta * srcb);
    return GPixel_PackARGB(a,r,g,b);


}
//!<     Da * S
static inline GPixel kSrcInFunc (const GPixel src, const GPixel dest){
    int desta = GPixel_GetA(dest);
    // if(desta == 0){
    //     return kClearFunc(src, dest);
    
    // }
    // if (desta == 255){
    //     return kSrcFunc(src, dest);
    // }
    int srca = GPixel_GetA(src); // from 0 - 255
    int srcr = GPixel_GetR(src);
    int srcg = GPixel_GetG(src);
    int srcb = GPixel_GetB(src);

    
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);
    // Have I added GDiv 255 here correctly?
    unsigned a = GDiv255(desta * srca);
    unsigned r = GDiv255(desta * srcr);
    unsigned g = GDiv255(desta *srcg);
    unsigned b = GDiv255(desta * srcb);
    return GPixel_PackARGB(a,r,g,b);
}
//!<     Sa * D
static inline GPixel kDstInFunc (const GPixel src, const GPixel dest){
    int srca = GPixel_GetA(src);
    
    int desta = GPixel_GetA(dest);
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);

    unsigned a = GDiv255(srca * desta);
    unsigned r = GDiv255(srca *destr);
    unsigned g = GDiv255(srca * destg);
    unsigned b = GDiv255(srca * destb);
    return GPixel_PackARGB(a,r,g,b);
}
//!<     (1 - Da)*S
static inline GPixel kSrcOutFunc (const GPixel src, const GPixel dest){
    int desta = GPixel_GetA(dest);

    // if(desta == 0){
    //     return kSrcFunc(src, dest);

    // }
    // if(desta == 255){
    //     return kClearFunc(src,dest);
    // }
    int srca = GPixel_GetA(src);
    int srcr = GPixel_GetR(src);
    int srcg = GPixel_GetG(src);
    int srcb = GPixel_GetB(src);


    int idesta = 255 - desta;
    unsigned a = GDiv255(idesta * srca);
    unsigned r = GDiv255(idesta * srcr);
    unsigned g = GDiv255(idesta * srcg);
    unsigned b = GDiv255(idesta * srcb);
    return GPixel_PackARGB(a,r,g,b);

}
//!<     (1 - Sa)*D
static inline GPixel kDstOutFunc (const GPixel src, const GPixel dest){
    int srca = GPixel_GetA(src); //from 0-255

    int isrca = 255 - srca;

    int desta = GPixel_GetA(dest);
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);

    unsigned a = GDiv255(isrca * desta);
    unsigned r = GDiv255(isrca * destr);
    unsigned g = GDiv255(isrca * destg);
    unsigned b = GDiv255(isrca * destb);
    return GPixel_PackARGB(a,r,g,b);
}
//!<     Da*S + (1 - Sa)*D
static inline GPixel kSrcATopFunc (const GPixel src, const GPixel dest){
    int srca = GPixel_GetA(src); //from 0-255
    int srcr = GPixel_GetR(src);
    int srcg = GPixel_GetG(src);
    int srcb = GPixel_GetB(src);
    int isrca = 255 - srca;

    int desta = GPixel_GetA(dest);
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);

    unsigned a = GDiv255(desta * srca) + GDiv255(isrca * desta);
    unsigned r = GDiv255(desta * srcr) +GDiv255(isrca * destr);
    unsigned g = GDiv255(desta * srcg) +GDiv255(isrca * destg);
    unsigned b = GDiv255(desta * srcb) +GDiv255(isrca * destb);

    return GPixel_PackARGB(a,r,g,b);
}
//!<     Sa*D + (1 - Da)*S
static inline GPixel kDstATopFunc (const GPixel src, const GPixel dest){
    int srca = GPixel_GetA(src);
    int srcr = GPixel_GetR(src);
    int srcg = GPixel_GetG(src);
    int srcb = GPixel_GetB(src);

    int desta = GPixel_GetA(dest);
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);

    int idesta = 255 - desta;
    unsigned a = GDiv255(srca * desta) + GDiv255(idesta * srca);
    unsigned r = GDiv255(srca * destr) + GDiv255(idesta * srcr);
    unsigned g = GDiv255(srca * destg) + GDiv255(idesta * srcg);
    unsigned b = GDiv255(srca * destb) + GDiv255(idesta * srcb);
    return GPixel_PackARGB(a,r,g,b);
}

//!<     (1 - Sa)*D + (1 - Da)*S
static inline GPixel kXorFunc (const GPixel src, const GPixel dest){
    int srca = GPixel_GetA(src); //from 0-255
    int srcr = GPixel_GetR(src);
    int srcg = GPixel_GetG(src);
    int srcb = GPixel_GetB(src);
    int isrca = 255 - srca;

    int desta = GPixel_GetA(dest);
    int destr = GPixel_GetR(dest);
    int destg = GPixel_GetG(dest);
    int destb = GPixel_GetB(dest);
    int idesta = 255 - desta;

    unsigned a = GDiv255(isrca * desta) + GDiv255(idesta * srca);
    unsigned r = GDiv255(isrca * destr) + GDiv255(idesta * srcr);
    unsigned g = GDiv255(isrca * destg)+ GDiv255(idesta * srcg);
    unsigned b = GDiv255(isrca * destb)+ GDiv255(idesta * srcb);

    return GPixel_PackARGB(a,r,g,b);
}

#endif
