// ThemeRiver.h - a subclass of ModelView

#ifndef THEMERIVER_H
#define THEMERIVER_H

#include "ShaderIF.h"

#include <string>

#ifdef __APPLE__
#include "GLFW/glfw3.h"
#else
#include <GL/gl.h>
#endif

#include "ModelView.h"

typedef float vec2[2];
typedef float vec3[3];

class ThemeRiver : public ModelView
{
public:
	ThemeRiver(ShaderIF* sIF, int numTimeStepsIn, int numCurrentsIn,
		float** currentWidths); // currentWidths[currentID][timeStep]
	virtual ~ThemeRiver();

	// You can declare and implement methods here to be called from
	// ExtendedController to handle various events.

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimitsF) const;
	bool handleCommand(unsigned char anASCIIChar, double ldsX, double ldsY);
	bool handleCommand(int aFunctionKey, double ldsX, double ldsY, int mods);
	bool handleCommand(Controller::SpecialKey aSpecialKey,
					   double ldsX, double ldsY, int mods);
	void render();
private:
	ShaderIF* shaderIF;
	int numTimeSteps, numCurrents;
	float maxRiverWidth;

	// I put all of mine in one VAO/VBO pair. That is what the following
	// declarations assume. If you want to have one VAO/VBO per current,
	// then replace these two declarations with
	//     GLuint *vao, *vbo;
	// and dynamically allocate them in defineGeometry.
	GLuint vao[1];
	GLuint vbo[1];

	vec3* color;

	void defineGeometry(float** currentWidths);
};

#endif
