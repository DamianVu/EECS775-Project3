// ThemeRiver.c++ - subclass of ModelView

#include <iostream>
#include <cmath>

#include "ThemeRiver.h"

ThemeRiver::ThemeRiver(ShaderIF* sIF, int numTimeStepsIn, int numCurrentsIn,
	float** currentWidths) : shaderIF(sIF), numTimeSteps(numTimeStepsIn),
	numCurrents(numCurrentsIn), maxRiverWidth(0.0)
{
	defineGeometry(currentWidths);
}

ThemeRiver::~ThemeRiver()
{
	glDeleteBuffers(1, vbo);
	glDeleteVertexArrays(1, vao);
}

void ThemeRiver::defineGeometry(float** currentWidths)
{
	// If you are putting all current data into one VAO/VBO, then
	// nPoints = numCurrents * numTimeSteps * 2; otherwise it is
	// just numTimeSteps * 2.
	int numPoints = 1;
	vec2* xyCurrents = new vec2[numPoints];

	// Recall currentWidths is indexed as: [currentIndex][timeStep]
	// For each time step, find sum of widths; track largest overall width
	// and store it in the instance variable maxRiverWidth.
	// Create and populate xyCurrents as you go. See also the comments following
	// this for-loop.
	for (int t=0 ; t<numTimeSteps ; t++)
	{
		// ...
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

		// 2. draw the current
		glDrawArrays(GL_TRIANGLE_STRIP, offset, 2*numTimeSteps);

		// 3. if all currents are in one VAO/VBO, then update offset:
		offset += 2*numTimeSteps;
	}

	// restore the previous program
	glUseProgram(pgm);
}
