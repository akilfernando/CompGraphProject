
/************************************************************************************

	File: 			main.c

	Description:	The main OpenGL program for the flight simulator


	Author:			Akil Fernando

*************************************************************************************/



//include the library header files
#include <stdlib.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "texture.h"
#include "terrain.h"

//define the constants
#define TEXTUREWIDTH  64
#define TEXTUREHEIGHT 64
#define PI 3.1415927
#define NUMBER_OF_OBJ_PLANE 33
#define NUMBER_OF_OBJ_PROP 2
#define SEA_RADIUS 5000
#define SKY_HEIGHT 4000
#define PLANE 0
#define PROP 1
#define LEFT 0
#define RIGHT 1
#define MOUNTROWS 1280
#define MOUNTCOLS 1104
#define SEAROWS 1600
#define SEACOLS 1200
#define SKYROWS 896
#define SKYCOLS 385


//window variables
GLfloat windowWidth = 1600;
GLfloat windowHeight = 900;

//camera coordinates
GLfloat cameraCoords[3] = { 0,10,50 };

//quadrics
GLUquadricObj* quadricSeaPtr;
GLUquadricObj* quadricSkyPtr;
GLUquadricObj* quadricOriginSphere;

//emission materials
GLfloat emissionR[4] = {1,0,0,1};
GLfloat emissionG[4] = {0,1,0,1};
GLfloat emissionB[4] = {0,0,1,1};
GLfloat emissionW[4] = {1,1,1,1};

//variables to control the plane
GLfloat planeVertices[6763][3] = { 0 };
GLfloat planeNormals[6763][3] = { 0 };
GLint planeFaces[3704][18] = { 0 };
GLfloat propVertices[6763][3] = { 0 };
GLfloat propNormals[6763][3] = { 0 };
GLint propFaces[3704][18] = { 0 };
GLint planeObjectIndices[NUMBER_OF_OBJ_PLANE] = { -1 };
GLint planeVertexCount = 0;
GLint planeNormalCount = 0;
GLint planeFaceCount = 0;
GLint propObjectIndices[NUMBER_OF_OBJ_PROP] = { -1 };
GLint propVertexCount = 0;
GLint propNormalCount = 0;
GLint propFaceCount = 0;
GLfloat moveSpeed = 0.2;
GLfloat maxSpeed = 1.5;
GLfloat minSpeed = 0.1;
GLfloat dirVector[2] = { 0,-1 };
GLfloat turnRate = 0;
GLfloat maxAscendRate = 0.25;
GLfloat ascendRateDelta = 0.01;
GLfloat ascendRate = 0;
GLfloat ascendNormrate = 0.0009;
GLfloat acceleration = 0.01;
GLfloat rightAileron = 0;
GLfloat leftAileron = 0;
GLfloat propConstant = 50;
GLfloat yawRate = 1000;
GLfloat rollRate = 3600;
GLfloat planeOffset[3] = { 0, - 0.5,-2 };
GLfloat planeTheta = 0;
GLfloat propTheta = 0;
GLfloat propROffset[3] = { 0.37, 0.05, -0.52 };
GLfloat propLOffset[3] = { -0.37, 0.05, -0.52 };
GLfloat propScale[3] = { 1.5,1.5,1.5 };

//booleans to control scene
GLboolean wireframe = false;
GLboolean seaVisible = false;
GLboolean mountainsVisible = false;
GLboolean textureOn = false;
GLboolean fogOn = false;

//light source position 
GLfloat lightPosition[] = { 1000, 500, 0, 0 };


