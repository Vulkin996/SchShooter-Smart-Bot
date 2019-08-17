#ifndef COLLISION_H
#define COLLISION_H

#include <Box2D/Box2D.h>

class MyContactListener : public b2ContactListener {
public:
  MyContactListener();
  void BeginContact(b2Contact* contact);
};

#endif
