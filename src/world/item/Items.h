#pragma once
#include <string>
#include "Window.h"
#include "GUIHandler.h"
#include "Player.h"

class Items {
public:


    void init(GUIHandler* gh, Player* pl);
    
    static std::string getItemName(int id);
    Color getSandboxRGB();

    void setSandboxRGB(uint8_t r, uint8_t g, uint8_t b);
	void putItemInterfaceDesc(Window* win, int dX, int ttpDY, std::string itemDesc, int rowNum);
	void putItemInterfaceTitle(Window* win, int dX, int ttpDY, int itemID);
	void putItemInterface(int itemID);

	enum IDs {
		SANDBOX = -100,
		UNDEFINED = -1,
		ION_BLASTER = 0,
		GEOPORTER,
		SOLID_PLASMA_ANNIHILATOR,
		ATOM_PRINTER,
		NYLON_EXOSUIT_HELMET, NYLON_EXOSUIT_BODY, NYLON_EXOSUIT_LEGGINGS,
		FOOD_RATION_A,
		FOOD_RATION_B,
		FOOD_RATION_C,
		FOOD_RATION_D,
		FOOD_RATION_E,
		FOOD_RATION_F,
		INFINITE_OXYGEN_TANK,
		QUANTUM_EXOSUIT_HELMET, QUANTUM_EXOSUIT_BODY, QUANTUM_EXOSUIT_LEGGINGS,
		WORLDTILE,
	};
	
private:
    GUIHandler* guiHandler = nullptr;
    Player* player = nullptr;
    Color sandboxRGB;
};