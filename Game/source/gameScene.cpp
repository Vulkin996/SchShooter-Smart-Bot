#include <iostream>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include "../header/image.h"
#include "../header/player.h"
#include "../header/geometry.h"
#include "../header/weapon.h"
#include "../header/bullet.h"
#include "../header/gameScene.h"
#include "../header/collision.h"
#include "../header/basicItems.h"
#include "../header/enemySpawner.h"
#include "../header/particleSystem.h"
#include "../header/util.h"
#include "../header/annPlayer.h"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <Box2D/Box2D.h>
#include <AL/alut.h>


//All necessary elements

extern std::vector<ANNPlayer*> networks;
extern int currentNetwork;

extern float windowWidth, windowHeight, aspectRatio;
extern GLuint textureIDs[];
extern std::map<std::string, int> sounds;
extern std::map<std::string, int> textures;
extern std::vector< std::vector<char> > map;
extern ALuint soundIDs[18];
int updateCount;
extern bool reset;
Player* myPlayer;
b2World* world;
MyContactListener* contactListener;
std::vector<Block*> walls;
std::chrono::high_resolution_clock::time_point lastFrameTime;
double accumulator = 0;
double phisycsUpdateInterval = 0.02;
std::vector<Bullet*> bullets;
std::vector<Grenade*> thrownGrenades;
std::vector<Player*> players;
std::vector<b2Vec2> spawnPositions;
std::vector<AudioWrapper*> audioWrappers;
ItemPool* itemPool;
EnemySpawner* enemySpawner;
ParticleSystem* particleSystem;
ALuint ambientSource[1];
bool GameOver;
char text[100];

//Types of scene
enum scene {
	GAME,
	MENU,
	EASTER_EGG
};
extern enum scene currentScene;

//Initializing the game
void InitGame() {
	updateCount = 0;
	networks[currentNetwork]->ResetFitness();

	//Setting up background music
	alGenSources(1, ambientSource);
	//CREDITS:  www.bensound.com
	alSourcei(ambientSource[0], AL_BUFFER, sounds["music"]);
	alSourcef(ambientSource[0], AL_GAIN, 0.1);
	alSourcef(ambientSource[0], AL_PITCH, 1);
	alSourcei(ambientSource[0], AL_LOOPING, AL_TRUE);


	srand(clock());

	//Creating world and gravity for physics
	b2Vec2 gravity(0.0f, 0.0f);
	world = new b2World(gravity);

	//Contact Listener for collisions
	contactListener = new MyContactListener();
	world->SetContactListener(contactListener);

	//Loading the map
	LoadWalls();


	//Adding players
	GameOver = false;
	myPlayer = new Player();
	myPlayer->SetBrain(new playerBrain(*myPlayer));
	myPlayer->SetMaxHealth(200);
	myPlayer->SetMaxArmor(200);
	myPlayer->SetAmmo(100);
	myPlayer->body->SetTransform(b2Vec2(0, 0), 1);
	myPlayer->Revive();
	players.push_back(myPlayer);

	for (int i = 1; i < 30; i++) {
		players.push_back(new Player());
		players[i]->body->SetTransform(b2Vec2(-100, 0), 0);
		players[i]->SetBrain(new botBrain(*players[i]));
		players[i]->team = !myPlayer->team;
	}


	//spawnPositions.push_back(b2Vec2(0, 3));


	//Making a spawner that handles placing the bots in game
	enemySpawner = new EnemySpawner(players, spawnPositions);
	particleSystem = new ParticleSystem();
	itemPool = new ItemPool();
	//b2Vec2 pos, b2Vec2 force, int particleCount, float lifespan, std::string texture)
	//players[1]->input.shoot = true;

	//Test for the items
	itemPool->Add(new Rifle(-2, 0, 0));
	itemPool->Add(new Armor(-3, 0));
	itemPool->Add(new GrenadeItem(-4, 0));


	//Starting background music
	alSourcePlay(ambientSource[0]);

	lastFrameTime = std::chrono::high_resolution_clock::now();
}

