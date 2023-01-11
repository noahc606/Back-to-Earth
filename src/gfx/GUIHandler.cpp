#include "GUIHandler.h"
#include <set>
#include "CheckBox.h"
#include "Color.h"
#include "GUIAligner.h"
#include "Log.h"
#include "MainLoop.h"
#include "RadioButton.h"
#include "TextBox.h"
#include "Tooltip.h"
#include "WindowData.h"

GUIHandler::GUIHandler(){}
void GUIHandler::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    BTEObject::init(sh, fh, ctrls);

    setGUIs(MAIN);
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
        if( gui->getType()==BTEObject::GUI_window ) {
            gui->draw();
        }
    }

    //Draw window components (we do it after windows since wc's go on top of windows)
    for( GUI* gui : guis ) {
        if( gui->isWindowComponent() ) {
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

    //Iterate through all guis
    for( GUI* gui : guis ) {
        //If gui is a textbox
        if( gui->getType()==BTEObject::GUI_textbox ) {
            //cast gui to textbox
            TextBox* txtb = ((TextBox*)gui);
            //If text box selected
            if( txtb->isSelected() ) {
                if(kbInput!="") {
                    lastKBInput = kbInput;
                    lastKBInputSpecial = kbInputSpecial;
                    txtb->passKeyboardInput(kbInput, kbInputSpecial);
                }
            }
            //If pressing enter in textbox
            if( txtb->isEntered() ) {
                Commands::executeCMD(txtb->getString());
                txtb->setEntered(false);
                txtb->setString("");
            }
            int a = txtb->getActionID();
            if( a!=TextBox::Actions::NONE ) {

                if(txtb->getID()==ID::tb_DEBUG ) {
                    if( a==TextBox::Actions::UP_ARROW ) {
                        std::string s = Commands::cycleCMDs(-1);
                        if( s!="" ) {
                            txtb->setString(s);
                            txtb->setInsertionPoint(s.size());
                        }
                    }
                    if( a==TextBox::Actions::DOWN_ARROW ) {
                        std::string s = Commands::cycleCMDs(1);
                        if( s!="" ) {
                            txtb->setString(s);
                            txtb->setInsertionPoint(s.size());
                        }
                    }
                    if( a==SDLK_ESCAPE ) {
                        txtb->setString("");
                        txtb->setInsertionPoint(0);
                        txtb->deselect();
                        Commands::cycleCMDs(1000000);
                    }

                }

                txtb->resetActionID(__PRETTY_FUNCTION__);
            }
        }
        //If gui is a button
        if( gui->getType()==BTEObject::GUI_button ) {
            //cast gui to button
            Button* btn = ((Button*)gui);
            //If button clicked
            if( btn->isSelected() ) {
                guiActionID = btn->getID();
                removeGUI( btn->getID() );
            }
        }
        //If gui is a radiobutton
        if( gui->getType()==BTEObject::GUI_radiobutton ) {
            //cast gui to radiobutton
            RadioButton* rbtn = ((RadioButton*)gui);
            //If radiobutton clicked
            if( rbtn->justClicked() && rbtn->isSelected() ) {
                rbtn->unclick();
                int idMin = rbtn->getMinGroupMemberID();
                int idMax = rbtn->getMaxGroupMemberID();

                for( GUI* potentialRBtn : guis ) {
                    if( potentialRBtn->getType()==BTEObject::GUI_radiobutton ) {
                        int thisID = potentialRBtn->getID();

                        if( thisID>=idMin && thisID<=idMax && thisID!=rbtn->getID() ) {
                            ((RadioButton*)potentialRBtn)->deselect();
                        }
                    }
                }
            }
        }

        //Tick gui
        gui->tick();
    }

    //Set keyboard input to nothing (Keep this at the end of the method!)
    kbInput = "";
    kbInputSpecial = false;
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

void GUIHandler::passKeyboardInput(std::string text, bool special)
{
    kbInput = text;
    kbInputSpecial = special;
}

void GUIHandler::info(std::stringstream& ss, int& tabs)
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

void GUIHandler::addGUI(GUI* gui)
{
    switch(gui->getType()) {
        case BTEObject::GUI_tooltip: {
            gui->init(sdlHandler);
        } break;
        default: {
            gui->init(sdlHandler, controls);
        } break;
    }

    guis.push_back(gui);
}

void GUIHandler::setGUIs(int guis)
{
    switch(guis) {
        case MAIN: {
            AudioLoader* al = sdlHandler->getAudioLoader();
            al->play(AudioLoader::TITLE_impact);

            removeAllUserGUIs();
            addGUI(new Window(win_MAIN));
            addGUI(new Tooltip(getWindow(win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, "Back to Earth", ttp_MAIN_title ));
            addGUI(new Button( getWindow(win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Play", btn_MAIN_play ));
            addGUI(new Button( getWindow(win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Options", btn_MAIN_options ));
            addGUI(new Button( getWindow(win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Exit", btn_MAIN_exit ));


        } break;
        case OPTIONS: {
            removeGUI(ttp_MAIN_title);
            removeGUI(btn_MAIN_play);
            removeGUI(btn_MAIN_options);
            removeGUI(btn_MAIN_exit);

            removeGUI(win_PAUSED);

            removeGUI(win_CONTROLS);
            removeGUI(win_GRAPHICS_SETTINGS);

            int width = 300;
            addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Options", "", win_OPTIONS ));
            addGUI(new Button( getWindow(win_OPTIONS), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, width, "Controls", btn_OPTIONS_controls ));
            addGUI(new Button( getWindow(win_OPTIONS), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, width, "Graphics Settings", btn_OPTIONS_graphics_settings ));
            addGUI(new Button( getWindow(win_OPTIONS), GUIAlignable::CENTER_H, 730, width, "Back", btn_OPTIONS_back ));

        } break;
        case CONTROLS: {
            removeGUI(win_OPTIONS);

            int width = 300;
            addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Controls", "", win_CONTROLS ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 80, width, "Move North: [w]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 80, width, "Move East: [d]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 120, width, "Move South: [s]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 120, width, "Move West: [a]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 730, width, "Back", btn_back_to_OPTIONS ));
        } break;
        case GRAPHICS_SETTINGS: {
            removeGUI(win_OPTIONS);

            int width = 300;
            addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Graphics Settings", "", win_GRAPHICS_SETTINGS ));
            //addGUI(new CheckBox( getWindow(win_CONTROLS), 40, 80, "Use Custom Cursor", CheckBox::FALSE, cbx_GRAPHICS_SETTINGS_bteCursor ));
            addGUI(new CheckBox( getWindow(win_CONTROLS), 40, 120, "Force Fullscreen on Startup", CheckBox::FALSE, cbx_GRAPHICS_SETTINGS_fullscreen ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 730, width, "Back", btn_back_to_OPTIONS ));
        } break;


        case PAUSE: {
            removeGUI(win_OPTIONS);

            addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 600, 500, "Paused", "", win_PAUSED ));
            addGUI(new Button( getWindow(win_PAUSED), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 300, "Back to Game", btn_PAUSED_back ));
            addGUI(new Button( getWindow(win_PAUSED), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 300, "Options", btn_PAUSED_options ));
            addGUI(new Button( getWindow(win_PAUSED), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 300, "Save & Exit", btn_PAUSED_exit ));
        } break;
        case UNPAUSE: {
            removeGUI(win_PAUSED);
        } break;

        case WORLD: {
            removeAllUserGUIs();
        } break;

        case WORLD_characterMenu_open: {
            if( getGUI(BTEObject::GUI_window, ID::win_CHARACTER)==nullptr ) {

                int w = 18;
                int h = 10;

                /*
                    's': Scroll through buttons representing other menu options
                    'd': Brief Character description. Can be clicked on to view even more info
                    'c': Character menu which is dynamic
                    'x': Character graphic. Translucent background
                    'e': Essential info. 3 graphics which represent Health, nutrition, oxygen + buttons
                         to view specifics of these 3 graphics. More on the right half.
                    'b': Bottom tab for extra pages (next/prev)

                */
                WindowData* wd = new WindowData(w, h);
                wd->setPanelData(0, "ssssssddddcccccccc");
                wd->setPanelData(1, "ssssssxxxxcccccccc");
                wd->setPanelData(2, "ssssssxxxxcccccccc");
                wd->setPanelData(3, "ssssssxxxxcccccccc");
                wd->setPanelData(4, "ssssssxxxxcccccccc");
                wd->setPanelData(5, "ssssssxxxxcccccccc");
                wd->setPanelData(6, "ssssssddddcccccccc");
                wd->setPanelData(7, "ssssssddddcccccccc");
                wd->setPanelData(8, "eeeeeeeeeeeeeeeeee");
                wd->setPanelData(9, "eeeeeeeeeeeeeeeeee");
                wd->setPanelData(10,"bbbbbbbbbbbbbbbbbb");

                wd->setPanelColor('s', Color(0, 0, 200, 240) );
                wd->setPanelColor('d', Color(150, 105, 55, 240) );
                wd->setPanelColor('c', Color(255, 255, 255, 240) );
                wd->setPanelColor('x', Color(0, 100, 0, 240) );
                wd->setPanelColor('e', Color(100, 0, 0, 240) );
                wd->setPanelColor('b', Color(130, 210, 180, 240) );

                addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, wd, win_CHARACTER ));
                addGUI(new Tooltip( getWindow(win_CHARACTER), 30, 30, "Character Tabs", ttp_CHARACTER_tabs_desc ) );
                addGUI(new RadioButton( getWindow(win_CHARACTER), 30, 60, "Inventory", true, rbtn_CHARACTER_inventory, rbtn_CHARACTER_tabs_1a, rbtn_CHARACTER_tabs_1b ) );
                addGUI(new RadioButton( getWindow(win_CHARACTER), 30, 100, "Engineering", rbtn_CHARACTER_engineering, rbtn_CHARACTER_tabs_1a, rbtn_CHARACTER_tabs_1b ) );
            }
        } break;
        case WORLD_characterMenu_close: {
            removeGUI(win_CHARACTER);
        } break;


    }


    //Window is updated whenever new GUIs are set
    onWindowUpdate();
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

