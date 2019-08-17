#include "../header/weapon.h"
#include "../header/bullet.h"
#include "../header/player.h"
#include "../header/util.h"
#include "../header/particleSystem.h"
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <GL/glut.h>


const float RE_AMM_MAX = 1, RE_AMM_MIN = 0, RE_AMM_UP = 0.15, RE_AMM_DOWN = 0.005;

extern double phisycsUpdateInterval;
extern std::vector<Bullet*> bullets;
extern std::vector<Player*> players;
extern std::map<std::string, int> sounds;
extern std::vector<AudioWrapper*> audioWrappers;
extern b2World* world;

//Weapon cclass constructor
Weapon::Weapon(float x, float y, float angle, float pickupDistance, std::string icon) : Item(x, y, pickupDistance, icon){
	dmg = 10;
	ammo = 30;
	ammo_cap = 30;
	speed = 0.01;
	spread = 0.15;
	fire_delay= 0.15;
	reload_delay = 2;
	fire_timer = 0;
	reload_timer= 0;
	pos_x = x;
	pos_y = y;
	angle = angle;
	recoilAmount = 0;

	alGenSources(NUM_OF_SOURCES_WEAP, soundSource);
	alSourcei(soundSource[0], AL_BUFFER, sounds[icon]);


	//Setting the correct sound
	if (icon == "sniper") {
		alSourcef(soundSource[0], AL_GAIN, 0.6);
	}
	if(icon == "shotgun"){
		alSourcei(soundSource[1], AL_BUFFER, sounds[std::string("reloadShotgun")]);
		alSourcef(soundSource[0], AL_GAIN, 0.6);
	}
	else{
		alSourcei(soundSource[1], AL_BUFFER, sounds[std::string("reload")]);
		alSourcef(soundSource[0], AL_GAIN, 0.15);
	}

	alSourcei(soundSource[2], AL_BUFFER, sounds[std::string("pickup")]);
	alSourcef(soundSource[0], AL_PITCH, 1);
};


//Activating effects when a gun is fired

weaponType Weapon::getWeaponType(){return WEAPON;}
weaponType Pistol::getWeaponType(){return PISTOL;}
weaponType Rifle::getWeaponType(){return RIFLE;}
weaponType Shotgun::getWeaponType(){return SHOTGUN;}
weaponType Sniper::getWeaponType() { return SNIPER; }

void PlayGunShotEffect(float posx, float posy, float angle) {
	float gunDistance = 0.05;
	Emitter* casing = new Emitter(b2Vec2(posx - cos(angle)*0.1, posy - sin(angle)*0.1), b2Vec2(0, 0), b2Vec2(0, 0), 1, 0.3, "bulletCasing");
	casing->SetScale(0.04, 0.04);
	casing->SetSpeed(0.5, 0.6);
	casing->SetRotation(5, 10);
	casing->SetAlpha(1, 0.5);
	casing->Start();

	Emitter* flash = new Emitter(b2Vec2(posx+cos(angle)*gunDistance, posy + sin(angle)*gunDistance), b2Vec2(0, 0), b2Vec2(0, 0), 1, 0.1, "muzzleFlash");
	flash->SetScale(0.13, 0.15);
	flash->SetSpeed(0, 0);
	flash->SetStartRotation(angle);
	flash->SetAlphaTween(TW_CUBIC);
	flash->Start();

}

//Function for firing a bullet, it sends the bullet its position and angle
void Weapon::fire(){
	if(fire_timer <= 0 && reload_timer <= 0 && this->ammo != 0){
		//Calculating new angle from random spread
		float firing_angle = angle + spread*randomNumber(-1,1)*recoilAmount;
		Bullet* firedBullet = new Bullet(pos_x, pos_y, firing_angle, dmg, bulletSIze, speed);
		//Adding bullet to the list of fired bullets
		bullets.push_back(firedBullet);

		alSourcef(soundSource[0], AL_PITCH, 1+randomNumber(-0.3, 0.3));
		alSourcePlay(soundSource[0]);

		PlayGunShotEffect(pos_x, pos_y, firing_angle);

		this->ammo--;
		fire_timer = fire_delay;
		if(recoilAmount < RE_AMM_MAX)
			recoilAmount+=RE_AMM_UP;
		else if(recoilAmount > RE_AMM_MAX)
			recoilAmount = RE_AMM_MAX;
	}
}

