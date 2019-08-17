#include <Box2D/Box2D.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>
#include <map>
#include "../header/particleSystem.h"
#include "../header/util.h"

extern ParticleSystem* particleSystem;
extern std::map<std::string, int> textures;
extern double phisycsUpdateInterval;

Particle::Particle(b2Vec2 pos, b2Vec2 vel, float scale, float rotation, float rotationSpeed, float alpha) :
	pos(pos), vel(vel), scale(scale), rotation(rotation), rotationSpeed(rotationSpeed), alpha(alpha) {}

void Particle::Draw() {
	glColor4f(1, 1, 1, alpha);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glNormal3f(0, 0, 1);
	glTranslatef(pos.x, pos.y, 0.3);


	glScalef(scale, scale, 1);
	glRotatef(rotation/M_PI*180, 0, 0, 1);
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

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Particle::Update() {
	pos += phisycsUpdateInterval*vel;
	rotation += phisycsUpdateInterval * rotationSpeed;
}

void Particle::AddForce(b2Vec2 force) {
	vel += phisycsUpdateInterval*force;
}

Emitter::Emitter(b2Vec2* pos, b2Vec2 initialVelocity, b2Vec2 force, int particleCount, float lifespan, std::string texture){
	Emitter(b2Vec2(0, 0), initialVelocity, force, particleCount, lifespan, texture);
	isLocal = true;
	root = pos;
}

Emitter::Emitter(b2Vec2 pos, b2Vec2 initialVelocity, b2Vec2 force, int particleCount, float lifespan, std::string texture):
	pos(pos), force(force), particleCount(particleCount), lifespan(lifespan), texture(texture), initialVelocity(initialVelocity)
{
	lifespanTimer = 0.0;
	minSpeed = 0.1;
	maxSpeed = 0.3;
	minRotationSpeed = 0;
	maxRotationSpeed = 0;
	startRotation = -10;
	startScale = 0.1;
	endScale = 0;
	startAlpha = 1;
	endAlpha = 0;

	alphaTween = scaleTween = TW_LINEAR;
	isLocal = false;
}

Emitter::~Emitter() {
	for (std::vector<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		delete (*it);
	}
	particles.clear();
}

float TweenValue(Tween tw, float t) {
	switch (tw) {
	case TW_LINEAR:
		return t;
	case TW_CUBIC:
		return t * t;
	case TW_INVERSE_CUBIC:
		return sqrt(t);
	case TW_SINUSOIDAL:
		return sin(t*M_PI/4);
	default:
		return t;
	}
}

void Emitter::Update() {
	for (int i = 0; i < particleCount; i++)
	{
		particles[i]->AddForce(force);
		particles[i]->scale = lerp(startScale, endScale, TweenValue(scaleTween, lifespanTimer / lifespan));
		particles[i]->alpha = lerp(startAlpha, endAlpha, TweenValue(alphaTween, lifespanTimer / lifespan));
		particles[i]->Update();
	}
	lifespanTimer += phisycsUpdateInterval;
}

void Emitter::Draw() {
	glBindTexture(GL_TEXTURE_2D, textures[texture]);
	for (int i = 0; i < particleCount; i++)
	{
		particles[i]->Draw();
	}
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Emitter::Start() {
	for (int i = 0; i < particleCount; i++)
	{
		float angle = randomNumber(0, M_PI*2);
		float speed = randomNumber(minSpeed, maxSpeed);
		float rotationSpeed = randomNumber(minRotationSpeed, maxRotationSpeed);
		b2Vec2 vel(cos(angle)*speed, sin(angle)*speed);
		vel += initialVelocity;
		if (startRotation != -10)
			angle = startRotation;
		particles.push_back(new Particle(pos, vel, startScale, angle, rotationSpeed, startAlpha));
	}

	particleSystem->AddEmitter(this);
}


void Emitter::SetSpeed(float mins, float maxs) {
	minSpeed = mins;
	maxSpeed = maxs;

}

void Emitter::SetScale(float start, float end) {
	startScale = start;
	endScale = end;
}

void Emitter::SetScaleTween(Tween tw) {
	scaleTween = tw;
}

void Emitter::SetAlpha(float start, float end) {
	startAlpha = start;
	endAlpha = end;
}
void Emitter::SetAlphaTween(Tween tw) {
	alphaTween = tw;
}

void Emitter::SetRotation(float min, float max) {
	minRotationSpeed = min;
	maxRotationSpeed = max;
}

void Emitter::SetStartRotation(float rot) {
	startRotation = rot;
}

bool Emitter::IsDone() {
	return lifespan < lifespanTimer;
}

void ParticleSystem::AddEmitter(Emitter* emitter) {
	emitters.push_back(emitter);
}

void ParticleSystem::Update() {
	for (int i = emitters.size()-1; i >= 0; i--)
	{
		emitters[i]->Update();
		if (emitters[i]->IsDone()) {
			Emitter* tmp = emitters[i];
			emitters.erase(emitters.begin() + i);
			delete tmp;
		}
	}
}

void ParticleSystem::Draw() {
	for (unsigned i = 0; i < emitters.size(); i++)
	{
		emitters[i]->Draw();
	}

}

ParticleSystem::~ParticleSystem(){
	for (std::vector<Emitter*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
	{
		delete (*it);
	}
	emitters.clear();
}
