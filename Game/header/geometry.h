#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../header/util.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Box2D/Box2D.h>
#include <cmath>
#include <GL/glut.h>



// Block structure
struct Block : public Colider{
    Block(b2Vec2 A,double edge);
    b2Vec2 m_A;
    double m_edge;
    b2Vec2 m_vertexes[4];
    b2Body* m_body;

    virtual ClassID getClassID();
};




extern b2World* world;

//Loading map from file
void LoadWalls();
b2Vec2 ScaleVec(const b2Vec2 & A);
void DrawWalls();



#endif
