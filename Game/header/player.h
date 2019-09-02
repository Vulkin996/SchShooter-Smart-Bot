#ifndef PLAYER_DEF
#define PLAYER_DEF

#define NUM_OF_SOURCES_PLAYER 5
#include "weapon.h"
#include "../header/util.h"
#include <Box2D/Box2D.h>
#include <AL/alut.h>

typedef struct _Input{
    float horizontal, vertical, angle, cursorDist;
    bool shoot;
} Input;

class Brain;

class Player : public Colider{
public:
    Player(float x, float y, float r);
    Player();
    void Draw();
    void DrawShadow();
	void SwapWeapon(Weapon* newWeapon);
	void takeDmg(int dmg);
	void takeDmg(int dmg, b2Vec2 bulletVel, int m_team);
	void IncreaseHealth(int amount);
	void IncreaseGrenades(int amount);
	void throwGrenade();
	void Revive();
	void SetMaxHealth(int mh);
	void SetMaxArmor(int ma);
	void SetAmmo(int am);
	int GetAmmo();
    int GetHealth();
	void FillArmor();
	void SwitchToSecondary();
	void moveSoundSource();
	void FreeSources();


    virtual ClassID getClassID();
    void SetBrain(Brain* brain);
    Brain* m_brain;
    bool team;
	bool alive;
	bool deathFlag;
    Input input;
    Weapon* equiped_weapon;
    b2Body* body;
    float r;
	int grenades;
    bool see_player;
    void die();
	ALuint soundSource[NUM_OF_SOURCES_PLAYER];
	Pistol* secondaryGun;

private:
    float speed;
    int health;
	int maxHealth;
	int armor;
	int ammo;
	int maxArmor;


protected:

};

class Brain{
protected:
    Brain(Player& player);
    Player* m_player;


public:
    virtual void Update() = 0;
    virtual ~Brain(){}
};


class playerBrain: public Brain{
public:
    playerBrain(Player& player);
    float* generateInput();
		bool hasLineOfSight(Player* target);
public:
    void Update();
};

class botBrain: public Brain{
public:
    botBrain(Player& player);

public:
    void Update();
};

void BotMoves();
void BotAim();
void Move(int ip, int jp,std::vector<std::vector<int>>& pathMap);

#endif
