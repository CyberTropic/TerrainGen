#include <stdio.h>
#include <stdlib.h>
#include <vector>
using std::vector;
#include <math.h>
#include "vectorOps.h"

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

//float camPos[3] = {30.42f, 40.0f, 30.42f};
//float camTarget[3] = {0, 0, 0};
//float camDelta = 0.1;
//float camDist;
//float camTheta;

float camDist;
float camTwist;
float camElev;
float camAzimuth;
float camX = 0;
float camZ = 0;

int maxHeight = 1;
int minHeight = 1;
float range = 0;

char lightMode = 0; //Start with smooth lighting
float pos[4] = { 0,0,0,0 };
float pos2[4] = { 0,0,0,0 };
float m_dif[] = { 0.78, 0.57, 0.11, 1.0 };
float defaultmDif[4] = { 0.5, 0.5, 0.5, 1.0 }; //Default grey
float diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
float diffuse2[] = { 0.5, 0.5, 0.5, 1.0 };
float defaultDifuse[4] = { 0.5, 0.5, 0.5, 1.0 };
float no_mat[4] = { 0,0,0,1 };

int controlPos = 0; //Start in light 1 control

char gridMeshMode = 'y'; //Start in quads mode
int gridWireMode = 0;

int terrainTexture = 1; //Start in hypsometric

float wireCol[3] = { 0,1,0 };
float wireThic = 2;

int gridsize = 50;
vector<vector<float* > > grid;
vector<vector<float* > > gridColor;
vector<vector<float* > > gridColorH;
vector<vector<float* > > gridNormalsVec;
//vector<vector<float* > > gridWaterMesh;

//Color ramp values
float colorR1[3] = { 0.098, 0.756, 0.117 };
float colorR2[3] = { 0.937, 0.623, 0.149 };
float colorR3[3] = { 0.8, 0.8, 0.8 };
float offset = 0; //time elapsed
float tempColor[3] = { 0,0,0 };

int terrainAlg = 1;

int window1;
int window2;

//Helper functions

void introText(){
    std::cout<<"Hello, welcome to the terrain generator.\n"<<
    "CONTROLS:\n"<<
     "KEYBOARD\n" << 
     "f - switch wireframe mode (Note: switched from 'w' to make room for wasd controls)\n" << 
     "l - switch lighting mode\n" << 
     "r - regenerate terrain\n" << 
     "arrow keys - rotate camera around the center\n" << 
     ". , - zoom camera in/out\n" << 
     "wasd - move selected object along the axes\n" << 
     "\n" << 
     "RIGHT CLICK MENU\n" << 
     "change size - select new terrain size, default is 50x50\n" << 
     "generation - select terrain generation algorithm, will \nchange effect of regenerate terrain, default is circles\n" << 
     "	- smooth will smooth out the terrain by averaging each\n vertex with its neighbors\n" << 
     "control - select object to control: lights or camera\n" << 
     "texture - select mesh texture option:\n basic - flat texture,\n hypsometric - texture determined by height in multiple color ramps,\n"<<
     "greyscale - simple black-white color ramps,\n funky - cycling colors.\n Default is hypsometric.\n" << 
     "\n";
}

//Generate grid
void initGrid(int gridsize) {
	grid.resize(gridsize);
	gridColor.resize(gridsize);
	gridColorH.resize(gridsize);
	//gridWaterMesh.resize(gridsize);
	for (int i = 0; i < gridsize; i++) {
		grid[i].resize(gridsize);
		gridColor[i].resize(gridsize);
		gridColorH[i].resize(gridsize);
		//gridWaterMesh.resize(gridsize);
		for (int j = 0; j < gridsize; j++) {
			gridColor[i][j] = new float[3];
			gridColorH[i][j] = new float[3];
			grid[i][j] = new float[3];
			grid[i][j][X] = j;
			grid[i][j][Y] = 0;
			grid[i][j][Z] = i;
			/*gridWaterMesh[i][j] = new float[3];
			gridWaterMesh[i][j][X] = j;
			gridWaterMesh[i][j][Y] = 0;
			gridWaterMesh[i][j][Z] = i;*/
		}
	}

	//camX = ((float)gridsize / 2);
	//camZ = ((float)gridsize / 2);
	//    camTarget[0] = (gridsize / 2);
	//    camTarget[2] = (gridsize / 2);
	//    camPos[0] = (gridsize);
	//    camPos[2] = (gridsize);

}

//Clear array memory
void destroyGrid() { 
	for (int i; i < gridsize; i++) {
		for (int j; j < gridsize; j++) {
			delete[] grid[i][j];
			delete[] gridColor[i][j];
			delete[] gridColorH[i][j];
			delete[] gridNormalsVec[i][j];
		}
	}
}

void centerCam() {
	camX = 0;
	camZ = 0;
}

void resetLights() {
	pos[0] = 0; pos[1] = - (float)gridsize*1.2; pos[2] = 0;
	pos2[0] = -gridsize; pos2[1] = -gridsize * 0.8; pos2[2] = -gridsize;

}


