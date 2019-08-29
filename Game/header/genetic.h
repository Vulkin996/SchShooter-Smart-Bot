#ifndef BOT_DEF
#define BOT_DEF

#include <vector>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <numeric>
#include <algorithm>

class Chromosome{
public:
  Chromosome(float* content, float fitness);
  Chromosome(Chromosome* ch, int size);
  float* m_content;
  float m_fitness;
};

class GeneticAlgorithm {
public:
  GeneticAlgorithm();
  Chromosome* optimize();
  void Init(int a_target_size);
  void mutation(float* child);
  std::vector<Chromosome*> inintial_population();
  std::vector<Chromosome*> selection(std::vector<Chromosome*>& chromosomes);
  std::vector<Chromosome*> create_generation(std::vector<Chromosome*>& chromosomes);
  std::pair<float*, float*> crossover(float* parent1, float* parent2);
  Chromosome* selection_roulette_pick_one(std::vector<Chromosome*>& chromosomes, float sum);
  float fitness(float* content);
  bool stop_condition();

	void setIterations(int iter);

  std::vector<Chromosome*> temp_gen;
  std::vector<Chromosome*> for_reproduction;
  std::vector<Chromosome*> chromosomes;


  int target_size;
  int iterations;
  unsigned generation_size;
  float mutation_rate;
  int reproduction_size;
  int current_iteration;
  Chromosome* top_chromosome;
};




#endif
