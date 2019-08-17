#ifndef BULLET_DEF
#define BULLET_DEF

#include <Box2D/Box2D.h>

class Bullet {
public:
	Bullet(float x, float y, float angle);
	void Draw();
	
	b2Body* body;
	
private:
	float r;
	float speed;
};

#endif
