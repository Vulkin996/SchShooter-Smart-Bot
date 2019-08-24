#include <iostream>
#include "floatfann.h"
#include "fann_cpp.h"
#include "time.h"

const unsigned int num_layers = 4;
const unsigned int num_input = 2;
const unsigned int num_hidden = 3;
const unsigned int num_output = 2;

int main(){
  //------------------------ INICIJALIZACIJA SVAKE MREZE -------------------------------
  srand(time(NULL));
  FANN::neural_net net;

  //------------------------ MOZEMO DA NASTAVIMO TRENING AKO SMO SACUVALI MREZU U FAJL  -------------------------------
  if (!net.create_from_file("test_net.txt")){
    //AKO NE PRAVIMO NOVU SA RANDOM TEZINAMA
    net.create_standard(num_layers, num_input, num_hidden, num_hidden, num_output);
    net.randomize_weights(-10.2, 10.1);
  }
  net.print_connections();
  //------------------------------------------------------------------------------------

  //----------------- RACUNANJE OUTPUT-A MREZE NA OSNOVU INPUT-A -------------------
  float* input = new float[2]{0.4, 1};
  float* output = net.run(input);
  for (int i=0; i<num_output; i++){
    std::cout << "Output " << i << " " << output[i] << std::endl;
  }
  //--------------------------------------------------------------------------------


  int n = net.get_total_connections();
  FANN::connection* connections = (FANN::connection*)malloc(sizeof(struct fann_connection) * n);
  //------------------------------ UKRSTANJE I MUTACIJA ----------------------------
  net.get_connection_array(connections);
  for (int i=0; i<n; i++){
    //Ovde bismo imali dva niza connections-a i onda bismo ih ukrstali (samo weights) ili mutirali
    //std::cout << "From " << connections[i].from_neuron << " To " << connections[i].to_neuron << " Weight " << connections[i].weight << std::endl;
    connections[i].weight = -connections[i].weight;
  }
  net.set_weight_array(connections, n);
  //---------------------------------------------------------------------------------

  //---------- CUVANJE MREZE U FAJL ZA DALJE UPOTREBU U KONACNOJ IGRI ILI ZA NASTAVLJANJE TRENINGA -------
  net.save("test_net.txt");
  //-------------------------------------
  return 0;
}
