#include "../header/player.h"
#include "../header/weapon.h"
#include "../header/bullet.h"
#include "../header/util.h"
#include "../header/particleSystem.h"
#include "../header/geometry.h"
#include "../header/annPlayer.h"
#include <iterator>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <map>
#include <math.h>
#include <Box2D/Box2D.h>
#include <queue>
#include <cstdlib>
#include <unistd.h>
#include "../header/collision.h"
extern b2World* world;
extern std::vector< std::vector<char> > map;
extern std::vector<Player*> players;
extern std::vector<Grenade*> thrownGrenades;
extern float windowWidth, windowHeight;
extern std::map<std::string, int> sounds;
extern std::vector<Block*> walls;
extern ANNPlayer* currentPlayer;
extern std::vector<Bullet*> bullets;

//Number of players that we havent reached in the BFS search
int num;
unsigned int microseconds = 50000;



Player::Player() {
	r = 0.15;
	speed = 0.03;
	maxHealth = 100;
	health = maxHealth;
	grenades = 0;
	maxArmor = 100;
	armor = 0;
	input.vertical = 0;
	input.horizontal = 0;
	input.shoot = false;
	input.angle= M_PI/2;
	secondaryGun = new Pistol(0.0f, 0.0f, input.angle);
	secondaryGun->SetHolder(this);
	equiped_weapon = secondaryGun;
	deathFlag = false;
	alive = false;
	team = false;
	see_player = false;
	ammo = 0;

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.linearDamping = 2.0f;
	bodyDef.position.Set(-100.0f, 0.0f);
	body = world->CreateBody(&bodyDef);
	body->SetUserData(this);
	body->SetActive(false);

    // Define another box shape for our dynamic body.
    b2CircleShape circleShape;
    circleShape.m_p.Set(0, 0); //position, relative to body position
    circleShape.m_radius = r;
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 1.0f;
    // Override the default friction.
    fixtureDef.friction = 0.01f;
    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);


	//Setting up player sounds
    alGenSources(NUM_OF_SOURCES_PLAYER, soundSource);
  	alSourcei(soundSource[0], AL_BUFFER, sounds["death"]);
	alSourcei(soundSource[1], AL_BUFFER, sounds["bodyImpact1"]);
	alSourcei(soundSource[2], AL_BUFFER, sounds["bodyImpact2"]);
	alSourcei(soundSource[3], AL_BUFFER, sounds["grenadeThrow"]);
	alSourcei(soundSource[4], AL_BUFFER, sounds["grenadePull"]);
	for(int i = 0; i < NUM_OF_SOURCES_PLAYER; i++){
		alSourcef(soundSource[i], AL_GAIN, 0.2);
		alSourcef(soundSource[i], AL_PITCH, 1);
	}

};





void Player::SetAmmo(int am){
	ammo = am;
}

int Player::GetAmmo(){return ammo;}
int Player::GetHealth(){return health;}

//Freeing sounds bound to player
void Player::FreeSources(){
	 size_t size = sizeof(soundSource)/sizeof(soundSource[0]);
	 alDeleteSources(size, soundSource);
}

void Player::IncreaseGrenades(int amount){
	grenades += amount;
}

void Player::throwGrenade(){
	if(grenades > 0){
		alSourcePlay(soundSource[4]);

		float n = 0.18;
		float vx = n*cos(input.angle);
    float vy = n*sin(input.angle);

		Grenade* thrown_grenade = new Grenade(body->GetPosition().x + vx, body->GetPosition().y + vy);
		thrownGrenades.push_back(thrown_grenade);
	}
}

void Player::SetMaxHealth(int mh) {
	maxHealth = mh;
	health = maxHealth;
}

void Player::SetMaxArmor(int ma) {
	maxArmor = ma;
}

void Player::FillArmor() {
	armor = maxArmor;
}

void Player::SetBrain(Brain* brain){
    m_brain = brain;
}

