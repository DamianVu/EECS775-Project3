// main.c++

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "ExtendedController.h"
#include "ThemeRiver.h"

void createThemeRiver(Controller& c, ShaderIF* sIF, const char* csvFileName,
	std::string colID) // colID is a string because it may be, for example, "AB".
{
	// 1. Read the given CSV file, parsing out the indicated column data
	// 2. Create "currentWidths" and populate it as expected by the ThemeRiver
	//    constructor.

	int numDays = 0; // will be either 365 or 366
	int numCurrents = 0; // Actual number depends on which column was selected
	float** currentWidths = nullptr;
	ThemeRiver* tr = new ThemeRiver(sIF, numDays, numCurrents, currentWidths);
	c.addModel(tr);
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cerr << "Usage: " << argv[0] << " csvFileName columnID\n";
		exit(1);
	}
	ExtendedController c("Theme River");

	ShaderIF* sIF = new ShaderIF("shaders/proj3.vsh", "shaders/proj3.fsh");

	createThemeRiver(c, sIF, argv[1], argv[2]);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	double xyz[6];
	c.getOverallMCBoundingBox(xyz);
	ModelView::setMCRegionOfInterest(xyz);
	ModelView::setAspectRatioPreservationEnabled(false);

	c.run();

	delete sIF;

	return 0;
}
