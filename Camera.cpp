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

#include "Camera.h"

Camera::Camera()
{
    camPos = new vec3;
    camPos->x = 0;
    camPos->y = 5;
    camPos->z = 40;
    camTarget = new vec3;
    camTarget->x = 0.0f;
    camTarget->y = 0.0f;
    camTarget->z = 0.0f;

    camDist = 40;
    camTwist = 0;
    camElev = 40;
    camAzimuth = 40;
}

void Camera::setTargetObject(vec3* targetPos){
    //printf("Unimplemented");
    camTarget = targetPos;
}

void Camera::update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,0,0,0,0,-1,0,1,0);
    orbitView();
}

void Camera::orbitView(void)
{
    glLoadIdentity();
    if (this->camDist < 1) {
        camDist = 1;
    }
    if (camElev < 1) {
        camElev = 1;
    } else if (camElev > 85) {
        camElev = 85;
    }

	glTranslatef(0.0, 0.0, -camDist);
	glRotatef(camElev, 1.0, 0.0, 0.0);
	glRotatef(-camAzimuth, 0.0, 1.0, 0.0);
	glRotatef(camTwist, 0.0, 0.0, 1.0);
	glTranslatef(-camTarget->x, 0, 0);
	glTranslatef(0, -camTarget->y, 0);
    glTranslatef(0, 0, -camTarget->z);
}
