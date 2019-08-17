#ifndef ENEMY_SPAWNER_DEF
#define ENEMY_SPAWNER_DEF

#include <Box2D/Box2D.h>
#include <vector>

class Player;

class EnemySpawner {
public:
	EnemySpawner(std::vector<Player*> &players, std::vector<b2Vec2> &spawnPositions);
	void Update();
	int GetEnemiesInWave() const;
	int GetEnemiesSpawned() const;
	int GetCurrentWave() const;
	void Reset();

private:
	void Spawn();
	bool IsWaveCleared();
	b2Vec2 GetNextSpawnPosition();
	void StartNextWave();

	std::vector<Player*> players;
	std::vector<b2Vec2> &spawnPositions;
	int currentWave;
	int enemiesInWave;
	int enemiesSpawned;
	int totalEnemiesSpawned;
	float spawnRate;
	float spawnTimer;
};

#endif
