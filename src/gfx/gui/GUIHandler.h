#pragma once
#include <vector>
#include "BTEObject.h"
#include "GUI.h"
#include "Loggable.h"
#include "RadioButton.h"
#include "Window.h"

class GUIHandler : public BTEObject, public Loggable
{
public:
    GUIHandler();
    void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls, bool testing);
    virtual ~GUIHandler();
    /**/
    void draw();
    void tick();
    void onWindowUpdate();
    void passSpecialInput(ControlBinding& cbsi);
    /**/
    void putInfo(std::stringstream& ss, int& tabs);
    int getGUIActionID();
    std::string getGUIActionData();
    RadioButton* getRadioButton(int id);
    Window* getWindow(int id);
    GUI* getGUI(int type, int id, int extraId);
    GUI* getGUI(int type, int id);
    /**/
    void resetGUIAction(std::string methodName);
    void setAllWindowsActiveState(bool isActive);
    void setWindowActiveState(Window* win, bool isActive);
    GUI* addGUI(GUI* gui, int extraID);
    GUI* addGUI(GUI* gui);
    void setGUIs(int guis, int param);
    void setGUIs(int guis);
    void resetGUIs(int extraID);
    void removeGUI(int id);
    void removeGUIs(int idMin, int idMax);
    void removeChildGUIs(int id);
    void removeAllUserGUIs();
    /* Individual GUIs */
    enum ID {
        /* UIs with specific functions */
        null = -1,
        tbx_DEBUG = 0,
        win_COLORSELECTOR = 1,
            sdr_COLORSELECTOR_set_hue,
            btn_COLORSELECTOR_back,

        /* Generic UIs with no specific function */
        ttp_GENERIC,
        btn_GENERIC,

        /* Update prompt */
        win_UPDATE_PROMPT,
        btn_UPDATE_PROMPT_not_now,
        btn_UPDATE_PROMPT_accept,
        btn_UPDATE_PROMPT_dont_ask,

        /* Updating screen */
        win_UPDATING_SCREEN,
        pbr_UPDATING_SCREEN,

        /* Titlescreen */
        win_MAIN,
        ttp_MAIN_title,
        btn_MAIN_play,
        btn_MAIN_options,
        btn_MAIN_exit,

        /* Select Campaign */
        win_SELECT_CAMPAIGN,
            ssr_SELECT_CAMPAIGN_select,
            btn_SELECT_CAMPAIGN_back,
            btn_SELECT_CAMPAIGN_createNew,
            btn_SELECT_CAMPAIGN_openSaveDirectory,

            win_SELECT_CAMPAIGN_CN,
            tbx_SELECT_CAMPAIGN_CN_levelName,
            tbx_SELECT_CAMPAIGN_CN_worldSeed,
            rbtn_SELECT_CAMPAIGN_CN_gameMode0,
            rbtn_SELECT_CAMPAIGN_CN_gameMode1,
            rbtn_SELECT_CAMPAIGN_CN_gameMode2,

            btn_SELECT_CAMPAIGN_CN_showAdditional,
            cbx_SELECT_CAMPAIGN_CN_sp,

            btn_SELECT_CAMPAIGN_CN_mkdir,
            btn_SELECT_CAMPAIGN_CN_defaults,
            btn_SELECT_CAMPAIGN_CN_back,

        /* Main Options */
        win_OPTIONS,
        btn_OPTIONS_controls,
        btn_OPTIONS_graphics_settings,
        btn_OPTIONS_audio_settings,
		btn_OPTIONS_character_settings,
		btn_OPTIONS_debug_settings,
        btn_OPTIONS_back,
        btn_back_to_OPTIONS,

            // Main Options -> Controls
            win_CONTROLS,
            ttp_CONTROLS_keybind,
            tbx_CONTROLS_set_keybind,
            cbx_CONTROLS_set_defaults,

            // Main Options -> Graphics Settings
            win_GRAPHICS_SETTINGS,
			ttp_GRAPHICS_SETTINGS_maxFps, sdr_GRAPHICS_SETTINGS_maxFps, tbx_GRAPHICS_SETTINGS_maxFps,
            cbx_GRAPHICS_SETTINGS_bteCursor,
            cbx_GRAPHICS_SETTINGS_fullscreen,
			ttp_GRAPHICS_SETTINGS_maxRLT, sdr_GRAPHICS_SETTINGS_maxRLT, tbx_GRAPHICS_SETTINGS_maxRLT,

            // Main Options -> Audio Settings
            win_AUDIO_SETTINGS,
            ttp_AUDIO_SETTINGS_masterVolume, sdr_AUDIO_SETTINGS_masterVolume, tbx_AUDIO_SETTINGS_masterVolume,
            ttp_AUDIO_SETTINGS_musicVolume, sdr_AUDIO_SETTINGS_musicVolume, tbx_AUDIO_SETTINGS_musicVolume,
            ttp_AUDIO_SETTINGS_sfxVolume, sdr_AUDIO_SETTINGS_sfxVolume, tbx_AUDIO_SETTINGS_sfxVolume,
            btn_AUDIO_SETTINGS_test,

            // Main Options -> Character Settings
			win_CHARACTER_SETTINGS,
			ttp_CHARACTER_SETTINGS_tooltip,
			csr_CHARACTER_SETTINGS_set_val,
			tbx_CHARACTER_SETTINGS_set_val,
			cbx_CHARACTER_SETTINGS_set_defaults,

            // Main Options -> Debug Settings
            win_DEBUG_SETTINGS,
            cbx_DEBUG_SETTINGS_debugEnabled,
            cbx_DEBUG_SETTINGS_debugOverlayEnabled,
            cbx_DEBUG_SETTINGS_logging,
            ttp_DEBUG_SETTINGS_debugHacks, tbx_DEBUG_SETTINGS_debugHacks,
            ttp_DEBUG_SETTINGS_debugTesting, tbx_DEBUG_SETTINGS_debugTesting,
            cbx_DEBUG_SETTINGS_showIntro,
            btn_DEBUG_SETTINGS_checkForUpdates,
            btn_DEBUG_SETTINGS_forceUpdate,
        /* Pause Screen */
        win_PAUSED,
        btn_PAUSED_back,
        btn_PAUSED_howto,
        btn_PAUSED_options,
        btn_PAUSED_exit,

        /* How-to tutorial screen */
        win_PAUSED_howto,

        /* Character */
        win_CHARACTER,

            // Character -> Inventory
            rbtn_CHARACTER_tabs_1a,
            rbtn_CHARACTER_tabs_1b,

            // Character -> Inventory -> tab tooltip descriptions
            ttp_CHARACTER_tabs_desc,

            // Character -> Inventory -> item tooltip, button, textboxes
            ttp_CHARACTER_item,
            btn_CHARACTER_item,
    		tbx_CHARACTER_item,
    };

    /* Groups of GUIs */
    enum GUIs {
        MAIN_MENU,
        OPTIONS,
            CONTROLS,
            GRAPHICS,
            AUDIO,
			CHARACTER,
			DEBUGGING,

        SELECT_CAMPAIGN,
            SELECT_CAMPAIGN_createNew,

        PAUSE,
            UNPAUSE,
            PAUSE_HOWTO,
        WORLD,
            WORLD_characterMenu_open,
            WORLD_characterMenu_close,
    };

protected:
    std::vector<GUI*> guis;

private:
    ControlBinding cbSpecialInput;
    int guiActionID = -1;
    std::string guiActionData = "";

    /**/
    /**/
    void removeAllGUIs();
    void removeGUIByIndex(int index);
};