//material variables
GLfloat planeColours[NUMBER_OF_OBJ_PLANE][4] =
{ {1,1,0,1}, {1,1,0,1}, {1,1,0,1}, {1,1,0,1},
	{0,0,0,1}, {0,0,0,1},
	{1,0.5,1,1},
	{0,0,1,1},
	{1,1,0,1}, {1,1,0,1}, {1,1,0,1}, {1,1,0,1},{1,1,0,1}, {1,1,0,1},
	{0,0,1,1}, {0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},{0,0,1,1},
	{1,1,0,1}, {1,1,0,1}, {1,1,0,1}, {1,1,0,1},{1,1,0,1}, {1,1,0,1},{1,1,0,1}
};
GLfloat propColours[NUMBER_OF_OBJ_PROP][4] = { { 1,0,0,1 }, {1,0,1,1} };
GLfloat zeroMaterial[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat gridPlaneMaterial[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat planeSpecular[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat planeShininess = 150;
GLfloat skyDiffuse[4] = { 1, 1, 1, 1.0 };
GLfloat seaDiffuse[4] = { 1, 1, 1, 1.0 };


//texture variables
GLubyte mount03[MOUNTROWS][MOUNTCOLS][3];
GLint mountTexID;
GLubyte sea02[SEAROWS][SEACOLS][3];
GLint seaTexID;
GLubyte sky08[SKYROWS][SKYCOLS][3];
GLint skyTexID;
GLubyte whiteTextureArray[64][64][3];
GLint whiteTexID;

//function to print instructions
void printInstructions() {
	printf("Screen Controls\n");
	printf("q: quit\n");
	printf("w: toggle wireframe\n");
	printf("f: toggle fullscreen\n");
	printf("s: toggle sky & sea\n");
	printf("m: toggle mountains\n");
	printf("t: toggle mountain texture\n");
	printf("b: toggle fog\n\n");
	printf("Plane Controls\n");
	printf("Up Arrow\t\t\t: ascend\n");
	printf("Down Arrow\t\t\t: descend\n");
	printf("Page Up\t\t\t\t: increase throttle\n");
	printf("Page Down\t\t\t: decrease throttle\n");
	printf("Mouse x-axis within window\t: steer\n\n");

	printf("Number of vertices\t(plane) %d\n", planeVertexCount);
	printf("Number of faces\t\t(plane) %d\n", planeFaceCount);
	printf("Number of vertices\t(prop) %d\n", propVertexCount);
	printf("Number of faces\t\t(prop) %d", propFaceCount);
}


//function to generate the texture for the mountains
void createMountTex() {
	FILE* file;
	if (fopen_s(&file, "mount03.ppm", "r") != 0) {
		printf("Could not open the file.\n");
		return;
	}

	char word[6];
	GLint skip = 0;
	GLint col = 0;
	GLint row = 0;
	GLint element = 0;
	while (fscanf(file, "%s", word) != EOF) {
		if (skip < 4) {
			skip++;
			continue;
		}
		GLubyte value = atoi(word);

		if (col >= MOUNTCOLS) {
			col = 0;
			row++;
		}
		if (element > 2) {
			element = 0;
			col++;
		}
		mount03[row][col][element] = value;
		element++;
	}

	fclose(file);

}

//function to generate the texture for the sea
void createSeaTex() {
	FILE* file;
	if (fopen_s(&file, "sea02.ppm", "r") != 0) {
		printf("Could not open the file.\n");
		return;
	}

	char word[6];
	GLint skip = 0;
	GLint col = 0;
	GLint row = 0;
	GLint element = 0;
	while (fscanf(file, "%s", word) != EOF) {
		if (skip < 4) {
			skip++;
			continue;
		}
		GLubyte value = atoi(word);

		if (col >= SEACOLS) {
			col = 0;
			row++;
		}
		if (element > 2) {
			element = 0;
			col++;
		}
		sea02[row][col][element] = value;
		element++;
	}

	fclose(file);
}

//function to generate the texture for the sky
void createSkyTex() {
	FILE* file;
	if (fopen_s(&file, "sky08.ppm", "r") != 0) {
		printf("Could not open the file.\n");
		return;
	}

	char word[6];
	GLint skip = 0;
	GLint col = 0;
	GLint row = 0;
	GLint element = 0;
	while (fscanf(file, "%s", word) != EOF) {
		if (skip < 4) {
			skip++;
			continue;
		}
		GLubyte value = atoi(word);

		if (col >= SKYCOLS) {
			col = 0;
			row++;
		}
		if (element > 2) {
			element = 0;
			col++;
		}
		sky08[row][col][element] = value;
		element++;
	}

	fclose(file);
}

//function to generate the white texture
void createWhiteTex() {
	for (GLint s = 0; s < 64; s++)
	{
		for (GLint t = 0; t < 64; t++)
		{
			// assign a random color
			whiteTextureArray[s][t][0] = 255;
			whiteTextureArray[s][t][1] = 255;
			whiteTextureArray[s][t][2] = 255;
		}
	}
}

//function to set up all textures
void createTextures() {
	createMountTex();
	createSeaTex();
	createSkyTex();
	createWhiteTex();
	glGenTextures(1, &mountTexID);
	glGenTextures(1, &seaTexID);
	glGenTextures(1, &skyTexID);
	glGenTextures(1, &whiteTexID);
	glBindTexture(GL_TEXTURE_2D, mountTexID);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, MOUNTROWS, MOUNTCOLS, GL_RGB, GL_UNSIGNED_BYTE, mount03);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, seaTexID);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, SEAROWS, SEACOLS, GL_RGB, GL_UNSIGNED_BYTE, sea02);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, skyTexID);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, SKYROWS, SKYCOLS, GL_RGB, GL_UNSIGNED_BYTE, sky08);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glBindTexture(GL_TEXTURE_2D, whiteTexID);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 64, 64, GL_RGB, GL_UNSIGNED_BYTE, whiteTextureArray);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

