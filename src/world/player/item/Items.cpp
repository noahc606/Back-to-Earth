#include "Items.h"
#include "TextBox.h"
#include "Tooltip.h"

void Items::init(GUIHandler* gh, Player* pl)
{
	sandboxRGB = nch::Color(255, 255, 255);
    guiHandler = gh;
    player = pl;
}

std::string Items::getItemName(int itemID)
{
	switch(itemID) {
	case ION_BLASTER: 				return "Ion Blaster";
	case GEOPORTER:					return "Geo-porter";
	case SOLID_PLASMA_ANNIHILATOR:	return "Solid-Plasma Annihilator";
	case ATOM_PRINTER:				return "Atom Printer";
	case NYLON_EXOSUIT_HELMET:		return "T1 Nylon Exo-Suit Helmet";
	case NYLON_EXOSUIT_BODY:		return "T1 Nylon Exo-Suit Body";
	case NYLON_EXOSUIT_LEGGINGS:	return "T1 Nylon Exo-Suit Leggings";
	case INFINITE_OXYGEN_TANK:		return "Infinite Oxygen Tank";
	case FOOD_RATION_A:				return "Nutrient Powder A";
	case FOOD_RATION_B:				return "Nutrient Powder B";
	case FOOD_RATION_C:				return "MRE Pack (Eggs, Bacon, Toast)";
	case FOOD_RATION_D:				return "MRE Pack (Steak and Potatoes)";
	case FOOD_RATION_E:				return "MRE Pack (Chicken and Rice)";
	case FOOD_RATION_F:				return "MRE Pack (Vegetable Stew)";
	case QUANTUM_EXOSUIT_HELMET:	return "Quantum Exo-Suit Helmet (Infinite Atmosphere)";
	case QUANTUM_EXOSUIT_BODY:		return "Quantum Exo-Suit Body";
	case QUANTUM_EXOSUIT_LEGGINGS:	return "Quantum Exo-Suit Leggings";
	default: 						return "???null???";
	}
}

nch::Color Items::getSandboxRGB() { return sandboxRGB; }
void Items::setSandboxRGB(uint8_t r, uint8_t g, uint8_t b)
{
    sandboxRGB.r = r;
    sandboxRGB.g = g;
    sandboxRGB.b = b;
}

void Items::putItemInterfaceDesc(Window* win, int dX, int ttpDY, std::string itemDesc, int rowNum)
{
	guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*rowNum+ttpDY, itemDesc, GUIHandler::ttp_CHARACTER_item) );
}

void Items::putItemInterfaceTitle(Window* win, int dX, int ttpDY, int itemID)
{
	guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*0+ttpDY, "\""+ getItemName(itemID) + "\"", GUIHandler::ttp_CHARACTER_item) );
}

void Items::putItemInterface(int itemID)
{
	Window* win = guiHandler->getWindow(GUIHandler::win_CHARACTER);
	if( win==nullptr ) {
		return;
	}
	
	guiHandler->removeGUIs(GUIHandler::ttp_CHARACTER_item, GUIHandler::tbx_CHARACTER_item);

	int dX = 12;
	int tbxDY = 26;
	int ttpDY = tbxDY+8;

	//If there is no item name, stop
	if(Items::getItemName(itemID)=="???null???") {
		return;
	}
	
	//Add tooltip for every item type
	guiHandler->addGUI( new Tooltip(win, dX, 524+4, Items::getItemName(itemID), GUIHandler::ttp_CHARACTER_item) );
	
	switch( itemID ) {
	case GEOPORTER: {


		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*0+ttpDY, "X-position (meters):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx1 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*0+tbxDY, 100, TextBox::FREE_NUMBERS_INTEGERS, GUIHandler::tbx_CHARACTER_item), 1000 );
		std::stringstream ss1; ss1 << (int)std::get<0>(player->getPos());
		tbx1->setString( ss1.str() );
		
		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*1+ttpDY, "Y-position (meters):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx2 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*1+tbxDY, 100, TextBox::FREE_NUMBERS_INTEGERS, GUIHandler::tbx_CHARACTER_item), 1001 );
		std::stringstream ss2; ss2 << (int)std::get<1>(player->getPos());
		tbx2->setString( ss2.str() );

		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*2+ttpDY, "Z-position (meters):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx3 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*2+tbxDY, 100, TextBox::FREE_NUMBERS_INTEGERS, GUIHandler::tbx_CHARACTER_item), 1002 );
		std::stringstream ss3; ss3 << (int)std::get<2>(player->getPos());
		tbx3->setString( ss3.str() );

		guiHandler->addGUI( new Tooltip(win, dX+32*10, 524+32*0+ttpDY, "Planet ID:", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx4 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*13+4, 524+32*0+tbxDY, 350, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 1003 );
		
		
		guiHandler->addGUI( new Button (win, dX+32*10, 524+32*2+tbxDY, 200, "Teleport", GUIHandler::btn_CHARACTER_item), 1004 );
	} break;
	case SOLID_PLASMA_ANNIHILATOR: {
		putItemInterfaceDesc(win, dX, ttpDY, "Destroy solid matter with Left Click", 1);
	} break;
	case ATOM_PRINTER: {
		putItemInterfaceDesc(win, dX, ttpDY, "Build tiles with Right Click - select the tile type from Backpack/Sandbox/etc.", 1);
	} break;



	case FOOD_RATION_A: {
		putItemInterfaceDesc(win, dX, ttpDY, "Servings: 128. Calories: 120.", 1);
		putItemInterfaceDesc(win, dX, ttpDY, "Provides more nutrients but less calories than normal food.", 2);
	} break;
	case FOOD_RATION_B: {
		putItemInterfaceDesc(win, dX, ttpDY, "Servings: 128. Calories: 120.", 1);
		putItemInterfaceDesc(win, dX, ttpDY, "Provides more nutrients but less calories than normal food.", 2);
	} break;
	case FOOD_RATION_C: {
		putItemInterfaceDesc(win, dX, ttpDY, "Servings: 12. Calories: 350.", 1);
		putItemInterfaceDesc(win, dX, ttpDY, "\"I'm tired of eating the same breakfast every day.\"", 2);
	} break;
	case FOOD_RATION_D: {
		putItemInterfaceDesc(win, dX, ttpDY, "Servings: 12. Calories: 410.", 1);
		putItemInterfaceDesc(win, dX, ttpDY, "\"Actually tastes pretty good.\"", 2);
	} break;
	case FOOD_RATION_E: {
		putItemInterfaceDesc(win, dX, ttpDY, "Servings: 10. Calories: 360.", 1);
		putItemInterfaceDesc(win, dX, ttpDY, "\"It doesn't look that bad.\"", 2);
	} break;
	case FOOD_RATION_F: {
		putItemInterfaceDesc(win, dX, ttpDY, "Servings: 10. Calories: 210.", 1);
		putItemInterfaceDesc(win, dX, ttpDY, "\"Looks pretty disgusting, but it'll have to do for now...\"", 2);
	} break;
	}
}