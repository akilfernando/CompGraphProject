#pragma once

//include libaries
#include <GL/freeglut.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//define constants
#define MAP_DIM 65
#define MOUNTROWS 1280
#define MOUNTCOLS 1104
#define SEAROWS 1600
#define SEACOLS 1200
#define SKYROWS 896
#define SEACOLS 385
#define PLANE_DIM 100
#define	PLANE_WIDTH 10000
#define MOD_PROB 1.6
#define MAG_DIV 2

//map arrays
GLfloat heightMap[MAP_DIM][MAP_DIM];
GLfloat terrainCoords0[MAP_DIM][MAP_DIM][3];
GLfloat terrainCoords1[MAP_DIM][MAP_DIM][3];
GLfloat terrainCoords2[MAP_DIM][MAP_DIM][3];
GLfloat planeCoords[PLANE_DIM][PLANE_DIM][3];

//terrain material variables
GLfloat terrainDiffuse[4] = { 1, 0.5, 0.5, 1  };
GLfloat terrainEmission[4] = { 0,0,0,1 };
GLfloat terrainAmbient[4] = { 0,0,0,1 };
GLfloat terrainSpecular[] = { 0.1, 0.1, 0.1, 1.0 };
GLfloat terrainShininess = 5;

//initialize the map arrays
void initializeTerrain(GLuint index) {
	for (GLint i = 0; i < MAP_DIM; i++) {
		for (GLint j = 0; j < MAP_DIM; j++) {
			heightMap[i][j] = 0;
			if (index == 0) {
				terrainCoords0[i][j][0] = 0;
				terrainCoords0[i][j][1] = 0;
				terrainCoords0[i][j][2] = 0;
			}
			else if (index == 1) {
				terrainCoords1[i][j][0] = 0;
				terrainCoords1[i][j][1] = 0;
				terrainCoords1[i][j][2] = 0;
			}
			else if (index == 2) {
				terrainCoords2[i][j][0] = 0;
				terrainCoords2[i][j][1] = 0;
				terrainCoords2[i][j][2] = 0;
			}
		}
	}
}

//debugging function to print the height map values
void printHeightMap() {
	for (GLint i = 0; i < MAP_DIM; i++) {
		for (GLint j = 0; j < MAP_DIM; j++) {
			printf("%f\t", heightMap[i][j]);
		}
		printf("\n");
	}
}

//debugging function to print the coordinates of terrain0
void printTerrainCoords() {
	for (GLint i = 0; i < MAP_DIM; i++) {
		for (GLint j = 0; j < MAP_DIM; j++) {
			if (terrainCoords0[i][j][1] == -1) break;
			printf("%f,", terrainCoords0[i][j][0]);
			printf("%f,", terrainCoords0[i][j][1]);
			printf("%f\t", terrainCoords0[i][j][2]);
		}
		printf("\n");
	}
}

//recursive function to create the heightmap of a mountain
void recursiveHeightMod(GLint maxRec, GLint currentRec, float modMag, GLint lowerPointerX, GLint upperPointerX, GLint lowerPointerZ, GLint upperPointerZ) {

	if (currentRec == maxRec) {
		return;
	}

	GLint midpointZ = floorf(((float)lowerPointerZ + (float)upperPointerZ) / 2);
	GLint midpointX = floorf(((float)lowerPointerX + (float)upperPointerX) / 2);

	float randNum = 1 + (rand() / (float)RAND_MAX);
	float moddedMod = modMag;
	if (randNum < MOD_PROB && currentRec > 0) moddedMod = -modMag * randNum;
	else moddedMod = modMag * randNum;

	heightMap[midpointZ][midpointX] += moddedMod;

	for (GLint i = lowerPointerZ; i < midpointZ + 1; i++) {
		for (GLint j = lowerPointerX; j < midpointX + 1; j++) {
			if (i == midpointZ && j == midpointX) continue;
			float totalDistance = sqrtf((powf(midpointX - lowerPointerX, 2) + powf(midpointZ - lowerPointerZ, 2)));
			float currentDistance = sqrtf((powf(j - midpointX, 2) + powf(i - midpointZ, 2)));
			heightMap[i][j] += moddedMod * ((totalDistance - currentDistance) / totalDistance);
		}
	}
	for (GLint i = midpointZ + 1; i < upperPointerZ + 1; i++) {
		for (GLint j = lowerPointerX; j < midpointX + 1; j++) {
			if (i == midpointZ && j == midpointX) continue;
			float totalDistance = sqrtf((powf(midpointX - lowerPointerX, 2) + powf(midpointZ - lowerPointerZ, 2)));
			float currentDistance = sqrtf((powf(j - midpointX, 2) + powf(i - midpointZ, 2)));
			heightMap[i][j] += moddedMod * ((totalDistance - currentDistance) / totalDistance);
		}
	}
	for (GLint i = midpointZ + 1; i < upperPointerZ + 1; i++) {
		for (GLint j = midpointX + 1; j < upperPointerX + 1; j++) {
			if (i == midpointZ && j == midpointX) continue;
			float totalDistance = sqrtf((powf(midpointX - lowerPointerX, 2) + powf(midpointZ - lowerPointerZ, 2)));
			float currentDistance = sqrtf((powf(j - midpointX, 2) + powf(i - midpointZ, 2)));
			heightMap[i][j] += moddedMod * ((totalDistance - currentDistance) / totalDistance);
		}
	}
	for (GLint i = lowerPointerZ; i < midpointZ + 1; i++) {
		for (GLint j = midpointX + 1; j < upperPointerX + 1; j++) {
			if (i == midpointZ && j == midpointX) continue;
			float totalDistance = sqrtf((powf(midpointX - lowerPointerX, 2) + powf(midpointZ - lowerPointerZ, 2)));
			float currentDistance = sqrtf((powf(j - midpointX, 2) + powf(i - midpointZ, 2)));
			heightMap[i][j] += moddedMod * ((totalDistance - currentDistance) / totalDistance);
		}
	}

	recursiveHeightMod(maxRec, currentRec + 1, modMag / MAG_DIV, midpointX, upperPointerX, lowerPointerZ, midpointZ);
	recursiveHeightMod(maxRec, currentRec + 1, modMag / MAG_DIV, lowerPointerX, midpointX, lowerPointerZ, midpointZ);
	recursiveHeightMod(maxRec, currentRec + 1, modMag / MAG_DIV, lowerPointerX, midpointX, midpointZ, upperPointerZ);
	recursiveHeightMod(maxRec, currentRec + 1, modMag / MAG_DIV, midpointX, upperPointerX, midpointZ, upperPointerZ);
}