/************************************************************************

	Function:		createPlane

	Description:	Set up all the points to enable drawing the plane.

*************************************************************************/
void createPlane() {
	FILE* file;
	if (fopen_s(&file, "plane.txt", "r") != 0) {
		printf("Could not open the file.\n");
		return 1;
	}

	char line[1024];
	float num1, num2, num3;
	GLint int1, int2, int3;
	GLint objectIndex = 0;
	planeVertexCount = 0;
	planeNormalCount = 0;
	planeFaceCount = 0;
	while (fgets(line, sizeof(line), file) != NULL) {
		if (line[0] == 'v') {
			if (sscanf_s(line, " %*c %f %f %f", &num1, &num2, &num3) == 3) {
				planeVertices[planeVertexCount][2] = num1 + 0.17;
				planeVertices[planeVertexCount][1] = num2 + 0.21;
				planeVertices[planeVertexCount][0] = num3;
				planeVertexCount++;
			}
			else {
				printf("Invalid input string format in line: %s", line);
			}
		}
		else if (line[0] == 'n') {
			if (sscanf_s(line, " %*c %f %f %f", &num1, &num2, &num3) == 3) {
				planeNormals[planeNormalCount][2] = num1;
				planeNormals[planeNormalCount][1] = num2;
				planeNormals[planeNormalCount][0] = num3;
				planeNormalCount++;
			}
			else {
				printf("Invalid input string format in line: %s", line);
			}
		}
		else if (line[0] == 'f') {
			char delim[] = " ";
			char* portion;
			portion = strtok(line, delim);

			GLint i = 0;
			while (portion != NULL)
			{
				if (i == 0) {
					portion = strtok(NULL, delim);
					i++;
					continue;
				}
				planeFaces[planeFaceCount][i-1] = atoi(portion);
				portion = strtok(NULL, delim);
				i++;
			}
			planeFaceCount++;
		}
		else if (line[0] == 'g') {
			planeObjectIndices[objectIndex] = planeFaceCount;
			objectIndex++;
		}
	}
	planeFaceCount--;
	fclose(file);
}