void Player::Draw(){
    glPushMatrix();
		if (team){
			glColor3f(1, 0.3, 0.3);
		}
		else{
			glColor3f(0.3, 0.3, 1);
		}
		b2Vec2 pos = body->GetPosition();
		glTranslatef(pos.x, pos.y, r);
		glutSolidSphere(r, 15, 5);
		glPushMatrix();
			glRotatef(input.angle*180/M_PI, 0, 0, 1);
			glTranslatef(r, 0, 0);
			glColor3f(0, 0, 0);
			glutSolidSphere(r/4, 10, 2);
		glPopMatrix();

		//Draw health bar
		glTranslatef(0, 0.3, 0.4);

		glScalef(1, 1, 0.1);
		glColor3f(0, 0, 0);
		glScalef(0.3, 0.1, 0.1);
		glutWireCube(1);
		float percentage = (float)health / maxHealth;
		glPushMatrix();
		glTranslatef(-0.5*(1-percentage), 0, 0);
		glScalef(percentage, 1, 1);
		glColor3f(1 - percentage, percentage, 0);
		glutSolidCube(1);
		glPopMatrix();

		if (armor > 0) {
			glTranslatef(0, -0.75, 0);
			glScalef(1, 0.5, 1);
			glPushMatrix();
			percentage = (float)armor / maxArmor;
			glTranslatef(-0.5*(1 - percentage), 0, 0);
			glScalef(percentage, 1, 1);
			glColor3f(.7, .7, .7);
			glutSolidCube(1);
			glColor3f(0, 0, 0);
			glutWireCube(1);
			glPopMatrix();

		}
    glPopMatrix();
};

void Player::DrawShadow() {
	//Drawing player shadow
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTranslatef(body->GetPosition().x + r / 2, body->GetPosition().y - r / 2, 0);
	glRotatef(45, 0, 0, 1);
	glScalef(1, 1.5, 0.1);
	glColor4f(0, 0, 0, 0.2);
	glutSolidSphere(r, 20, 2);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glPopMatrix();
}

//Brain and its children constructors and methods
Brain::Brain(Player& player)
    : m_player(&player) {}


playerBrain::playerBrain(Player& player)
    : Brain(player) {}


bool playerBrain::hasLineOfSight(Player* target){

	RayCastCallback ray_callback;
	b2Vec2 target_pos(target->body->GetPosition().x, target->body->GetPosition().y);
	b2Vec2 player_pos(players[0]->body->GetPosition().x, players[0]->body->GetPosition().y);

	bool hasLoS = false;
	int it = 0;
	do{
		world->RayCast(&ray_callback, player_pos, target_pos);
		if(ray_callback.m_fixture){
			//if it is a wall break and dont apply the damage
			void* object = ray_callback.m_fixture->GetBody()->GetUserData();
			Colider* c;
			if(object){
				c = static_cast<Colider*>(object);
				if(c->getClassID() == BLOCK){
					return hasLoS;
				}
			}
			player_pos.x = ray_callback.m_fixture->GetBody()->GetPosition().x;
			player_pos.y = ray_callback.m_fixture->GetBody()->GetPosition().y;


			if(it>=7){
				for(unsigned i = 0; i < bullets.size(); i++){
					if(ray_callback.m_fixture->GetBody() == bullets[i]->body){
						// std::cout << "WINNER" << std::endl;
						Bullet* tmp = bullets[i];
						bullets.erase(bullets.begin() + i);
						delete tmp;
					}
					else if((std::fabs(bullets[i]->body->GetLinearVelocity().x) <= 0.1 && std::fabs(bullets[i]->body->GetLinearVelocity().y) <= 0.1) || bullets[i]->toDelete == 1){
						Bullet* tmp = bullets[i];
						bullets.erase(bullets.begin() + i);
						delete tmp;
					}
				}
			}
		}
		it++;
		// std::cout << "BRE" << std::endl;

	}
	while((player_pos.x != target_pos.x) && (player_pos.y != target_pos.y));
	return true;
}

