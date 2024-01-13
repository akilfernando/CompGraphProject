#include <stdlib.h>
#include <GL/freeglut.h>

#define TEXTUREWIDTH  256
#define TEXTUREHEIGHT 256

/************************************************************************

	Function:		createTexture

	Description:	Contruct a random texture.

*************************************************************************/
void createTexture()
{

	// enable depth testing
	GLubyte whiteTexture[TEXTUREWIDTH][TEXTUREHEIGHT][3];
	int s, t;

	// enable texture mapping
	glEnable(GL_TEXTURE_2D);

	// for each texture element
	for (s = 0; s < TEXTUREWIDTH; s++)
	{
		for (t = 0; t < TEXTUREHEIGHT; t++)
		{
			// assign a random color
			whiteTexture[s][t][0] = (GLubyte)(255 * rand() / RAND_MAX);
			whiteTexture[s][t][1] = (GLubyte)(255 * rand() / RAND_MAX);
			whiteTexture[s][t][2] = (GLubyte)(255 * rand() / RAND_MAX);
			/*whiteTexture[s][t][0] = 255;
			whiteTexture[s][t][1] = 255;
			whiteTexture[s][t][2] = 255;*/
			//whiteTexture[s][t][2] = (GLubyte)(255 * rand() / RAND_MAX);
		}
	}

	// tell openGL how to scale the texture image up if needed
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// tell openGL how to scale the texture image down if needed
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	// set the current texture
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TEXTUREWIDTH, TEXTUREHEIGHT, 0,
		GL_RGB, GL_UNSIGNED_BYTE, whiteTexture);

}