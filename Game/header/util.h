#ifndef UTIL_H
#define UTIL_H

#define NUM_OF_SOURCES_WRAP 1

#include <iostream>
#include <Box2D/Box2D.h>
#include <time.h>
#include <string>
#include <AL/alut.h>
#include <map>

extern std::map<std::string, int> sounds;

enum ClassID{
  COLIDER,
  PLAYER,
  BULLET,
  BLOCK
};

//Empty Base Class of all classes that have collision logic
class Colider{
public:
  virtual ClassID getClassID() { return COLIDER; }
	virtual ~Colider(){}
};

class RayCastCallback : public b2RayCastCallback{
public:
  RayCastCallback(): m_fixture(NULL)
  {};

  float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
    m_fixture = fixture;
    return fraction;
  }

  b2Fixture* m_fixture;
};

//Class that handles sounds from objects that get destroyed
class AudioWrapper{
public:
	AudioWrapper(float x, float y, std::string sound): m_x(x), m_y(y), m_sound(sound) {
	alGenSources(NUM_OF_SOURCES_WRAP, soundSource);
	alSourcei(soundSource[0], AL_BUFFER, sounds[sound]);
	if(sound == std::string("grenade"))
		alSourcef(soundSource[0], AL_GAIN, 0.8);
	if(sound == std::string("heal"))
		alSourcef(soundSource[0], AL_GAIN, 0.4);

	alSource3f(soundSource[0], AL_POSITION, m_x, m_y, 0.2);
	alSourcef(soundSource[0], AL_PITCH, 1);

	toDelete = false;
	}
	~AudioWrapper(){
		alDeleteSources(NUM_OF_SOURCES_WRAP, soundSource);
	}
	void playSound(){
		alSourcePlay(soundSource[0]);
	}
	bool isPlaying(){
		ALenum state = 0;

    alGetSourcei(soundSource[0], AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);
	}
	bool toDelete;
private:
	ALuint soundSource[NUM_OF_SOURCES_WRAP];
	float m_x;
	float m_y;
	std::string m_sound;
};

bool IsOnScreen(b2Vec2 position);
float randomNumber(float start, float end);
float lerp(float start, float end, float t);

float fitness(int kills, int time_alive);

#endif
