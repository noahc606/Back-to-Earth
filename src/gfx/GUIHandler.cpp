#include "GUIHandler.h"
#include <set>
#include "CheckBox.h"
#include "Color.h"
#include "GUIAligner.h"
#include "GUIBuilder.h"
#include "Log.h"
#include "MainLoop.h"
#include "RadioButton.h"
#include "Slider.h"
#include "TextBox.h"
#include "Tooltip.h"
#include "WindowData.h"

GUIHandler::GUIHandler(){}
void GUIHandler::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls, bool testing)
{
    BTEObject::init(sh, fh, ctrls);
    if(!testing) {
        setGUIs(MAIN);
    }
}
GUIHandler::~GUIHandler()
{
    //Delete all GUIs, making sure to go from back to front
    for( int i = (int)guis.size()-1; i>=0; i-- ) {
        guis[i]->destroy();
        delete guis[i];
        guis.erase(guis.begin()+i);
    }
}

void GUIHandler::draw()
{
    //Draw windows
    for( GUI* gui : guis ) {
        if( gui->getType()==BTEObject::GUI_window && gui->exists() ) {
            gui->draw();
        }
    }

    //Draw window components (we do it after windows since window components go on top of windows)
    for( GUI* gui : guis ) {
        if( gui->isWindowComponent() && gui->exists() ) {
            gui->draw();
        }
    }

}

void GUIHandler::tick()
{
    //Delete all guis that should be deleted
    for( int i = (int)guis.size()-1; i>=0; i-- ) {
        GUI* gui = guis[i];
        int id = gui->getID();
        int type = gui->getType();

        if( id==-1 && type==BTEObject::Type::null ) {
            //Call object destructor
            delete guis[i];

            //Erase object from guis vector
            guis.erase( guis.begin()+i );
        }
    }

    //Upon click
    if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
        for( GUI* gui : guis ) {
            //If we find a textbox
            if( gui->getType()==BTEObject::GUI_textbox ) {
                //Deselect it if user is not hovering over it
                TextBox* tbx = ((TextBox*)gui);
				if( !tbx->isHovering() ) {
					tbx->deselect();
				}
            }
        }
    }


    //Iterate through all guis
    for( GUI* gui : guis ) {

		switch( gui->getType() ) {
		
		case BTEObject::GUI_textbox: {
			//cast gui to textbox
			TextBox* tbx = ((TextBox*)gui);
			//If text box selected
			if( tbx->isSelected() ) {
				if( cbSpecialInput.getType()!=ControlBinding::NOTHING ) {
					tbx->passSpecialInput(cbSpecialInput);
				}
			}
			//If pressing enter in textbox
			if( tbx->isEntered() ) {
				Commands::executeCMD(tbx->getString());
				tbx->setEntered(false);
				tbx->setString("");
			}
			int a = tbx->getActionID();
			if( a!=TextBox::Actions::NONE ) {

				if(tbx->getID()==ID::tbx_DEBUG ) {
					if( a==TextBox::Actions::UP_ARROW ) {
						std::string s = Commands::cycleCMDs(-1);
						if( s!="" ) {
							tbx->setString(s);
							tbx->setInsertionPoint(s.size());
						}
					}
					if( a==TextBox::Actions::DOWN_ARROW ) {
						std::string s = Commands::cycleCMDs(1);
						if( s!="" ) {
							tbx->setString(s);
							tbx->setInsertionPoint(s.size());
						}
					}
					if( a==SDLK_ESCAPE ) {
						tbx->setString("");
						tbx->setInsertionPoint(0);
						tbx->deselect();
						Commands::cycleCMDs(1000000);
					}

				}

				tbx->resetActionID(__PRETTY_FUNCTION__);
			}
		} break;
		
		case BTEObject::GUI_button: {
			//cast gui to button
			Button* btn = ((Button*)gui);
			//If button selected (not clicked)
			if( btn->isSelected() ) {
				guiActionID = btn->getID();
				removeGUI( btn->getID() );
				AudioLoader* al = sdlHandler->getAudioLoader();
				al->play(AudioLoader::TITLE_button, 0.15);
			}
		} break;
		
		case BTEObject::GUI_radiobutton: {
			//cast gui to radiobutton
			RadioButton* rbtn = ((RadioButton*)gui);
			//If radiobutton clicked
			if( rbtn->justClicked() && rbtn->isSelected() ) {
				rbtn->unclick();
				int idMin = rbtn->getMinGroupMemberID();
				int idMax = rbtn->getMaxGroupMemberID();

				for( GUI* potentialRBtn : guis ) {
					//Look through every radioButton
					if( potentialRBtn->getType()==BTEObject::GUI_radiobutton ) {
						int thisID = potentialRBtn->getID();
						
						if( thisID>=idMin && thisID<=idMax && thisID!=rbtn->getID() ) {
							//Deselect all radio buttons that are in this group, except the one that was just clicked.
							((RadioButton*)potentialRBtn)->deselect();
						} else {
							//SET gui action ID to the ID of the radio button that was just clicked
							guiActionID = thisID;
						}
					}
				}
			}
		} break;
	
		case BTEObject::GUI_checkbox: {
			CheckBox* cbx = ((CheckBox*)gui);

			//If that checkbox is clicked and needs to reset something
			if( cbx->justClicked() ) {
				//Unclick
				cbx->unclick();
				//If this is a reset checkbox
				if( cbx->getState()==CheckBox::RESET ) {
					//Reset all GUIs with the same extraID as this checkBox
					resetGUIs(cbx->getExtraID());
				}
			}
		} break;
		
		case BTEObject::GUI_slider: {
			Slider* sdr = ((Slider*)gui);
			sdr->syncWithTextboxes(this);
		} break;
		
		}

        //Tick gui
        gui->tick();
    }

    //Reset special input (Keep this at the end of the method!)
    cbSpecialInput.reset();
}

