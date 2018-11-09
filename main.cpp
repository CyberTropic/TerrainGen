#include <stdio.h>
#include <stdlib.h>
#include <vector>
using std::vector;
#include <math.h>

//#include "vertStruct.h"
#include "vectorOps.h"
#include "Camera.h"
#include "TerrainGrid.h"
#include "TerrainGen.h"

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

#define X 0
#define Y 1
#define Z 2
#define W 3

//Light 0 properties
float l0pos[4] = {0.25f,0.5f,0.25f,0};
float l0dif[4] = {0.7f,0.7f,0.7f,0};
float l0amb[4] = {1.0f,1.0f,1.0f,0};
float l0spe[4] = {1,1,1,1};

float gameTime;
bool debugMode = true;
bool debugModeNormals = false;
Camera* orbitCam;
TerrainGrid* terrain;
int lightMode = 0;
float defaultmDif[4] = { 0.5, 0.5, 0.5, 1.0 }; //Default grey

void setLighting(){
    if (lightMode == 0) {
        glEnable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
    }
    else if (lightMode == 1) {
        glEnable(GL_LIGHTING);
        glShadeModel(GL_FLAT);
    }
    else if (lightMode == 2) {
        glDisable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
    }
}

void init(void){
    gameTime = 0;
    glClearColor(0, 0, 0, 0);
    glColor3f(1,1,1);

    glMatrixMode(GL_PROJECTION);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    setLighting();
    glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
	glEnable(GL_TEXTURE_2D);

	//Apply light 0 settings
	glLightfv(GL_LIGHT0, GL_POSITION, l0pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l0spe);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 1000);

    orbitCam = new Camera();
    std::cout<<"Generating terrain...\n";
    terrain = new TerrainGrid(50);
    std::cout<<"Terrain done.\n";
    orbitCam->setTargetObject(terrain->centerPoint);
    genCircles(terrain, 15, 5, 20);
    clampTerrain(terrain, 0, 9999);
    terrain->setWaterLevel(5);
    std::cout<<"Generating terrain normals...\n";
    terrain->calculateNormals(terrain->gridVertex);
    std::cout<<"Normals done.\n";
}

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    orbitCam->update();

    glPushMatrix();
        if(debugMode){
            glColor3f(0,0,1);
            //glutSolidTeapot(1);
        }
        terrain->drawTerrain(defaultmDif);
        terrain->drawTerrainEdges(defaultmDif);
        terrain->drawWater(terrain->waterColor);
        terrain->drawWaterEdges(terrain->waterColorDark);
        if(debugModeNormals) {terrain->debugDrawNormals();}
    glPopMatrix();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int xIn, int yIn)
{
    int mod = glutGetModifiers();
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case '.':
        orbitCam->camDist += 1;
        break;
    case ',':
        orbitCam->camDist -= 1;
        break;
    case 'w':
        orbitCam->camElev += 1;
        break;
    case 's':
        orbitCam->camElev -= 1;
        break;
    case 'a':
        orbitCam->camAzimuth += 1;
        break;
    case 'd':
        orbitCam->camAzimuth -= 1;
        break;
    case 'l': //lighting mode
		lightMode = (lightMode + 1) % 3;
        setLighting();
		break;
    case 'r': //lighting mode
        genFlat(terrain, 0);
        genCircles(terrain, 15, 5, 20);
        clampTerrain(terrain, 0, 9999);
        terrain->calculateNormals();
        break;
    case 'p': //lighting mode
        terrain->setWaterLevel(terrain->waterLevel+= 0.25);
        break;
    case 'o': //lighting mode
        terrain->setWaterLevel(terrain->waterLevel-= 0.25);
        break;
    }
}

void special(int key, int xIn, int yIn){

}

void mouseClick(int btn, int state, int x, int y){

}

void mouseMove(int x, int y){

}

void gameUpdate() {
    gameTime += 17;
    terrain->updateWater(gameTime);
}

void FPSTimer(int value) { //60fps
	glutTimerFunc(17, FPSTimer, 0);
	//glutSetWindow(window1);
    gameUpdate();
	glutPostRedisplay();
	//std::cout<<"Timer update\n";
}

void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w, 0, h);
	gluPerspective(45, (float)((w + 0.0f) / h), 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

/* main function - program entry point */
int main(int argc, char** argv)
{
    //srand(time(NULL));
	glutInit(&argc, argv);		//starts up GLUT
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    //Main Window
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(50, 50);
	int mainWindow = glutCreateWindow("Terrain");	//creates the window

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	//mouse callbacks
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMove);
	glutReshapeFunc(reshape);
	glutTimerFunc(17, FPSTimer, 0);

	init();

	glutMainLoop();				//starts the event glutMainLoop
	return(0);					//return may not be necessary on all compilers
}