/************************************************************************

	Function:		createProp

	Description:	Set up all the points to enable drawing the propellers.

*************************************************************************/
void createProp() {
	FILE* file;
	if (fopen_s(&file, "propeller.txt", "r") != 0) {
		printf("Could not open the file.\n");
		return 1;
	}

	char line[1024];
	float num1, num2, num3;
	GLint int1, int2, int3;
	GLint objectIndex = 0;
	propVertexCount = 0;
	propNormalCount = 0;
	propFaceCount = 0;
	while (fgets(line, sizeof(line), file) != NULL) {
		if (line[0] == 'v') {
			if (sscanf_s(line, " %*c %f %f %f", &num1, &num2, &num3) == 3) {
				propVertices[propVertexCount][2] = num1 + 0.56;
				propVertices[propVertexCount][1] = num2 + 0.1475;
				propVertices[propVertexCount][0] = (num3 - 0.353) * -1;
				propVertexCount++;
			}
			else {
				printf("Invalid input string format in line: %s", line);
			}
		}
		else if (line[0] == 'n') {
			if (sscanf_s(line, " %*c %f %f %f", &num1, &num2, &num3) == 3) {
				propNormals[propNormalCount][2] = num1 + 0.56;
				propNormals[propNormalCount][1] = num2 + 0.1475;
				propNormals[propNormalCount][0] = (num3 - 0.353) * -1;
				propNormalCount++;
			}
			else {
				printf("Invalid input string format in line: %s", line);
			}
		}
		else if (line[0] == 'f') {
			char delim[] = " ";
			char* portion;
			portion = strtok(line, delim);

			GLint i = 0;
			while (portion != NULL)
			{
				if (i == 0) {
					portion = strtok(NULL, delim);
					i++;
					continue;
				}
				propFaces[propFaceCount][i - 1] = atoi(portion);
				portion = strtok(NULL, delim);
				i++;
			}
			propFaceCount++;
		}
		else if (line[0] == 'g') {
			propObjectIndices[objectIndex] = propFaceCount;
			objectIndex++;
		}
	}
	fclose(file);
}

/************************************************************************

	Function:		polygon

	Description:	Draw a single polygon of the selected entity, given the face index

*************************************************************************/
void polygon(GLint face, GLint entity) {
	glBegin(GL_POLYGON);
	GLint i = 0;
	while (true) {
		if (entity == PLANE) {
			if (planeFaces[face][i] == 0) break;
			glNormal3fv(planeNormals[planeFaces[face][i] - 1]);
			glVertex3fv(planeVertices[planeFaces[face][i] - 1]);
			i++;
		}
		else if (entity == PROP) {
			if (propFaces[face][i] == 0) break;
			glNormal3fv(propNormals[propFaces[face][i] - 1]);
			glVertex3fv(propVertices[propFaces[face][i] - 1]);
			i++;
		}
	}
	glEnd();
}

/************************************************************************

	Function:		propellers

	Description:	Draw a propeller given the side

*************************************************************************/
void propellers(GLint side) {
	glPushMatrix();
	if (side == LEFT) {
		glTranslatef(propLOffset[0], propLOffset[1], propLOffset[2]);
	}
	else {
		glTranslatef(propROffset[0], propROffset[1], propROffset[2]);
	}
	glRotatef(-propTheta, 0, 0, 1);
	glScalef(propScale[0], propScale[1], propScale[2]);
	GLint currentObj = 0;
	for (GLint j = 0; j < propFaceCount; j++) {
		if (j == propObjectIndices[currentObj + 1]) {
			currentObj++;
		}
		glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, propColours[currentObj]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
		glMaterialf(GL_FRONT, GL_SHININESS, planeShininess);
		polygon(j, PROP);
	}
	glPopMatrix();
}


/************************************************************************

	Function:		plane

	Description:	Main function to control drawing the plane

*************************************************************************/
void plane() {
	GLint currentObj = 0;
	GLfloat objRot = 0;
	glPushMatrix();
	glTranslatef(planeOffset[0], planeOffset[1], planeOffset[2]);
	glRotatef(turnRate* yawRate, 0, 1, 0);
	glRotatef(turnRate* rollRate, 0, 0, 1);
	glRotatef((moveSpeed * 2) + (ascendRate * 10), 1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, whiteTexID);
	for (GLint j = 0; j < planeFaceCount; j++) {
		if (j == planeObjectIndices[currentObj + 1]) {
			currentObj++;
		}
		if (currentObj == 18) objRot = leftAileron;
		else if (currentObj == 14) objRot = rightAileron;
		else objRot = 0;
		glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
		glMaterialfv(GL_FRONT, GL_EMISSION, zeroMaterial);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, planeColours[currentObj]);
		glMaterialfv(GL_FRONT, GL_SPECULAR, planeSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, planeShininess);
		glPushMatrix();
		glRotatef(objRot, 1, 0, 0);
		polygon(j, PLANE);
		glPopMatrix();
	}
	propellers(LEFT);
	propellers(RIGHT);
	glPopMatrix();
}