float* playerBrain::generateInput(){
	float* input = new float[10];
	int indexClosest = -1;
	float minDistance = 100000;
	for (unsigned i=1; i<players.size(); i++){
		if (players[i]->alive && hasLineOfSight(players[i])){
			float distance = (players[0]->body->GetPosition() - players[i]->body->GetPosition()).Length();
			if (distance < minDistance){
				indexClosest = i;
				minDistance = distance;
			}
		}
	}

	if(indexClosest != -1){
		b2Vec2 dir = players[indexClosest]->body->GetPosition() - players[0]->body->GetPosition();
		input[0] = (atan2((float)dir.y, (float)dir.x)+M_PI)/(2*M_PI);
	}
	else{
		input[0] = atan2(0,0);
	}
	input[1] = (float)Brain::m_player->equiped_weapon->GetAmmo() / Brain::m_player->equiped_weapon->GetAmmoCap();

	// ######################### CREATING MAP AROUND THE PLAYER ################################
	float h, w;
	int up, left, ip, jp, n, m;

	h = tan(30 * M_PI / 180) * 4;
	w = h * windowWidth / windowHeight;
	up = h/walls[0]->m_edge;
	left = w/walls[0]->m_edge;
	n = (2*up);
	m = (2*left+1);

	std::vector<std::vector<char>> in;
	for(int i = 0; i < n; i++){
		std::vector<char> tmp;
		for(int j = 0; j < m; j++){
			tmp.push_back(' ');
		}
		in.push_back(tmp);
	}


	ip = map.size()-1-(floor((players[0]->body->GetPosition().y + 9.0)/18*map.size()));
	jp = floor((players[0]->body->GetPosition().x + 9.0)/18*map.size());
	int ih, jh;
	for(int i = 0; i < n; i++){
		for(int j = 0; j < m; j++){
			ih = i+(ip-up);
			jh = j+(jp-left);
			if(ih>=40 || jh >=40 || ih < 0 || jh < 0){
				continue;
			}
			if(map[ih][jh]=='#'){
				in[i][j] = '#';
			}
		}
	}


	// std::cout <<"PLAYER HP: "<< players[0]->GetHealth() << " PLAYER KILLS: " << currentPlayer->kills << std::endl;
	// std::cout << players[0]->equiped_weapon->GetAmmo() << std::endl;
	for(unsigned k=1;k<players.size();++k){
		if (!players[k]->alive)
			continue;
		int i = map.size()-1-(floor((players[k]->body->GetPosition().y + 9.0)/18*map.size()));
		int j = floor((players[k]->body->GetPosition().x + 9.0)/18*map.size());
		if (!(i > ip + up - 1 || i < ip - up || j > jp + left || j < jp - left)){
			ih = i-(ip-up);
			jh = j-(jp-left);
			in[ih][jh] = 'B';


		}
		// std::cout <<"BOT HP: "<< players[k]->GetHealth() << std::endl;
		// std::cout <<"BOT LOS: "<< players[k]->see_player << std::endl;
		// std::cout << players[k]->equiped_weapon->GetAmmo() <<  std::endl;;
	}

	in[up][left]= 'P';


	// for(int i = 0; i < n; i++){
	// 	for(int j = 0; j < m; j++){
	// 		std::cout<<in[i][j];
	// 	}
	// 	std::cout <<std::endl;
	// }
	//
	// std::cout << std::endl;
	// std::cout << std::endl;
	// usleep(microseconds);

	//############### CALCULATING UP DOWN LEFT RIGHT AND DIAGONAL ########
	// input[2] = FREE SPACE UP
	// input[3] = FREE SPACE DOWN
	// input[4] = FREE SPACE LEFT
	// input[5] = FREE SPACE RIGHT
	// input[6] = FREE SPACE UPLEFT
	// input[7] = FREE SPACE UPRIGHT
	// input[8] = FREE SPACE DOWNLEFT
	// input[9] = FREE SPACE DOWNRIGHT


	//UP
	for(int i = 1; i < up+1; i++){
		if(in[up-i][left] == '#' || in[up-i][left] == 'B'){
			input[2] = (i-1)*1.0/up;
			break;
		}
		if(i==up){
			input[2]=1.0;
		}
	}
	//DOWN
	for(int i = 1; i < up; i++){
		if(in[up+i][left] == '#' || in[up-i][left] == 'B'){
			input[3] = (i-1)*1.0/(up-1);
			break;
		}
		if(i==up-1){
			input[3]=1.0;
		}
	}
	//LEFT
	for(int i = 1; i < left+1; i++){
		if(in[up][left-i] == '#' || in[up][left-i] == 'B'){
			input[4] = (i-1)*1.0/left;
			break;
		}
		if(i==left){
			input[4]=1.0;
		}
	}
	//RIGHT
	for(int i = 1; i < left+1; i++){
		if(in[up][left+i] == '#' || in[up][left+i] == 'B'){
			input[5] = (i-1)*1.0/left;
			break;
		}
		if(i==left){
			input[5]=1.0;
		}
	}
	//UPLEFT
	for(int i = 1; i < up+1; i++){
		if(in[up-i][left-i] == '#' || in[up-i][left-i] == 'B'){
			input[6] = (i-1)*1.0/up;
			break;
		}
		if(i==up){
			input[6]=1.0;
		}
	}
	//UPRIGHT
	for(int i = 1; i < up+1; i++){
		if(in[up-i][left+i] == '#' || in[up-i][left+i] == 'B'){
			input[7] = (i-1)*1.0/up;
			break;
		}
		if(i==up){
			input[7]=1.0;
		}
	}
	//DOWNLEFT
	for(int i = 1; i < up; i++){
		if(in[up+i][left-i] == '#' || in[up+i][left-i] == 'B'){
			input[8] = (i-1)*1.0/(up-1);
			break;
		}
		if(i==up-1){
			input[8]=1.0;
		}
	}
	//DOWNRIGHT
	for(int i = 1; i < up; i++){
		if(in[up+i][left+i] == '#' || in[up+i][left+i] == 'B'){
			input[9] = (i-1)*1.0/(up-1);
			break;
		}
		if(i==up-1){
			input[9]=1.0;
		}
	}


	return input;

}
void playerBrain::Update(){
	float* input = generateInput();



	float* output = currentPlayer->GetOutput(input);
	//test always look at closest enemy:
	//float angle = input[0];
	delete[] input;

	// for (int i=0; i<5; i++) {
    //     std::cout << output[i] << " ";
    // }

	// std::cout << std::endl;
	// std::cout << std::endl;

	// std::cout << windowHeight << " " << windowWidth <<std::endl;
	float vx,vy;
	if(output[0] < 0.33){
		vy = -1;
	}
	else if(output[0] >= 0.33 && output[0] < 0.66){
		vy = 0;
	}
	else{
		vy = 1;
	}

	if(output[1] < 0.33){
		vx = -1;
	}
	else if(output[1] >= 0.33 && output[1] < 0.66){
		vx = 0;
	}
	else{
		vx = 1;
	}

    b2Vec2 vel(vx, vy);

    Brain::m_player->body->SetLinearVelocity(vel);



	float angle = output[2]*(2*M_PI)-M_PI;

	Brain::m_player->input.angle = angle;
    vx = cos(angle);
    vy = sin(angle);
    float a = 0.18;
    Brain::m_player->equiped_weapon->SetPositionAndAngle(Brain::m_player->body->GetPosition().x + vx*a, Brain::m_player->body->GetPosition().y + vy*a, angle);

	if(output[3] < 0.5){
		Brain::m_player->input.shoot = true;
	} else{
		Brain::m_player->input.shoot = false;
	}

	if(output[4] < 0.5){
		Brain::m_player->equiped_weapon->reload(Brain::m_player->equiped_weapon->GetAmmoCap());
	}

	Brain::m_player->moveSoundSource();
}

