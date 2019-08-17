#ifndef BULLET_DEF
#define BULLET_DEF

#define TRAIL_LENGTH 10

#include "../header/util.h"
#include <Box2D/Box2D.h>

class Bullet : public Colider {
public:
	Bullet(float x, float y, float angle, int dmg, float bulletSize, float speed);
	~Bullet();
	void Draw();
	void StartSparkEffect();
	virtual ClassID getClassID();

	b2Body* body;
	int m_dmg;
	int toDelete;

private:
	float r;
	float speed;
	b2Vec2 trail[TRAIL_LENGTH];
};

#endif
