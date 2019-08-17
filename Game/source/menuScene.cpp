#include <iostream>
#include "../header/menuScene.h"
#include "../header/gameScene.h"
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <Box2D/Box2D.h>
#include <AL/alut.h>
#include <chrono>
#include <string>
#include <map>

//All necessary elementss
extern float windowWidth, windowHeight, aspectRatio;
extern std::map<std::string, int> textures;
extern std::map<std::string, int> sounds;
extern std::map<std::string, int> textures;
extern std::chrono::high_resolution_clock::time_point lastFrameTime;
std::map<std::string, bool> pressedButtons;
extern ALuint ambientSource[1];
extern bool GameOver;
bool menuActive;
bool creditsActive;
bool controlsActive;
bool resetGame;
float logoAnimation = 0;
float buttonOffset = -1;

enum scene {
	GAME,
	MENU,
	EASTER_EGG
};
extern enum scene currentScene;

//Initializing the game
void InitMenu() {
	//Playing background music and adding buttons
	alSourcePlay(ambientSource[0]);
		pressedButtons.insert( std::pair<std::string,bool>(std::string("play"),false) );
		pressedButtons.insert( std::pair<std::string,bool>(std::string("reset"),false) );
		pressedButtons.insert( std::pair<std::string,bool>(std::string("credits"),false) );
		pressedButtons.insert( std::pair<std::string,bool>(std::string("controls"),false) );
		pressedButtons.insert( std::pair<std::string,bool>(std::string("exit"),false) );
		pressedButtons.insert( std::pair<std::string,bool>(std::string("back"),false) );
    menuActive = true;
	resetGame = true;
    creditsActive = false;
    controlsActive = false;
}


//When button is pressed
void pressButton(int x, int y){
	float h = tan(30 * M_PI / 180) * 4;
	float w = h * aspectRatio;
	float y2 = -(y- windowHeight / 2)/windowHeight*2*h;
	y += buttonOffset*windowHeight/4.5;

    float x1 = (x- windowWidth / 2)/windowWidth*2*w;
    float y1 = -(y- windowHeight / 2)/windowHeight*2*h;
    if(resetGame && x1>=-w/8 && x1 <= w/8 && y1 <= h/2+h/20 && y1 >= h/2-h/20 && menuActive){
        pressedButtons["play"] = true;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/2+h/20 && y1 >= h/2-h/20 && menuActive){
        pressedButtons["play"] = true;
    }
    else if(GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20+h/4 && y1 >= h/4-h/20+h/4 && menuActive){
        pressedButtons["reset"] = true;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20 && y1 >= h/4-h/20 && menuActive){
        pressedButtons["reset"] = true;
    }
    else if((GameOver||resetGame) && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20 && y1 >= h/4-h/20 && menuActive ){
        pressedButtons["controls"] = true;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20-h/4 && y1 >= h/4-h/20-h/4 && menuActive ){
        pressedButtons["controls"] = true;
    }
    else if((GameOver||resetGame) && x1>=-w/8 && x1 <= w/8 && y1 <= h/20  && y1 >= -h/20 && menuActive ){
        pressedButtons["credits"] = true;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/20-h/4  && y1 >= -h/20-h/4 && menuActive ){
        pressedButtons["credits"] = true;
    }
    else if((GameOver||resetGame) && x1>=-w/8 && x1 <= w/8 && y1 <= -h/4+h/20  && y1 >= -h/4-h/20 && menuActive){
        pressedButtons["exit"] = true;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= -h/4+h/20-h/4 && y1 >= -h/4-h/20-h/4 && menuActive){
        pressedButtons["exit"] = true;
    }
    else if(x1>=-w/8 && x1 <= w/8 && y2 <= -h/1.8+h/20  && y2 >= -h/1.8-h/20 && (creditsActive || controlsActive)){
        pressedButtons["back"] = true;
    }

}

