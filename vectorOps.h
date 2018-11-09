#ifndef VECTOROPS_H
#define VECTOROPS_H

#include <stdio.h>
#include <iostream>
#include <math.h>

class vec3
{
    public:
    float x;
    float y;
    float z;
    float arr[3];

    vec3(){
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }

    inline vec3(float xArg, float yArg, float zArg)
    {
        this->x = xArg;
        this->y = yArg;
        this->z = zArg;
    }

    inline vec3 operator= (vec3 other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
        return *this;
    }

    inline vec3 operator+ (vec3 other)
    {
        vec3 ret;
        ret.x = this->x + other.x;
        ret.y = this->y + other.y;
        ret.z = this->z + other.z;
        return ret;
    }

    inline vec3 operator+= (vec3 other)
    {
        this->x += other.x;
        this->y += other.y;
        this->z += other.z;
        return *this;
    }

    inline vec3 operator- (vec3 other)
    {
        //printf("Hello");
        vec3 ret;
        ret.x = this->x - other.x;
        ret.y = this->y - other.y;
        ret.z = this->z - other.z;
        return ret;
    }

    inline vec3 operator-= (vec3 other)
    {
        this->x -= other.x;
        this->y -= other.y;
        this->z -= other.z;
        return *this;
    }

    inline vec3 operator* (float m)
    {
        this->x *= m;
        this->y *= m;
        this->z *= m;
        return *this;
    }

    inline vec3 operator/ (float m)
    {
        this->x /= m;
        this->y /= m;
        this->z /= m;
        return *this;
    }

    float magnitude(){
        return (float)sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }

    
    inline vec3 normalize(){
        float mag;
        mag = this->magnitude();
        return *this / mag;
    }

    float* getArray(){
        arr[0] = this->x;
        arr[1] = this->y; 
        arr[2] = this->z;
        return arr;
    }
};
/*
inline vec3 vec3::operator-(const vec3& other) const
{
    std::cout << "oops\n";
    vec3 ret;
    ret.x = this->x - other.x;
    ret.y = this->y - other.y;
    ret.z = this->z - other.z;
    return ret;
}*/

inline vec3 cross(vec3 vectorA, vec3 vectorB){
    vec3 ret;
    ret.x = vectorA.y * vectorB.z - vectorA.z * vectorB.y;
    ret.y = vectorA.z * vectorB.x - vectorA.x * vectorB.z;
    ret.z = vectorA.x * vectorB.y - vectorA.y * vectorB.x;
    return ret;
}

inline float dotProduct(vec3 vectorA, vec3 vectorB){
    return vectorA.x * vectorB.x + vectorA.y * vectorB.y + vectorA.z * vectorB.z;
};

#endif /* VECTOROPS_H */