//function to start the recursiveHeightMod function with initialized parameter values
void heightMod(GLint modMag, GLint maxRec) {
	GLint midpoint = floorf((MAP_DIM - 1) / 2);
	recursiveHeightMod(maxRec, 0, modMag, 0, MAP_DIM - 1, 0, MAP_DIM - 1);
}

//generate the coordinates of terrain vertices using the heightmap
void generateTerrainCoords(GLuint index, GLfloat length, GLfloat breadth) {
	for (GLint i = 0; i < MAP_DIM; i++) {
		for (GLint j = 0; j < MAP_DIM; j++) {
			if (index == 0) {
				terrainCoords0[i][j][0] = i * breadth / (float)MAP_DIM;
				terrainCoords0[i][j][1] = heightMap[i][j];
				terrainCoords0[i][j][2] = j * length / (float)MAP_DIM;
			}
			if (index == 1) {
				terrainCoords1[i][j][0] = i * breadth / (float)MAP_DIM;
				terrainCoords1[i][j][1] = heightMap[i][j];
				terrainCoords1[i][j][2] = j * length / (float)MAP_DIM;
			}
			if (index == 2) {
				terrainCoords2[i][j][0] = i * breadth / (float)MAP_DIM;
				terrainCoords2[i][j][1] = heightMap[i][j];
				terrainCoords2[i][j][2] = j * length / (float)MAP_DIM;
			}
		}
	}
}

//generate the coordinates of grid plane vertices
void generateGridPlaneCoords() {
	for (GLint i = 0; i < PLANE_DIM; i++) {
		for (GLint j = 0; j < PLANE_DIM; j++) {
			planeCoords[i][j][0] = (j * PLANE_WIDTH / PLANE_DIM);
			planeCoords[i][j][1] = 0;
			planeCoords[i][j][2] = (i * PLANE_WIDTH / PLANE_DIM);
		}
	}
}

//draw a mountain using the respective coordinates
void drawTerrain(GLuint index) {
	glMaterialfv(GL_FRONT, GL_EMISSION, terrainEmission);
	glMaterialfv(GL_FRONT, GL_AMBIENT, terrainAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, terrainDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, terrainSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, terrainShininess);
	glNormal3f(0, 1, 0);
	glBegin(GL_TRIANGLE_STRIP);
	for (GLint i = 0; i < MAP_DIM - 1; i++) {
		for (GLint j = 0; j < MAP_DIM; j++) {
			if (index == 0) {
				glTexCoord2f(terrainCoords0[i + 1][j][0] / terrainCoords0[MAP_DIM - 1][0][0], terrainCoords0[i + 1][j][2] / terrainCoords0[0][MAP_DIM - 1][2]);
				glVertex3fv(terrainCoords0[i + 1][j]);
				glTexCoord2f(terrainCoords0[i][j][0] / terrainCoords0[MAP_DIM - 1][0][0], terrainCoords0[i][j][2] / terrainCoords0[0][MAP_DIM - 1][2]);
				glVertex3fv(terrainCoords0[i][j]);
			}
			else if (index == 1) {
				glTexCoord2f(terrainCoords1[i + 1][j][0] / terrainCoords1[MAP_DIM - 1][0][0], terrainCoords1[i + 1][j][2] / terrainCoords1[0][MAP_DIM - 1][2]);
				glVertex3fv(terrainCoords1[i + 1][j]);
				glTexCoord2f(terrainCoords1[i][j][0] / terrainCoords1[MAP_DIM - 1][0][0], terrainCoords1[i][j][2] / terrainCoords1[0][MAP_DIM - 1][2]);
				glVertex3fv(terrainCoords1[i][j]);
			}
			else if (index == 2) {
				glTexCoord2f(terrainCoords2[i + 1][j][0] / terrainCoords2[MAP_DIM - 1][0][0], terrainCoords2[i + 1][j][2] / terrainCoords2[0][MAP_DIM - 1][2]);
				glVertex3fv(terrainCoords2[i + 1][j]);
				glTexCoord2f(terrainCoords2[i][j][0] / terrainCoords2[MAP_DIM - 1][0][0], terrainCoords2[i][j][2] / terrainCoords2[0][MAP_DIM - 1][2]);
				glVertex3fv(terrainCoords2[i][j]);
			}
		}
	}
	glEnd();
}