//When keyboard button is pressed
void on_keyboard_game(unsigned char key, int x, int y)
{
	switch (key) {
	// case 27:
	// 	/* Going back to thes Menu */
	// 	currentScene = MENU;
	// 	break;
	case 'a':
		myPlayer->input.horizontal -= 1;
		break;
	case 'd':
		myPlayer->input.horizontal += 1;
		break;
	case 'w':
		myPlayer->input.vertical += 1;
		break;
	case 's':
		myPlayer->input.vertical -= 1;
		break;
	case 'r':{
		if(myPlayer->equiped_weapon->getWeaponType() == PISTOL){
			myPlayer->equiped_weapon->reload(myPlayer->equiped_weapon->GetAmmoCap());
		}
		else{
			if(myPlayer->GetAmmo() > 0){
				myPlayer->SetAmmo(myPlayer->equiped_weapon->reload(myPlayer->GetAmmo()));
			}
		}
		break;
	}
	case '2':{
		myPlayer->SwitchToSecondary();
		break;
	}
		break;
	case 'g':{
		if(myPlayer->alive)
			myPlayer->throwGrenade();
		break;
	}
    case 'f':
        glutFullScreen();
        break;
		case 'e':{
			if(myPlayer->alive)
				itemPool->CheckPickups(myPlayer);
			break;
		}

	}
}

//When keyboard button is released
void keyboard_up_game(unsigned char key, int x, int y) {
	switch (key) {
	case 'a':
		myPlayer->input.horizontal += 1;
		break;
	case 'd':
		myPlayer->input.horizontal -= 1;
		break;
	case 'w':
		myPlayer->input.vertical -= 1;
		break;
	case 's':
		myPlayer->input.vertical += 1;
		break;
	case 'g':{
		if(myPlayer->alive && myPlayer->grenades != 0){
			thrownGrenades[thrownGrenades.size() - 1]->throwMe(myPlayer->input.angle, myPlayer->input.cursorDist);
			alSourcePlay(myPlayer->soundSource[3]);
			myPlayer->grenades--;
		}
		break;
		}
	}
}

//Mouse click and release handler
void on_mouse_pressed_released_game(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			myPlayer->input.shoot = true;
		else if (state == GLUT_UP)
			myPlayer->input.shoot = false;
		break;
	}

}


void on_mouse_move_active_game(int x, int y) {
	on_mouse_move_game(x, y);
}

//Following mouse position at all times to adjust the aim
void on_mouse_move_game(int x, int y) {
	float dx = x - windowWidth / 2;
	float dy = y - windowHeight / 2;

	myPlayer->input.cursorDist = sqrt(dx*dx + dy*dy)/windowWidth;

	float angle = atan2(dy, dx);
	myPlayer->input.angle = -angle;
}



extern bool trainingEnabled;

void on_timer_game()
{

	//Clock to make sure that game doesnt depend on framerate
	auto now = std::chrono::high_resolution_clock::now();;
	std::chrono::duration<double>  deltaTime = now - lastFrameTime;
	accumulator += deltaTime.count();
	lastFrameTime = now;

	if (trainingEnabled){
		accumulator = phisycsUpdateInterval*1.1;
	}
	//Do as many updates of the physics as should have happend in normal conditions
	while (accumulator > phisycsUpdateInterval) {
		networks[currentNetwork]->timeAlive++;
		updateCount++;
		world->Step(phisycsUpdateInterval, 6, 2);

		//Setting up bot movement velocity and aim
        BotMoves();

		//Updating players and sounds aswell as checking if they are dead
		for (unsigned i = 0; i < players.size(); i++) {
			if (players[i]->deathFlag) {
				players[i]->deathFlag = false;
				itemPool->SpawnRandom(players[i]->body->GetPosition());
				players[i]->die();
				if(i!=0){
					networks[currentNetwork]->kills+=1;
				}
			}
			if (!players[i]->alive) {
				continue;
			}

			players[i]->m_brain->Update();
			if(i == 0){
				alListener3f(AL_POSITION, players[i]->body->GetPosition().x, players[i]->body->GetPosition().y, 0);
			}
			players[i]->equiped_weapon->Update(players[i]->input.shoot);

		}


		enemySpawner->Update();
		particleSystem->Update();


		//Updating granades and deleting ones that are flagged for deletion
		for(unsigned i = 0; i < thrownGrenades.size(); i++){
			if(thrownGrenades[i]->toDelete){
				Grenade* tmp = thrownGrenades[i];
				thrownGrenades.erase(thrownGrenades.begin() + i);
				i--;
				delete tmp;
				continue;
			}

			float Gn = 0.18;
			float Gvx = Gn*cos(myPlayer->input.angle);
	    float Gvy = Gn*sin(myPlayer->input.angle);
			thrownGrenades[i]->Update(myPlayer->body->GetPosition().x+Gvx, myPlayer->body->GetPosition().y+Gvy);
		}


		//Deleting AudioWrappers that finished playing
		for(unsigned i = 0; i < audioWrappers.size(); i++){
			if(!(audioWrappers[i]->isPlaying()) && audioWrappers[i]->toDelete){
				AudioWrapper* tmp = audioWrappers[i];
				audioWrappers.erase(audioWrappers.begin() + i);
				i--;
				delete tmp;
			}
		}

		alSource3f(ambientSource[0], AL_POSITION, myPlayer->body->GetPosition().x, myPlayer->body->GetPosition().y, 0);


		//In case the human player is dead going back to Main menu
		if(!myPlayer->alive){
			alSourceStop(ambientSource[0]);
			//currentScene = GAME;
			GameOver = true;
			break;
		}

		accumulator -= phisycsUpdateInterval;
	}

}


