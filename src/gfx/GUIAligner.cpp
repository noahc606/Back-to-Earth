#include "GUIAligner.h"
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
    for( GUI* gui : guis ) {
        if( gui->getType()==BTEObject::GUI_button ||
            gui->getType()==BTEObject::GUI_textbox ||
            gui->getType()==BTEObject::GUI_tooltip ||
            gui->getType()==BTEObject::GUI_progressbar
        ) {
            WindowComponent* wc = (WindowComponent*)gui;
            Window* pWin = wc->getParentWindow();
            if( pWin!=nullptr && pWin->getType()==BTEObject::GUI_window ) {
                //Insert into 'windows' this parent window, if it doesn't exist there already
                windows.insert( wc->getParentWindow() );

                //Insert into 'compCoords' a GUI Y value, if it doesn't exist there already
                if(
                    (align==GUIAlignable::L        && wc->getHorAlignment()==GUIAlignable::L) ||
                    (align==GUIAlignable::CENTER_H && wc->getHorAlignment()==GUIAlignable::CENTER_H) ||
                    (align==GUIAlignable::R        && wc->getHorAlignment()==GUIAlignable::R)
                    ) {
                    compCoords.insert( wc->getTY() );
                }

                //Insert into 'compCoords' a GUI X value, if it doesn't exist there already
                if(
                    (align==GUIAlignable::U        && wc->getVerAlignment()==GUIAlignable::U) ||
                    (align==GUIAlignable::CENTER_V && wc->getVerAlignment()==GUIAlignable::CENTER_V) ||
                    (align==GUIAlignable::D        && wc->getVerAlignment()==GUIAlignable::D)
                    ) {
                    compCoords.insert( wc->getTX() );
                }
            }
        }
    }

    /* Using the information from above, align window comoponents */
    switch(align) {
        /** Left/Right alignment */
        case GUIAlignable::L:
        case GUIAlignable::R: {
            alignWindowComponentsLR(guis, align, windows, compCoords);
        } break;

        /** Up/Down alignment */
        case GUIAlignable::U:
        case GUIAlignable::D: {
            alignWindowComponentsUD(guis, align, windows, compCoords);
        } break;

        /** Center vertically/horizontally alignment */
        case GUIAlignable::CENTER_V:
        case GUIAlignable::CENTER_H: {
            alignWindowComponentsVH(guis, align, windows, compCoords);
        } break;
    }
}

void GUIAligner::alignWindowComponentsVH(std::vector<GUI*>& guis, int align, std::set<GUI*>& windows, std::set<int>& compCoords)
{
    //Go through all windows
    for(GUI* win : windows) {
        ((Window*)win)->translateSPos();

        for(int thisCoord : compCoords) {

            int spacing = 36;
            int tacw = -48;         //Total aligned components' widths
            int tach = -spacing;    //Total aligned components' heights
            int tac = 0;            //Total number of aligned components

            /* Find 'tacw', 'tach', and 'tac' */
            for( GUI* gui : guis ) {
                if (gui->getType()==BTEObject::GUI_button ||
                    gui->getType()==BTEObject::GUI_textbox ||
                    gui->getType()==BTEObject::GUI_tooltip ||
                    gui->getType()==BTEObject::GUI_progressbar    
                )
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
                    gui->getType()==BTEObject::GUI_tooltip ||
                    gui->getType()==BTEObject::GUI_progressbar
                )
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
}

void GUIAligner::alignWindowComponentsLR(std::vector<GUI*>& guis, int align, std::set<GUI*>& windows, std::set<int>& compCoords)
{
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
}

void GUIAligner::alignWindowComponentsUD(std::vector<GUI*>& guis, int align, std::set<GUI*>& windows, std::set<int>& compCoords)
{

}