void calculateNormalsQuads() {
	//std::cout << "Calculating Normals...\n";
	//    float normalVec;
	gridNormalsVec.resize(gridsize);
	for (int i = 0; i < gridsize; i++) {
		gridNormalsVec[i].resize(gridsize);
		for (int j = 0; j < gridsize; j++) {
			gridNormalsVec[i][j] = new float[3];
			gridNormalsVec[i][j][0] = 0;
			gridNormalsVec[i][j][1] = 0;
			gridNormalsVec[i][j][2] = 0;
		}
	}
	//    std::cout<<gridNormals[2][2][X];
	//std::cout << "Calculating Vectors...\n";
	for (int i = 0; i < gridsize - 1; i++) {
		for (int j = 0; j < gridsize - 1; j++) {

			//TODO: move vector algebra to a separate class
			float vecA[3] = { grid[i + 1][j][X] - grid[i][j][X], grid[i + 1][j][Y] - grid[i][j][Y], grid[i + 1][j][Z] - grid[i][j][Z] };
			float vecB[3] = { grid[i][j][X] - grid[i][j + 1][X], grid[i][j][Y] - grid[i][j + 1][Y], grid[i][j][Z] - grid[i][j + 1][Z] };
			float cross[3] = { vecA[1] * vecB[2] - vecA[2] * vecB[1],
							  vecA[2] * vecB[0] - vecA[0] * vecB[2],
							  vecA[0] * vecB[1] - vecA[1] * vecB[0] };
			float mag = sqrt(pow(cross[X], 2) + pow(cross[Y], 2) + pow(cross[Z], 2));
			float unitVec[3] = { cross[X] / mag, cross[Y] / mag, cross[Z] / mag };

			gridNormalsVec[i][j][X] = unitVec[X];
			gridNormalsVec[i][j][Y] = unitVec[Y];
			gridNormalsVec[i][j][Z] = unitVec[Z];

			//            std::cout<<"Calculating vector\n";
			//            std::cout<<unitVec[0]<<" "<<unitVec[1]<<" "<<unitVec[2]<<"\n;
		}
	}

	//Fix up lighting on edges - FIX LATER
	for (int i = 0; i < gridsize - 1; i++)
	{
		gridNormalsVec[gridsize - 1][i] = gridNormalsVec[gridsize - 2][i];
		gridNormalsVec[i][gridsize - 1] = gridNormalsVec[i][gridsize - 2];
	}
}


//Fault algorithm
void generateFaults(float height, int iterations) {
	for (int iter = 0; iter < iterations; iter++) {
		int startSide = rand() % 2; //Either line is left-right or top-bottom
		float slope;
		float intercept;
		float p1x;
		float p1y;
		float p2x;
		float p2y;
		if (startSide == 0) { //left-right
			p1x = (float)rand() / RAND_MAX * gridsize;
			p1y = 0;
			p2x = (float)rand() / RAND_MAX * gridsize;
			p2y = gridsize;
		}
		else { //top bottom
			p1x = 0;
			p1y = (float)rand() / RAND_MAX * gridsize;
			p2x = gridsize;
			p2y = (float)rand() / RAND_MAX * gridsize;
		}
		//        std::cout<<p1x<<","<<p1y<<" to "<<p2x<<","<<p2y<<"\n";
		slope = (p2y - p1y) / (p2x - p1x);
		intercept = p1y - slope * p1x;
		if (rand() % 2 == 0) { //Raise above line
			for (int i = 0; i < gridsize; i++) {
				for (int j = 0; j < gridsize; j++) {
					if ((float)i > (slope * (float)j + intercept)) {
						grid[i][j][Y] += height;
					}
					else {
						grid[i][j][Y] -= height;
					}
				}
			}
		}
		else { //Raise below line
			for (int i = 0; i < gridsize; i++) {
				for (int j = 0; j < gridsize; j++) {
					if ((float)i < (slope * (float)j + intercept)) {
						grid[i][j][Y] += height;
					}
					else {
						grid[i][j][Y] -= height;
					}
				}
			}
		}
	}
}

//Circles algorithm
void generateCircles(int radius, float height, int iterations) {
	for (int iter = 0; iter < iterations; iter++) {
		int randx = rand() % gridsize;
		int randz = rand() % gridsize;
		int up = rand() % 2;
		for (int i = 0; i < gridsize; i++) {
			for (int j = 0; j < gridsize; j++) {
				float pd = sqrt(pow((float)(j - randx), 2) + pow((float)(i - randz), 2));
				if (abs(pd) <= radius) {
					if (up == 1) {
						grid[i][j][Y] += height / 2 + cos(pd * 3.14 / radius) * height / 2;
					}
					else {
						grid[i][j][Y] -= height / 2 + cos(pd * 3.14 / radius) * height / 2;
					}
				}
				//                std::cout<<"("<<i<<","<<j<<")"<<grid[i][j][Y]<<"\n";
			}
		}
	}
}

