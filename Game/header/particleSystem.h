#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <Box2D/Box2D.h>
#include <vector>

enum Tween {
	TW_LINEAR,
	TW_CUBIC,
	TW_INVERSE_CUBIC,
	TW_SINUSOIDAL
};

class Particle {
public:
	Particle(b2Vec2 pos, b2Vec2 vel, float scale, float rotation, float rotationSpeed, float alpha);
	b2Vec2 pos;
	b2Vec2 vel;
	float scale;
	float rotation;
	float rotationSpeed;
	float alpha;

	void Draw();
	void Update();
	void AddForce(b2Vec2 force);
};

class Emitter {
public:
	Emitter(b2Vec2 pos, b2Vec2 initialVelocity, b2Vec2 force, int particleCount, float lifespan, std::string texture);
	Emitter(b2Vec2* pos, b2Vec2 initialVelocity, b2Vec2 force, int particleCount, float lifespan, std::string texture);
	~Emitter();
	void Update();
	void Draw();
	void Start();
	void SetSpeed(float minSpeed, float maxSpeed);
	void SetScale(float startScale, float endScale);
	void SetScaleTween(Tween tw);
	void SetAlpha(float startScale, float endScale);
	void SetAlphaTween(Tween tw);
	void SetRotation(float minRotation, float maxRotation);
	void SetStartRotation(float rot);
	bool IsDone();

private:
	std::vector<Particle*> particles;
	b2Vec2 pos;
	b2Vec2 force;
	int particleCount;
	float lifespan;
	std::string texture;
	b2Vec2 initialVelocity;
	b2Vec2* root;

	float lifespanTimer;

	bool isLocal;

	float minSpeed;
	float maxSpeed;

	float minRotationSpeed;
	float maxRotationSpeed;
	float startRotation;

	Tween scaleTween;
	float startScale;
	float endScale;

	Tween alphaTween;
	float startAlpha;
	float endAlpha;
};

class ParticleSystem {
public:
	~ParticleSystem();
	void AddEmitter(Emitter* emitter);
	void Update();
	void Draw();

private:
	std::vector<Emitter*> emitters;
};

#endif
