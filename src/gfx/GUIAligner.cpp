#include "GUIAligner.h"
#include <set>
#include "GUI.h"
#include "WindowComponent.h"
#include "SDLHandler.h"


void GUIAligner::alignWindows(SDLHandler* sdlHandler, std::vector<GUI*>& guis)
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

void GUIAligner::alignWindowComponents(std::vector<GUI*>& guis, int align)
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
                    int tacw = -16;         //Total aligned components' widths
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
                                        wc->onWindowUpdate();
                                        cxp += (wc->getWidth()+32 );
                                    }
                                }
                                if( align==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V ) {
                                    if( pWin==win && wc->getTX()==thisCoord ) {
                                        wc->setTPos( wc->getTX(), cyp-6 );
                                        wc->onWindowUpdate();
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