void generateMidpoint(float jitter, float height) {
	grid[0][0][Y] = (float)rand() / RAND_MAX * height;
	grid[0][gridsize - 1][Y] = (float)rand() / RAND_MAX * height;
	grid[gridsize - 1][gridsize - 1][Y] = (float)rand() / RAND_MAX * height;
	grid[gridsize - 1][0][Y] = (float)rand() / RAND_MAX * height;
	int lastStep = gridsize;
	int step = gridsize / 2;
	while (step >= 1) {
		if (step + step < lastStep) { //if division is ODD
			grid[step][step][Y] = (grid[0][0][Y] + grid[0][2*step][Y] + grid[2*step][0][Y] + grid[2*step][2*step][Y]) / 4;
		}
		else {
			grid[step][step][Y] = (grid[0][0][Y] + grid[0][2*step-1][Y] + grid[2*step-1][0][Y] + grid[2*step-1][2*step-1][Y]) / 4;
		}
		// grid[step][step][Y] = (float)rand() / RAND_MAX;
		lastStep = step;
		step = step / 2;
	}
} 

void generateRandom(float jitter){
	for(int i = 0; i<gridsize; i++){
		for(int j = 0; j<gridsize;j++){
			grid[i][j][Y] = (float)rand() / RAND_MAX * jitter;
		}
	}
}

void smoothing(int iterations){
	vector<vector<float* > > newGrid;
	newGrid.resize(gridsize);
	for (int i = 0; i < gridsize; i++) {
		newGrid[i].resize(gridsize);
		for (int j = 0; j < gridsize; j++) {
			newGrid[i][j] = new float[3];
			newGrid[i][j][X] = j;
			newGrid[i][j][Y] = 0;
			newGrid[i][j][Z] = i;
		}
	}
	float newAvg;
	int neighbors;
	for (int iter=0; iter<iterations; iter++){
		for (int i=0; i<gridsize; i++){
			for (int j=0; j<gridsize;j++){
				newAvg=0;
				neighbors=0;
				newAvg = grid[i][j][Y]; //This cell
				neighbors += 1;
				if (j - 1 > 0) { //Top
					newAvg += grid[i][j - 1][Y];
				}
				if (j + 1 < gridsize) { //Bottom
					newAvg += grid[i][j + 1][Y];
				}
				if (i - 1 > 0) { //Left
					newAvg += grid[i - 1][j][Y];
					neighbors += 1;
					if (j - 1 > 0) { //Top-Left
						newAvg += grid[i - 1][j - 1][Y];
						neighbors += 1;
					}
					if (j + 1 < gridsize) { //Bottom-left 
						newAvg += grid[i - 1][j + 1][Y];
						neighbors += 1;
					}
				}
				if (i + 1 < gridsize) { //Right
					newAvg += grid[i + 1][j][Y];
					neighbors += 1;
					if (j - 1 > 0) { //Top-Right
						newAvg += grid[i + 1][j - 1][Y];
						neighbors += 1;
					}
					if (j + 1 < gridsize) { //Bottom-Right 
						newAvg += grid[i + 1][j + 1][Y];
						neighbors += 1;
					}
				}
				newGrid[i][j][Y] = newAvg / neighbors;
			}
		}
		for (int i = 0; i<gridsize; i++) {
			for (int j = 0; j<gridsize;j++) {
				grid[i][j][Y] = newGrid[i][j][Y];
			}
		}

	}

} 

//Calculate colourmap greyscale
void calculateColourMap(void) {
	maxHeight = 1;
	minHeight = 1;
	for (int i = 0; i < gridsize; i++) {
		for (int j = 0; j < gridsize; j++) {
			if (grid[i][j][Y] > maxHeight) {
				maxHeight = grid[i][j][Y];
			}
			if (grid[i][j][Y] < minHeight) {
				minHeight = grid[i][j][Y];
			}
		}
	}
	range = maxHeight - minHeight;
	float relHeight = 0; //Relative height min-max
	float rampHeight = 0; //Relative height within color ramp

	for (int i = 0; i < gridsize; i++) {
		for (int j = 0; j < gridsize; j++) {
			relHeight = (grid[i][j][Y] - minHeight) / range;
			gridColor[i][j][0] = relHeight;
			gridColor[i][j][1] = relHeight;
			gridColor[i][j][2] = relHeight;

		}
	}

	//Calculate colormap hypsometric
	for (int i = 0; i < gridsize; i++) {
		for (int j = 0; j < gridsize; j++) {
			relHeight = (grid[i][j][Y] - minHeight) / range;
			if (relHeight < 0.7) { //Bottom color 
				rampHeight = relHeight / 0.7;
				gridColorH[i][j][0] = colorR1[0] * (1 - rampHeight) + colorR2[0] * rampHeight;
				gridColorH[i][j][1] = colorR1[1] * (1 - rampHeight) + colorR2[1] * rampHeight;
				gridColorH[i][j][2] = colorR1[2] * (1 - rampHeight) + colorR2[2] * rampHeight;
			}
			else { //Top color 
				rampHeight = (relHeight - 0.7) * (1 / (1- 0.7));
				gridColorH[i][j][0] = colorR2[0] * (1 - rampHeight) + colorR3[0] * rampHeight;
				gridColorH[i][j][1] = colorR2[1] * (1 - rampHeight) + colorR3[1] * rampHeight;
				gridColorH[i][j][2] = colorR2[2] * (1 - rampHeight) + colorR3[2] * rampHeight;
			}
		}
	}

}

