#include "../header/annPlayer.h"
#include <iostream>
const unsigned int num_layers = 3;
const unsigned int num_input = 10;
const unsigned int num_hidden = 15;
const unsigned int num_output = 5;

extern const float randomWeigtRange = 10;

ANNPlayer::ANNPlayer(){
  MakeBaseNet();
  printf("Napravio basic mrezu\n");
  std::cout << numberOfConnections << std::endl;
};

ANNPlayer::ANNPlayer(std::string file){
  if (!net.create_from_file(file)){
    MakeBaseNet();
    printf("Napravio basic mrezu\n");

  }
  else{
    numberOfConnections = net.get_total_connections();
    ResetFitness();
    printf("Ucitao mrezu\n");
  }
};

void ANNPlayer::MakeBaseNet(){
  net.create_standard(num_layers, num_input, num_hidden, num_output);
  net.randomize_weights(-randomWeigtRange, randomWeigtRange);
  net.set_activation_steepness_output(0.1);
  numberOfConnections = net.get_total_connections();

  ResetFitness();
}

void ANNPlayer::CalculateFitness(){
  //const float p_t = 0.05;
  const float p_k = 50.0;
  const float p_d = 0.5;
  m_fitness = /*timeAlive*p_t + */kills*p_k + dmgDone*p_d;
};

void ANNPlayer::ResetFitness(){
  timeAlive = 0;
  kills = 0;
  dmgDone = 0;
  m_fitness = -1;

  if (tempConnections == NULL){
    tempConnections = new FANN::connection[numberOfConnections];
    GetChromosome();
  }

};

float* ANNPlayer::GetChromosome(){
  net.get_connection_array(tempConnections);
  float* out = new float[numberOfConnections];

  for (int i=0; i<numberOfConnections; i++){
    out[i] = tempConnections[i].weight;
  }
  return out;
};

void ANNPlayer::SetChromosome(float* chromosome){
  net.get_connection_array(tempConnections);

  for (int i=0; i<numberOfConnections; i++){
    tempConnections[i].weight = chromosome[i];
  }

  net.set_weight_array(tempConnections, numberOfConnections);
};


float* ANNPlayer::GetOutput(float* input){
  return net.run(input);
}
