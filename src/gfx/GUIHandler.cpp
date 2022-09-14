#include "GUIHandler.h"
#include <set>
#include "Log.h"
#include "MainLoop.h"
#include "TextBox.h"
#include "Tooltip.h"
GUIHandler::GUIHandler(){}
void GUIHandler::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    BTEObject::init(sh, fh, ctrls);

    setGUIs(MAIN_MENU);
}
GUIHandler::~GUIHandler()
{
    /*
    //Delete all GUIs, making sure to go from back to front
    for( int i = (int)guis.size()-1; i>=0; i-- ) {
        guis[i]->destroy();
        delete guis[i];
        guis.erase(guis.begin()+i);
    }
    */
}

void GUIHandler::draw()
{
    //Draw windows
    for( GUI* gui : guis )
        if( gui->getType()==BTEObject::GUI_window )
            gui->draw();

    //Draw buttons/ttbs
    for( GUI* gui : guis )
        if( gui->getType()==BTEObject::GUI_textbox ||
            gui->getType()==BTEObject::GUI_button ||
            gui->getType()==BTEObject::GUI_tooltip
           )
            gui->draw();
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
    for( GUI* gui : guis ) {
        if( gui->getType()==BTEObject::Type::GUI_button ) {
            Button* btn = ((Button*)gui);
            btn->onWindowUpdate(true);
        } else
        if( gui->getType()==BTEObject::Type::GUI_textbox ) {
            TextBox* txtb = ((TextBox*)gui);
            txtb->onWindowUpdate(true);
        } else
        if( gui->getType()==BTEObject::Type::GUI_window ) {
            Window* win = ((Window*)gui);
            win->onWindowUpdate(true);
        } else
        if( gui->getType()==BTEObject::Type::GUI_tooltip ) {
            Tooltip* ttp = ((Tooltip*)gui);
            ttp->onWindowUpdate(true);
        }
    }

    /** Prepare all guis that need to be aligned */
    //Set horizontal GUIs' TX to 0, vertical GUIs' TY to 0.
    for( GUI* gui : guis ) {
        if (gui->getType()==BTEObject::GUI_button ||
            gui->getType()==BTEObject::GUI_textbox ||
            gui->getType()==BTEObject::GUI_tooltip
            ) {
            WindowComponent* wc = (WindowComponent*)gui;
            if( wc->getAlignment(0)==WindowComponent::CENTER_H ) {
                wc->setTX(0);
            }
            if( wc->getAlignment(1)==WindowComponent::CENTER_V ) {
                wc->setTY(0);
            }
        }
    }

    /** Align window components (buttons, textboxes, etc) */
    //Make sure to center GUIs vertically in the screen THEN horizontally.

    //alignWindowComponents(GUIAlignable::L);
    alignWindowComponents(GUIAlignable::CENTER_V);
    alignWindowComponents(GUIAlignable::CENTER_H);
    //alignWindowComponents(GUIAlignable::R);
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
        case MAIN_MENU: {
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
            removeAllUserGUIs();
            removeGUIs(btn_MAIN_play, 199);

            //addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Options", "", win_OPTIONS ));
            addGUI(new Window( 300, 300, 500, 500, "Options", "", win_OPTIONS ));
            addGUI(new Button( getWindow(win_OPTIONS), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Graphics Settings", btn_OPTIONS_gs ));
            addGUI(new Button( getWindow(win_OPTIONS), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Back", btn_OPTIONS_back ));

        } break;
        case WORLD: {
            removeAllUserGUIs();
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
        Log::warn(__PRETTY_FUNCTION__, ss.str());
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
    //Call texture destructor
    gui->destroy();
}

void GUIHandler::alignWindowComponents(int p_alignment)
{
    /* Create a set of parent windows and a set of component (x/y) values rounded to the nearest 16. */
    std::set<GUI*> windows;
    std::set<int> compCoords;
    switch(p_alignment) {
        case GUIAlignable::L:
        case GUIAlignable::CENTER_H:
        case GUIAlignable::R:
        case GUIAlignable::U:
        case GUIAlignable::CENTER_V:
        case GUIAlignable::D: {
            for( GUI* gui : guis ) {
                if (gui->getType()==BTEObject::GUI_button ||
                    gui->getType()==BTEObject::GUI_textbox ||
                    gui->getType()==BTEObject::GUI_tooltip
                    ) {
                    WindowComponent* wc = (WindowComponent*)gui;
                    Window* pWin = wc->getParentWindow();
                    if( pWin!=nullptr && pWin->getType()==BTEObject::GUI_window ) {
                        windows.insert( wc->getParentWindow() );
                        if( p_alignment==GUIAlignable::CENTER_H && wc->getHorAlignment()==GUIAlignable::CENTER_H ) {
                            compCoords.insert( wc->getTY() );
                        }

                        if( p_alignment==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V ) {
                            compCoords.insert( wc->getTX() );
                        }
                    }
                }
            }
        } break;
    }

    switch(p_alignment) {
        case GUIAlignable::L: {
            for( GUI* gui : guis ) {
                if (gui->getType()==BTEObject::GUI_button ||
                    gui->getType()==BTEObject::GUI_textbox ||
                    gui->getType()==BTEObject::GUI_tooltip )
                {
                    Button* btn = (Button*)gui;
                    btn->setTPos( 12, btn->getTY() );
                }
            }
        } break;

        case GUIAlignable::CENTER_V:
        case GUIAlignable::CENTER_H: {
            //Go through all windows
            for(GUI* win : windows) {
                ((Window*)win)->updateScreenPos();

                for(int thisCoord : compCoords) {

                    int spacing = 48;
                    int tacw = -16;         //Total aligned components' widths
                    int tach = -spacing;    //Total aligned components' heights
                    int tac = 0;            //Total number of aligned components

                    /* Find 'tacw', 'tach', and 'tac' */
                    for( GUI* gui : guis ) {
                        if (gui->getType()==BTEObject::GUI_button ||
                            gui->getType()==BTEObject::GUI_textbox ||
                            gui->getType()==BTEObject::GUI_tooltip )
                        {
                            WindowComponent* wc = (WindowComponent*)gui;
                            Window* pWin = wc->getParentWindow();
                            if( pWin!=nullptr && pWin->getType()==BTEObject::GUI_window ) {
                                if( p_alignment==GUIAlignable::CENTER_H && wc->getHorAlignment()==GUIAlignable::CENTER_H ) {
                                    if( pWin==win && wc->getTY()==thisCoord ) {
                                        tacw += ( wc->getWidth()+32-12 );
                                    }
                                }
                                if( p_alignment==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V ) {
                                    if( pWin==win && wc->getTX()==thisCoord ) {
                                        tach += ( wc->getHeight()+spacing );
                                    }
                                }

                                tac++;
                            }
                        }
                    }

                    /* Update each window's coordinate value so that all are displayed next to each other and centered in the window. */
                    if(tac==0) break;


                    int cxp = ( ((Window*)win)->getWidth()-tacw )/2;  //xPos to set the current component in the loop to.
                    int cyp = ( ((Window*)win)->getHeight()-tach )/2;  //yPos to set the current component in the loop to.

                    for( GUI* gui : guis ) {

                        if (gui->getType()==BTEObject::GUI_button ||
                            gui->getType()==BTEObject::GUI_textbox ||
                            gui->getType()==BTEObject::GUI_tooltip )
                        {
                            WindowComponent* wc = (WindowComponent*)gui;
                            Window* pWin = wc->getParentWindow();
                            if( pWin!=nullptr && pWin->getType()==BTEObject::GUI_window ) {

                                if( p_alignment==GUIAlignable::CENTER_H && wc->getHorAlignment()==GUIAlignable::CENTER_H ) {
                                    if( pWin==win && wc->getTY()==thisCoord ) {
                                        wc->setTPos( cxp-6, wc->getTY() );
                                        wc->onWindowUpdate(true);
                                        cxp += (wc->getWidth()+32 );
                                    }
                                }
                                if( p_alignment==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V ) {
                                    if( pWin==win && wc->getTX()==thisCoord ) {
                                        wc->setTPos( wc->getTX(), cyp-6 );
                                        wc->onWindowUpdate(true);
                                        cyp += (wc->getHeight()+spacing );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } break;
    }
}