//initialization
void init(void) {


	//Initialize grid
	initGrid(gridsize);
	if (terrainAlg == 1) {
		generateCircles(15, 5, 20);
	}
	else if (terrainAlg == 2) {
		generateFaults(0.4, 10);
	}
	else if (terrainAlg == 3) {
		generateMidpoint(0.4, 10);
	}
	else if (terrainAlg == 5) {
		generateRandom(20);
	}
	calculateColourMap();
	calculateNormalsQuads();


	//Initialize display
	camDist = -1.2*gridsize;
	camTwist = 0;
	camElev = -45;
	camAzimuth = 0;

	glClearColor(0.3, 0.3, 0.5, 0);
	glColor3f(1, 1, 1);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 1000);
	
	glEnable(GL_CULL_FACE);
	//Light
	
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
	}
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	
	//Light 1
	float ambient[4] = { 0,0.1,0,0.1 };
	diffuse[0] = 0.8; diffuse[1] = 0.8; diffuse[2] = 0.8;
	float spec[4] = { 1,1,1,1 };
	pos[0] = 0; pos[1] = -gridsize * 1.2; pos[2] = 0;

	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

	//Light 
	float ambient2[4] = { 0,0.1,0,0.1 };
	diffuse2[0] = 0.5; diffuse2[1] = 0.1; diffuse2[2] = 0.04;
	float spec2[4] = { 1,0,0,1 };
	pos2[0] = -gridsize; pos2[1] = -gridsize * 0.8; pos2[2] = -gridsize;

	glLightfv(GL_LIGHT1, GL_POSITION, pos2);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec2);

	//Default materials
	float m_amb[] = { 0.33, 0.22, 0.03, 1 };
	m_dif[0] = 0.78; m_dif[1] = 0.57; m_dif[2] = 0.11; m_dif[3] = 1.0;
	float m_spec[] = { 0.2, 0.2, 0.2, 1.0 };
	float m_em[] = { 0,1,0,1 };
	float shiny = 27; //10, 100

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_spec);
	//        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_em);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shiny);


	//	myMesh = new Mesh();
	//	myMesh->InitCube(1); 
}

//MINIMAP
void init2(void) {
	glClearColor(0.3, 0.3, 0.5, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, gridsize, 0, gridsize);
}

//Drawcall for quad mode
void drawGridQuads(void) {

	if (lightMode != 2) {
		glBegin(GL_QUADS);
		for (int i = 0; i < gridsize - 1; i++) {
			for (int j = 0; j < gridsize - 1; j++) {
				if (terrainTexture == 0) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultmDif);
				}
				else if (terrainTexture == 1) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gridColorH[i][j]);
				}
				else if (terrainTexture == 2) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gridColor[i][j]);
				}
				else if (terrainTexture == 3) {
					offset = (float)glutGet(GLUT_ELAPSED_TIME)/1000;
					tempColor[0] = 0.5 + 0.5 * sin(gridColor[i][j][0] * 1.6 + offset);
					tempColor[1] = 0.5 + 0.5 * sin(gridColor[i][j][1] * 2.6 + offset + 2);
					tempColor[2] = 0.5 + 0.5 * sin(gridColor[i][j][2] * 3.6 + offset + 4);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tempColor );
				}
				glNormal3fv(gridNormalsVec[i][j]);
				glVertex3fv(grid[i][j]);
				glNormal3fv(gridNormalsVec[i + 1][j]);
				glVertex3fv(grid[i + 1][j]);
				glNormal3fv(gridNormalsVec[i + 1][j + 1]);
				glVertex3fv(grid[i + 1][j + 1]);
				glNormal3fv(gridNormalsVec[i][j + 1]);
				glVertex3fv(grid[i][j + 1]);
			}
		}
		glEnd();
	}
	else {
		glBegin(GL_QUADS);
		for (int i = 0; i < gridsize - 1; i++) {
			for (int j = 0; j < gridsize - 1; j++) {
				glColor3fv(gridColor[i][j]);
				glNormal3fv(gridNormalsVec[i][j]);
				glVertex3fv(grid[i][j]);
				glColor3fv(gridColor[i + 1][j]);
				glNormal3fv(gridNormalsVec[i + 1][j]);
				glVertex3fv(grid[i + 1][j]);
				glColor3fv(gridColor[i + 1][j + 1]);
				glNormal3fv(gridNormalsVec[i + 1][j + 1]);
				glVertex3fv(grid[i + 1][j + 1]);
				glColor3fv(gridColor[i][j + 1]);
				glNormal3fv(gridNormalsVec[i][j + 1]);
				glVertex3fv(grid[i][j + 1]);
			}
		}
		glEnd();
	}
}