botBrain::botBrain(Player& player)
    : Brain(player) {}

void botBrain::Update(){
    float vx = Brain::m_player->input.horizontal;
    float vy = Brain::m_player->input.vertical;

    b2Vec2 vel(vx, vy);

    Brain::m_player->body->SetLinearVelocity(vel);

	vx = cos(Brain::m_player->input.angle);
	vy =  sin(Brain::m_player->input.angle);
	float n = 0.18;

	Brain::m_player->equiped_weapon->SetPositionAndAngle(Brain::m_player->body->GetPosition().x + vx*n, Brain::m_player->body->GetPosition().y + vy*n, Brain::m_player->input.angle);

	Brain::m_player->moveSoundSource();

    RayCastCallback ray_callback;
    b2Vec2 bot_pos(Brain::m_player->body->GetPosition().x, Brain::m_player->body->GetPosition().y);
    b2Vec2 player_pos(players[0]->body->GetPosition().x, players[0]->body->GetPosition().y);
	int it = 0;
    m_player->see_player = true;
		if (players[0]->alive){
			do{
	    	world->RayCast(&ray_callback, bot_pos, player_pos);
	    	if(ray_callback.m_fixture){
					void* object = ray_callback.m_fixture->GetBody()->GetUserData();
					Colider* c;
					if(object){
						c = static_cast<Colider*>(object);
						// std::cout <<c->getClassID() << std::endl;
						if(ray_callback.m_fixture->GetBody()== players[0]->body){
							m_player->see_player = false;
							break;
						}
						else if (c->getClassID() != BULLET){
							m_player->see_player = true;
							break;
						}
					}
					bot_pos.x = ray_callback.m_fixture->GetBody()->GetPosition().x;
					bot_pos.y = ray_callback.m_fixture->GetBody()->GetPosition().y;
					if(it>=7){
						for(unsigned i = 0; i < bullets.size(); i++){
							if(ray_callback.m_fixture->GetBody() == bullets[i]->body){
								// std::cout << "WINNER" << std::endl;
								Bullet* tmp = bullets[i];
								bullets.erase(bullets.begin() + i);
								delete tmp;
							}
							else if((std::fabs(bullets[i]->body->GetLinearVelocity().x) <= 0.1 && std::fabs(bullets[i]->body->GetLinearVelocity().y) <= 0.1) || bullets[i]->toDelete == 1){
								Bullet* tmp = bullets[i];
								bullets.erase(bullets.begin() + i);
								delete tmp;
							}
						}
					}
					// std::cout << "WAT" << std::endl;
					//
					// std::cout << "PLAYER: " << player_pos.x <<  " " << player_pos.y << std::endl;
					// std::cout << "FIXTURE: " << bot_pos.x <<  " " << bot_pos.y << std::endl;
					// std::cout << "GUN: " << Brain::m_player->equiped_weapon->getPosX() << " " << Brain::m_player->equiped_weapon->getPosY() << std::endl;
					// std::cout << "BOT: " << Brain::m_player->body->GetPosition().x << " " << Brain::m_player->body->GetPosition().y << std::endl;
				}


				it++;
			}while((player_pos.x != bot_pos.x) && (player_pos.y != bot_pos.y));
		}
		m_player->see_player = !m_player->see_player;


}

