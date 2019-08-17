#ifndef PLAYER_DEF
#define PLAYER_DEF

#include "../header/items.h"
#include <Box2D/Box2D.h>

typedef struct _Input{
    float horizontal, vertical, angle;
    bool shoot;
} Input;


class Player{
public:
    Player(float x, float y, float r);
    Player();
	void Draw();
	void DrawShadow();
    void Update();
    void Move();
    bool team;
    Input input;
    int health;
    Weapon* equiped_weapon;
    b2Body* body;
    float r;
private:
    float speed;
protected:

};

#endif
