#include "../header/items.h"
#include "../header/bullet.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

extern double phisycsUpdateInterval;
extern std::vector<Bullet*> bullets;

float randomNumber(float start, float end){
	srand(clock());
	return ( float(rand())/float(RAND_MAX) )*(end - start) + start;
}

Weapon::Weapon(float x, float y, float angle){
    dmg = 1;
	ammo = 100;
	ammo_cap = 100;
	spread = 0.15;
	fire_delay= 0.15;
	reload_delay = 2;
	fire_timer = 0;
    reload_timer= 0;
    pos_x = x;
    pos_y = y;
    angle = angle;
};

//Function for firing a bullet, it sends the bullet its position and angle
void Weapon::fire(){
	if(fire_timer <= 0 && reload_timer <= 0 && this->ammo != 0){
		//Calculating new angle from random spread
		float firing_angle = angle + spread*randomNumber(-1,1);
		Bullet* firedBullet = new Bullet(pos_x, pos_y, firing_angle);
		//Adding bullet to the list of fired bullets
		bullets.push_back(firedBullet);

		this->ammo--;
		fire_timer = fire_delay;
	}
}

void Weapon::reload(){
	//std::cout << ammo << std::endl;
	if(ammo < ammo_cap){
		std::cout << "Reloadin!" << std::endl;
		this->ammo = this->ammo_cap;
	}
	if (reload_timer <= 0){
        reload_timer = reload_delay;
    }
}

void Weapon::Update(bool shoot){
    UpdateTimers();
	if(shoot){
		this->fire();
	}
}

void Weapon::UpdateTimers(){
    fire_timer -= phisycsUpdateInterval;
    reload_timer -= phisycsUpdateInterval;
}

//Function for updating position and angle at which weapon is being pointed, required for firing a bulet
void Weapon::SetPositionAndAngle(float x, float y, float angle){
	this->pos_x = x;
	this->pos_y = y;
	this->angle = angle;
}
