#include "../header/geometry.h"


extern std::vector<Line> walls;
// m_x = m_x/855*18-9;
// m_y = -m_y/855*18.4+9.2;

//Definicije konstruktora i metoda za klasu Line
Line::Line(b2Vec2 A, b2Vec2 B)
{
    m_A = A;
    m_B = B;
    len = sqrt(pow(B.x-A.x, 2)+ pow(B.y-A.y, 2));
}



void ScaleVec(b2Vec2 * A){
    A->x = A->x/885*18.4 - 9.2;
    A->y = -A->y/885*18.4 +9.2;
}

// Vector2D Line::A() const
// {
//     return m_A;
// }
//
// void Line::setA(const Vector2D & A)
// {
//     m_A = A;
// }
//
// Vector2D Line::B() const
// {
//     return m_B;
// }
//
// void Line::setB(const Vector2D & B)
// {
//     m_B = B;
// }

void LoadWalls()
{

    std::string lineFile;
    b2Vec2 A;
    b2Vec2 B;
    b2Vec2 C;
    char c;
    std::ifstream myfile ("walls.txt");
    if (myfile.is_open()){
        while(getline(myfile,lineFile)){
            if(lineFile.compare("\n") == 0){
                return;
            }
            std::stringstream ss;
            ss << lineFile;
            ss >> A.x >> c >> A.y >> c >> B.x >> c >> B.y;
            ScaleVec(&A);
            ScaleVec(&B);
            walls.push_back(Line(A,B));

        }
    }

    myfile.close();
    int n = walls.size();
    int i;
    for(i=0;i<n;i++){
        C.x = (walls[i].m_A.x+walls[i].m_B.x)/2+0.2;
        C.y = (walls[i].m_A.y+walls[i].m_B.y)/2;
        if(walls[i].m_A.x == walls[i].m_B.x){
            AddWall(C.x, C.y, 0.15, walls[i].len/2);
        }
        else{
            AddWall(C.x, C.y, walls[i].len/2,0.15);
        }
    }
}

void AddWall(float x, float y, float w, float h){
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(x, y);
    b2Body* groundBody = world->CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(w, h);
    groundBody->CreateFixture(&groundBox, 0.0f);
}

// void AddOuterWalls(){
//     AddWall(0, 10.2, 9, 1);
//     AddWall(0, -10.2, 9, 1);
//     AddWall(10, 0, 1, 9);
//     AddWall(-10, 0, 1, 9);
// }


// int main(int argc, const char *argv[])
// {
//
//     LoadWalls();
//     int n = walls.size();
//     int i;
//     for(i=0;i<n;i++){
//         std::cout << walls[i].m_A.x << " " << walls[i].m_A.y << "    " << walls[i].m_B.x << " " << walls[i].m_B.y << "         " << walls[i].len<< std::endl;
//     }
//
//
//     return 0;
// }
