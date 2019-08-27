#include "../header/annPlayer.h"

const unsigned int num_layers = 3;
const unsigned int num_input = 190;
const unsigned int num_hidden = 10;
const unsigned int num_output = 4;

extern const float randomWeigtRange = 10;

ANNPlayer::ANNPlayer(){
  net.create_standard(num_layers, num_input, num_hidden, num_output);
  net.randomize_weights(-randomWeigtRange, randomWeigtRange);
  numberOfConnections = net.get_total_connections();
  ResetFitness();
};

ANNPlayer::ANNPlayer(std::string file){
  if (!net.create_from_file(file)){
    ANNPlayer();
  }
  else{
    numberOfConnections = net.get_total_connections();
    ResetFitness();
  }
};

void ANNPlayer::CalculateFitness(){
  const float p_t = 0.05;
  const float p_k = 50.0;
  const float p_d = 0.5;
  m_fitness = timeAlive*p_t + kills*p_k + dmgDone*p_d;
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
