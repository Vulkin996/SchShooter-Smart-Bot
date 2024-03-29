#include "../header/genetic.h"
#include "../header/annPlayer.h"
#include <limits.h>


// extern const float randomWeigtRange;
extern const float randomWeigtRange;

GeneticAlgorithm::GeneticAlgorithm(){
  iterations = 5;
  generation_size = 50;
  mutation_rate = 0.05;
  reproduction_size = 10;
  current_iteration = 0;
  srand(time(0));

}

void GeneticAlgorithm::Init(int a_target_size){
  target_size = a_target_size;
  float* v = new float[target_size];
  for(int i = 0; i < target_size; i++){
      v[i] = 0;
  }
  top_chromosome = new Chromosome(v, 0);

  chromosomes = inintial_population();
}

void GeneticAlgorithm::setIterations(int iter){
	iterations = iter;
}





std::vector<Chromosome*> GeneticAlgorithm::create_generation(std::vector<Chromosome*>& for_reproduction){
  std::vector<Chromosome*> new_generation;
  while(new_generation.size() < generation_size){
    int parent1, parent2;

    parent1 = rand() % reproduction_size;
    parent2 = rand() % reproduction_size;

    std::pair<float*,float*> children = crossover(for_reproduction[parent1]->m_content, for_reproduction[parent2]->m_content);


    mutation(children.first);
    mutation(children.second);

    new_generation.push_back(new Chromosome(children.first, 0));
    new_generation.push_back(new Chromosome(children.second, 0));
  }

  return new_generation;
}




std::pair<float*, float*> GeneticAlgorithm::crossover(float* parent1, float* parent2){
  float* child1 = new float[target_size];
  float* child2 = new float[target_size];
  for(int i = 0; i < target_size; ++i){
    float shouldCross = ((float)rand()) / INT_MAX;
    if(shouldCross < 0.5){
        child1[i] = parent1[i];
        child2[i] = parent2[i];
    }
    else{
        child1[i] = parent2[i];
        child2[i] = parent1[i];
    }
  }
  return std::make_pair(child1, child2);
}



void GeneticAlgorithm::mutation(float* child){
  for (int i=0; i<target_size; i++){
    float t = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
    if (t < mutation_rate){
      float j = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(2*randomWeigtRange))) - 10;
      child[i] = j;
    }
  }
}



std::vector<Chromosome*> GeneticAlgorithm::selection(std::vector<Chromosome*>& chromosomes){
  float the_sum = std::accumulate(chromosomes.cbegin(), chromosomes.cend(), 0, [](int a, Chromosome* b){return a + b->m_fitness;});
  std::vector<Chromosome*> selected;
  for(int i = 0; i < reproduction_size; ++i){
    selected.push_back(selection_roulette_pick_one(chromosomes, the_sum));
  }
  return selected;
}




Chromosome* GeneticAlgorithm::selection_roulette_pick_one(std::vector<Chromosome*>& chromosomes, float the_sum){
  float pick = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(the_sum)));

  float value = 0;
  for(auto ch : chromosomes){
    value += ch->m_fitness;
    if(value > pick){
      return ch;
    }
  }

  return chromosomes[generation_size-1];
}








std::vector<Chromosome*> GeneticAlgorithm::inintial_population(){
    std::vector<Chromosome*> init_pop;
    float r;

    for(unsigned i = 0; i < generation_size; ++i){
        float* weights = new float[target_size];
        for(int j = 0; j < target_size; ++j){
            r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(2*randomWeigtRange))) - 10;
            weights[j] = r;
        }

        init_pop.push_back(new Chromosome(weights, 0));
    }

    return init_pop;
}

bool GeneticAlgorithm::stop_condition(){
  return current_iteration > iterations;
}

Chromosome::Chromosome(float* content, float fitness){
  m_content = content;
  m_fitness = fitness;
}

Chromosome::Chromosome(Chromosome* ch, int size){
  m_content = new float[size];
  for(int i = 0; i < size; i++){
      m_content[i] = ch->m_content[i];
  }
  m_fitness = ch->m_fitness;
}
