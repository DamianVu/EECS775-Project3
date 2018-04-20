// ThemeRiver.c++ - subclass of ModelView

#include <iostream>
#include <cmath>

#include "ThemeRiver.h"

ThemeRiver::ThemeRiver(ShaderIF* sIF, int numTimeStepsIn, int numCurrentsIn,
	float** currentWidths) : shaderIF(sIF), numTimeSteps(numTimeStepsIn),
	numCurrents(numCurrentsIn), maxRiverWidth(0.0)
{
	// Will use 12 different colors and repeat if necessary
	color = new vec3[12];

	color[0][0] = .1216;
	color[0][1] = .4706;
	color[0][2] = .7059;
	color[1][0] = .2;
	color[1][1] = .6275;
	color[1][2] = .1725;
	color[2][0] = .9843;
	color[2][1] = .6039;
	color[2][2] = .6;
	color[3][0] = .651;
	color[3][1] = .8078;
	color[3][2] = .8902;
	color[4][0] = .4157;
	color[4][1] = .2392;
	color[4][2] = .6039;
	color[5][0] = .7922;
	color[5][1] = .698;
	color[5][2] = .8392;
	color[6][0] = 1;
	color[6][1] = .498;
	color[6][2] = 0;
	color[7][0] = .6941;
	color[7][1] = .349;
	color[7][2] = .1569;
	color[8][0] = 1;
	color[8][1] = 1;
	color[8][2] = .6;
	color[9][0] = .9922;
	color[9][1] = .749;
	color[9][2] = .4353;
	color[10][0] = .8902;
	color[10][1] = .102;
	color[10][2] = .11;
	color[11][0] = .6980;
	color[11][1] = .8745;
	color[11][2] = .5412;

	defineGeometry(currentWidths);
}

ThemeRiver::~ThemeRiver()
{
	glDeleteBuffers(1, vbo);
	glDeleteVertexArrays(1, vao);
	if (color != nullptr)
		delete [] color;
}

void ThemeRiver::defineGeometry(float** currentWidths)
{
	// If you are putting all current data into one VAO/VBO, then
	// nPoints = numCurrents * numTimeSteps * 2; otherwise it is
	// just numTimeSteps * 2.
	int numPoints = numCurrents * numTimeSteps * 2;
	vec2* xyCurrents = new vec2[numPoints];

	// Recall currentWidths is indexed as: [currentIndex][timeStep]
	// For each time step, find sum of widths; track largest overall width
	// and store it in the instance variable maxRiverWidth.
	// Create and populate xyCurrents as you go. See also the comments following
	// this for-loop.

	std::vector<float> y_coords;

	for (int t=0 ; t<numTimeSteps ; t++)
	{
		float totalWidth = 0;
		for (int j = 0; j < numCurrents; j++)
			totalWidth += currentWidths[j][t];

		y_coords.push_back(totalWidth/2);

		if (totalWidth > maxRiverWidth) maxRiverWidth = totalWidth;
	}

	int counter = 0;
	for (int i = 0; i < numCurrents; i++) {
		for (int j = 0; j < numTimeSteps; j++) {
			xyCurrents[counter][0] = j;
			xyCurrents[counter][1] = y_coords[j];

			y_coords[j] = y_coords[j] - currentWidths[i][j];
			xyCurrents[counter+1][0] = j;
			xyCurrents[counter+1][1] = y_coords[j];

			counter += 2;
		}
	}	

	// If you are using one VAO/VBO pair for all currents in the river, then
	// you do the following here. If you are using one VAO/VBO pair per current,
	// (i) see the comment in ThemeRiver.h where vao and vbo are declared, (ii)
	// dynamically allocate vao and vbo before the for-loop above, (iii) move
	// the two glGenXxx calls to before the loop and make them be:
	//       glGenVertexArrays(numCurrents, vao);
	//       glGenBuffers(numCurrents, vbo);
	// (iv) move the two glBindXxx calls and the glBufferData call inside the
	// for-loop, replacing the [0] with [c], where c = theme river current index.
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(1, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

	glBufferData(GL_ARRAY_BUFFER, numCurrents*numTimeSteps*2*sizeof(vec2), xyCurrents, GL_STATIC_DRAW);

	glVertexAttribPointer(shaderIF->pvaLoc("mcPosition"), 2,
		GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shaderIF->pvaLoc("mcPosition"));

	delete [] xyCurrents;
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void ThemeRiver::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = 0.0;
	xyzLimits[1] = numTimeSteps;
	xyzLimits[2] = -0.5 * maxRiverWidth;
	xyzLimits[3] =  0.5 * maxRiverWidth;
	xyzLimits[4] = -1.0; xyzLimits[5] = 1.0; // (zmin, zmax) (really 0..0)
}

// In each of these three handleCommand methods, return false if you
// handle the event and no one else should. Execute the 'return' statement
// that is there now if you do NOT handle the event.

bool ThemeRiver::handleCommand(unsigned char anASCIIChar,
							double ldsX, double ldsY)
{
	return this->ModelView::handleCommand(anASCIIChar, ldsX, ldsY);
}

bool ThemeRiver::handleCommand(int aFunctionKey,
						double ldsX, double ldsY, int mods)
{
	return this->ModelView::handleCommand(aFunctionKey, ldsX, ldsY, mods);
}

bool ThemeRiver::handleCommand(Controller::SpecialKey aSpecialKey,
						double ldsX, double ldsY, int mods)
{
	return this->ModelView::handleCommand(aSpecialKey, ldsX, ldsY, mods);
}

void ThemeRiver::render()
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
	glUseProgram(shaderIF->getShaderPgmID());

	// define the mapping from MC to -1..+1 Logical Device Space:
	float scaleTrans[4];
	compute2DScaleTrans(scaleTrans);
	glUniform4fv(shaderIF->ppuLoc("scaleTrans"), 1, scaleTrans);

	// This assumes one VAO/VBO for the entire river. If you are doing one per
	// current, move this statement to be the first statement in the for-loop
	// below and replace [0] with [c].
	glBindVertexArray(vao[0]);

	int offset = 0;
	for (int c=0 ; c<numCurrents ; c++)
	{
		// 1. set the color for this current
		// ...
	    glUniform3fv(shaderIF->ppuLoc("currentColor"), 1, color[c % 12]);
		// 2. draw the current
		glDrawArrays(GL_TRIANGLE_STRIP, offset, 2*numTimeSteps);

		// 3. if all currents are in one VAO/VBO, then update offset:
		offset += 2*numTimeSteps;
	}

	// restore the previous program
	glUseProgram(pgm);
}
