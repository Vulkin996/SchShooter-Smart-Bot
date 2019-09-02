#include <iostream>
#include <GL/glut.h>
#include "../header/image.h"
#include "../header/player.h"
#include "../header/geometry.h"
#include "../header/weapon.h"
#include "../header/bullet.h"
#include "../header/gameScene.h"
#include "../header/menuScene.h"
#include "../header/annPlayer.h"
#include "../header/genetic.h"
#include <string.h>
#include <string>
#include <vector>
#include <Box2D/Box2D.h>
#include <chrono>
#include <map>
#include <AL/alut.h>

// //vector of temp population representation
// std::vector<ANNPlayer*> networks;
// //pointer to the current unit playing the game

//Current network
const std::string outputFile = "fann.txt";
const std::string outputFileTest = "fannTest.txt";
const std::string outputFolderFile = "fanns/fann";
ANNPlayer* currentPlayer;

//Genetic algorithm
GeneticAlgorithm genetic;

unsigned currentNetwork = 0;
//function initializing population
// void setupNetworks(){
// 	for(int i = 0; i < 1; i++){
// 		networks.push_back(new ANNPlayer());
// 	}
// 	currentNetwork = 0;
// }

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

bool trainingEnabled = true;
bool trainingWatchable = true;
extern bool GameOver;
int generation = 0;
float windowWidth, windowHeight, aspectRatio;

#define TIMER_ID 0
#define TIMER_INTERVAL 15

enum scene {
	GAME,
	MENU
};
enum scene currentScene;


static int animation_ongoing;
/*
static void on_keyboard(unsigned char key, int x, int y);
static void keyboard_up(unsigned char key, int x, int y);
*/
static void on_timer(int value);
static void on_display(void);
static void on_reshape(int width, int height);