//Drawcall for tri mode
void drawGridTris(void) {

	if (lightMode != 2) {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < gridsize - 1; i++) {
			for (int j = 0; j < gridsize - 1; j++) {
				//Triangle 
				if (terrainTexture == 0) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultmDif);
				}
				else if (terrainTexture == 1) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gridColorH[i][j]);
				}
				else if (terrainTexture == 2) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gridColor[i][j]);
				}
				else if (terrainTexture == 3) {
					offset = (float)glutGet(GLUT_ELAPSED_TIME) / 1000;
					tempColor[0] = 0.5 + 0.5 * sin(gridColor[i][j][0] * 1.6 + offset);
					tempColor[1] = 0.5 + 0.5 * sin(gridColor[i][j][1] * 2.6 + offset + 2);
					tempColor[2] = 0.5 + 0.5 * sin(gridColor[i][j][2] * 3.6 + offset + 4);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tempColor);
				}
				glNormal3fv(gridNormalsVec[i][j]);
				glVertex3fv(grid[i][j]);
				glNormal3fv(gridNormalsVec[i + 1][j]);
				glVertex3fv(grid[i + 1][j]);
				glNormal3fv(gridNormalsVec[i][j + 1]);
				glVertex3fv(grid[i][j + 1]);
				//Triangle 
				if (terrainTexture == 0) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultmDif);
				}
				else if (terrainTexture == 1) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gridColorH[i + 1][j]);
				}
				else if (terrainTexture == 2) {
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, gridColor[i + 1][j]);
				}
				else if (terrainTexture == 3) {
					offset = (float)glutGet(GLUT_ELAPSED_TIME) / 1000;
					tempColor[0] = 0.5 + 0.5 * sin(gridColor[i][j][0] * 1.6 + offset);
					tempColor[1] = 0.5 + 0.5 * sin(gridColor[i][j][1] * 2.6 + offset + 2);
					tempColor[2] = 0.5 + 0.5 * sin(gridColor[i][j][2] * 3.6 + offset + 4);
					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tempColor);
				}
				glNormal3fv(gridNormalsVec[i + 1][j]);
				glVertex3fv(grid[i + 1][j]);
				glNormal3fv(gridNormalsVec[i + 1][j + 1]);
				glVertex3fv(grid[i + 1][j + 1]);
				glNormal3fv(gridNormalsVec[i][j + 1]);
				glVertex3fv(grid[i][j + 1]);
			}
		}
		glEnd();
	}
	else {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < gridsize - 1; i++) {
			for (int j = 0; j < gridsize - 1; j++) {
				//Triangle 1
				glColor3fv(gridColor[i][j]);
				glNormal3fv(gridNormalsVec[i][j]);
				glVertex3fv(grid[i][j]);
				glColor3fv(gridColor[i + 1][j]);
				glNormal3fv(gridNormalsVec[i + 1][j]);
				glVertex3fv(grid[i + 1][j]);
				glColor3fv(gridColor[i][j + 1]);
				glNormal3fv(gridNormalsVec[i][j + 1]);
				glVertex3fv(grid[i][j + 1]);
				//Triangle 2
				glColor3fv(gridColor[i + 1][j]);
				glNormal3fv(gridNormalsVec[i + 1][j]);
				glVertex3fv(grid[i + 1][j]);
				glColor3fv(gridColor[i + 1][j + 1]);
				glNormal3fv(gridNormalsVec[i + 1][j + 1]);
				glVertex3fv(grid[i + 1][j + 1]);
				glColor3fv(gridColor[i][j + 1]);
				glNormal3fv(gridNormalsVec[i][j + 1]);
				glVertex3fv(grid[i][j + 1]);
			}
		}
		glEnd();
	}
}

//Wireframe for tri
void drawWireTris(void) {

	if (lightMode == 2) {
		glColor3fv(wireCol);
	}
	else {
		m_dif[0] = 0; m_dif[1] = 1; m_dif[2] = 0;
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_dif);
	}
	glLineWidth(wireThic);

	for (int i = 0; i < gridsize - 1; i++) {
		for (int j = 0; j < gridsize - 1; j++) {

			glBegin(GL_LINE_STRIP);
			//Triangle 1
			glVertex3fv(grid[i][j]);
			glVertex3fv(grid[i + 1][j]);
			glVertex3fv(grid[i][j + 1]);
			glVertex3fv(grid[i][j]);
			glEnd();
			glBegin(GL_LINE_STRIP);
			//Triangle 2
			glVertex3fv(grid[i + 1][j]);
			glVertex3fv(grid[i + 1][j + 1]);
			glVertex3fv(grid[i][j + 1]);
			glVertex3fv(grid[i + 1][j]);
			glEnd();
		}
	}
	glEnd();
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);
}

