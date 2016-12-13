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

//in-project headers
#include <algos/learning/neunet.hpp>
#include <algos/learning/neuron.hpp>

//standard c++ headers
#include <vector>

//in-project namespaces
#include <types.hpp>
#include <miscs/utils.hpp>

//standard c++ namespaces
using namespace std;

//in-project namespaces
using namespace Algos::Learning;
using namespace Miscs;

/**
 * Constructor
 * @param Input_Count Number of input values
 * @param Neuron_Nums Numbers of neurons in each layer (no input layer)
 */
neunet::neunet(long Input_Count,vector<long> Neuron_Nums) {

  //initialise input vector
  for (long Index=0; Index<Input_Count; Index++)
    this->Inputs.push_back(0);

  //bias
  this->Inputs.push_back(1);

  //initialise layers
  for (long Index=0; Index<(long)Neuron_Nums.size(); Index++) {
    this->Layers.push_back((vector<neuron*>){});

    //neurons in a layer
    for (long Jndex=0; Jndex<Neuron_Nums[Index]; Jndex++) {
      neuron* Neuron;

      //fisrt layer
      if (Index==0)
        Neuron = new neuron(Input_Count+1);

      //second layer to last layer
      else
        Neuron = new neuron(Neuron_Nums[Index-1]);

      this->Layers[Index].push_back(Neuron);
    }//jndex
  }//index

  this->randomise_weights();
}

/**
 * Destructor 
 */
neunet::~neunet() {
  for (long Index=0; Index<(long)this->Layers.size(); Index++)
    for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++)
      delete this->Layers[Index][Jndex];
}

/**
 * Randomise weights of all neurons of all layers
 */
void neunet::randomise_weights() {
  for (long Index=0; Index<(long)this->Layers.size(); Index++)
    for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++)
      this->Layers[Index][Jndex]->randomise_weights();
}

/**
 * Get outputs of all neurons in a layer
 */
vector<double> neunet::get_layer_outputs(long Layer_Index) {
  vector<double> Layer_Outputs;

  for (long Index=0; Index<(long)this->Layers[Layer_Index].size(); Index++) {
    neuron* Neuron = this->Layers[Layer_Index][Index];
    Layer_Outputs.push_back(Neuron->get_output());
  }

  return Layer_Outputs;
}

/**
 * Feedforward to get final outputs
 */
vector<double> neunet::feedforward(vector<double> Training_Inputs) {

  //save inputs
  this->Inputs = Training_Inputs;

  //propagate forwards thru' all layers
  for (long Index=0; Index<(long)this->Layers.size(); Index++) {
    vector<double> Layer_Inputs;

    //inputs for neurons in a layer
    if (Index==0)
      Layer_Inputs = this->Inputs;
    else
      Layer_Inputs = this->get_layer_outputs(Index-1);

    //set input for all neurons in layer and compute
    for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++) {
      neuron* Neuron = this->Layers[Index][Jndex];

      Neuron->set_inputs(Layer_Inputs);
      Neuron->compute_output();
    }//jndex
  }//index

  //return outputs
  long Layer_Count = (long)this->Layers.size();
  return this->get_layer_outputs(Layer_Count-1);
}

/**
 * Backpropagate to calculate errors and update weights
 * @return Average of all abs(error) at last layer
 */
double neunet::backpropagate(vector<double> Expected_Outputs,
double Learning_Rate,double Momentum) {
  long Layer_Count = (long)this->Layers.size();

  //calculate errors
  for (long Index=Layer_Count-1; Index>=0; Index--) {
    if (Index==Layer_Count-1) {
      for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++) {
        this->Layers[Index][Jndex]->Error = 
        Expected_Outputs[Jndex]-this->Layers[Index][Jndex]->Output;
      }
    }
    else {
      vector<double> Errors;
      vector<double> Weights;

      for (long Jndex=0; Jndex<(long)this->Layers[Index+1].size(); Jndex++)
        Errors.push_back(this->Layers[Index+1][Jndex]->Error);

      for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++) {
        Weights.clear();
        neuron* Neuron = this->Layers[Index][Jndex];

        for (long Kndex=0; Kndex<(long)this->Layers[Index+1].size(); Kndex++)
          Weights.push_back(this->Layers[Index+1][Kndex]->Weights[Jndex]);

        Neuron->Error = 
        utils::scalar_product(Errors,Weights)*utils::dsigmoid(Neuron->Output);
      }
    }
  }//error calculation loop

  //update weights
  for (long Index=0; Index<(long)this->Layers.size(); Index++) {
    for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++) {
      neuron* Neuron = this->Layers[Index][Jndex];

      for (long Kndex=0; Kndex<(long)Neuron->Weights.size(); Kndex++) {
        double Weight_Change = 
        Neuron->Inputs[Kndex]*Neuron->Error*Learning_Rate +
        Neuron->Weights[Kndex]*Momentum;

        Neuron->Weights[Kndex] += Weight_Change;
        Neuron->Changes[Kndex]  = Weight_Change;
      }
    }
  }

  return 0;
}

/**
 * Train weights
 */
void neunet::train_weights(vector<sample> Samples,
double Learning_Rate,double Momentum){

  //loop until error sum is in acceptable range
  for (long Index=0; Index<1000; Index++) {

    for (long Index=0; Index<(long)Samples.size(); Index++) {
      sample& Sample = Samples[Index];

      this->feedforward(Sample.first);
      this->backpropagate(
        Sample.second,Learning_Rate,Momentum
      );
    }//for
  }
}

//end of file