void LoadSounds(){
	ALuint buffer;
	for (unsigned i = 0; i < soundNames.size(); i++) {
		buffer = alutCreateBufferFromFile(("sound/"+soundNames[i]+".wav").c_str());

		if ( alutGetError() != ALUT_ERROR_NO_ERROR ){
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

void HandleSimulationStep(){

	if (GameOver || currentPlayer->timeAlive > 5000){
		//BRANKOGEN FITNESS
		//Kraj partije jedne mreze
		currentPlayer->CalculateFitness();
		std::cout << "Network: " << currentNetwork << " finished with fitness: " << currentPlayer->m_fitness << " kills: " << currentPlayer->kills << " damage: " << currentPlayer->dmgDone<<" time: " << currentPlayer->timeAlive<<std::endl;
		if(currentPlayer->m_fitness > genetic.top_chromosome->m_fitness){
			float* gen = currentPlayer->GetChromosome();
			for(int i = 0; i < genetic.target_size; i++){
			    genetic.top_chromosome->m_content[i] = gen[i];
			    genetic.top_chromosome->m_fitness = currentPlayer->m_fitness;
			}
			delete[] gen;
		}
		// if(currentPlayer->timeAlive > 5000){
		// 	std::cout << std::endl << "Save FITNESS: "<< genetic.top_chromosome->m_fitness << std::endl;
		// 	currentPlayer->SetChromosome(genetic.top_chromosome->m_content);
		// 	currentPlayer->net.save(outputFileTest);
		// 	exit(0);
		// }

		// if(currentPlayer->kills > 4){
		// 	std::cout << "Network: " << currentNetwork << " finished with fitness: " << currentPlayer->m_fitness << " kills: " << currentPlayer->kills << " damage: " << currentPlayer->dmgDone<<" time: " << currentPlayer->timeAlive<<std::endl;
		//
		// 	exit(0);
		// }

		// if(trainingEnabled){
		if(currentNetwork == genetic.generation_size-1){
			//BRANKOGEN CROSSOVER
			//I nova populacija i onda ispocetka sve
			genetic.for_reproduction = genetic.selection(genetic.chromosomes);
			genetic.temp_gen = genetic.create_generation(genetic.for_reproduction);
			genetic.for_reproduction.clear();

			for (std::vector<Chromosome*>::iterator ch = genetic.chromosomes.begin(); ch != genetic.chromosomes.end(); ++ch) {
		        delete[] (*ch)->m_content;
		        delete *ch;
		    }
			genetic.chromosomes.clear();

			for(unsigned i = 0; i < genetic.generation_size-1; i++){
	      		genetic.chromosomes.push_back(genetic.temp_gen[i]);
	    	}
			genetic.chromosomes.push_back(new Chromosome(genetic.top_chromosome, genetic.target_size));
			genetic.temp_gen.clear();

			// std::cout << "SOLUTION: " << std::endl;
			// for(int j = 0; j < genetic.target_size; j++){
			// 	std::cout << genetic.top_chromosome->m_content[j]<< " ";
			// }
			if(genetic.current_iteration == 150){
			std::cout << std::endl << "Save FITNESS: "<< genetic.top_chromosome->m_fitness << std::endl;
			for(int j = 0; j < genetic.target_size; j++){
				std::cout << genetic.top_chromosome->m_content[j]<< " ";
			}
			currentPlayer->SetChromosome(genetic.top_chromosome->m_content);
			currentPlayer->net.save(outputFile);
		}


			currentNetwork = 0;



			genetic.current_iteration++;
			if(genetic.current_iteration > genetic.iterations){
				std::cout << "SOLUTION: " << std::endl;
				for(int j = 0; j < genetic.target_size; j++){
					std::cout << genetic.top_chromosome->m_content[j]<< " ";
				}
				std::cout << std::endl;
				std::cout << "FITNESS: "<< genetic.top_chromosome->m_fitness << std::endl;
				std::cout << std::endl;
				currentPlayer->SetChromosome(genetic.top_chromosome->m_content);
				currentPlayer->net.save(outputFile);


				for(unsigned i = 0; i < genetic.generation_size; i++){
					currentPlayer->SetChromosome(genetic.chromosomes[i]->m_content);
					const std::string file = outputFolderFile + std::to_string(i) + ".txt";
					currentPlayer->net.save(file);
				}

				for (std::vector<Chromosome*>::iterator ch = genetic.chromosomes.begin(); ch != genetic.chromosomes.end(); ++ch) {
					delete[] (*ch)->m_content;
					delete *ch;
				}
				genetic.chromosomes.clear();

				Clean(true);
				exit(0);
			}
			currentPlayer->SetChromosome(genetic.chromosomes[currentNetwork]->m_content);
			std::cout << "GENERATION: " << genetic.current_iteration << std::endl;
		}
		else{
			currentNetwork++;
			currentPlayer->SetChromosome(genetic.chromosomes[currentNetwork]->m_content);
		}
	// }

		Clean(false);
		InitGame();
	}
}

int main(int argc, char **argv)
{
	trainingEnabled = ((argc == 2 || argc == 3) && strcmp(argv[1], "-t") == 0);
	trainingWatchable = (argc == 2 && strcmp(argv[1], "-tv") == 0);


  B2_NOT_USED(argc);
  B2_NOT_USED(argv);
	// Initialising glut

	if (!trainingEnabled){
		glutInit(&argc, argv);
		alutInit(NULL,NULL);
		alDistanceModel(AL_LINEAR_DISTANCE);

		glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_MULTISAMPLE);
		//Creating the window
		glutInitWindowSize(1280, 720);
		glutCreateWindow(argv[0]);

		// Registering functions for event detection
		//glutKeyboardFunc(on_keyboard);
		glutDisplayFunc(on_display);
	  //glutKeyboardUpFunc(keyboard_up);
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
	}

	//Setting current scene and initializing
	currentPlayer = new ANNPlayer(outputFile);

	currentScene = GAME;
	InitGame();

	if (trainingEnabled){
		int iter = argc == 3 ? atoi(argv[2]) : 10;
		genetic.Init(currentPlayer->numberOfConnections);
		genetic.setIterations(iter);
		windowWidth = 1280;
		windowHeight = 720;
		std::cout << "Connections: " << currentPlayer->numberOfConnections << std::endl;
		std::cout << "GENERATION: " << genetic.current_iteration << std::endl;
		//currentPlayer->SetChromosome(genetic.chromosomes[0]->m_content);
		while(true){
			on_timer_game();
			HandleSimulationStep();
		}
	}
	else{
		if (trainingWatchable){
			int iter = argc == 3 ? atoi(argv[2]) : 10;
			genetic.Init(currentPlayer->numberOfConnections);
			genetic.setIterations(iter);
		}
		glutMainLoop();
	}

  return 0;
}

static void on_reshape(int width, int height){
	windowWidth = width;
	windowHeight = height;
	aspectRatio = (float)width/height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60,  (float)width/height, 0.01 ,  1000);
}
/*
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
*/


static void on_timer(int value)
{
	//Checking if the callback comes from the right timer
	if (value != TIMER_ID){
		return;
	}

	switch (currentScene) {
	case GAME:
		on_timer_game();
		if (GameOver){
			if (trainingWatchable){
				HandleSimulationStep();
			}
			Clean(false);
			InitGame();
		}
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
