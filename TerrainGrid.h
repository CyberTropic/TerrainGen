#ifndef TERRAINGRID_H
#define TERRAINGRID_H

#include <stdio.h>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
//#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <iostream>
#endif

#include "vectorOps.h"
#include "TextureLoader.h"

#include <vector>
using std::vector;
#include "vertStruct.h"

class TerrainGrid{
    public:
    GLubyte *texUnderground;
    // int* width;
    // int* height;
    // int* max;
    int gridSize;
    float waterLevel = -0.5;
    float waterColor[4] = {0.0f,0.45f,0.75f,0.6f};
    float waterColorDark[4] = {0.0f,0.40f,0.5f,0.7f};
    float waveHeight = 0.025;
    float waveSpeed = 0.0025;
    float waveAmount = 0.1;
    vec3* centerPoint;
    TerrainGrid(int gridSizeArg);
    void drawTerrain(float* texture);
    void drawTerrainEdges(float* texture);
    void setWaterLevel(float waterLevelArg);
    void drawWater(float* waterColorArg);
    void drawWaterEdges(float* waterColorArg);
    void debugDrawNormals();
    void calculateNormals();
    void calculateNormals(vector<vector<VertStruct*>> gridArg);
    void calculateColors();
    void updateWater(float gameTime);
    vector<vector<VertStruct*>> gridVertex;
    vector<vector<VertStruct*>> waterVertex;

    private:
    int width, height, max;

};

#endif