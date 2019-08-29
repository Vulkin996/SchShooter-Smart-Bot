#include "floatfann.h"
#include "fann_cpp.h"

#ifndef TMP_NN
#define TMP_NN

class ANNPlayer{
public:
	ANNPlayer();
	ANNPlayer(std::string file);
	void MakeBaseNet();
	int timeAlive;
	int kills;
	int dmgDone;
	int numberOfConnections;
	void CalculateFitness();
	void ResetFitness();
	FANN::neural_net net;
	float m_fitness;
	float* GetChromosome();
	void SetChromosome(float* chromosome);
	float* GetOutput(float* input);
private:
	FANN::connection* tempConnections = NULL;
};

#endif