/************************************************************************

	Function:		origin

	Description:	Main function to control drawing the x,y,z axes at the origin

*************************************************************************/
void origin() {
	glBindTexture(GL_TEXTURE_2D, whiteTexID);
	glLineWidth(5);
	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissionR);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, emissionR);
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);
	glEnd();
	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissionG);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, emissionG);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);
	glEnd();
	glBegin(GL_LINES);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissionB);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, emissionB);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd();
	glMaterialfv(GL_FRONT, GL_EMISSION, emissionW);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, emissionW);
	gluSphere(quadricOriginSphere, 2, 10, 10);
}


/************************************************************************

	Function:		initializeQuadrics

	Description:	Set up the quadrics for the sea and sky

*************************************************************************/
void initializeQuadrics() {
	quadricSeaPtr = gluNewQuadric();
	gluQuadricTexture(quadricSeaPtr, GL_TRUE);
	gluQuadricNormals(quadricSeaPtr, GLU_SMOOTH);
	quadricSkyPtr = gluNewQuadric();
	gluQuadricTexture(quadricSkyPtr, GL_TRUE);
	gluQuadricNormals(quadricSkyPtr, GLU_SMOOTH);
	quadricOriginSphere = gluNewQuadric();
	gluQuadricTexture(quadricOriginSphere, GL_TRUE);
	gluQuadricNormals(quadricOriginSphere, GLU_SMOOTH);
}


/************************************************************************

	Function:		drawQuadrics

	Description:	Main function to control drawing the sea and sky quadrics

*************************************************************************/
void drawQuadrics() {
	glLineWidth(2);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	if (fogOn) glEnable(GL_FOG);
	glBindTexture(GL_TEXTURE_2D, seaTexID);
	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, seaDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, planeSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, planeShininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, seaDiffuse);
	gluDisk(quadricSeaPtr, 0, SEA_RADIUS + 5, 100, 5);
	if (fogOn) glDisable(GL_FOG);

	glTranslatef(0, 0, -1);
	glBindTexture(GL_TEXTURE_2D, skyTexID);
	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, skyDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, planeSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, planeShininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, skyDiffuse);
	gluCylinder(quadricSkyPtr, SEA_RADIUS, SEA_RADIUS, SKY_HEIGHT, 50, 10);
	glPopMatrix();
}


/************************************************************************

	Function:		drawGridPlane

	Description:	Draws the grid plane that appears when the sea and sky aren't visible

*************************************************************************/
void drawGridPlane() {
	glBindTexture(GL_TEXTURE_2D, whiteTexID);
	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_EMISSION, gridPlaneMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, gridPlaneMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, 0);
	glPushMatrix();
	glTranslatef(-PLANE_WIDTH / 2, 0, -PLANE_WIDTH / 2);
	for (GLint i = 0; i < PLANE_DIM - 1; i++) {
		glBegin(GL_QUAD_STRIP);
		for (GLint j = 0; j < PLANE_DIM; j++) {
			glVertex3f(planeCoords[i + 1][j][0], 0, planeCoords[i + 1][j][2]);
			glVertex3f(planeCoords[i][j][0], 0, planeCoords[i][j][2]);
		}
		glEnd();
	}
	glPopMatrix();

}

/************************************************************************

	Function:		toggleWireframe

	Description:	toggles wireframe for polygons and quadrics

*************************************************************************/
void toggleWireframe() {
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		gluQuadricDrawStyle(quadricSeaPtr, GLU_FILL);
		gluQuadricDrawStyle(quadricSkyPtr, GLU_FILL);
		wireframe = false;
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		gluQuadricDrawStyle(quadricSeaPtr, GLU_LINE);
		gluQuadricDrawStyle(quadricSkyPtr, GLU_LINE);
		wireframe = true;
	}
}

/************************************************************************

	Function:		toggleSeaVisible

	Description:	toggles whether or not the Sea & Sky are visible

*************************************************************************/
void toggleSeaVisible() {
	if (seaVisible) {
		seaVisible = false;
	}
	else {
		seaVisible = true;
	}
}

