#ifndef VERTSTRUCT_H
#define VERTSTRUCT_H

#include "vectorOps.h"

struct VertStruct
{ 
    vec3 *pos;  
    vec3 *normal;

    VertStruct(){
        pos = new vec3;
        normal = new vec3;
    }

    VertStruct(float argX, float argY, float argZ){
        pos = new vec3;
        normal = new vec3;
        pos->x = argX;
        pos->y = argY;
        pos->z = argZ;
    }
};

#endif