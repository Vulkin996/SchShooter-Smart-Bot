#ifndef ITEMS_DEF
#define ITEMS_DEF

class Weapon {
public:
	Weapon(float x, float y, float angle);
	void Update(bool shoot);
	void UpdateTimers();
	void fire();
	void reload();
	void SetPositionAndAngle(float x, float y, float angle);
	bool allowed_fire;
	
private:
	int dmg;
	int ammo;
	int ammo_cap;
	float spread;
	double fire_delay;
	double reload_delay;
	double fire_timer;
	double reload_timer;
	float pos_x;
	float pos_y;
	float angle;
};

#endif
