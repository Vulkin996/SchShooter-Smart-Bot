#include "../header/geometry.h"
#include "../header/util.h"

std::vector< std::vector<char> > map;
extern std::vector<Block*> walls;
extern std::vector<b2Vec2> spawnPositions;
extern std::map<std::string, int> textures;





/*            3'
 *             ######## 2'
 *           # #    # #
 *       0'######## 1'#
 *         #   #  #   #
 *         # 3 #  #   # 2
 *         # #    # #
 *         ########
 *        0       1
 *
 *
 */



//Constructor and method definitions for Block class
Block::Block(b2Vec2 A, double edge)
{
    m_A = A;
    m_edge = edge;
    m_vertexes[0] = b2Vec2(m_A.x-edge/2,m_A.y-edge/2);
    m_vertexes[1] = b2Vec2(m_A.x+edge/2,m_A.y-edge/2);
    m_vertexes[2] = b2Vec2(m_A.x+edge/2,m_A.y+edge/2);
    m_vertexes[3] = b2Vec2(m_A.x-edge/2,m_A.y+edge/2);
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(m_A.x, m_A.y);
    m_body = world->CreateBody(&groundBodyDef);
    m_body->SetUserData(this);

    b2PolygonShape groundBox;
    groundBox.SetAsBox(m_edge/2, m_edge/2);
    m_body->CreateFixture(&groundBox, 0.0f);
}




void ScaleVec(b2Vec2 * A){
    A->x = A->x - 9;
    A->y = -A->y + 9;
}

//Loading map from file
void LoadWalls()
{
    std::string lineFile;
    b2Vec2 A;
    std::ifstream myfile ("walls.txt");
    int n = 0;
    int i;
    int j;
    std::vector<char> line;
    double edge = 0;
     if (myfile.is_open()){
        while(getline(myfile,lineFile)){
            n = lineFile.length();
            for(i=0;i<n;i++){
                line.push_back(lineFile[i]);
            }
            map.push_back(line);
            line.clear();
			line.shrink_to_fit();

        }
    }
    edge = 18.0/n;


     for(i=0;i<n;i++){
            for(j=0;j<n;j++){
                A.x = j*edge+edge/2;
                A.y = i*edge+edge/2;
                ScaleVec(&A);
                if(map[i][j] == '#'){
                    walls.push_back(new Block(A, edge));
                }
				else if (map[i][j] == 'S') {
					map[i][j] = ' ';
					spawnPositions.push_back(A);
				}

            }
    }

    myfile.close();
}



void DrawWalls(){
    int n = walls.size();
    int i;
    for(i=0;i<n;i++){
        glPushMatrix();
            glColor3f(1,1,1);
            glBindTexture(GL_TEXTURE_2D, textures["wall3"]);
            glBegin(GL_QUADS);
                //front quad
                glNormal3f(0, -1, 0);
                glTexCoord2f(0,0);
                glVertex3f(walls[i]->m_vertexes[0].x, walls[i]->m_vertexes[0].y, 0);
                glTexCoord2f(1,0);
                glVertex3f(walls[i]->m_vertexes[1].x, walls[i]->m_vertexes[1].y, 0);
                glTexCoord2f(1,1);
                glVertex3f(walls[i]->m_vertexes[1].x, walls[i]->m_vertexes[1].y, walls[i]->m_edge);
                glTexCoord2f(0,1);
                glVertex3f(walls[i]->m_vertexes[0].x, walls[i]->m_vertexes[0].y, walls[i]->m_edge);
                //right quad
                glNormal3f(1, 0, 0);
                glTexCoord2f(0,0);
                glVertex3f(walls[i]->m_vertexes[1].x, walls[i]->m_vertexes[1].y, 0);
                glTexCoord2f(1,0);
                glVertex3f(walls[i]->m_vertexes[2].x, walls[i]->m_vertexes[2].y, 0);
                glTexCoord2f(1,1);
                glVertex3f(walls[i]->m_vertexes[2].x, walls[i]->m_vertexes[2].y, walls[i]->m_edge);
                glTexCoord2f(0,1);
                glVertex3f(walls[i]->m_vertexes[1].x, walls[i]->m_vertexes[1].y, walls[i]->m_edge);
                //left quad
                glNormal3f(-1, 0, 0);
                glTexCoord2f(0,0);
                glVertex3f(walls[i]->m_vertexes[3].x, walls[i]->m_vertexes[3].y, 0);
                glTexCoord2f(1,0);
                glVertex3f(walls[i]->m_vertexes[0].x, walls[i]->m_vertexes[0].y, 0);
                glTexCoord2f(1,1);
                glVertex3f(walls[i]->m_vertexes[0].x, walls[i]->m_vertexes[0].y, walls[i]->m_edge);
                glTexCoord2f(0,1);
                glVertex3f(walls[i]->m_vertexes[3].x, walls[i]->m_vertexes[3].y, walls[i]->m_edge);
                //back quad
                glNormal3f(0, 1, 0);
                glTexCoord2f(0,0);
                glVertex3f(walls[i]->m_vertexes[2].x, walls[i]->m_vertexes[2].y, 0);
                glTexCoord2f(1,0);
                glVertex3f(walls[i]->m_vertexes[3].x, walls[i]->m_vertexes[3].y, 0);
                glTexCoord2f(1,1);
                glVertex3f(walls[i]->m_vertexes[3].x, walls[i]->m_vertexes[3].y, walls[i]->m_edge);
                glTexCoord2f(0,1);
                glVertex3f(walls[i]->m_vertexes[2].x, walls[i]->m_vertexes[2].y, walls[i]->m_edge);
                //top quad
                glNormal3f(0, 0, 1);
                glTexCoord2f(0,0);
                glVertex3f(walls[i]->m_vertexes[0].x, walls[i]->m_vertexes[0].y, walls[i]->m_edge);
                glTexCoord2f(1,0);
                glVertex3f(walls[i]->m_vertexes[1].x, walls[i]->m_vertexes[1].y, walls[i]->m_edge);
                glTexCoord2f(1,1);
                glVertex3f(walls[i]->m_vertexes[2].x, walls[i]->m_vertexes[2].y, walls[i]->m_edge);
                glTexCoord2f(0,1);
                glVertex3f(walls[i]->m_vertexes[3].x, walls[i]->m_vertexes[3].y, walls[i]->m_edge);
            glEnd();
            glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
    }
}


ClassID Block::getClassID(){return BLOCK;}