void Player::die(){
	alSourcePlay(soundSource[0]);
	alive = false;
	body->SetTransform(b2Vec2(-100, 0), 0);
	body->SetActive(false);
}

void StartBloodEffet(b2Vec2 pos, b2Vec2 bulletVel) {
	bulletVel.Normalize();
	//dir = 10 * dir;
	Emitter* blood = new Emitter(pos, bulletVel, b2Vec2(0, 0), 3, 0.5, "blood");
	blood->SetScale(0.07, 0.1);
	blood->SetSpeed(0.1, 0.3);
	blood->Start();
}


void Player::takeDmg(int dmg) {
	takeDmg(dmg, b2Vec2(0, 0), -1);
}

void Player::takeDmg(int dmg, b2Vec2 bulletVel, int m_team){
	if (m_team == team){
		return;
	}
	if(team){
		currentPlayer->dmgDone += dmg;
	}
	float t = rand();
	if(t > (RAND_MAX)/2.0)
		alSourcePlay(soundSource[1]);
	else
		alSourcePlay(soundSource[2]);

	int tmp = health;
	if (armor > 0) {
		armor -= dmg;
	}
	else {
		health -= dmg;
	}

	if (armor < 0) {
		health += armor;
		armor = 0;
	}
	if (health <= 0) {
		health = 0;
		if (tmp > 0) {
			deathFlag = true;
		}

	}

	StartBloodEffet(body->GetPosition(), bulletVel);
}


