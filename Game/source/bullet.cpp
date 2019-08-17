#include "../header/bullet.h"
#include <math.h>
#include <GL/glut.h>
#include <Box2D/Box2D.h>

extern b2World* world;

/*Bullet constructor: takes a point from which a bullet is being fired and a trajectory angle*/
Bullet::Bullet(float x, float y, float angle){
	r = 0.022;
	speed = 0.058;

	/*Creating Bullet Body*/
	b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
	bodyDef.linearDamping = 2.0f;
    bodyDef.position.Set(x, y);
    body = world->CreateBody(&bodyDef);

    b2CircleShape cShape;
    cShape.m_p.Set(0, 0);
    cShape.m_radius = r;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &cShape;

    fixtureDef.density = 1.0f;
    
    fixtureDef.friction = 0.01f;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);

    //When bullet is created its being fired so we set it a linear impulse
    float vx = speed*cos(angle);
    float vy =  speed*sin(angle);
    
    body->ApplyLinearImpulse(b2Vec2(vx,vy), body->GetWorldCenter(), true);

};

void Bullet::Draw(){

    glColor3f(1, 1, 1);
    
    glPushMatrix();
    glTranslatef(body->GetPosition().x, body->GetPosition().y, r);
    glutSolidSphere(r, 20, 20);
    glPopMatrix();
};