//When button is released
void releaseButton(int x, int y){
	float h = tan(30 * M_PI / 180) * 4;
	float w = h * aspectRatio;
	float y2 = -(y- windowHeight / 2)/windowHeight*2*h;

	y += buttonOffset * windowHeight / 4.5;

    float x1 = (x- windowWidth / 2)/windowWidth*2*w;
    float y1 = -(y- windowHeight / 2)/windowHeight*2*h;

    if(resetGame && x1>=-w/8 && x1 <= w/8 && y1 <= h/2+h/20  && y1 >= h/2-h/20  && pressedButtons["play"] ){
        lastFrameTime = std::chrono::high_resolution_clock::now();
		resetGame = false;
        currentScene = GAME;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/2+h/20  && y1 >= h/2-h/20  && pressedButtons["play"] ){
        lastFrameTime = std::chrono::high_resolution_clock::now();
		resetGame = false;
        currentScene = GAME;
    }
    else if(GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20+h/4 && y1 >= h/4-h/20+h/4  && pressedButtons["reset"]){
	   Clean(false);
       	   InitGame();
	   resetGame = true;
    }
    else if(!resetGame && !GameOver  && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20  && y1 >= h/4-h/20  && pressedButtons["reset"]){
	   Clean(false);
       InitGame();
	   resetGame = true;
    }
    else if((resetGame || GameOver) && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20 && y1 >= h/4-h/20  && pressedButtons["controls"]){
       controlsActive = true;
       menuActive = false;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/4+h/20-h/4 && y1 >= h/4-h/20-h/4 && pressedButtons["controls"]){
       controlsActive = true;
       menuActive = false;
    }
    else if((resetGame || GameOver) && x1>=-w/8 && x1 <= w/8 && y1 <= h/20  && y1 >= -h/20 && pressedButtons["credits"]){
       creditsActive = true;
       menuActive = false;
    }
    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= h/20-h/4  && y1 >= -h/20-h/4 && pressedButtons["credits"]){
       creditsActive = true;
       menuActive = false;
    }
    else if((resetGame ||(!resetGame && GameOver))&& x1>=-w/8 && x1 <= w/8 && y1 <= -h/4+h/20  && y1 >= -h/4-h/20 && pressedButtons["exit"]){
		Clean(true);
		glutLeaveMainLoop();
    }

    else if(!resetGame && !GameOver && x1>=-w/8 && x1 <= w/8 && y1 <= -h/4+h/20-h/4  && y1 >= -h/4-h/20-h/4 && pressedButtons["exit"]){
		Clean(true);
		glutLeaveMainLoop();
    }
    else if(x1>=-w/8 && x1 <= w/8 && y2 <= -h/1.8+h/20 && y2 >= -h/1.8-h/20 && pressedButtons["back"] && creditsActive ){
       creditsActive = false;
       menuActive = true;
    }
    else if(x1>=-w/8 && x1 <= w/8 && y2 <= -h/1.8+h/20 && y2 >= -h/1.8-h/20 && pressedButtons["back"] && controlsActive ){
       controlsActive = false;
       menuActive = true;
    }

    pressedButtons["play"] = false;
	pressedButtons["reset"] = false;
    pressedButtons["controls"] = false;
    pressedButtons["credits"] = false;
    pressedButtons["exit"] = false;
    pressedButtons["back"] = false;
}


//Handling mouse presses
void on_mouse_pressed_released_menu(int button, int state, int x, int y) {
	switch (button) {
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
			pressButton(x,y);
		else if (state == GLUT_UP)
			releaseButton(x,y);

		break;
	}

}



void on_timer_menu()
{
	logoAnimation += 0.04;
}

//Toggling fullscreen and windowed mode
void on_keyboard_menu(unsigned char key, int x, int y)
{
	switch (key) {
        case 27:
            glutPositionWindow(5,30);
            glutReshapeWindow(1280, 720);
            break;
        case 'f':
            glutFullScreen();
            break;

	}
}

//Drawing a quad for logo
void DrawQuad(std::string str) {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glNormal3f(0, 0, 1);

	glBindTexture(GL_TEXTURE_2D, textures[str]);
	glScalef(2, 2, 1);
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
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

//Drawing the logo
void DrawLogo() {
	glPushMatrix();
	glTranslatef(0.1, 1, 0);
	glScalef(.7, .7, 1);
	if (GameOver) {
		DrawQuad("youDied");
		glPopMatrix();
		return;
	}

	float alpha = (cos(logoAnimation)+1)/2;
	glColor4f(1, 1, 1, alpha);
	DrawQuad("bannerAlive");
	glColor4f(1, 1, 1, 1- alpha);
	DrawQuad("bannerDead");
	glPopMatrix();

}

void DrawButton(float w, float h, std::string str) {
	glNormal3f(0, 0, 1);

	glBindTexture(GL_TEXTURE_2D, textures[str]);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-w, -h, 0);
	glTexCoord2f(1, 0);
	glVertex3f(w, -h, 0);
	glTexCoord2f(1, 1);
	glVertex3f(w, h, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-w, h, 0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);


}

