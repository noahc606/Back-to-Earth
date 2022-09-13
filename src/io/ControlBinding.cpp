#include "ControlBinding.h"

ControlBinding::ControlBinding()
{
    type = NOTHING;
    key = -1;
    miscButton = -1;
}

ControlBinding::ControlBinding(int p_type, int id)
{
    type = p_type;

    if( p_type==Type::KEY ) {
        miscButton = -1;
        key = id;
    }

    if( p_type==Type::MISC_BUTTON ) {
        miscButton = id;
        key = -1;
    }
}

ControlBinding::ControlBinding(std::string id)
{
    if( id=="MOUSE_LEFT" ) {
        type=MISC_BUTTON;
        key = -1;
        miscButton = MOUSE_LEFT;
    }

    if( id=="MOUSE_MIDDLE" ) {
        type=MISC_BUTTON;
        key = -1;
        miscButton = MOUSE_MIDDLE;
    }

    if( id=="MOUSE_RIGHT" ) {
        type=MISC_BUTTON;
        key = -1;
        miscButton = MOUSE_RIGHT;
    }
}

ControlBinding::~ControlBinding(){}
