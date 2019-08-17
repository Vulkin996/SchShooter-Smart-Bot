#include "../header/item.h"
#include "../header/player.h"
#include "../header/basicItems.h"
#include <vector>
#include <GL/glut.h>
#include <Box2D/Box2D.h>
#include <map>
#include <cstdlib>

extern std::map<std::string, int> textures;
extern int updateCount;


Item::Item(float x, float y, float pickupDistance, std::string icon): icon(icon), pickupDistance(pickupDistance) {
	itemPosition.Set(x, y);
}

Item::~Item(){

}

//Checking if item is colliding with a player
bool Item::IsColliding(Player *picker){
	b2Vec2 playerPos = picker->body->GetPosition();
	if (fabs(playerPos.x - itemPosition.x) < pickupDistance && fabs(playerPos.y - itemPosition.y) < pickupDistance) {
		return true;
	}
	return false;
};

void Item::Draw() {
	glColor4f(1, 1, 1, 1);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glNormal3f(0, 0, 1);
	glTranslatef(itemPosition.x, itemPosition.y, 0.2);

	glBindTexture(GL_TEXTURE_2D, textures[icon]);
	float animationScale = 0.05*sin(updateCount / 20.0);
	glScalef(pickupDistance+animationScale, pickupDistance+animationScale, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, -0);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, -0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, -0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, -0);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

};
//Getter
std::string Item::Name() {
	return icon;
};

ItemPool::ItemPool() {
	m_items = std::vector<Item*>();
}
//Getter
std::string Item::GetIcon() const{
	return icon;
}
//Activating item pickup and removing them from itempool
void ItemPool::CheckPickups(Player *picker) {
	for (int i = 0; i < (int)m_items.size(); i++) {
		if (m_items[i]->IsColliding(picker)) {
			m_items[i]->Pickup(picker);
			Remove(i);
			return;
		}
	}
}

ItemPool::~ItemPool(){
	for (std::vector<Item*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		if((*it)->GetIcon() == "pistol" || (*it)->GetIcon() == "rifle" || (*it)->GetIcon() == "shotgun" || (*it)->GetIcon() == "sniper")
			((Weapon*)(*it))->FreeSources();
		delete (*it);
	}
	m_items.clear();
}


void ItemPool::Add(Item *item) {
	m_items.push_back(item);
}
//Random item spawn
void ItemPool::SpawnRandom(b2Vec2 pos) {
	Item* item = NULL;
	float r = float(rand())/float(RAND_MAX);
	if(r <= 0.3) // 0.2 chance HP
		item = new HealthPotion(pos.x, pos.y, 20);
	else if(r <= 0.4) // 0.1 chance Armor
		item = new Armor(pos.x, pos.y);
	else if(r <= 0.55) // 0.15 chance Grenade
		item = new GrenadeItem(pos.x, pos.y);
	else if(r <= 0.7) // 0.2 chance Shoty
		item = new Shotgun(pos.x, pos.y, 0, 4);
	else if(r <= 0.85) // 0.2 chance Rifle
		item = new Rifle(pos.x, pos.y, 0);
	else if (r <= 0.9) // 0.05 chance Sniper
		item = new Sniper(pos.x, pos.y, 0);

	// 0.1 chance nothing
	if(item)
		Add(item);
}


void ItemPool::Remove(Item *item) {
	int n = m_items.size();
	for (int i = 0; i < n; i++) {
		if (m_items[i] == item) {
			m_items.erase(m_items.begin() + i);
			return;
		}
	}
}

void ItemPool::Remove(int index) {
	m_items.erase(m_items.begin() + index);
}

void ItemPool::DrawItems() {
	for (Item* item : m_items) {
		item->Draw();
	}
}
