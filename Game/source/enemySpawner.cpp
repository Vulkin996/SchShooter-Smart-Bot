#include "../header/item.h"
#include "../header/player.h"
#include "../header/enemySpawner.h"
#include "../header/particleSystem.h"
#include <Box2D/Box2D.h>
#include <vector>
#include <map>
#include <iostream>
#include <time.h>


extern double phisycsUpdateInterval;

EnemySpawner::EnemySpawner(std::vector<Player*> &players, std::vector<b2Vec2> &spawnPositions):
players(players), spawnPositions(spawnPositions){
	Reset();
}

int enemyWaveIncrement = 2;

void EnemySpawner::Reset() {
	currentWave = 0;
	enemiesInWave = 4;
	enemiesSpawned = 0;
	totalEnemiesSpawned = 0;
	spawnRate = 5; //Seconds
	spawnTimer = 0;
}

void WaweClearedEffect(b2Vec2 pos) {
	Emitter* shockwave = new Emitter(pos, b2Vec2(0, 0), b2Vec2(0, 0), 1, 2, "waveCleared");
	shockwave->SetScale(2, 1);
	shockwave->SetSpeed(0, 0);
	shockwave->SetStartRotation(0);
	shockwave->SetScaleTween(TW_INVERSE_CUBIC);
	shockwave->SetAlphaTween(TW_CUBIC);
	shockwave->Start();
}

void EnemySpawner::Update() {
	spawnTimer += phisycsUpdateInterval;

	if (enemiesSpawned < enemiesInWave && spawnTimer > spawnRate) {
		Spawn();
		return;
	}

	if (IsWaveCleared()) {
		WaweClearedEffect(players[0]->body->GetPosition());
		StartNextWave();
	}

}

//Getters
int EnemySpawner::GetEnemiesInWave() const{
	return enemiesInWave;
}
int EnemySpawner::GetEnemiesSpawned() const{
	return enemiesSpawned;
}
int EnemySpawner::GetCurrentWave() const{
	return currentWave;
}

bool EnemySpawner::IsWaveCleared() {
	int numberOfAliveBots = 0;
	for (unsigned i = 1; i < players.size(); i++) {
		if (players[i]->alive)
			numberOfAliveBots++;
	}
	return (numberOfAliveBots == 0 && enemiesSpawned == enemiesInWave);
}

void EnemySpawner::Spawn() {
	spawnTimer = 0;
	for (unsigned i = 1; i < players.size(); i++) {
		if (!players[i]->alive) {
			players[i]->Revive();
			b2Vec2 spawnPosition = GetNextSpawnPosition();
			players[i]->body->SetTransform(spawnPosition, 0);

			enemiesSpawned++;
			totalEnemiesSpawned++;
			return;
		}
	}
}

//Randomizing bot spawn position
b2Vec2 EnemySpawner::GetNextSpawnPosition() {
	while (true) {
		int index = rand() % spawnPositions.size();
		if (!IsOnScreen(spawnPositions[index])) {
			return spawnPositions[index];
		}
	}
}

void EnemySpawner::StartNextWave() {
	currentWave++;
	enemiesInWave += enemyWaveIncrement;
	enemiesSpawned = 0;
	spawnTimer = 0;
}