//Wireframe for quad
void drawWireQuads(void) {

	if (lightMode == 2) {
		glColor3fv(wireCol);
	}
	else {
		m_dif[0] = 0; m_dif[1] = 1; m_dif[2] = 0;
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m_dif);
	}

	glColor3fv(wireCol);
	glLineWidth(wireThic);

	for (int i = 0; i < gridsize - 1; i++) {
		for (int j = 0; j < gridsize - 1; j++) {

			glBegin(GL_LINE_STRIP);
			glVertex3fv(grid[i][j]);
			glVertex3fv(grid[i + 1][j]);
			glVertex3fv(grid[i + 1][j + 1]);
			glVertex3fv(grid[i][j + 1]);
			glVertex3fv(grid[i][j]);
			glEnd();
		}
	}
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, no_mat);

}

void drawMinimap() {
	//    glBegin(GL_QUADS);
	//    glVertex2f(0,0);
	//    glVertex2f(0, gridsize);
	//    glVertex2f(gridsize, gridsize);
	//    glVertex2f(gridsize, 0);
	//    glEnd();
	float c1, c2, c3, c4;
	for (int i = 0; i < gridsize - 1; i++) {
		for (int j = 0; j < gridsize - 1; j++) {
			glBegin(GL_QUADS);
			c1 = (grid[i][j][Y] - minHeight) / range;
			glColor3f(c1, c1, c1);
			glVertex2f(i, j);
			c2 = (grid[i][j + 1][Y] - minHeight) / range;
			glColor3f(c2, c2, c2);
			glVertex2f(i, j + 1);
			c3 = (grid[i + 1][j + 1][Y] - minHeight) / range;
			glColor3f(c3, c3, c3);
			glVertex2f(i + 1, j + 1);
			c4 = (grid[i + 1][j][Y] - minHeight) / range;
			glColor3f(c4, c4, c4);
			glVertex2f(i + 1, j);
			glEnd();
		}
	}
}


void orbitView(float dist, float twist, float elev, float azimuth) {
	glTranslatef(0.0, 0.0, dist);
	glRotatef(-twist, 0.0, 0.0, 1.0);
	glRotatef(-elev, 1.0, 0.0, 0.0);
	glTranslatef(-camX, 0, 0);
	glTranslatef(0, 0, -camZ);
	glRotatef(azimuth, 0.0, 1.0, 0.0);
	glTranslatef(-(float)gridsize / 2, 0, -(float)gridsize/2);
}

/* display function - GLUT display callback function
 *		clears the screen, sets the camera position, draws the ground plane and movable box
 */
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

	if (camDist > 1) {
		camDist = 1;
	}
	if (camElev < -85) {
		camElev = -85;
	}
	else if (camElev > 85) {
		camElev = 85;
	}
	orbitView(camDist, camTwist, camElev, camAzimuth);


	//Place light + light indicator
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glPushMatrix();
		glTranslatef(-pos[0], -pos[1], -pos[2]);
		glutSolidSphere(1.0, 5, 5);
	glPopMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION, pos2);
	glPushMatrix();
		glTranslatef(-pos2[0], -pos2[1], -pos2[2]);
		glutSolidSphere(1.0, 5, 5);
	glPopMatrix();

	switch (gridMeshMode) {
	case 't':
		switch (gridWireMode) {
		case 0:
			drawGridTris();
			break;
		case 1:
			drawWireTris();
			break;
		case 2:
			drawWireTris();
			drawGridTris();
			break;
		}
		break;
	case 'y':
		switch (gridWireMode) {
		case 0:
			drawGridQuads();
			break;
		case 1:
			drawWireQuads();
			break;
		case 2:
			drawWireQuads();
			drawGridQuads();
			break;
		}
		break;
	}

	glutSwapBuffers();
}


//Draw call for minimap window
void display2() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//    std::cout<<"Minimap update\n";
	drawMinimap();

	glutSwapBuffers();
}


void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(0, w, 0, h);
	gluPerspective(45, (float)((w + 0.0f) / h), 1, 1000);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void FPSTimer(int value) { //60fps
	glutTimerFunc(17, FPSTimer, 0);
	glutSetWindow(window1);
	glutPostRedisplay();
	//    std::cout<<"Timer update\n";
}

