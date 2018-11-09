#include "TerrainGrid.h"
#include <math.h>

TerrainGrid::TerrainGrid(int gridSizeArg)
{
    this->gridSize = gridSizeArg;
    gridVertex.resize(gridSize);
    waterVertex.resize(gridSize);
    for (int i = 0; i < gridSize; i++)
    {
        gridVertex[i].resize(gridSize);
        waterVertex[i].resize(gridSize);
        for (int j = 0; j < gridSize; j++)
        {
            gridVertex[i][j] = new VertStruct(i, 0, j);
            waterVertex[i][j] = new VertStruct(i, -0.5, j);
        }
    }
    this->centerPoint = new vec3((float)gridSize / 2, 0, (float)gridSize / 2);
    //this->texUnderground = LoadPPM((char*)"Assets/Textures/underground.ppm", &width, &height, &max);
}

void TerrainGrid::drawTerrain(float* texture)
{
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, texture);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, texture);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < gridSize - 1; i++)
    {
        for (int j = 0; j < gridSize - 1; j++)
        {
            //First triangle
            glNormal3fv(gridVertex[i][j]->normal->getArray());
            glVertex3fv(gridVertex[i][j]->pos->getArray());
            glNormal3fv(gridVertex[i][j+1]->normal->getArray());
            glVertex3fv(gridVertex[i][j+1]->pos->getArray());
            glNormal3fv(gridVertex[i+1][j]->normal->getArray());
            glVertex3fv(gridVertex[i+1][j]->pos->getArray());
            //Second triangle
            glNormal3fv(gridVertex[i][j+1]->normal->getArray());
            glVertex3fv(gridVertex[i][j+1]->pos->getArray());
            glNormal3fv(gridVertex[i + 1][j + 1]->normal->getArray());
            glVertex3fv(gridVertex[i + 1][j + 1]->pos->getArray());
            glNormal3fv(gridVertex[i+1][j]->normal->getArray());
            glVertex3fv(gridVertex[i+1][j]->pos->getArray());
        }
    }
    glEnd();
}

void TerrainGrid::drawTerrainEdges(float* texture)
{
    bool TEX = false;
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, texture);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, texture);
	if(TEX){
		glBindTexture(GL_TEXTURE_2D, *texUnderground);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texUnderground);
	}
    glBegin(GL_QUADS);
    for (int i = 0; i < gridSize - 1; i++)
    {
        glNormal3f(0, 0, -1);
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8), 0.0); }
        glVertex3f(i, -1, 0);
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8), 1.0); }
        glVertex3fv(gridVertex[i][0]->pos->getArray());
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8) + 0.2, 1.0); }
        glVertex3fv(gridVertex[i + 1][0]->pos->getArray());
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8) + 0.2, 0.0); }
        glVertex3f(i + 1, -1, 0);

        glNormal3f(0, 0, 1);
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8), 0.0); }
        glVertex3f(i, -1, gridSize - 1);
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8) + 0.2, 0.0); }
        glVertex3f(i + 1, -1, gridSize - 1);
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8) + 0.2, 1.0); }
        glVertex3fv(gridVertex[i + 1][gridSize - 1]->pos->getArray());
        if(TEX){ glTexCoord2f(fmod(i*0.2, 0.8), 1.0); }
        glVertex3fv(gridVertex[i][gridSize - 1]->pos->getArray());
    }
    for (int j = 0; j < gridSize - 1; j++)
    {
        glNormal3f(-1, 0, 0);
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8), 0.0); }
        glVertex3f(0, -1, j);
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8) + 0.2, 0.0); }
        glVertex3f(0, -1, j + 1);
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8) + 0.2, 1.0); }
        glVertex3fv(gridVertex[0][j + 1]->pos->getArray());
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8), 1.0); }
        glVertex3fv(gridVertex[0][j]->pos->getArray());

        glNormal3f(1, 0, 0);
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8), 0.0); }
        glVertex3f(gridSize - 1, -1, j);
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8), 1.0); }
        glVertex3fv(gridVertex[gridSize - 1][j]->pos->getArray());
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8) + 0.2, 1.0); }
        glVertex3fv(gridVertex[gridSize - 1][j + 1]->pos->getArray());
        if(TEX){ glTexCoord2f(fmod(j*0.2, 0.8) + 0.2, 0.0); }
        glVertex3f(gridSize - 1, -1, j + 1);
    }
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TerrainGrid::drawWaterEdges(float* waterColorArg)
{
    if(this->waterLevel <= 0){
        return;
    }
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, waterColorArg);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, waterColorArg);
    glBegin(GL_QUADS);
    for (int i = 0; i < gridSize - 1; i++)
    {
        glNormal3f(0, 0, -1);
        glVertex3f(i, gridVertex[i][0]->pos->y, 0);
        glVertex3fv(waterVertex[i][0]->pos->getArray());
        glVertex3fv(waterVertex[i + 1][0]->pos->getArray());
        glVertex3f(i + 1, gridVertex[i+1][0]->pos->y, 0);

        glNormal3f(0, 0, 1);
        glVertex3f(i, gridVertex[i][gridSize - 1]->pos->y, gridSize - 1);
        glVertex3f(i + 1, gridVertex[i+1][gridSize - 1]->pos->y, gridSize - 1);
        glVertex3fv(waterVertex[i + 1][gridSize - 1]->pos->getArray());
        glVertex3fv(waterVertex[i][gridSize - 1]->pos->getArray());
    }
    for (int j = 0; j < gridSize - 1; j++)
    {
        glNormal3f(-1, 0, 0);
        glVertex3f(0, gridVertex[0][j]->pos->y, j);
        glVertex3f(0, gridVertex[0][j+1]->pos->y, j + 1);
        glVertex3fv(waterVertex[0][j + 1]->pos->getArray());
        glVertex3fv(waterVertex[0][j]->pos->getArray());

        glNormal3f(1, 0, 0);
        glVertex3f(gridSize - 1, gridVertex[gridSize - 1][j]->pos->y, j);
        glVertex3fv(waterVertex[gridSize - 1][j]->pos->getArray());
        glVertex3fv(waterVertex[gridSize - 1][j + 1]->pos->getArray());
        glVertex3f(gridSize - 1, gridVertex[gridSize - 1][j+1]->pos->y, j + 1);
    }
    glEnd();
}

