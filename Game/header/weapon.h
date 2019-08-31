#ifndef WEAPON_DEF
#define WEAPON_DEF

#define NUM_OF_SOURCES_WEAP 3

#include "item.h"
#include <AL/alut.h>

//Different weapon classes

enum weaponType{
  PISTOL,
	RIFLE,
	SHOTGUN,
	WEAPON,
	SNIPER
};

class Player;

class Weapon : public Item{
public:
	Weapon(float x, float y, float angle, float pickupDistance, std::string icon);
	void Pickup(Player* picker) override;
	void Update(bool shoot);
	void UpdateTimers();
	void FreeSources();
  void SetHolder(Player* holder);
	virtual weaponType getWeaponType();

	virtual void fire();
  int GetAmmo() const;
	int GetAmmoCap() const;
	float GetReloadTimer() const;
	int reload(int mag);
	void SetPositionAndAngle(float x, float y, float angle);
	bool allowed_fire;
    float getPosX() {
        return pos_x;
    }
    float getPosY() {
        return pos_y;
    }

protected:
	int dmg;
	int ammo;
	int ammo_cap;
	float spread;
	float speed;
	double fire_delay;
	double reload_delay;
	double fire_timer;
	double reload_timer;
	float pos_x;
	float pos_y;
	float angle;
	float recoilAmount;
	float bulletSIze;
  Player* m_holder;
	ALuint soundSource[NUM_OF_SOURCES_WEAP];
};

class Pistol : public Weapon {
public:
	weaponType getWeaponType();
	Pistol(float x, float y, float angle) : Weapon(x, y, angle, 0.2, std::string("pistol")) {
		dmg = 5;
		ammo = 15;
		ammo_cap = 15;
		spread = 0.1;
		fire_delay = 0.25;
		reload_delay = 1.5;
		bulletSIze = 0.019;
	};
};

class Rifle : public Weapon {
public:
	weaponType getWeaponType();
	Rifle(float x, float y, float angle) : Weapon(x, y, angle, 0.4, std::string("rifle")) {
		dmg = 10;
		ammo = 30;
		ammo_cap = 30;
		spread = 0.2;
		fire_delay = 0.15;
		reload_delay = 2;
		bulletSIze = 0.022;
	};
};

class Sniper : public Weapon {
public:
	weaponType getWeaponType();
	Sniper(float x, float y, float angle) : Weapon(x, y, angle, 0.4, std::string("sniper")) {
		dmg = 50;
		ammo = 5;
		ammo_cap = 5;
		spread = 0.2;
		speed = 0.02;
		fire_delay = 1.5;
		reload_delay = 4;
		bulletSIze = 0.026;
	};
};

class Shotgun : public Weapon {
public:
	weaponType getWeaponType();
	Shotgun(float x, float y, float angle, int palletNumber) : Weapon(x, y, angle, 0.4, std::string("shotgun")), palletNumber(palletNumber) {
		dmg = 5;
		ammo = 10;
		ammo_cap = 10;
		spread = 0.3;
		fire_delay = 0.6;
		reload_delay = 3;
		bulletSIze = 0.012;
	};
	int palletNumber;
	void fire() override;
};

class Grenade{
public:
	Grenade(float x, float y);
	~Grenade();
	void explode();
	void throwMe(float angle, float strength);
	void Draw();
	float GetExplodeTimer() const;
	void Update(float x, float y);
	b2Body* body;
	bool toDelete;
	bool thrown;
private:
	int dmg;
	float r;
	float blastRadius;
	float explodeTimer;
	float m_strength;
	float animationTimer;
};
#endif
