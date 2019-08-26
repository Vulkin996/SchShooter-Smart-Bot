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
  Chromosome(std::string content, float fitness);
  std::string m_content;
  float m_fitness;
};

class GeneticAlgorithm {
public:
  GeneticAlgorithm(std::string target, std::string allowed_gene_values);
  Chromosome* optimize();
  std::string mutation(std::string child);
  std::vector<Chromosome*> inintial_population();
  std::vector<Chromosome*> selection(std::vector<Chromosome*>& chromosomes);
  std::vector<Chromosome*> create_generation(std::vector<Chromosome*>& chromosomes);
  std::pair<std::string, std::string> crossover(std::string parent1, std::string parent2);
  Chromosome* selection_roulette_pick_one(std::vector<Chromosome*>& chromosomes, int sum);
  int fitness(std::string& content);
  bool stop_condition();

private:
  std::string m_target;
  std::string m_allowed_gene_values;
  int target_size;
  int iterations;
  unsigned generation_size;
  float mutation_rate;
  int reproduction_size;
  int current_iteration;
  std::string top_chromosome;
};




#endif
