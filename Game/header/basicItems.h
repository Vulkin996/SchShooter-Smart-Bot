#ifndef BASIC_ITEMS_DEF
#define BASIC_ITEMS_DEF

#include <math.h>
#include <Box2D/Box2D.h>
#include <vector>
#include <map>
#include "item.h"

extern std::vector<AudioWrapper*> audioWrappers;

class HealthPotion : public Item {
public:
	HealthPotion(float x, float y, int amount) :
		Item(x, y, 0.2, "healthPotion"), m_amount(amount) {
			itemSounder = new AudioWrapper(x, y, std::string("heal"));
			audioWrappers.push_back(itemSounder);
		}
	void Pickup(Player *picker) override {
		picker->IncreaseHealth(m_amount);
		itemSounder->playSound();
		itemSounder->toDelete = true;
		delete this;
	}
private:
	int m_amount;
	AudioWrapper* itemSounder;
};

class Armor : public Item {
public:
	Armor(float x, float y) :Item(x, y, 0.2, "armor"){
			itemSounder = new AudioWrapper(x, y, std::string("armor"));
			audioWrappers.push_back(itemSounder);
		}
	void Pickup(Player *picker) override {
		picker->FillArmor();
		itemSounder->playSound();
		itemSounder->toDelete = true;
		delete this;
	}
private:
	int m_amount;
	AudioWrapper* itemSounder;
};

class GrenadeItem : public Item {
public:
	//NEED TO PUT GRENADE SPRITE HERE!!
	GrenadeItem(float x, float y) :Item(x, y, 0.2, "grenade"){
		itemSounder = new AudioWrapper(x, y, std::string("grenadePickup"));
		audioWrappers.push_back(itemSounder);
	}
	void Pickup(Player *picker) override {
		picker->IncreaseGrenades(1);
		itemSounder->playSound();
		itemSounder->toDelete = true;
		delete this;
	}
private:
	AudioWrapper* itemSounder;
};

#endif
