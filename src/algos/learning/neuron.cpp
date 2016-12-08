/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Neuron implementation file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//in-project headers
#include <algos/learning/neuron.hpp>
#include <miscs/utils.hpp>

//standard c++ namespaces
using namespace std;

//in-project namespaces
using namespace Algos::Learning;
using namespace Miscs;

/**
 * Constructor 
 */
neuron::neuron(long Input_Count) {
  for (long Index=0; Index<Input_Count; Index++) {
    this->Inputs.push_back(0);
    this->Weights.push_back(0);
    this->Changes.push_back(0);
  }
}

/**
 * Destructor 
 */
neuron::~neuron() {
}

/**
 * Set inputs
 */
void neuron::set_inputs(vector<double> Inputs) {
  for (long Index=0; Index<this->Inputs.size(); Index++)
    this->Inputs[Index] = Inputs[Index];
}

/**
 * Randomise weights (between -0.25 to 0.25)
 */
void neuron::randomise_weights() {
  for (long Index=0; Index<this->Weights.size(); Index++)
    this->Weights[Index] = (utils::random()/2) - 0.25;
}

/**
 * Compute output of neuron
 */
void neuron::compute_output() {
  this->Output = utils::sigmoid(utils::scalar_product(
    this->Inputs,this->Weights
  ));
}

/**
 * Get output of neuron
 */
double neuron::get_output() {
  return this->Output;
}

//end of file