//Drawing the Menu
void DrawMenu() {
	//Height and width of the quad we see at 0 z coordinate
    float h = tan(30 * M_PI / 180) * 4;
	float w = h * aspectRatio;
	glPushMatrix();
	glColor3f(1, 1, 1);
	DrawButton(w,h,"menu");
    glPopMatrix();

    glPushMatrix();
	glTranslatef(0, buttonOffset, 0);

    //Play button
	if(resetGame || !GameOver){
		if(pressedButtons["play"])
			glColor4f(0.3,0.3,0.3,0.8);
		else
			glColor3f(1,1,1);
		glTranslatef(0, h/2, 0);
		DrawButton(w/8, h/20, "button");
	}

	//Reset button
	if(GameOver || !resetGame){
		if(GameOver)
			glTranslatef(0, 3*h/4, 0);
		if(pressedButtons["reset"])
			glColor4f(0.3,0.3,0.3,0.8);
		else
			glColor3f(1,1,1);
		glTranslatef(0, -h/4, 0);
		DrawButton(w/8, h/20, "button");
	}


    //Controls button
    if(pressedButtons["controls"])
        glColor4f(0.3,0.3,0.3,0.8);
    else
        glColor3f(1,1,1);
    glTranslatef(0, -h/4, 0);
	DrawButton(w/8, h/20, "button");

    //Credits button
    if(pressedButtons["credits"])
        glColor4f(0.3,0.3,0.3,0.8);
    else
        glColor3f(1,1,1);
    glTranslatef(0, -h/4, 0);
	DrawButton(w/8, h/20, "button");

    //Exit button
    if(pressedButtons["exit"])
        glColor4f(0.3,0.3,0.3,0.8);
    else
        glColor3f(1,1,1);
    glTranslatef(0, -h/4, 0);
    DrawButton(w/8, h/20, "button");
    glPopMatrix();


	//Writing the Text for the buttons
    unsigned char play[] = "Play";
	unsigned char reset[] = "Reset";
    unsigned char credits[] = "Credits";
    unsigned char controls[] = "Controls";
    unsigned char exit[] = "Exit";
    glPushMatrix();


	//Play button text
	glTranslatef(0, buttonOffset, 0);
	if(resetGame || !GameOver){
		glTranslatef(-w/30, h/2-h/80, 0);
		glColor3f(1,1,1);
		glRasterPos3f(0, 0, 0);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,play);
	}

	//Reset button text
	if(GameOver || !resetGame){
		if(GameOver)
			glTranslatef(-w/30, 3*h/4-h/80, 0);
		glTranslatef(-w/80, -h/4, 0);
		glRasterPos3f(0, 0, 0);
		glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,reset);
		glTranslatef(w/80, 0, 0);
	}

	//Controls button text
    glTranslatef(-w/30, -h/4, 0);
    glRasterPos3f(0, 0, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,controls);


	//Credits button text
    glTranslatef(w/60, -h/4, 0);
    glRasterPos3f(0, 0, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,credits);


	//Exit button text
    glTranslatef(w/60, -h/4, 0);
    glRasterPos3f(0, 0, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,exit);
    glPopMatrix();
}

//Drawing Controlls screen
void DrawControls(){
	//Height and width of the quad we see at 0 z coordinate
    float h = tan(30 * M_PI / 180) * 4;
	float w = h * aspectRatio;
	glPushMatrix();

	glColor3f(1, 1, 1);
	DrawButton(w,h,"menu");


    unsigned char controls[] = "w - Move up\na - Move left\ns - Move down\nd - Move right\n\nShoot - Mouse Left\nGrenade - g\nSidearm - 2\n\n f - Fullscreen\nESC in menu - Windowed mode\nESC in game- Main menu";


    glTranslatef(-w/7, h/1.6, 0);
    glColor3f(1,1,1);
    glRasterPos3f(0, 0, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,controls);
    glPopMatrix();


    glBindTexture(GL_TEXTURE_2D, textures["button"]);
    if(pressedButtons["back"])
        glColor4f(0.3,0.3,0.3,0.8);
    else
        glColor3f(1,1,1);
    glTranslatef(0, -h/1.8, 0);
    DrawButton(w/8,h/20,"button");

    glPushMatrix();
    unsigned char back[] = "Back";
    glTranslatef(-w/30, -h/60, 0);
    glRasterPos3f(0, 0, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,back);
    glPopMatrix();
}


//Drawing the Credits screen
void DrawCredits(){
	//Height and width of the quad we see at 0 z coordinate
    float h = tan(30 * M_PI / 180) * 4;
	float w = h * aspectRatio;
	glPushMatrix();

	glColor3f(1, 1, 1);
	DrawButton(w,h,"menu");
	glBindTexture(GL_TEXTURE_2D, 0);


    unsigned char credits[] = "Branko Djakovic 41/2015\n\nMladen Krcmarevic 119/2015\n\nFilip Kristic 335/2015\n\nSounds from:\n-http://soundbible.com\n-https://www.zapsplat.com\n-www.bensound.com";


    glTranslatef(-w/5, h/2, 0);
    glColor3f(1,1,1);
    glRasterPos3f(0, 0, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,credits);
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, textures["button"]);
    if(pressedButtons["back"])
        glColor4f(0.3,0.3,0.3,0.8);
    else
        glColor3f(1,1,1);
    glTranslatef(0, -h/1.8, 0);
	DrawButton(w/8, h/20, "button");
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    glPushMatrix();
    unsigned char back[] = "Back";
    glTranslatef(-w/30, -h/60, 0);
    glRasterPos3f(0, 0, 0);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,back);
    glPopMatrix();
}

void on_display_menu(void)
{

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)windowWidth / windowHeight, 0.01, 1000);

	glViewport(0, 0, windowWidth, windowHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 4,
		0, 0, 0,
		0, 1, 0);


    glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	if (menuActive) {
        DrawMenu();
		DrawLogo();
	}
    else if(controlsActive)
        DrawControls();
    else
        DrawCredits();
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}