void Player::IncreaseHealth(int amount) {
	health += amount;
	health = health > maxHealth ? maxHealth : health;
}

void Player::Revive() {
	health = maxHealth;
	alive = true;
	equiped_weapon->reload(equiped_weapon->GetAmmoCap());
	body->SetActive(true);
}

void Player::SwitchToSecondary() {
	SwapWeapon(secondaryGun);
}

void Player::SwapWeapon(Weapon* newWeapon) {
	Weapon* old = equiped_weapon;
	if (old->getWeaponType() == PISTOL) {
		old = NULL;
	}else {
		old->FreeSources();
	}
	equiped_weapon = newWeapon;
	newWeapon->SetHolder(this);
	delete(old);
}
//Making sure sound is centered on the player
void Player::moveSoundSource(){
	for(int i = 0; i < NUM_OF_SOURCES_PLAYER; i++){
		alSource3f(soundSource[i], AL_POSITION, body->GetPosition().x, body->GetPosition().y, 0.2);
	}
}

ClassID Player::getClassID() {return PLAYER;}

//Function that finds the player we found in the search and adjusts his movement
void Move(int ip, int jp,std::vector<std::vector<int>>& pathMap){
    int i,j;
    int minu,mind,minl,minr;
    int minimum;
    float edge = 18.0/map.size();


    for(unsigned k=1;k<players.size(); k++){
		if (!players[k]->alive)
			continue;
        i = map.size()-1-(floor((players[k]->body->GetPosition().y + 9.0)/18*map.size()));
        j = floor((players[k]->body->GetPosition().x + 9.0)/18*map.size());
        if(i == ip && j == jp){
            if(pathMap[i-1][j] == pathMap[ip][jp]-1 && pathMap[ip][jp]-1 != 0){
                players[k]->input.vertical+=1;
            }else if(pathMap[i+1][j] == pathMap[ip][jp]-1 && pathMap[ip][jp]-1 != 0){
                players[k]->input.vertical-=1;

            }else if(pathMap[i][j+1] == pathMap[ip][jp]-1 && pathMap[ip][jp]-1 != 0){
                players[k]->input.horizontal+=1;

            }else if(pathMap[i][j-1] == pathMap[ip][jp]-1 && pathMap[ip][jp]-1 != 0){
                players[k]->input.horizontal-=1;

            }
            else if(pathMap[i-1][j-1] == pathMap[ip][jp]-1 && pathMap[ip][jp]-1 != 0 && map[i-1][j] != '#' && map[i][j-1] != '#'){
                players[k]->input.vertical+=1;
                players[k]->input.horizontal-=1;

            }
            else if(pathMap[i+1][j-1] == pathMap[ip][jp]-1&& pathMap[ip][jp]-1 != 0 && map[i+1][j] != '#' && map[i][j-1] != '#'){
                players[k]->input.vertical-=1;
                players[k]->input.horizontal-=1;

            }
            else if(pathMap[i+1][j+1] == pathMap[ip][jp]-1 && pathMap[ip][jp]-1 != 0 && map[i+1][j] != '#' && map[i][j+1] != '#'){
                players[k]->input.horizontal+=1;
                players[k]->input.vertical-=1;
            }
            else if(pathMap[i-1][j+1] == pathMap[ip][jp]-1&& pathMap[ip][jp]-1 != 0 && map[i-1][j] != '#' && map[i][j+1] != '#'){
                players[k]->input.horizontal+=1;
                players[k]->input.vertical+=1;
            }

            //Backup in case block is up/down/right/left during diagonal moving
            else if (players[k]->input.vertical == 0 && players[k]->input.horizontal == 0){
                minu = pathMap[i-1][j];
                mind = pathMap[i+1][j];
                minl = pathMap[i][j-1];
                minr = pathMap[i][j+1];

                if(minu == -1 || minu == -2)
                    minu = map.size()*map.size();
                if(mind == -1 || mind == -2)
                    mind = map.size()*map.size();
                if(minl == -1 || minl == -2)
                    minl = map.size()*map.size();
                if(minr == -1 || minr == -1)
                    minr = map.size()*map.size();

                minimum = std::min(minu, std::min(mind, std::min(minl,minr)));

                if(minu == 0 || mind == 0 || minl == 0 || minr == 0){

                }else if(minimum == minu){
                    players[k]->input.vertical+=1;
                }else if(minimum == mind){
                    players[k]->input.vertical-=1;
                }else if(minimum == minl){
                    players[k]->input.horizontal-=1;
                }else{
                    players[k]->input.horizontal+=1;
                }
            }

            //Moves the bot towards the middle of the block if he is too close to the edge
            if(-(i+1) * edge > (players[k]->body->GetPosition().y - 9.0 - (players[k]->r + 0.05)) && players[k]->input.vertical!=1 && players[k]->input.vertical!=-1){
                players[k]->input.vertical+=1;

            }else if(-i * edge < (players[k]->body->GetPosition().y - 9.0 + (players[k]->r + 0.05))  && players[k]->input.vertical!=-1 && players[k]->input.vertical!=1){
                players[k]->input.vertical-=1;

            }
            if((j+1) * edge < (players[k]->body->GetPosition().x + 9.0 + (players[k]->r + 0.05)) && players[k]->input.horizontal!=-1 && players[k]->input.horizontal !=1){
                players[k]->input.horizontal-=1;


            }else if(j * edge > (players[k]->body->GetPosition().x + 9.0 - (players[k]->r + 0.05)) && players[k]->input.horizontal!=1  && players[k]->input.horizontal != -1){
                players[k]->input.horizontal+=1;
            }


            //Removing the player marker from the map and reducing the number of players that werent reached
            map[i][j] = ' ';
			pathMap[i][j] = -2;
            num--;
        }

    }
}


