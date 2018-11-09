#include "TerrainGen.h"
#include <math.h>
#include <stdlib.h>

void genFlat(TerrainGrid *terrain, float height)
{
    for(int i=0; i < terrain->gridSize; i++){
        for(int j=0; j < terrain->gridSize; j++){
            terrain->gridVertex[i][j]->pos->y = height;
        }
    }
}

void genCircles(TerrainGrid *terrain, float radius, float height, int iterations)
{
    for (int iter = 0; iter < iterations; iter++)
    {
        int randx = rand() % terrain->gridSize;
        int randz = rand() % terrain->gridSize;
        int up = rand() % 2 == 1 ? 1 : -1;
        for (int i = 0; i < terrain->gridSize; i++)
        {
            for (int j = 0; j < terrain->gridSize; j++)
            {
                float pd = sqrt(pow((float)(j - randx), 2) + pow((float)(i - randz), 2));
                if (abs(pd) <= radius)
                {
                    terrain->gridVertex[i][j]->pos->y += (height / 2 + cos(pd * 3.14 / radius) * height / 2) * up;
                }
                //                std::cout<<"("<<i<<","<<j<<")"<<grid[i][j][Y]<<"\n";
            }
        }
    }
}

void genFaults(TerrainGrid *terrain, float height, int iterations) 
{
	for (int iter = 0; iter < iterations; iter++) {
		int startSide = rand() % 2; //Either line is left-right or top-bottom
		float slope;
		float intercept;
		float p1x;
		float p1y;
		float p2x;
		float p2y;
		if (startSide == 0) { //left-right
			p1x = (float)rand() / RAND_MAX * terrain->gridSize;
			p1y = 0;
			p2x = (float)rand() / RAND_MAX * terrain->gridSize;
			p2y = terrain->gridSize;
		}
		else { //top bottom
			p1x = 0;
			p1y = (float)rand() / RAND_MAX * terrain->gridSize;
			p2x = terrain->gridSize;
			p2y = (float)rand() / RAND_MAX * terrain->gridSize;
		}
		//        std::cout<<p1x<<","<<p1y<<" to "<<p2x<<","<<p2y<<"\n";
		slope = (p2y - p1y) / (p2x - p1x);
		intercept = p1y - slope * p1x;
        for (int i = 0; i < terrain->gridSize; i++) {
            for (int j = 0; j < terrain->gridSize; j++) {
                if ((float)i > (slope * (float)j + intercept)) {
                    terrain->gridVertex[i][j]->pos->y += height;
                }
                else {
                    terrain->gridVertex[i][j]->pos->y -= height;
                }
            }
        }
		
	}
}

void clampTerrain(TerrainGrid *terrain, int min, int max)
{
    for (int i = 0; i < terrain->gridSize; i++)
    {
        for (int j = 0; j < terrain->gridSize; j++)
        {
           if (terrain->gridVertex[i][j]->pos->y < 0) { terrain->gridVertex[i][j]->pos->y = 0; 
           }else if(terrain->gridVertex[i][j]->pos->y > max) { terrain->gridVertex[i][j]->pos->y = max;
           };

        }
    }
}