void keyboard(unsigned char key, int xIn, int yIn) {
	int mod = glutGetModifiers();
	switch (key) {
	case 'q':
	case 27: //27 is the esc key
		exit(0);
		break;
	case 'y': //quads
		std::cout << "Changing mesh mode: quads\n";
		gridMeshMode = 'y';
		break;
	case 't': //tris
		std::cout << "Changing mesh mode: tris\n";
		gridMeshMode = 't';
		break;
	case 'f': //wireframe
		gridWireMode += 1;
		gridWireMode = gridWireMode % 3;
		break;
	case 'r': //reload
		grid.clear();
		gridColor.clear();
		destroyGrid();
		initGrid(gridsize);
		centerCam();
		if (terrainAlg == 1) {
			generateCircles(15, 5, 20 * gridsize / 50 * gridsize / 50);
		}
		else if (terrainAlg == 2) {
			generateFaults(0.4, 20 * gridsize / 50 * gridsize / 50);
		}
		else if (terrainAlg == 3) {
			generateFaults(0.4, 20 * gridsize / 50 * gridsize / 50);
			generateCircles(15, 3, 20 * gridsize / 50 * gridsize / 50);
		}
		else if (terrainAlg == 4) {
			generateMidpoint(0.4, 30);
		}
		else if (terrainAlg == 5) {
			generateRandom(10);
		}
		calculateNormalsQuads();
		calculateColourMap();
		resetLights();
		glutSetWindow(window2);
		glutPostRedisplay();
		//reposition the camera
		break;
	case 'l': //lighting mode
		lightMode += 1;
		lightMode = lightMode % 3;
		std::cout << "Changing light mode: " << (int)lightMode << "\n";
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
		break;

		//Camera zoom
	case '.':
		camDist += 1;
		break;
	case ',':
		camDist -= 1;
		break;
	

	//Light controls
	case 'w':
		if (controlPos == 0) {
			pos[X] += 0.5;
		}
		else if (controlPos == 1) {
			pos2[X] += 0.5;
		}
		else if (controlPos == 2) {
			camZ -= 0.5;
		}
		break;
	case 's':
		if (controlPos == 0) {
			pos[X] -= 0.5;
		}
		else if (controlPos == 1) {
			pos2[X] -= 0.5;
		}
		else if (controlPos == 2) {
			camZ += 0.5;
		}
		break;
	case 'a':
		if (controlPos == 0) {
			pos[Z] -= 0.5;
		}
		else if (controlPos == 1) {
			pos2[Z] -= 0.5;
		}
		else if (controlPos == 2) {
			camX -= 0.5;
		}
		break;
	case 'd':
		if (controlPos == 0) {
			pos[Z] += 0.5;
		}
		else if (controlPos == 1) {
			pos2[Z] += 0.5;
		}
		else if (controlPos == 2) {
			camX += 0.5;
		}
		break;
	}
}

void special(int key, int xIn, int yIn) {
	switch (key) {
	case GLUT_KEY_DOWN:
		camElev -= 1;
		//            camPos[Y] -= 0.2f;
					//			camTarget[Y] -= 0.2f;
		break;
	case GLUT_KEY_UP:
		camElev += 1;
		//            camPos[Y] += 0.2f;
					//			camTarget[Y] += 0.2f;
		break;
	case GLUT_KEY_LEFT:
		camAzimuth -= 1;
                //Tried to implement it without using matrix transformations, only rotation around y axis works.
                
		//            glMatrixMode(GL_MODELVIEW);
		//            glLoadIdentity();
		//            glRotatef(10, 1, 0, 0);
		//            std::cout<<"Target: "<<camTarget[X]<<","<<camTarget[Z]<<"\n"; 
		//            std::cout<<"Old: "<<camPos[X]<<","<<camPos[Z]<<"\n";
		//            camDist = sqrt(pow((camPos[X] - camTarget[X]), 2) + pow((camPos[Z] - camTarget[Z]), 2));
		//            camTheta = asin((camPos[Z]-camTarget[Z]) / camDist);
		//            if (camPos[X]<camTarget[X]){
		//                camTheta = 3.1415926 - camTheta;
		//            }
		//            camPos[X] = camDist*cos(camTheta+camDelta) + camTarget[X];
		//            camPos[Z] = camDist*sin(camTheta+camDelta) + camTarget[Z];
		//            std::cout<<"Theta: "<<camTheta<<"; Hyp: "<<camDist<<"\n";
		//            std::cout<<"New: "<<camPos[X]<<","<<camPos[Z]<<"\n\n";
		//            camPos[X] -= 0.2f;
					//			camTarget[X] -= 0.2f;
		break;
	case GLUT_KEY_RIGHT:
		camAzimuth += 1;
		//            std::cout<<"Target: "<<camTarget[X]<<","<<camTarget[Z]<<"\n"; 
		//            std::cout<<"Old: "<<camPos[X]<<","<<camPos[Z]<<"\n";
		//            camDist = sqrt(pow((camPos[X] - camTarget[X]), 2) + pow((camPos[Z] - camTarget[Z]), 2));
		//            camTheta = asin((camPos[Z]-camTarget[Z]) / camDist);
		//            if (camPos[X]<camTarget[X]){
		//                camTheta = 3.1415926 - camTheta;
		//            }
		//            camPos[X] = camDist*cos(camTheta-camDelta) + camTarget[X];
		//            camPos[Z] = camDist*sin(camTheta-camDelta) + camTarget[Z];
		//            std::cout<<"Theta: "<<camTheta<<"; Hyp: "<<camDist<<"\n";
		//            std::cout<<"New: "<<camPos[X]<<","<<camPos[Z]<<"\n\n";
		//            camPos[X] += 0.2f;
					//			camTarget[X] += 0.2f;
		break;
	}
}

