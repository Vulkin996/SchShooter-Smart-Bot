#include <iostream>
#include <GL/glut.h>
#include "../header/image.h"
#include "../header/player.h"
#include "../header/geometry.h"
#include "../header/weapon.h"
#include "../header/bullet.h"
#include "../header/gameScene.h"
#include "../header/menuScene.h"
#include <string>
#include <vector>
#include <Box2D/Box2D.h>
#include <chrono>
#include <map>
#include <AL/alut.h>

std::vector<std::string> textureNames = {
	std::string("sand"),
	std::string("wall3"),
	std::string("pistol"),
	std::string("rifle"),
	std::string("healthPotion"),
   	std::string("menu"),
    std::string("button"),
	std::string("shotgun"),
	std::string("blood"),
	std::string("hudbar"),
	std::string("armor"),
	std::string("shockwave"),
	std::string("fireball"),
	std::string("smoke"),
	std::string("muzzleFlash"),
	std::string("bulletCasing"),
	std::string("grenade"),
	std::string("bannerAlive"),
	std::string("bannerDead"),
	std::string("spark"),	
	std::string("bullet"),
	std::string("sniper"),
	std::string("youDied"),
	std::string("waveCleared")
	

};

std::vector<std::string> soundNames = {
	std::string("pistol"),
	std::string("rifle"),
	std::string("shotgun"),
	std::string("reload"),
	std::string("reloadShotgun"),
	std::string("death"),
	std::string("pickup"),
	std::string("heal"),
	std::string("music"),
	std::string("wallImpact1"),
	std::string("bodyImpact1"),
	std::string("bodyImpact2"),
	std::string("grenadeThrow"),
	std::string("grenade"),
	std::string("grenadePull"),
	std::string("grenadePickup"),
	std::string("armor"),
	std::string("sniper")
};

std::vector<std::string> textureLocations;
std::vector<std::string> soundLocations;

std::map<std::string, int> textures;
std::map<std::string, int> sounds;

GLuint textureIDs[4];
ALuint soundIDs[18];

#define TIMER_ID 0
#define TIMER_INTERVAL 15

enum scene {
	GAME,
	MENU
	//EASTER_EGG
};
enum scene currentScene;


static int animation_ongoing;
static void on_keyboard(unsigned char key, int x, int y);
static void keyboard_up(unsigned char key, int x, int y);
static void on_mouse_move(int x, int y);
static void on_mouse_move_active(int x, int y);
static void on_mouse_pressed_released(int button, int state, int x, int y);
static void on_timer(int value);
static void on_display(void);
static void on_reshape(int width, int height);

void LoadSounds(){
	ALuint buffer;
	for (unsigned i = 0; i < soundNames.size(); i++) {
		buffer = alutCreateBufferFromFile(("sound/"+soundNames[i]+".wav").c_str());

		if ( alutGetError() != ALUT_ERROR_NO_ERROR ){
   		// TODO: handle the error
			std::cerr<<"Failed to Load Sounds!" <<std::endl;
   		return;
		}
		soundIDs[i] = buffer;
	}

	for (unsigned i = 0; i < soundNames.size(); i++) {
		sounds[soundNames[i]] = soundIDs[i];
	}
}

void LoadTextures(){
	textureLocations = std::vector<std::string>();
	for (unsigned i = 0; i < textureNames.size(); i++) {
		textureLocations.push_back("textures/"+textureNames[i]+".bmp");
	}

	glGenTextures(textureLocations.size(), textureIDs);
	for (unsigned i = 0; i < textureNames.size(); i++) {
		textures[textureNames[i]] = textureIDs[i];
	}

	Image* image;
	image = image_init(0, 0);

	for (unsigned i=0; i<textureLocations.size(); i++){
		char *cstr = &textureLocations[i][0u];
		image_read(image, cstr);
            glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
            glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 image->width, image->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	}



	image_done(image);
}


int main(int argc, char **argv)
{

    B2_NOT_USED(argc);
    B2_NOT_USED(argv);
	// Initialising glut
	glutInit(&argc, argv);
	alutInit(NULL,NULL);
	alDistanceModel(AL_LINEAR_DISTANCE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	//Creating the window
	glutInitWindowSize(1280, 720);
	glutCreateWindow(argv[0]);

	// Registering functions for event detection
	glutKeyboardFunc(on_keyboard);
	glutDisplayFunc(on_display);
	glutMotionFunc(on_mouse_move_active);
	glutPassiveMotionFunc(on_mouse_move);
	glutMouseFunc(on_mouse_pressed_released);
    glutKeyboardUpFunc(keyboard_up);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);


    glutReshapeFunc(on_reshape);
	// Opengl initialization
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GLUT_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);

	glShadeModel (GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
              GL_MODULATE);

	LoadTextures();
	LoadSounds();

	GLfloat lightPos0[] = { -1, 1, 1, 0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

    
	glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
  	animation_ongoing = 1;

	//Setting current scene and initializing 
	currentScene = MENU;
	InitGame();
	InitMenu();
		
	// Entering main glut Loop
	glutMainLoop();
	


    return 0;
}


float windowWidth, windowHeight, aspectRatio;

static void on_reshape(int width, int height){
	windowWidth = width;
	windowHeight = height;
	aspectRatio = (float)width/height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,  (float)width/height, 0.01 ,  1000);
}

static void on_keyboard(unsigned char key, int x, int y)
{
	switch (currentScene) {
	case GAME:
		on_keyboard_game(key, x, y);
		break;

    case MENU:
        on_keyboard_menu(key,x,y);
        break;
    }
}

static void keyboard_up(unsigned char key, int x, int y){
	switch (currentScene) {
	case GAME:
		keyboard_up_game(key, x, y);
		break;
	case MENU:
		break;
	}
}


static void on_mouse_pressed_released(int button, int state, int x, int y) {
	switch (currentScene) {
	case GAME:
		on_mouse_pressed_released_game(button, state, x, y);
		break;
    case MENU:
        on_mouse_pressed_released_menu(button, state, x, y);
        break;
    }

}

static void on_mouse_move_active(int x, int y) {
	switch (currentScene) {
	case GAME:
		on_mouse_move_active_game(x, y);
		break;
	case MENU:
		break;
	}
}


static void on_mouse_move(int x, int y){
	switch (currentScene) {
	case GAME:
		on_mouse_move_game(x, y);
		break;
	case MENU:
		break;
	}
}



static void on_timer(int value)
{
	//Checking if the callback comes from the right timer
	if (value != TIMER_ID){
		return;
	}

	switch (currentScene) {
	case GAME:
		on_timer_game();
		break;
	case MENU:
		on_timer_menu();
		break;
	}

	glutPostRedisplay();
	// Setting a new timer if needed

	if (animation_ongoing) {
		glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
	}

}


static void on_display(void)
{	// Setting all pixel colors for given backround
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (currentScene) {
	case GAME:
		on_display_game();
		break;
    case MENU:
        on_display_menu();
        break;
	}

	// Sending new image to the screen
	glutSwapBuffers();

}
