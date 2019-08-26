#include "../header/util.h"
#include "../header/player.h"


extern std::vector<Player*> players;
extern float windowWidth, windowHeight;

bool IsOnScreen(b2Vec2 position) {
	float x1, x2;
	float y1, y2;
	float h, w;
	x1 = players[0]->body->GetPosition().x;
	y1 = players[0]->body->GetPosition().y;

	x2 = position.x;
	y2 = position.y;
	h = tan(30 * M_PI / 180) * 4;
	w = h * windowWidth / windowHeight;

	if (!(x2 > x1 + w || x2 < x1 - w || y2 > y1 + h || y2 < y1 - h))
		return true;
	else
		return false;
}

float randomNumber(float start, float end) {
	return (float(rand()) / float(RAND_MAX))*(end - start) + start;
}

float lerp(float start, float end, float t) {
	return t * end + (1 - t)*start;
}

float fitness(int kills, double time_alive){
	const float p_t = 1.0;
	const float p_k = 1.0;
	float fitness = time_alive*p_t + kills*p_k;
	return fitness;
}