void TerrainGrid::debugDrawNormals(){
    float m_dif[4];
    m_dif[0] = 1; m_dif[1] = 1; m_dif[2] = 0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_dif);
    for(int i = 0; i < gridSize; i++){
        for(int j = 0; j < gridSize; j++){
            glBegin(GL_LINE_STRIP);
            glVertex3fv(gridVertex[i][j]->pos->getArray());
            glVertex3fv((*gridVertex[i][j]->pos + *gridVertex[i][j]->normal).getArray()) ;
            glEnd();
        }
    }
    m_dif[0] = 0; m_dif[1] = 0; m_dif[2] = 0;
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_dif);
}

void TerrainGrid::setWaterLevel(float waterLevelArg){
    this->waterLevel = waterLevelArg;
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            waterVertex[i][j]->pos = new vec3(i, this->waterLevel, j);
        }
    }
}

void TerrainGrid::drawWater(float* waterColorArg){
    if(this->waterLevel <= 0){
        return;
    }
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, waterColorArg);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, waterColorArg);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < gridSize - 1; i++)
    {
        for (int j = 0; j < gridSize - 1; j++)
        {
            //First triangle
            glNormal3fv(waterVertex[i][j]->normal->getArray());
            glVertex3fv(waterVertex[i][j]->pos->getArray());
            glNormal3fv(waterVertex[i][j+1]->normal->getArray());
            glVertex3fv(waterVertex[i][j+1]->pos->getArray());
            glNormal3fv(waterVertex[i+1][j]->normal->getArray());
            glVertex3fv(waterVertex[i+1][j]->pos->getArray());
            //Second triangle
            glNormal3fv(waterVertex[i][j+1]->normal->getArray());
            glVertex3fv(waterVertex[i][j+1]->pos->getArray());
            glNormal3fv(waterVertex[i + 1][j + 1]->normal->getArray());
            glVertex3fv(waterVertex[i + 1][j + 1]->pos->getArray());
            glNormal3fv(waterVertex[i+1][j]->normal->getArray());
            glVertex3fv(waterVertex[i+1][j]->pos->getArray());
        }
    }
    glEnd();
}

void TerrainGrid::updateWater(float gameTime){
    for(int i=0; i<gridSize; i++){
        for(int j = 0; j<gridSize; j++){
            waterVertex[i][j]->pos->y += sin((gameTime * waveSpeed) + ((waterVertex[i][j]->pos->x + 0.5* waterVertex[i][j]->pos->z) * waveAmount)) * waveHeight;
        }
    }
}

void TerrainGrid::calculateNormals()
{
    this->calculateNormals(this->gridVertex);
}

void TerrainGrid::calculateNormals(vector<vector<VertStruct*>> gridArg)
{
    for (int i = 0; i < gridSize; i++)
    {
        for (int j = 0; j < gridSize; j++)
        {
            vec3 tempVec;
            if (i - 1 > 0 && j + 1 < gridSize)
            {
                //printf("%f, %f, %f \n", gridArg[i+1][j]->pos->x, gridArg[i+1][j]->pos->y, gridArg[i+1][j]->pos->z);
                //printf("%f, %f, %f \n", gridArg[i][j]->pos->x, gridArg[i][j]->pos->y, gridArg[i][j]->pos->z);
                tempVec += cross(*gridArg[i - 1][j]->pos - *gridArg[i][j]->pos, *gridArg[i][j + 1]->pos - *gridArg[i][j]->pos);
                //printf("%f, %f, %f \n\n", tempVec.x, tempVec.y, tempVec.z);
            }
            if (i + 1 < gridSize && j + 1 < gridSize)
            {
                tempVec += cross(*gridArg[i][j + 1]->pos - *gridArg[i][j]->pos, *gridArg[i + 1][j]->pos - *gridArg[i][j]->pos);
            }
            if (i + 1 < gridSize && j - 1 > 0)
            {
                tempVec += cross(*gridArg[i + 1][j]->pos - *gridArg[i][j]->pos, *gridArg[i][j - 1]->pos - *gridArg[i][j]->pos);
            }
            if (i - 1 > 0 && j - 1 > 0)
            {
                tempVec += cross(*gridArg[i][j - 1]->pos - *gridArg[i][j]->pos, *gridArg[i - 1][j]->pos - *gridArg[i][j]->pos);
            }

            tempVec.normalize();
            gridArg[i][j]->normal = new vec3(tempVec.x, tempVec.y, tempVec.z);
        }
    }
}