//BFS search of the map starting from player to all of the bots and calls BotAim function
void BotMoves(){
	if (!players[0]->alive)
		return;
    BotAim();
    unsigned i, j, ip, jp;
    num = 0;

    //Matrix that contains the number of fields on the shortest path from each field to the player
    std::vector<std::vector<int>> pathMap(map.size());
    std::queue<std::pair<int,int>> queue;
    for(i=0;i<map.size();i++){
        pathMap[i] = std::vector<int>(map.size());
        for(j=0;j<map.size();j++){
          pathMap[i][j] = -1;
        }
    }




    //Player position based on coordinates
    ip = map.size()-1-(floor((players[0]->body->GetPosition().y + 9.0)/18*map.size()));
    jp = floor((players[0]->body->GetPosition().x + 9.0)/18*map.size());
    pathMap[ip][jp] = 0;


    //Bot positions based on coordinates
    for(unsigned k=1;k<players.size();++k){
		if (!players[k]->alive)
			continue;
        i = map.size()-1-(floor((players[k]->body->GetPosition().y + 9.0)/18*map.size()));
        j = floor((players[k]->body->GetPosition().x + 9.0)/18*map.size());

        players[k]->input.horizontal=0;
        players[k]->input.vertical=0;


        if(i == ip && j == jp)
            continue;
        map[i][j] = 'B';
        num++;
    }
    queue.push(std::pair<int,int>{ip,jp});

    //BFS
    while(!queue.empty() && num > 0){
        i = queue.front().first;
        j = queue.front().second;
        queue.pop();
        if(map[i][j] == 'B'){
            Move(i, j, pathMap);
			continue;
        }

        if(i - 1 >= 0 && map[i-1][j] != '#' && pathMap[i-1][j] == -1 && pathMap[i-1][j] != -2){
            queue.push(std::pair<int,int>{i-1, j});
            pathMap[i-1][j] = pathMap[i][j]+1;
        }
        if(i + 1 < map.size() && map[i+1][j] != '#' && pathMap[i+1][j] == -1&& pathMap[i+1][j] != -2){
            queue.push(std::pair<int,int>{i+1, j});
            pathMap[i+1][j] = pathMap[i][j]+1;
        }
        if(j - 1 >= 0 && map[i][j-1] != '#' && pathMap[i][j-1] == -1 && pathMap[i][j-1] != -2){
            queue.push(std::pair<int,int>{i, j-1});
            pathMap[i][j-1] = pathMap[i][j]+1;
        }
        if(j + 1 < map.size() && map[i][j+1] != '#' && pathMap[i][j+1] == -1 && pathMap[i][j+1] != -2){
            queue.push(std::pair<int,int>{i, j+1});
            pathMap[i][j+1] = pathMap[i][j]+1;
        }
        if(i - 1 >= 0 && j - 1 >= 0 && map[i-1][j-1] != '#' && pathMap[i-1][j-1] == -1 && pathMap[i-1][j-1] != -2){
            queue.push(std::pair<int,int>{i-1, j-1});
            pathMap[i-1][j-1] = pathMap[i][j]+1;
        }
        if(i - 1 >= 0 && j + 1 < map.size()  && map[i-1][j+1] != '#' && pathMap[i-1][j+1] == -1 && pathMap[i-1][j+1] != -2){
            queue.push(std::pair<int,int>{i-1, j+1});
            pathMap[i-1][j+1] = pathMap[i][j]+1;
        }
        if(i + 1 < map.size() && j - 1 >= 0 && map[i+1][j-1] != '#' && pathMap[i+1][j-1] == -1 && pathMap[i+1][j-1] != -2){
            queue.push(std::pair<int,int>{i+1, j-1});
            pathMap[i+1][j-1] = pathMap[i][j]+1;
        }
        if(i + 1 < map.size() && j + 1 < map.size() && map[i+1][j+1] != '#' && pathMap[i+1][j+1] == -1 && pathMap[i+1][j+1] != -2){
            queue.push(std::pair<int,int>{i+1, j+1});
            pathMap[i+1][j+1] = pathMap[i][j]+1;
        }

    }
    pathMap.clear();
    while(!queue.empty())
        queue.pop();
}

//Adjusting bot aiming and checking if he sees the player
void BotAim(){
    float x1, x2;
    float y1, y2;
    x1 = players[0]->body->GetPosition().x;
    y1 = players[0]->body->GetPosition().y;
    float angle;
    for(unsigned k = 1; k < players.size(); ++k){
		if (!players[k]->alive) {
			continue;
		}
		if (players[k]->equiped_weapon->GetAmmo() == 0) {
            players[k]->equiped_weapon->reload(players[k]->equiped_weapon->GetAmmoCap());
		}

		x2 = players[k]->body->GetPosition().x;
		y2 = players[k]->body->GetPosition().y;

        if( IsOnScreen(players[k]->body->GetPosition()) && players[k]->see_player )
            players[k]->input.shoot = true;
        else
            players[k]->input.shoot = false;

        angle = atan2(y1-y2, x1-x2);
        players[k]->input.angle = angle;

    }
}
