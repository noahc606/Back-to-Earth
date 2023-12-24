#include "Planet.h"
#include "DebugScreen.h"

/**
 * https://en.wikipedia.org/wiki/Peirce_quincuncial_projection
 * 
 * This is how the world terrain looks as defined in Noise.cpp and Terain.cpp.
 * The TileMap will wrap back around itself based on this projection (tiled infinitely across the XY plane) and the diameter of the planet itself
 * Based on this shape, the planet's view from space will be determined.
 * 
 * Note: Nearly all references of NORTH/EAST/SOUTH/WEST in other areas of Back to Earth merely mean up/left/down/right in the rectangular sense, across the XY plane.
 */ 


void Planet::init()
{
	initialized = true;
	

}

void Planet::putInfo(std::stringstream& ss, int& tabs)
{
	DebugScreen::indentLine(ss, tabs);
	ss << "(rotation, rotationPeriod)=(" << rotation << ", " << rotationPeriod << "); ";
	DebugScreen::newLine(ss);
	
	DebugScreen::indentLine(ss, tabs);
	ss << "time=" << time << "; ";
	DebugScreen::newLine(ss);
	DebugScreen::indentLine(ss, tabs);
	ss << "time24=" << getPartOfDay(24) << ":" << (getPartOfDay(24*60)%60) << ":" << (getPartOfDay(24*60*60)%60) << "; ";
	DebugScreen::newLine(ss);
}

void Planet::tick()
{
	if(!initialized) { return; }
	
	//Calculate current rotation
	rotation++;
	if(rotation>=rotationPeriod) {
		rotation = 0;
	}
	
	//Calculate current time
	time = (double)rotation/(double)rotationPeriod;
	
	//Calculate current sky color
	double pod = getPartOfDayAccurate(24);
	//Night sky
	if(pod<7.0) {
		skyColor = sky[0];
	//Sunrise beginning
	} else if(pod<8.0) {
		skyColor = sky[0].getInterpolColor(sky[1], pod-8.0);
	//Sunrise ending
	} else if(pod<9.0) {
		skyColor = sky[1].getInterpolColor(sky[2], pod-9.0);
	//Day sky
	} else if(pod<19.0) {
		skyColor = sky[2];
	//Sunset beginning
	} else if(pod<20.0) {
		skyColor = sky[2].getInterpolColor(sky[3], pod-20.0);
	//Sunset ending
	} else if(pod<21.0) {
		skyColor = sky[3].getInterpolColor(sky[0], pod-21.0);
	//Night sky again
	} else {
		skyColor = sky[0];
	}
	
	
	
	//skyColor.interpolateBetween()
}

double Planet::getTime() { return time; }
double Planet::getPartOfDayAccurate(double segments) { return time*segments; }
int Planet::getPartOfDay(int segments)
{
	return time*(double)segments;
}
Color Planet::getSkyColor() { return skyColor; }