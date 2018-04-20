// main.c++

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

#include "ExtendedController.h"
#include "ThemeRiver.h"

// Global variables because i'm a little lazy...
bool leapYear = false;
bool checkedForLeapYear = false;

int numberOfDaysPerMonth[] = {31,28,31,30,31,30,31,31,30,31,30}; // Don't need number of Days in December.

int getDayOfYear(std::string date) {
	int currentDay = 0;
	int prevNum = 0;
	int currentString = 0; // Will only be 0 or 1 when used.
	int vals[3]; // [Day], [Month], [Year]

	for (int i = 0; i < date.length(); i++) {
		if (date.at(i) == '/') {
			vals[currentString] = std::stoi(date.substr(prevNum, i - prevNum));
			currentString++;
			prevNum = i + 1;
		}
	}
	vals[2] = std::stoi(date.substr(prevNum, date.length() - prevNum));

	if (!checkedForLeapYear && vals[2] % 4 == 0) {
		leapYear = true;
		checkedForLeapYear = true;
	}

	for (int i = 0; i < vals[1] - 1; i++) {
		currentDay += numberOfDaysPerMonth[i];
	}

	currentDay += vals[0] - 1; // Since we want January 1st to be Day 0.

	if (vals[1] > 2 && leapYear) currentDay++; // If it is a leap year, and a month past february, add 1.

	return currentDay;
}

// This function will get the column number from 0 to cols-1, based on letters.
// This function should work for columns A-Z, and AA-ZZ
int getColumnNumberByString(std::string column) {
	int value = ((int) column.at(0)) - 65;

	// Shouldn't have a column ID with length > 2...
	if (column.length() == 2) {
		value = (value + 1) * 26;
		value += ((int) column.at(1) - 65);
	}
	return value;
}

void createThemeRiver(Controller& c, ShaderIF* sIF, const char* csvFileName,
	std::string colID) // colID is a string because it may be, for example, "AB".
{
	// 1. Read the given CSV file, parsing out the indicated column data
	// 2. Create "currentWidths" and populate it as expected by the ThemeRiver
	//    constructor.

	std::ifstream input;
	input.open(csvFileName);

	std::string currentLine;

	int prevNum;
	int count = 0;
	bool firstLine = true;
	int lineCount = 0;

	int columnNumberToTrack = getColumnNumberByString(colID);

	// These are essentially paired.
	std::vector<int> dates;
	std::vector<int> values;

	// Need to keep track of values as we read them...

	bool findingDate = false;
	bool findingValue = false;

	std::getline(input, currentLine); // Read first line, since they are headers. Maybe use them later?
	while (std::getline(input, currentLine)) {
		count = 0;
		for (int i = 0; i < currentLine.length(); i++) {
			if (count == 9 && !findingDate) { // Get date string
				prevNum = i;
				findingDate = true;
			} else if (count == columnNumberToTrack && !findingValue) {
				prevNum = i;
				findingValue = true;
			}



			if (currentLine.at(i) == ',') {
				if (findingDate) {
					dates.push_back(getDayOfYear(currentLine.substr(prevNum, i - prevNum)));
					findingDate = false;
				} else if (findingValue) {
					values.push_back(std::stoi(currentLine.substr(prevNum, i - prevNum)));
					findingValue = false;
				}


				count++;
			}
		}
		lineCount++;
	}

	std::vector<int> uniqueValues;
	for (int i = 0; i < values.size(); i++) {
		bool isUnique = true;
		for (int j = 0; j < uniqueValues.size(); j++) {
			if (uniqueValues[j] == values[i]) isUnique = false;
		}
		if (isUnique) {
			uniqueValues.push_back(values[i]);
		}
	}

	std::sort(uniqueValues.begin(), uniqueValues.end());

	int numDays = 365; // will be either 365 or 366
	if (leapYear) numDays++;

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