/************************************************************************

	Function:		toggleMountainsVisible

	Description:	toggles whether or not the mountains are visible

*************************************************************************/
void toggleMountainsVisible() {
	if (mountainsVisible) {
		mountainsVisible = false;
	}
	else {
		mountainsVisible = true;
	}
}

/************************************************************************

	Function:		toggleTexture

	Description:	toggles whether or not the mountains' textures are visible

*************************************************************************/
void toggleTexture() {
	if (textureOn) {
		textureOn = false;
	}
	else {
		textureOn = true;
	}
}

/************************************************************************

	Function:		toggleFog

	Description:	toggles whether or not the fog on the sea is visible

*************************************************************************/
void toggleFog() {
	if (fogOn) {
		fogOn = false;
	}
	else {
		fogOn = true;
	}
}

/************************************************************************

	Function:		generateTerrain

	Description:	Main function to control the generation of the terrain (mountains)

*************************************************************************/
void generateTerrain(GLuint index, GLfloat modMag, GLfloat maxRec, GLfloat length, GLfloat breadth) {
	initializeTerrain(index);
	heightMod(modMag, maxRec);
	generateTerrainCoords(index, length, breadth);
}


/************************************************************************

	Function:		myDisplay

	Description:	Display callback, clears frame buffer and depth buffer,
					Draws the texture mapped polygon.

*************************************************************************/
void myDisplay(void)
{

	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load the identity matrix into the model view matrix
	glLoadIdentity();

	//set line width to 1 for wireframe
	glLineWidth(1);

	//calculate the direction the plane is facing
	dirVector[0] = cos(planeTheta);
	dirVector[1] = sin(planeTheta) * -1;

	//draw the plane
	plane();

	//position the camera
	gluLookAt(cameraCoords[0], cameraCoords[1], cameraCoords[2], cameraCoords[0] + dirVector[0] * INFINITE, cameraCoords[1], dirVector[1] * INFINITE, 0, 1, 0);

	//position the light
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	//draw mountains if they are set to visible
	if (mountainsVisible) {
		if(textureOn)
		glBindTexture(GL_TEXTURE_2D, mountTexID);
		glPushMatrix();
		glTranslatef(0, -250, -500);
		drawTerrain(0);
		glRotatef(45, 0, 1, 0);
		glTranslatef(-3000, 50, -500);
		drawTerrain(1);
		glRotatef(45, 0, 1, 0);
		glTranslatef(-500, 50, 4500);
		drawTerrain(2);
		glPopMatrix();
	}

	//draw sea & sky if they are set to visible
	if (seaVisible) drawQuadrics();
	else drawGridPlane();

	//draw origin
	origin();

	//swap buffers
	glutSwapBuffers();
}

/************************************************************************

	Function:		idle

	Description:	Controls the modification of variables between frames

*************************************************************************/
void idle() {
	planeTheta += turnRate;
	propTheta += propConstant * moveSpeed;
	cameraCoords[0] += dirVector[0] * moveSpeed;
	cameraCoords[2] += dirVector[1] * moveSpeed;
	if (ascendRate > 0) ascendRate -= ascendNormrate;
	else if (ascendRate < 0) ascendRate += ascendNormrate;
	cameraCoords[1] += ascendRate;
	glutPostRedisplay();
}

/************************************************************************

	Function:		resize

	Description:	Handles window reshape

*************************************************************************/
void resize(GLint newWidth, GLint newHeight)
{

	// update the new width
	windowWidth = newWidth;
	// update the new height
	windowHeight = newHeight;

	// update the viewport to still be all of the window
	glViewport(0, 0, windowWidth, windowHeight);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// gluPerspective(fovy, aspect, near, far)
	gluPerspective(60, (float)windowWidth / (float)windowHeight, 0.1, 10000);
	//gluPerspective(175, (float)windowWidth / (float)windowHeight, 0.1, 10000);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
}

/************************************************************************

	Function:		mouseMotion

	Description:	Controls the modification of variables when mouse position within the window changes

*************************************************************************/
void mouseMotion(GLint x, GLint y)
{
	turnRate = (x - windowWidth / 2) * -0.00001;
	rightAileron = -(x - windowWidth / 2)/(windowWidth/45);
	leftAileron = (x - windowWidth / 2)/(windowWidth/45);
}

