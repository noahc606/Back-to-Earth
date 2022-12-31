#include "TextBox.h"
#include <sstream>
#include "Text.h"
#include "Log.h"

TextBox::TextBox(int p_x, int p_y, int p_width, int p_id)
: Button::Button(nullptr, p_x, p_y, p_width, "", p_id )
{
    setSubType(BTEObject::Type::GUI_textbox);
}
void TextBox::init(SDLHandler* sh, Controls* ctrls)
{
    Button::init(sh, ctrls);
    onWindowUpdate();
}
TextBox::~TextBox(){}


void TextBox::draw()
{
    Button::draw();
}

void TextBox::tick()
{
    Button::tick();

    if(entered) {
        selected = false;
    }
}

void TextBox::onWindowUpdate()
{
    Button::onWindowUpdate();
}

void TextBox::passKeyboardInput(std::string s, bool specialInput)
{
    int ip = btnText.getInsertionPoint();
    std::string btn = btnText.getString();

    std::string s1 = btn.substr(0, ip);
    std::string s2 = s;
    std::string s3 = btn.substr(ip);

    if(s=="\n") {
        entered = true;
        return;
    }
    try {
        if( specialInput ) {

            int i = std::stoi(s2);
            s2="";

            switch( i )
            {
            case SDLK_BACKSPACE:
                s1 = s1.substr(0, s1.size()-1);
                ip--;
            break;
            case SDLK_DELETE:
                s3 = s3.substr(1);
            break;

            case SDLK_LEFT: ip--; break;
            case SDLK_RIGHT: if(s3!="") ip++; break;
            case SDLK_HOME: ip = 0; break;
            case SDLK_END: ip = btn.size(); break;

            case SDLK_ESCAPE: actionID = SDLK_ESCAPE; break;
            case SDLK_UP: actionID = UP_ARROW; break;
            case SDLK_DOWN: actionID = DOWN_ARROW; break;



            }
        }
    } catch (std::exception e) { }

    btnText.setString(s1+s2+s3);
    btnText.setInsertionPoint( ip+s2.size() );
}

void TextBox::setEntered(bool p_entered) { entered = p_entered; }
void TextBox::setString(std::string s)
{
    int i = btnText.getInsertionPoint();
    Button::setString(s);
    if( i>=(int)btnText.getString().size() ) {
        i = btnText.getString().size();
    }
    btnText.setInsertionPoint(i);
}

void TextBox::setInsertionPoint(int ip) { btnText.setInsertionPoint(ip); }
void TextBox::deselect() { selected = false; }

int TextBox::getActionID() { return actionID; }
bool TextBox::isEntered() { return entered; }

void TextBox::resetActionID(std::string methodName)
{
    if( actionID==-1 ) {
        std::stringstream ss;
        ss << "Tried to reset Action ID that already = -1.";
        Log::warn(methodName, ss.str());
    }

    actionID = -1;
}