//Drawing all elements
void DrawMap() {
	glPushMatrix();
	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, textureIDs[0]);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-9, -9, 0);
		glTexCoord2f(20, 0);
		glVertex3f(9, -9, 0);
		glTexCoord2f(20, 20);
		glVertex3f(9, 9, 0);
		glTexCoord2f(0, 20);
		glVertex3f(-9, 9, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
    glPushMatrix();
    glColor3f(0,0,0);

//     for(int i = 0; i < 40; ++i){
//         glBegin(GL_LINES);
//         glVertex3f(-9,i*18.0/40-9,0.05);
//         glVertex3f(9,i*18.0/40-9,0.05);
//         glEnd();
//     }
//
//     for(int i = 0; i < 40; ++i){
//         glBegin(GL_LINES);
//         glVertex3f(i*18.0/40-9,9,0.05);
//         glVertex3f(i*18.0/40-9,-9,0.05);
//         glEnd();
//     }
	glPopMatrix();
}

void DrawPlayers() {
	for (unsigned i = 0; i < players.size(); i++) {
		if (!players[i]->alive)
			continue;

		players[i]->DrawShadow();
	}
	for (unsigned i = 0; i < players.size(); i++) {
		if (!players[i]->alive)
			continue;

		players[i]->Draw();
	}
}

void DrawHUDPlayers() {
	for (unsigned i = 0; i < players.size(); i++) {
		if (!players[i]->alive)
			continue;

		if (players[i]->team) {
			glColor3f(1, 0, 0);
		}
		else {
			glColor3f(0, 0, 1);
		}

		glPushMatrix();
		b2Vec2 pos = players[i]->body->GetPosition();

		glTranslatef(pos.x, pos.y, 0);
		glScalef(1, 1, 0.1);
		glutSolidCube(0.5);

		glPopMatrix();

	}
}

void DrawBullets() {
	for (unsigned i = 0; i < bullets.size(); i++) {
        if( (abs(bullets[i]->body->GetLinearVelocity().x) <= 0.1 && abs(bullets[i]->body->GetLinearVelocity().y) <= 0.1) || bullets[i]->toDelete == 1){
					Bullet* tmp = bullets[i];
					bullets.erase(bullets.begin() + i);
					delete tmp;

          i--;
          continue;
        }
		bullets[i]->Draw();
	}
}

void DrawGrenades() {
	for (unsigned i = 0; i < thrownGrenades.size(); i++) {
		if(thrownGrenades[i]->thrown)
			thrownGrenades[i]->Draw();
	}
}

void DrawHUDMap() {
	glPushMatrix();
	glColor3f(0.8, 0.65, 0);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(-9, -9, 0);
	glVertex3f(9, -9, 0);
	glVertex3f(9, 9, 0);
	glVertex3f(-9, 9, 0);
	glEnd();
	glPopMatrix();
}