void GUIHandler::onWindowUpdate()
{
    //Update properties of all buttons or tooltips in case the window was just resized.
    //Update all window GUIs
    for( GUI* gui : guis ) {
        if( gui->getType()==BTEObject::Type::GUI_window ) {
            Window* win = ((Window*)gui);
            win->onWindowUpdate(true);
        }
    }

    /** Prepare all guis that need to be aligned */
    //Set horizontal GUIs' TX to 0, vertical GUIs' TY to 0.
    for( GUI* gui : guis ) {
        if (gui->isWindowComponent()) {
            WindowComponent* wc = (WindowComponent*)gui;
            if( wc->getAlignment(0)==WindowComponent::CENTER_H ) {
                wc->setTX(0);
            }
            if( wc->getAlignment(1)==WindowComponent::CENTER_V ) {
                wc->setTY(0);
            }
        }
    }

    /** Align GUIs */
    //Align windows
    GUIAligner::alignWindows(sdlHandler, guis);

    //Align window components (buttons, textboxes, etc). Make sure to center GUIs vertically in the screen THEN horizontally.
    GUIAligner::alignWindowComponents(guis, GUIAlignable::CENTER_V);
    GUIAligner::alignWindowComponents(guis, GUIAlignable::CENTER_H);

    //Update all non-window guis
    for( GUI* gui : guis ) {
        if( gui->isWindowComponent() ) {
            ((WindowComponent*)gui)->onWindowUpdate();
        }
    }
}

void GUIHandler::passSpecialInput(ControlBinding& passedCB) { cbSpecialInput += passedCB; }

void GUIHandler::putInfo(std::stringstream& ss, int& tabs)
{
    DebugScreen::indentLine(ss, tabs);
    ss << "guis.size()=" << guis.size() << ";";
    DebugScreen::newLine(ss);
}

int GUIHandler::getGUIActionID() { return guiActionID; }

Window* GUIHandler::getWindow(int id)
{
    GUI* gui = getGUI(BTEObject::Type::GUI_window, id);
    if(gui!=nullptr) {
        return (Window*)gui;
    }

    return nullptr;
}


GUI* GUIHandler::getGUI(int type, int id, int extraId)
{
	for( int i = 0; i<(int)guis.size(); i++ ) {
		GUI* gui = guis[i];
		if( gui->getType()==type ) {
			if( gui->getID()==id && gui->getExtraID()==extraId ) {
				return guis[i];
			}
		}
	}
	
	return nullptr;
}
GUI* GUIHandler::getGUI(int type, int id)
{
	for( int i = 0; i<(int)guis.size(); i++ ) {
		if( (guis[i])->getType()==type ) {
			if( (guis[i])->getID()==id ) {
				return guis[i];
			}
		}
	}

    return nullptr;
}

