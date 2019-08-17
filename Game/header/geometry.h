#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <Box2D/Box2D.h>
#include <cmath>


// Klasa Duz
struct Line{
// public:
    Line(b2Vec2 A, b2Vec2 B);

    // Vector2D A() const;
    // void setA(const Vector2D & A);
    //
    // Vector2D B() const;
    // void setB(const Vector2D & B);

    double len;
// private:
    b2Vec2 m_A;
    b2Vec2 m_B;
};


extern b2World* world;

//Funkcija koja ucitava zidove iz fajla
void LoadWalls();
b2Vec2 ScaleVec(const b2Vec2 & A);
void AddWall(float x, float y, float w, float h);




#endif