void DrawHUDBar() {
    float h = tan(30 * M_PI / 180) * 4;
	float w = h * aspectRatio;

	glPushMatrix();
	glTranslatef(myPlayer->body->GetPosition().x,myPlayer->body->GetPosition().y-h+h/10,0);
	glBindTexture(GL_TEXTURE_2D, textures["hudbar"]);
	glNormal3f(0, 0, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-w,-h/10, 0);
		glTexCoord2f(1, 0);
		glVertex3f(w, -h/10, 0);
		glTexCoord2f(1, 1);
		glVertex3f(w, h/10, 0);
		glTexCoord2f(0, 1);
		glVertex3f(-w, h/10, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);


	glTranslatef(-w+w/10,0,0);
	DrawWepon();

	//Writing player information needed for gameplay
	int numberOfAliveBots = 0;
	for (unsigned i = 1; i < players.size(); i++) {
		if (players[i]->alive)
			numberOfAliveBots++;
	}
	int botsLeft = enemySpawner->GetEnemiesInWave() - (enemySpawner->GetEnemiesSpawned() - numberOfAliveBots);
	sprintf(text, "Ammo: %d/%d", myPlayer->equiped_weapon->GetAmmo(), myPlayer->GetAmmo());
	glTranslatef(w/10, -h/40, 0);
	WriteText();
    memset(text, 0, sizeof text);
	float timer = myPlayer->equiped_weapon->GetReloadTimer();


	if(timer > 0 && myPlayer->alive){
		glPushMatrix();
		glTranslatef(0,-h/20,0);
		glBegin(GL_QUADS);
			glVertex3f(0,-h/80,0);
			glVertex3f(w/10*(timer)/2,-h/80,0);
			glVertex3f(w/10*(timer)/2,h/80,0);
			glVertex3f(0,h/80,0);
		glEnd();
		glPopMatrix();
	}

	glTranslatef(w/4, 0, 0);
	sprintf(text, "Grenades: %d", myPlayer->grenades);
	WriteText();
	memset(text, 0, sizeof text);


	if(thrownGrenades.size() > 0){
		timer = thrownGrenades[thrownGrenades.size() - 1]->GetExplodeTimer();
		if(timer > 0 && !thrownGrenades[thrownGrenades.size() - 1]->thrown){
			glPushMatrix();
			glTranslatef(0,-h/20,0);
			glBegin(GL_QUADS);
				glVertex3f(0,-h/80,0);
				glVertex3f(w/10*(timer)/2,-h/80,0);
				glVertex3f(w/10*(timer)/2,h/80,0);
				glVertex3f(0,h/80,0);
			glEnd();
			glPopMatrix();
		}
	}


	sprintf(text, "Wave: %d\tLeft: %d", enemySpawner->GetCurrentWave(), botsLeft);
	glTranslatef(w*0.9,0, 0);
	WriteText();




	glPopMatrix();
}

//Function for writing text
void WriteText(){
	glColor3f(0,0,0);
    glRasterPos3f(0, 0, 0);
	for(unsigned i = 0; i < strlen(text); i++){
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,text[i]);
	}
}

void DrawWepon(){
	glColor4f(1, 1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glNormal3f(0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, textures[myPlayer->equiped_weapon->Name()]);
	float animationScale = 0.25;
	glScalef(animationScale,animationScale, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glDisable(GL_BLEND);
}

//Setting up view parameters and drawing the screen
void on_display_game(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)windowWidth / windowHeight, 0.01, 1000);

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(myPlayer->body->GetPosition().x, myPlayer->body->GetPosition().y, 4,
		myPlayer->body->GetPosition().x, myPlayer->body->GetPosition().y, 0,
		0, 1, 0);


	DrawMap();
	DrawBullets();
	itemPool->DrawItems();
	DrawPlayers();
	DrawGrenades();
	DrawWalls();
	particleSystem->Draw();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	DrawHUDBar();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	//MiniMap
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-9, 9, -9, 9, 0.1, 100);

	glViewport(windowWidth - windowWidth/6, 0, windowWidth / 6, windowWidth / 6);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 18,
		0, 0, 0,
		0, 1, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	DrawHUDMap();
	DrawWalls();
	DrawHUDPlayers();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}



//Cleaning the memory
void Clean(bool x){
	for (unsigned i = 0; i < bullets.size(); i++) {
		Bullet* tmp = bullets[i];
		bullets.erase(bullets.begin() + i);
		delete tmp;
		i--;
	}

	for (unsigned i = 0; i < thrownGrenades.size(); i++) {
		Grenade* tmp = thrownGrenades[i];
		thrownGrenades.erase(thrownGrenades.begin() + i);
		delete tmp;
		i--;
	}

	for (unsigned i = 0; i < walls.size(); i++) {
			Block* tmp = walls[i];
			walls.erase(walls.begin() + i);
			delete tmp;
			i--;
	}

	map.clear();

	for(unsigned i = 0; i < audioWrappers.size(); i++){
		AudioWrapper* tmp = audioWrappers[i];
		audioWrappers.erase(audioWrappers.begin() + i);
		i--;
		delete tmp;
	}

	delete itemPool;

	delete enemySpawner;
	delete particleSystem;
	delete contactListener;

	for (unsigned i = 0; i < players.size(); i++) {
		Player* tmp = players[i];
		players.erase(players.begin() + i);
		if(i==0 && tmp->equiped_weapon->getWeaponType() != tmp->secondaryGun->getWeaponType()){
			tmp->equiped_weapon->FreeSources();
			tmp->secondaryGun->FreeSources();
		}
		else
			tmp->equiped_weapon->FreeSources();
		tmp->FreeSources();
		delete tmp->equiped_weapon;
		delete tmp->m_brain;
		delete tmp;
		i--;
	}

	 alDeleteSources(1, ambientSource);


	if(x){
		size_t size =  sizeof(soundIDs)/sizeof(soundIDs[0]);
		alDeleteBuffers(size, soundIDs);
		alutExit();
	}


	delete world;
}
