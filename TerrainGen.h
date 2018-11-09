#ifndef TERRAINGEN_H
#define TERRAINGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "TerrainGrid.h"

void genFlat(TerrainGrid* terrain, float height);
void genCircles(TerrainGrid* terrain, float radius, float height, int iterations);
void genFaults(TerrainGrid* terrain, float height, float iterations);
void clampTerrain(TerrainGrid* terrain, int min, int max);

#endif