void GUIHandler::resetGUIAction(std::string methodName)
{
    if( guiActionID==-1 ) {
        std::stringstream ss;
        ss << "Tried to reset GUI Action that already = -1.";
        Log::warn(methodName, ss.str());
    }

    guiActionID = -1;
}

GUI* GUIHandler::addGUI(GUI* gui, int extraID)
{
    //Set extra ID
    gui->setExtraID(extraID);
    //Call init constructor based on its type
    switch(gui->getType()) {
        case BTEObject::GUI_tooltip: {
            gui->init(sdlHandler);
        } break;
        case BTEObject::GUI_textbox: {
            gui->init(sdlHandler, fileHandler, controls);
        } break;
        default: {
            gui->init(sdlHandler, controls);
        } break;
    }

    //Add the GUI to 'guis'
    guis.push_back(gui);
    //Return gui
    return gui;
}

GUI* GUIHandler::addGUI(GUI* gui) { return addGUI(gui, -1); }

void GUIHandler::setGUIs(int guis)
{
    GUIBuilder gb;

    switch(guis) {
        /** Main UIs */
        case MAIN: {
            AudioLoader* al = sdlHandler->getAudioLoader();
            gb.buildTitleScreen(*this);
        } break;
        case OPTIONS: {
            gb.buildMainOptions(*this);
        } break;
        case CONTROLS: {
            gb.buildMainControls(*this, *fileHandler);
        } break;
        case GRAPHICS: {
            gb.buildMainGraphics(*this, *fileHandler);
        } break;

        /** Pause/Unpause in World */
        case PAUSE: {
			gb.buildWorldPause(*this);
        } break;
        case UNPAUSE: {
            removeGUI(win_PAUSED);
			removeGUI(win_OPTIONS);
			removeGUI(win_CONTROLS);
			removeGUI(win_GRAPHICS_SETTINGS);
        } break;

        /** Active World UIs */
        case WORLD: {
            removeAllUserGUIs();
        } break;
        case WORLD_characterMenu_open: {
            gb.buildCharacterMenu(*this);
        } break;
        case WORLD_characterMenu_close: {
            removeGUI(win_CHARACTER);
        } break;
    }

    //Window is updated whenever new GUIs are set
    onWindowUpdate();
}

void GUIHandler::resetGUIs(int extraID)
{
    for( GUI* gui : guis ) {
        if( gui->getType()==BTEObject::GUI_textbox ) {
            TextBox* tbx = (TextBox*)gui;
            if( tbx->getExtraID()==extraID ) {
                tbx->resetEnteredData();
            }
        }
    }
}

void GUIHandler::removeGUI(int id) { removeGUIs(id, id); }

void GUIHandler::removeGUIs(int idMin, int idMax)
{
    bool foundID = false;
    for( int i = (int)guis.size()-1; i>=0; i-- ) {
        int id = (guis[i])->getID();
        if( id>=idMin && id<=idMax ) {
            foundID = true;
            removeGUIByIndex(i);
        }
    }

    if(!foundID) {
        //Create message
        std::stringstream ss;
        if( idMin==idMax ) {
            ss << "Tried to delete nonexistent ID '" << idMin << "'";
        } else {
            ss << "Could not find any GUI objects with an ID between '" << idMin << "' and '" << idMax << "'";
        }

        //Log message
        Log::trbshoot(__PRETTY_FUNCTION__, ss.str());
    }
}

void GUIHandler::removeChildGUIs(int id)
{
    GUI* parentGUI = guis[id];
    if( parentGUI->getType()==BTEObject::Type::GUI_window ) {

        for( int i = (int)guis.size()-1; i>=0; i-- ) {
            GUI* tempGUI = guis.at(i);
            if( tempGUI->isWindowComponent() ) {
                if( ((WindowComponent*)tempGUI)->getParentWindow()==(Window*)parentGUI ) {
                    removeGUIByIndex(i);
                }
            }
        }
    }
}

void GUIHandler::removeAllUserGUIs()
{
    for( int i = (int)guis.size()-1; i>=0; i-- ) {
        if( guis.at(i)->isUserGUI() ) {
            removeGUIByIndex(i);
        }
    }
}

/**/

/**
    NOTE: index = element in vector, index not the same as id
*/
void GUIHandler::removeGUIByIndex(int index)
{
    GUI* gui = guis[index];

    if( gui->getType()==BTEObject::Type::GUI_window ) {
        removeChildGUIs(index);
    }

    //Call texture destructor
    gui->destroy();
}