//Handling weapon reload
int Weapon::reload(int mag){
	int left = mag;
	if(ammo < ammo_cap && mag > 0){
		alSourcePlay(soundSource[1]);

		if ((this->ammo + mag) <= this->ammo_cap) {
			this->ammo = this->ammo + mag;
			left = 0;
		}
		else if((this->ammo + mag) > this->ammo_cap){
			left = mag - (this->ammo_cap - this->ammo);
			this->ammo = this->ammo_cap;
		}

		reload_timer = reload_delay;
	}
	if (reload_timer <= 0){
        reload_timer = 0;
    }
	return left;
}

//Updating weapon timers, recoil and sound location
void Weapon::Update(bool shoot){
    UpdateTimers();

		alSource3f(soundSource[0], AL_POSITION, pos_x, pos_y, 0.2);
		alSource3f(soundSource[1], AL_POSITION, pos_x, pos_y, 0.2);
		alSource3f(soundSource[2], AL_POSITION, pos_x, pos_y, 0.2);

	if(shoot){
		this->fire();
	}
	if(recoilAmount > RE_AMM_MIN)
		recoilAmount -= RE_AMM_DOWN;
	else if(recoilAmount < RE_AMM_MIN)
		recoilAmount = RE_AMM_MIN;
}

//Getters
int Weapon::GetAmmo() const{
    return ammo;
}

int Weapon::GetAmmoCap() const{
	return ammo_cap;
}

float Weapon::GetReloadTimer() const{
	return reload_timer;
}

void Weapon::UpdateTimers(){
    fire_timer -= phisycsUpdateInterval;
    reload_timer -= phisycsUpdateInterval;
}

//Freeing sounds bound to weapon
void Weapon::FreeSources(){
	 size_t size = sizeof(soundSource)/sizeof(soundSource[0]);
	 alDeleteSources(size, soundSource);
}


//Function for updating position and angle at which weapon is being pointed, required for firing a bulet
void Weapon::SetPositionAndAngle(float x, float y, float angle){
	this->pos_x = x;
	this->pos_y = y;
	this->angle = angle;
}

void Weapon::Pickup(Player* picker) {
	if(picker->equiped_weapon->getWeaponType() != this->getWeaponType()){
		picker->SwapWeapon(this);
		alSourcePlay(soundSource[2]);
	}
	else{
		picker->SetAmmo(picker->GetAmmo() + this->GetAmmoCap());
		alSourcePlay(soundSource[2]);
	}
}


void Shotgun::fire() {
	if (fire_timer <= 0 && reload_timer <= 0 && this->ammo != 0) {
		//Calculating new angle from random spread
		for (int i = 0; i < palletNumber; i++)
		{
			float rand = randomNumber(-1, 1);
			float firing_angle = angle + spread * rand * (i+1)/ (float)palletNumber;
			Bullet* firedBullet = new Bullet(pos_x, pos_y, firing_angle, dmg, bulletSIze, speed);
			//Adding bullet to the list of fired bullets
			bullets.push_back(firedBullet);
		}

		PlayGunShotEffect(pos_x, pos_y, angle);

		alSourcef(soundSource[0], AL_PITCH, 1 + randomNumber(-0.3, 0.3));
		alSourcePlay(soundSource[0]);

		this->ammo--;
		fire_timer = fire_delay;
	}
}

//Grenade constructor
Grenade::Grenade(float x, float y){
	dmg = 200;
	r = 0.04;
	blastRadius = 1.6;
	explodeTimer = 2;
	toDelete = false;
	thrown = false;

	b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
	bodyDef.bullet = false;
	bodyDef.linearDamping = 2.5;
	bodyDef.position.Set(x, y);
	body = world->CreateBody(&bodyDef);

	b2CircleShape cShape;
	cShape.m_p.Set(0, 0); //position, relative to body position
	cShape.m_radius = r;

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &cShape;

	fixtureDef.density = 5.0f;

	fixtureDef.friction = 1;

	// Add the shape to the body.
	body->CreateFixture(&fixtureDef);
	body->SetActive(false);
}

