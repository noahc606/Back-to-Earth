#include "BTEObject.h"
#include "Log.h"

/**/
BTEObject::BTEObject(){}
void BTEObject::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    sdlHandler = sh;
    fileHandler = fh; settings = fh->getSettings();
    controls = ctrls;

    existing = true;
}
void BTEObject::init(SDLHandler* sh, Controls* ctrls) { BTEObject::init(sh, nullptr, ctrls); }
void BTEObject::init(SDLHandler* sh) { BTEObject::init( sh, nullptr, nullptr ); }
void BTEObject::init(Controls* ctrls) { BTEObject::init( nullptr, nullptr, ctrls ); }
BTEObject::~BTEObject(){}
void BTEObject::destroy()
{
    type = null;
    id = -1;

    existing = false;
}
/**/
void BTEObject::draw(){}
void BTEObject::tick(){}
/**/
int BTEObject::getType() { return type; }
int BTEObject::getID() { return id; }
bool BTEObject::exists() { return existing; }
/**/
void BTEObject::setType(int p_type)
{
    if(type==Type::none) {
        type = p_type;
    } else {
        Log::warn(__PRETTY_FUNCTION__, "Tried to set the type of an object more than once");
    }
}
void BTEObject::setSubType(int p_subtype)
{
    if( type==Type::GUI_button ) {
        switch( p_subtype ) {
            case Type::GUI_radiobutton:
            case Type::GUI_textbox: {
                type = p_subtype;
                return;
            } break;
        }
    }

    Log::warn(__PRETTY_FUNCTION__, "Tried to set the type of an object to an invalid subtype");
}

void BTEObject::setID(int p_id)
{
    if(id==-1) {
        id = p_id;
    } else {
        Log::warn(__PRETTY_FUNCTION__, "Tried to set ID of an object more than once");
    }
}

void BTEObject::setTypeAndID(int p_type, int p_id)
{
    setType(p_type);
    setID(p_id);
}
