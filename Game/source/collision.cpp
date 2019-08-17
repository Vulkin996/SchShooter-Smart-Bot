#include "../header/collision.h"
#include "../header/util.h"
#include "../header/player.h"
#include "../header/bullet.h"
#include <iostream>
#include <Box2D/Box2D.h>
#include <typeinfo>

MyContactListener::MyContactListener(){}

//Detecting Collision when world objects begin contact and handling it
void MyContactListener::BeginContact(b2Contact* contact){

  b2Fixture* f1 = contact->GetFixtureA();
  b2Fixture* f2 = contact->GetFixtureB();

  b2Body* b1 = f1->GetBody();
  b2Body* b2 = f2->GetBody();

  void* o1 = b1->GetUserData();
  void* o2 = b2->GetUserData();

  Colider* c1 = NULL;
  Colider* c2 = NULL;

  if(o1 && o2){
    c1 = static_cast<Colider*>(o1);
    c2 = static_cast<Colider*>(o2);

    if(c1->getClassID() == PLAYER && c2->getClassID() == BULLET){
      Bullet* bullet = static_cast<Bullet*>(c2);
      bullet->toDelete = 1; // Mark bullet for deletion because it hit a player.
      int dmg = bullet->m_dmg;
      static_cast<Player*>(c1)->takeDmg(dmg, bullet->body->GetLinearVelocity());
    }

    if(c2->getClassID() == PLAYER && c1->getClassID() == BULLET){
      Bullet* bullet = static_cast<Bullet*>(c1);
      bullet->toDelete = 1; // Mark bullet for deletion because it hit a player.
      int dmg = bullet->m_dmg;
      static_cast<Player*>(c2)->takeDmg(dmg, bullet->body->GetLinearVelocity());
    }

		if(c1->getClassID() == BULLET && c2->getClassID() == BLOCK){
			static_cast<Bullet*>(c1)->toDelete = 1;
			static_cast<Bullet*>(c1)->StartSparkEffect();
		}

		if(c2->getClassID() == BULLET && c1->getClassID() == BLOCK){
			static_cast<Bullet*>(c2)->toDelete = 1;
			static_cast<Bullet*>(c2)->StartSparkEffect();
		}


  }
}