Grenade::~Grenade(){
	world->DestroyBody(this->body);
}
//Getter
float Grenade::GetExplodeTimer() const{
	return explodeTimer;
}
//Activating effects when a grenade explodes
void StartGrenadeEffet(b2Vec2 pos) {
	//dir = 10 * dir;
	Emitter* shockwave = new Emitter(pos, b2Vec2(0, 0), b2Vec2(0, 0), 1, 0.3, "shockwave");
	shockwave->SetScale(0, 2);
	shockwave->SetSpeed(0, 0);
	shockwave->SetScaleTween(TW_INVERSE_CUBIC);
	shockwave->Start();

	Emitter* fireball = new Emitter(pos, b2Vec2(0, 0), b2Vec2(0, 0), 1, 0.3, "fireball");
	fireball->SetScale(1, 0);
	fireball->SetSpeed(0, 0);
	fireball->SetAlphaTween(TW_LINEAR);
	fireball->Start();

	Emitter* smoke = new Emitter(pos, b2Vec2(0, 0), b2Vec2(0, 0), 10, 1, "smoke");
	smoke->SetScale(0.3, 0.6);
	smoke->SetSpeed(0.2, 1);
	smoke->SetRotation(1, 2);
	smoke->Start();
}

//Setting up grenade parameters when thrown
void Grenade::throwMe(float angle, float strength){
	thrown = true;
	body->SetActive(true);
	float vx = strength*cos(angle)*0.35;
	float vy = strength*sin(angle)*0.35;
	animationTimer = 0.5;
	body->ApplyLinearImpulse(b2Vec2(vx,vy), body->GetWorldCenter(), true);

}

void Grenade::explode(){
	auto gposX = body->GetPosition().x;
	auto gposY = body->GetPosition().y;

	StartGrenadeEffet(body->GetPosition());
	//for each player calculate distance from grenade
	for(unsigned i = 0; i < players.size(); i++){
		auto pposX = players[i]->body->GetPosition().x;
		auto pposY = players[i]->body->GetPosition().y;

		double dist = sqrt((gposX - pposX)*(gposX - pposX) + (gposY - pposY)*(gposY - pposY));

		//If a player is in the blast radius
		if (dist < blastRadius) {
			//cast rays until you hit a wall or you reach the target player
			RayCastCallback ray_callback;
			b2Vec2 pposVector(pposX, pposY);
			b2Vec2 gposVector(gposX, gposY);
			bool doDmg = true;
			do{
				world->RayCast(&ray_callback, gposVector, pposVector);
				if(ray_callback.m_fixture){
					//if it is a wall break and dont apply the damage
					void* object = ray_callback.m_fixture->GetBody()->GetUserData();
					Colider* c;
					if(object){
						c = static_cast<Colider*>(object);
						if(c->getClassID() == BLOCK){
							doDmg = false;
							break;
						}
					}
				}
				gposVector.x = ray_callback.m_fixture->GetBody()->GetPosition().x;
				gposVector.y = ray_callback.m_fixture->GetBody()->GetPosition().y;
			}
			while((gposVector.x != pposVector.x) && (gposVector.y != pposVector.y));
			//if we didnt find a wall finaly apply the damage
	    if(doDmg){
				dist = (dist == 0 ? 1 : dist);
				players[i]->takeDmg(dmg/(dist*1.3), players[i]->body->GetPosition()-body->GetPosition());
			}
		}
	}
	AudioWrapper* blastSounder = new AudioWrapper(gposX, gposY, std::string("grenade"));
	blastSounder->playSound();
	blastSounder->toDelete = true;
	audioWrappers.push_back(blastSounder);

	toDelete = true;
}

//Updating grenade timers and transformation
void Grenade::Update(float x, float y){
	if(explodeTimer <= 0)
		explode();

	if(!thrown){
		body->SetTransform(b2Vec2(x,y), 0);
	}

	explodeTimer -= phisycsUpdateInterval;
	animationTimer += phisycsUpdateInterval;
}

void Grenade::Draw(){
	glColor3f(0, 0.4, 0.14);

	glPushMatrix();
	glTranslatef(body->GetPosition().x, body->GetPosition().y, r);

	float tmp = 2 - animationTimer;
	tmp = tmp < 0 ? 0 : tmp;
	float scaleAnimation = 1 + (cos(M_PI+animationTimer*animationTimer * 8)+1)/5*tmp;

	glScalef(scaleAnimation, scaleAnimation, scaleAnimation);
	glutSolidSphere(r, 20, 20);
	glPopMatrix();
}
