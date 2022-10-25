#include "GUIHandler.h"
#include <set>
#include "Color.h"
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
            if( rbtn->isSelected() ) {
                int idMin = rbtn->getMinGroupMemberID();
                int idMax = rbtn->getMaxGroupMemberID();

                for( GUI* potentialRBtn : guis ) {
                    if( potentialRBtn->getType()==BTEObject::GUI_radiobutton ) {

                        RadioButton* potentialGroupMember = (RadioButton*)potentialRBtn;
                        int thisID = potentialGroupMember->getID();

                        if( thisID>=idMin && thisID<=idMax ){
                            rbtn->deselect();
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

    /** Align GUIs */
    //Align windows
    alignWindows();
    //Align window components (buttons, textboxes, etc)
    //Make sure to center GUIs vertically in the screen THEN horizontally.
    alignWindowComponents(GUIAlignable::CENTER_V);
    alignWindowComponents(GUIAlignable::CENTER_H);

    //Update all non-window guis
    for( GUI* gui : guis ) {
        if( gui->getType()==BTEObject::Type::GUI_button ) {
            Button* btn = ((Button*)gui);
            btn->onWindowUpdate(true);
        } else
        if( gui->getType()==BTEObject::Type::GUI_textbox ) {
            TextBox* txtb = ((TextBox*)gui);
            txtb->onWindowUpdate(true);
        } else
        if( gui->getType()==BTEObject::Type::GUI_tooltip ) {
            Tooltip* ttp = ((Tooltip*)gui);
            ttp->onWindowUpdate(true);
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
            addGUI(new Button( getWindow(win_OPTIONS), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, width, "Graphics Settings", btn_OPTIONS_gs ));
            addGUI(new Button( getWindow(win_OPTIONS), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, width, "Back", btn_OPTIONS_back ));

        } break;
        case CONTROLS: {
            removeGUI(win_OPTIONS);

            int width = 300;
            addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Controls", "", win_CONTROLS ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 80, width, "Move North: [w]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 80, width, "Move East: [d]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 120, width, "Move South: [s]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 120, width, "Move West: [a]", btn_CONTROLS_keybind ));
            addGUI(new Button( getWindow(win_CONTROLS), GUIAlignable::CENTER_H, 160, width, "Back", btn_CONTROLS_back ));

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

        case WORLD_character: {

            if( getGUI(BTEObject::GUI_window, ID::win_CHARACTER)==nullptr ) {

                int w = 18;
                int h = 10;

                /*
                    's': Scroll through buttons representing other menu options
                    'd': Brief Character description. Can be clicked on to view even more info
                    'c': Character menu which is dynamic
                    'x': Character graphic. Translucent background
                    'e': Essential info. 3 graphics which represent Health, oxygen, and nutrition + buttons
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
                addGUI(new RadioButton( getWindow(win_CHARACTER), 30, 60, "Inventory", rbtn_CHARACTER_inventory, rbtn_CHARACTER_tabs_1a, rbtn_CHARACTER_tabs_1b ) );
            } else {
                removeGUI(win_CHARACTER);
            }

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

void GUIHandler::alignWindows()
{
    if( sdlHandler==nullptr )
        return;

    for( GUI* gui : guis ) {
        if ( gui->getType()==BTEObject::GUI_window ) {
            Window* win = (Window*)gui;

            if( win->getHorAlignment()==GUIAlignable::CENTER_H ) {
                win->setSX( (sdlHandler->getWidth()/2-win->getWidth()/2) );
                win->onWindowUpdate(true);
            }
            if( win->getVerAlignment()==GUIAlignable::CENTER_V ) {
                win->setSY( (sdlHandler->getHeight()/2-win->getHeight()/2) );
                win->onWindowUpdate(true);
            }
        }
    }
}

void GUIHandler::alignWindowComponents(int align)
{
    /* Create a set of parent windows and a set of component (x/y) values rounded to the nearest 16. */
    std::set<GUI*> windows;
    std::set<int> compCoords;
    switch(align) {
        case GUIAlignable::L:
        case GUIAlignable::CENTER_H:
        case GUIAlignable::R:
        case GUIAlignable::U:
        case GUIAlignable::CENTER_V:
        case GUIAlignable::D: {
            for( GUI* gui : guis ) {
                if( gui->getType()==BTEObject::GUI_button ||
                    gui->getType()==BTEObject::GUI_textbox ||
                    gui->getType()==BTEObject::GUI_tooltip
                    ) {
                    WindowComponent* wc = (WindowComponent*)gui;
                    Window* pWin = wc->getParentWindow();
                    if( pWin!=nullptr && pWin->getType()==BTEObject::GUI_window ) {
                        windows.insert( wc->getParentWindow() );
                        if( align==GUIAlignable::CENTER_H && wc->getHorAlignment()==GUIAlignable::CENTER_H ) {
                            compCoords.insert( wc->getTY() );
                        }

                        if( align==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V ) {
                            compCoords.insert( wc->getTX() );
                        }
                    }
                }
            }
        } break;
    }

    switch(align) {
        /** Left alignment */
        case GUIAlignable::L: {
            for( GUI* win : windows ) {

                for(int thisCoord : compCoords) {
                    int spacing = 48;
                    int tacw = -16;         //Total aligned components' widths
                    int tach = -spacing;    //Total aligned components' heights
                    int tac = 0;            //Total number of aligned components

                    /* Find 'tac' */
                    for( GUI* gui : guis ) {
                        if( gui->isWindowComponent() ) {
                            WindowComponent* wc = (WindowComponent*)gui;
                            Window* pWin = wc->getParentWindow();
                            if( pWin!=nullptr && pWin->getType()==BTEObject::GUI_window ) {
                                if( align==GUIAlignable::L && wc->getHorAlignment()==GUIAlignable::L ) { }
                                tac++;
                            }
                        }
                    }

                    /* Update each window's coordinate value so that all are displayed next to each other and centered in the window. */
                    if(tac==0) break;

                    int cxp = ( -tacw )/2;  //xPos to set the current component in the loop to.

                    for( GUI* gui : guis ) {
                        if (gui->isWindowComponent()) {
                            WindowComponent* wc = (WindowComponent*)gui;
                            Window* pWin = wc->getParentWindow();
                            if( pWin!=nullptr && pWin->getType()==BTEObject::GUI_window ) {

                                if( align==GUIAlignable::L && wc->getHorAlignment()==GUIAlignable::L ) {
                                    if( pWin==win && wc->getTY()==thisCoord ) {
                                        //wc->setTPos( cxp-6, wc->getTY() );
                                        //wc->onWindowUpdate(true);
                                        cxp += (wc->getWidth()+32 );
                                    }
                                }
                            }
                        }
                    }

                }
            }
        } break;

        /** Center vertically/horizontally alignment */
        case GUIAlignable::CENTER_V:
        case GUIAlignable::CENTER_H: {
            //Go through all windows
            for(GUI* win : windows) {
                ((Window*)win)->translateSPos();

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
                                if( align==GUIAlignable::CENTER_H && wc->getHorAlignment()==GUIAlignable::CENTER_H ) {
                                    if( pWin==win && wc->getTY()==thisCoord ) {
                                        tacw += ( wc->getWidth()+32-12 );
                                    }
                                }
                                if( align==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V ) {
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

                                if( align==GUIAlignable::CENTER_H && wc->getHorAlignment()==GUIAlignable::CENTER_H ) {
                                    if( pWin==win && wc->getTY()==thisCoord ) {
                                        wc->setTPos( cxp-6, wc->getTY() );
                                        wc->onWindowUpdate(true);
                                        cxp += (wc->getWidth()+32 );
                                    }
                                }
                                if( align==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V ) {
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
