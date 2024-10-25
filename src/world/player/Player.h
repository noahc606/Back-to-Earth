#pragma once
#include <tuple>
#include "BTEObject.h"
#include "Canvas.h"
#include "Camera.h"
#include "ColorPalette.h"
#include "GUIHandler.h"
#include "Loggable.h"
#include "PlayerAnimation.h"
#include "SpriteSheet.h"
#include "Texture.h"
#include "TileMap.h"


class Player : public BTEObject, public Loggable
{
public:
	enum Action {
		NONE,
		GM_Place_Tile,
		GM_Destroy_Tile,
		SURV_Place_Tile,
		SURV_Destroy_Tile,
	};

	enum BoundingBoxType {
		MAIN,
		EAST, WEST,
		SOUTH, NORTH,
		LOWER, UPPER,
	};

	enum SpaceSuitStates {
		NO_SUIT,
		EMPTYING,
		LEAKING,
		STABLE,
	};

	enum GameModes {
		SANDBOX,
		SURVIVAL,
		HARDCORE,
	};

	/**/
	Player();
	void init(SDLHandler* sh, GUIHandler* guih, Settings* stngs, Controls* ctrls);
	virtual ~Player();
	void destroy();
	/**/
	void draw(Canvas* csEntities, bool debugging);
	void drawCharInMenu();
	void drawHUD();
	void tick(TileMap* tm);
	void tickAnimation();
	void tickControls();
	void applyVelAndCollision(TileMap* tm);
	void collision(TileMap* tm);
	void collisionSnap(int64_t tileCoord, double depthMod, double& playerCoordRef, bool& snapRef);
	void putInfo(std::stringstream& ss, int& tabs);
	/**/
	int getAction();
	Box3X<double> getBounds(int bbt);
	Box3X<double> getBounds();
	bool getCollideState();
	std::vector<double> getPos();
	std::vector<double> getVelComponents();
	double getVel();
	Camera* getCamera();
	bool inGodMode();
	int getGameMode();
	/**/
	void setPos(double, double, double);
	void setModeFromStr(std::string gameMode);
	void setMode(int gameMode);
	void setSpaceSuitState(int sss);

	static const double unit;
protected:

private:
	void updateFacingDirection();
	void rebuildPlayerTex(Texture& tex, bool alt);

/* Object accessors */
	Camera camera;
	GUIHandler* guiHandler;
	Settings* settings;

/* Player visuals */
	PlayerAnimation plan;	//Player texture, color palette, and animation
	Texture hud;			//Player's HUD

/* Player physicals */
	//Ability, body state properties
	double defense = 0; double maxDefense = 100;
	double health = 100; double maxHealth = 100;
	double nutrition = 100; double maxNutrition = 100;
	double water = 100; double maxWater = 100;
	double oxygen = 100; double maxOxygen = 100;
	int spaceSuitState = 0;

	//Supernatural properties
	int gameMode = SANDBOX;
	bool godMode = true;
	bool noclip = false;
	bool controllable = false;



	int action = NONE;

	double runSpeed = 4;
	double age = 0;						//Age in ticks (60ticks = 1 second)
	double x  = 0, y  = 0, z  = 0;    	//Position in world (meters)
	double vx = 0, vy = 0, vz = 0;    	//Velocity in world (meters per tick)
	double ax = 0, ay = 0, az = 0;		//Acceleration in world (meters per tick^2)

/* Player collision */
	bool snapU = 0, snapD = 0;
	bool snapN = 0, snapE = 0, snapS = 0, snapW = 0;

};
