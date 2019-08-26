#include "bot.h"

GeneticAlgorithm::GeneticAlgorithm(std::string target, std::string allowed_gene_values){
  m_target = target;
  target_size = target.length();
  m_allowed_gene_values = allowed_gene_values;
  iterations = 1000;
  generation_size = 5000;
  mutation_rate = 0.05;
  reproduction_size = 1000;
  current_iteration = 0;
  srand(time(0));
}



Chromosome* GeneticAlgorithm::optimize(){
  std::vector<Chromosome*> chromosomes = inintial_population();
  Chromosome* temp;
  std::vector<Chromosome*> temp_gen;
  std::vector<Chromosome*> for_reproduction;

  while(!stop_condition()){
    std::cout << "Iteration: " << current_iteration << std::endl;
    for_reproduction = selection(chromosomes);
    temp = (*std::max_element(chromosomes.cbegin(), chromosomes.cend(), [](auto a, auto b){return a->m_fitness < b->m_fitness;}));

    std::cout << "Top solution: " << temp->m_content <<  " " << temp->m_fitness << std::endl;

    temp_gen = create_generation(for_reproduction);
    for_reproduction.clear();
    for (std::vector<Chromosome*>::iterator ch = chromosomes.begin(); ch != chromosomes.end(); ++ch) {
        delete *ch;
    }

    chromosomes.clear();

    for(unsigned i = 0; i < generation_size; i++){
      chromosomes.push_back(temp_gen[i]);
    }
    temp_gen.clear();


    std::cout << std::endl;
    current_iteration += 1;
  }

  if (!(top_chromosome.empty())){
    return new Chromosome(top_chromosome,fitness(top_chromosome));
  }
  else {
    return (*std::max_element(chromosomes.cbegin(), chromosomes.cend(), [](auto a, auto b){return a->m_fitness < b->m_fitness;}));
  }
}




std::vector<Chromosome*> GeneticAlgorithm::create_generation(std::vector<Chromosome*>& for_reproduction){
  std::vector<Chromosome*> new_generation;
  while(new_generation.size() < generation_size){
    int parent1, parent2;

    parent1 = rand() % reproduction_size;
    parent2 = rand() % reproduction_size;

    std::pair<std::string,std::string> children = crossover(for_reproduction[parent1]->m_content, for_reproduction[parent2]->m_content);


    std::string child1 = mutation(children.first);
    std::string child2 = mutation(children.second);

    new_generation.push_back(new Chromosome(child1, fitness(child1)));
    new_generation.push_back(new Chromosome(child2, fitness(child2)));
  }

  return new_generation;
}




std::pair<std::string, std::string> GeneticAlgorithm::crossover(std::string parent1, std::string parent2){
  int cross_point = rand() % target_size;
  std::string ab = parent1.substr(0, cross_point) + parent2.substr(cross_point, target_size-cross_point);
  std::string ba = parent2.substr(0, cross_point) + parent1.substr(cross_point, target_size-cross_point);
  return std::make_pair(ab, ba);
}



std::string GeneticAlgorithm::mutation(std::string child){
  float t = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
  if (t < mutation_rate){
    int i = rand() % target_size;
    int j = rand() % m_allowed_gene_values.length();
    child[i] = m_allowed_gene_values[j];
  }
  return child;
}



std::vector<Chromosome*> GeneticAlgorithm::selection(std::vector<Chromosome*>& chromosomes){
  int the_sum = std::accumulate(chromosomes.cbegin(), chromosomes.cend(), 0, [](int a, Chromosome* b){return a + b->m_fitness;});
  std::vector<Chromosome*> selected;
  for(int i = 0; i < reproduction_size; ++i){
    selected.push_back(selection_roulette_pick_one(chromosomes, the_sum));
  }
  return selected;
}




Chromosome* GeneticAlgorithm::selection_roulette_pick_one(std::vector<Chromosome*>& chromosomes, int the_sum){
  float pick = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/(the_sum*1.0)));

  int value = 0;
  for(auto ch : chromosomes){
    value += ch->m_fitness;
    if(value > pick){
      return ch;
    }
  }

  return chromosomes[generation_size-1];
}





int GeneticAlgorithm::fitness(std::string& content){
  int fit = std::inner_product(content.cbegin(), content.cend(), m_target.cbegin(), 0, [](int a, int b){return a + b;}, [](char a, char b){return a==b;});
  if(content == m_target){
    std::cout << "Found solution at iteration: " << current_iteration << std::endl;
    top_chromosome = content;
  }

  return fit;
}




std::vector<Chromosome*> GeneticAlgorithm::inintial_population(){
  std::vector<Chromosome*> init_pop;
  std::string tmp;
  std::vector<char> temp;
  int r;
  for(unsigned i = 0; i < generation_size; ++i){
    for(int j = 0; j < target_size; ++j){
     r = (rand() % m_allowed_gene_values.length());
     temp.push_back(m_allowed_gene_values[r]);
    }
    tmp = std::accumulate(temp.begin(), temp.end(), std::string(), [](std::string& a, char b){return a + b;});
    init_pop.push_back(new Chromosome(tmp, fitness(tmp)));
    tmp.clear();
    std::vector<char>().swap(temp);
  }
  return init_pop;
}

bool GeneticAlgorithm::stop_condition(){
  return current_iteration > iterations or !top_chromosome.empty();
}

Chromosome::Chromosome(std::string content, float fitness){
  m_content = content;
  m_fitness = fitness;
}



int main(int argc, const char *argv[]){
    GeneticAlgorithm genetic = GeneticAlgorithm("helloworlasdaasdasdasdasdasasdfasasassdasddasd", "abcdefghijklmnopqrstuvwxyz");
    Chromosome* solution = genetic.optimize();
    std::cout << solution->m_content << " " << solution->m_fitness << std::endl;
    return 0;
}
