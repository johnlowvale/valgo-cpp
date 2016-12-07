/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Neural network implementation file
 * @version 0.1
 * @author  Dat Dinhquoc
 * 
 * Reference:
 * http://www.labri.fr/perso/nrougier/downloads/mlp.py
 */

//standard c++ headers
#include <cmath>

//in-project headers
#include <algos/learning/neunet.hpp>

//in-project namespaces
using namespace Algos::Learning;

/**
 * Constructor
 * @param Neuron_Nums Number of neurons in each layer
 */
neunet::neunet(vector<long> Neuron_Nums) {
  long Layer_Count = Neuron_Nums.size();

  //build outputs of first layer (with 1 bias entry)
  //first layer holds inputs only, ie. outputs==inputs
  vector<double> Layer0_Outputs;
  for (long Index=0; Index<Neuron_Nums[0]; Index++)
    Layer0_Outputs.push_back(1);

  //bias value on first layer
  Layer0_Outputs.push_back(1);
  this->Outputs.push_back(Layer0_Outputs);

  //build outputs of middle layers to last layer
  for (long Index=1; Index<Layer_Count; Index++) {
    vector<double> LayerIndex_Outputs;

    for (long Jndex=0; Jndex<Neuron_Nums[Index]; Jndex++)
      LayerIndex_Outputs.push_back(1);

    this->Outputs.push_back(LayerIndex_Outputs);
  }

  //build weights: each neuron has number of inputs equal to number of 
  //neurons in the previous layer, thus the same number of weights
  //layer loop:
  this->Weights.push_back([]);
  for (long Index=1; Index<Layer_Count; Index++) {
    vector<vector<double>> LayerIndex_Weights;

    //neuron loop:
    for (long Jndex=0; Jndex<Neuron_Nums[Index]; Jndex++) {
      vector<double> LayerIndex_NeuJndex_Weights;

      //input loop:
      for (long Kndex=0; Kndex<Neuron_Nums[Index-1]; Kndex++)
        LayerIndex_NeuJndex_Weights.push_back(0);

      LayerIndex_Weights.push_back(LayerIndex_NeuJndex_Weights);
    }//jndex

    this->Weights.push_back(LayerIndex_Weights);
  }//index

  //weight deltas (for momentum)
  for (long Index=0; Index<Layer_Count; Index++)
    this->Wds.push_back(0);

  //initialise weights
  this->randomise_weights();
}//constructor

/**
 * Destructor
 */
neunet::~neunet() {
}

/**
 * Sigmoid function for classification (using tanh)
 */
double neunet::sigmoid(double X) {
  return tanh(X);
}

/**
 * Derivative of the sigmoid function being used
 */
double neunet::dsigmoid(double X) {
  return (double)1-(X*X);
}

/**
 * Randomise all weights
 */
void neunet::randomise_weights() {

  //layer loop:
  for (long Index=1; Index<Layer_Count; Index++) {

    //neuron loop:
    for (long Jndex=0; Jndex<Neuron_Nums[Index]; Jndex++) {

      //input loop:
      for (long Kndex=0; Kndex<Neuron_Nums[Index-1]; Kndex++)
        this->Weights[Index][Jndex][Kndex] = (utils::random()/2) - 0.25;
    }//jndex
  }//index
}

/**
 * Scalar product (dot product) 
 * Params 'Left' and 'Right' supposed to have the same number of values
 */
double neunet::scalar_product(vector<double> Left,vector<double> Right) {
  double Scalar_Product = 0;

  for (long Index=0; Index<Left.size(); Index++)
    Scalar_Product += Left[Index]*Right[Index];

  return Scalar_Product;
}

/**
 * Propagate forwards
 * @return Last layer output
 */
vector<double> neunet::propagate_forwards(vector<double> One_Sample) {
  long Layer_Count = this->Outputs.size();
  
  //set first layer
  this->Outputs[0] = One_Sample;

  //propagate forwards using sigmoid as activation function
  //layer loop:
  for (long Index=1; Index<Layer_Count; Index++) {

    //neuron loop:
    for (long Jndex=0; Jndex<this->Outputs[Index].size(); Jndex++) {
      this->Outputs[Index][Jndex] = this->sigmoid(
        this->scalar_product(this->Outputs[Index-1],this->Weights[Index][Jndex])
      );
    }//jndex
  }//index

  return this->Outputs[Layer_Count-1];
}

/**
 * Propagate backwards
 * @return Error value compared to expected result
 */
double neunet::propagate_backwards(double Expected_Result,
double Learning_Rate,double Momentum) {
  long Layer_Count = this->Outputs.size();

  //weight changes
  ??? ???
}

//end of file