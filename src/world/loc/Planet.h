#pragma once
#include <nch/cpp-utils/gfx/Color.h>
#include "Loggable.h"

class Planet : public Loggable {
public:
	void init(int rotation);
	void init();
	void tick();
	
	void putInfo(std::stringstream& ss, int& tabs);
	int getRotationRaw();
	double getTime();
	double getPartOfDayAccurate(double segments);
	int getPartOfDay(int segments);
	nch::Color getSkyColor();

private:

	bool initialized = false;
	double time = 0.0;	//Set equal to rotation/rotationPeriod. At prime meridian/equator intersection: 0.0 = midnight, 0.5 = noon, 0.999 = 11:59pm
	nch::Color skyColor;
	nch::Color sky[4] = {
		nch::Color(0, 0, 128),		//Midnight blue		Midnight 	(10hrs, 21:00-7:00)
		nch::Color(240, 160, 20),	//Light orange		Sunrise 	(2hrs, 8:00)
		nch::Color(135, 206, 250),	//Light sky blue	Midday 		(10hrs, 9:00-19:00)
		nch::Color(240, 120, 20)		//Red orange		Sunset 		(2hrs, 20:00)
	};
	
	int rotation = 0;							//Ranges from 0 to (rotationPeriod-1), representing current the rotation of the planet
	int rotationPeriod = (int)(60*40.0*32.4);	//Length of time, in ticks, that it takes for the planet to complete one rotation
	
	uint64_t id;
};