void menuProc1(int value) {
	if (value == 11) {
		gridsize = 50;
	}
	else if (value == 12) {
		gridsize = 100;
	}
	else if (value == 13) {
		gridsize = 150;
	}
	else if (value == 14) {
		gridsize = 200;
	}
	else if (value == 15) {
		gridsize = 300;
	}
	destroyGrid();
	initGrid(gridsize);
	calculateNormalsQuads();
	calculateColourMap();
	//rezoom camera
	camDist = -1.2*gridsize;

	//reshape minimap
	glutSetWindow(window2);
	glutReshapeWindow(gridsize, gridsize);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, gridsize, 0, gridsize);
	glutPostRedisplay();
}

void menuProc2(int value) {
	if (value == 21) { //Circles
		terrainAlg = 1;
	}
	else if (value == 22) { //Faults
		terrainAlg = 2;
	}
	else if (value == 23) { //Combo
		terrainAlg = 3;
	}
	else if (value == 24) { //Combo
		terrainAlg = 4;
	}
	else if (value == 25) { //Random
		terrainAlg = 5;
	}
	else if (value == 29) { //Smooth
		smoothing(1);
		calculateColourMap();
		calculateNormalsQuads();
		glutSetWindow(window2);
		glutPostRedisplay();
	}
}

void menuProc3(int value) {
	if (value == 31) {
		controlPos = 0;
	}
	else if (value == 32) {
		controlPos = 1;
	}
	else if (value == 33) {
		controlPos = 2;
	}
}

void menuProc4(int value) {
	if (value == 41) {
		terrainTexture = 0;
	}
	else if (value == 42) {
		terrainTexture = 1;
	}
	else if (value == 43) {
		terrainTexture = 2;
	}
	else if (value == 44) {
		terrainTexture = 3;
	}
}

void createMenu() {

	int subMenu_id1 = glutCreateMenu(menuProc1);
	glutAddMenuEntry("50x50", 11);
	glutAddMenuEntry("100x100", 12);
	glutAddMenuEntry("150x150", 13);
	glutAddMenuEntry("200x200", 14);
	glutAddMenuEntry("300x300", 15);

	int subMenu_id2 = glutCreateMenu(menuProc2);
	glutAddMenuEntry("Circles", 21);
	glutAddMenuEntry("Faults", 22);
	glutAddMenuEntry("Combination", 23);
	glutAddMenuEntry("Midpoint", 24);
	glutAddMenuEntry("Random", 25);
	glutAddMenuEntry("Smooth", 29);

	int subMenu_id3 = glutCreateMenu(menuProc3);
	glutAddMenuEntry("Light 1", 31);
	glutAddMenuEntry("Light 2", 32);
	glutAddMenuEntry("Camera", 33);

	int subMenu_id4 = glutCreateMenu(menuProc4);
	glutAddMenuEntry("Basic", 41);
	glutAddMenuEntry("Hypsometric", 42);
	glutAddMenuEntry("Greyscale", 43);
	glutAddMenuEntry("Funky", 44);

	int main_id = glutCreateMenu(menuProc1);
	glutAddSubMenu("Change Size", subMenu_id1);
	glutAddSubMenu("Generation", subMenu_id2);
	glutAddSubMenu("Control", subMenu_id3);
	glutAddSubMenu("Texture", subMenu_id4);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

}

int main(int argc, char** argv) {
	glutInit(&argc, argv); //starts up GLUT
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	glutInitWindowSize(400, 400);
	glutInitWindowPosition(200, 100);
	window1 = glutCreateWindow("Terrain"); //creates the window
	//display callback
	glutDisplayFunc(display);
	//keyboard callback
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);

	//mouse callbacks
	//	glutMouseFunc(mouse);
	//	glutMotionFunc(mouseMotion);
	//	glutPassiveMotionFunc(mousePassiveMotion);

	//resize callback
	glutReshapeFunc(reshape);

	//fps timer callback
	glutTimerFunc(17, FPSTimer, 0);

	init();

	createMenu();

	glEnable(GL_DEPTH_TEST);

	//Minimap
	glutInitWindowSize(gridsize, gridsize);
	glutInitWindowPosition(50, 200);
	window2 = glutCreateWindow(argv[0]);
	glutSetWindowTitle("Minimap");
	init2();
	glutDisplayFunc(display2);
	//    glutMouseFunc(mouse2);

        introText(); //We booted successfully so display intro text 

	glutMainLoop(); //starts the event glutMainLoop
	return (0); //return may not be necessary on all compilers
}