/************************************************************************

	Function:		myKeyboard

	Description:	Handles keyboard events (for standard keys)

*************************************************************************/
void myKeyboard(unsigned char key, GLint x, GLint y) {
	if (key == 'q') exit(0);
	else if (key == 'w') toggleWireframe();
	else if (key == 'f') glutFullScreenToggle();
	else if (key == 's') toggleSeaVisible();
	else if (key == 'm') toggleMountainsVisible();
	else if (key == 't') toggleTexture();
	else if (key == 'b') toggleFog();
}

/************************************************************************

	Function:		mySpecial

	Description:	Handles keyboard events (for special keys)

*************************************************************************/
void special(unsigned char key, GLint x, GLint y) {
	if (key == GLUT_KEY_UP) {
		if (ascendRate < maxAscendRate) ascendRate += ascendRateDelta;
	}
	if (key == GLUT_KEY_DOWN) {
		if (ascendRate > -maxAscendRate) ascendRate -= ascendRateDelta;
	}
	if (key == GLUT_KEY_PAGE_UP && moveSpeed < maxSpeed) {
		moveSpeed += acceleration;
	}
	if (key == GLUT_KEY_PAGE_DOWN && moveSpeed > minSpeed) {
		moveSpeed -= acceleration;
	}
}



/************************************************************************

	Function:		initializeGL

	Description:	Initializes the OpenGL rendering context for display.

*************************************************************************/
void initializeGL(void)
{
	//set colour to white since we are using materials and textures
	glColor4f(1, 1, 1, 1);

	//enable smooth shading
	glShadeModel(GL_SMOOTH);

	//define the light color and intensity
	GLfloat ambientLight[] = { 0.2, 0.2, 0.2, 1.0 }; 
	GLfloat diffuseLight[] = { 1, 1, 1, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	//the global ambient light level
	GLfloat globalAmbientLight[] = { 0.48, 0.195, 0.3, 1.0 };

	//set the global ambient light level
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	// enable light 0
	glEnable(GL_LIGHT0);

	// define the color and intensity for light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

	// enable lighting 
	glEnable(GL_LIGHTING);

	// turn on depth testing so that polygons are drawn in the correct order
	glEnable(GL_DEPTH_TEST);

	// make sure the normals are unit vectors
	glEnable(GL_NORMALIZE);

	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);

	//enable blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//set up the vertices, normals, and faces for the plane and propeller
	createPlane();
	createProp();

	//set up fog
	glFogfv(GL_FOG_COLOR, globalAmbientLight);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.0008);
	
	//set up the quadrics
	initializeQuadrics();

	//enable textures and generate them
	glEnable(GL_TEXTURE_2D);
	createTextures();
	
	//generate vertex coordinates for the 3 mountains
	generateTerrain(0, 300, 5, 2000, 2000);
	generateTerrain(1, 250, 5, 2000, 2000);
	generateTerrain(2, 200, 5, 2000, 2000);

	//generate vertex coordinates for the grid plane (what is displayed when sea and sky are turned off)
	generateGridPlaneCoords();

	//set the view to be wireframe
	toggleWireframe();

	//print the instruction set
	printInstructions();
}



/************************************************************************

	Function:		main

	Description:	Sets up the openGL rendering context and the windowing
					system, then begins the display loop.

*************************************************************************/
void main(GLint argc, char** argv)
{
	//initialize the toolkit
	glutInit(&argc, argv);
	//set display mode
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	//set window size
	glutInitWindowSize(1600, 900);
	//set window position on screen
	glutInitWindowPosition(0, 0);
	//open the screen window
	glutCreateWindow("Flight Simulator");

	//register redraw function
	glutDisplayFunc(myDisplay);
	//register the idle function
	glutIdleFunc(idle);
	//register the keyboard function
	glutKeyboardFunc(myKeyboard);
	//register the special function
	glutSpecialFunc(special);
	//register the passive mouse function
	glutPassiveMotionFunc(mouseMotion);
	//register the window reshape function
	glutReshapeFunc(resize);
	//initialize the rendering context
	initializeGL();
	//go into a perpetual loop
	glutMainLoop();
}
