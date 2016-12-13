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

      //first layer, plus 1 for input from bias value
      if (Index==0)
        Neuron = new neuron(Input_Count+1);

      //second layer to last layer
      //number of inputs equals number of neurons in previous layer
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

  //delete memory allocated for neurons
  //layer loop
  for (long Index=0; Index<(long)this->Layers.size(); Index++) {

    //neuron loop
    for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++)
      delete this->Layers[Index][Jndex];
  }
}

/**
 * Randomise weights of all neurons of all layers
 */
void neunet::randomise_weights() {

  //randomise all weights in all neurons
  //layer loop
  for (long Index=0; Index<(long)this->Layers.size(); Index++) {

    //neuron loop
    for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++)
      this->Layers[Index][Jndex]->randomise_weights();
  }
}

/**
 * Get outputs of all neurons in a layer
 */
vector<double> neunet::get_layer_outputs(long Layer_Index) {
  vector<double> Layer_Outputs;

  //neuron loop
  for (long Index=0; Index<(long)this->Layers[Layer_Index].size(); Index++) {
    neuron* Neuron = this->Layers[Layer_Index][Index];

    //get_output method only returns the last computed Output in Neuron
    Layer_Outputs.push_back(Neuron->get_output());
  }

  return Layer_Outputs;
}

/**
 * Feedforward to get final outputs
 * @return Last layer outputs
 */
vector<double> neunet::feedforward(vector<double> Training_Inputs) {

  //save inputs
  this->Inputs = Training_Inputs;

  //loop forwards thru' all layers
  for (long Index=0; Index<(long)this->Layers.size(); Index++) {
    vector<double> Layer_Inputs;

    //inputs for neurons in a layer
    if (Index==0)
      Layer_Inputs = this->Inputs; //training inputs
    else
      Layer_Inputs = this->get_layer_outputs(Index-1); //previous layer outputs

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
 * @return Sum of all abs(error) at last layer
 */
double neunet::backpropagate(vector<double> Expected_Outputs,
double Learning_Rate,double Momentum) {
  long   Layer_Count = (long)this->Layers.size();
  double Error_Sum   = 0;

  //calculate errors
  //layer loop
  for (long Index=Layer_Count-1; Index>=0; Index--) {

    //last layer, calculate errors based on Expected_Outputs
    if (Index==Layer_Count-1) {

      //neuron loop
      for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++) {
        neuron* Neuron = this->Layers[Index][Jndex];        
        double  Error  = Expected_Outputs[Jndex]-Neuron->Output;
        Neuron->Error  = Error;

        Error_Sum += abs(Error);
      }
    }

    //first layer to second last layer
    else {
      vector<double> Errors;  //errors on right side layer
      vector<double> Weights; //weights from each neuron to right side layer

      //list of errors on right side layer
      for (long Jndex=0; Jndex<(long)this->Layers[Index+1].size(); Jndex++)
        Errors.push_back(this->Layers[Index+1][Jndex]->Error);

      //neuron loop
      for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++) {        
        neuron* Neuron = this->Layers[Index][Jndex];

        //clear to use the Weights vector for current neuron
        Weights.clear();

        //neuron loop (right side layer)
        for (long Kndex=0; Kndex<(long)this->Layers[Index+1].size(); Kndex++) {
          neuron* Right_Neuron = this->Layers[Index+1][Kndex];

          //Jndex map to the 'Neuron' in the Jndex loop above
          Weights.push_back(Right_Neuron->Weights[Jndex]);
        }

        //error calculation formula
        Neuron->Error = 
        utils::scalar_product(Errors,Weights)*utils::dsigmoid(Neuron->Output);
      }
    }
  }//layer loop for error calculation

  //update weights
  //layer loop
  for (long Index=0; Index<(long)this->Layers.size(); Index++) {

    //neuron loop
    for (long Jndex=0; Jndex<(long)this->Layers[Index].size(); Jndex++) {
      neuron* Neuron = this->Layers[Index][Jndex];

      //weights for each inputs of current neuron
      for (long Kndex=0; Kndex<(long)Neuron->Weights.size(); Kndex++) {

        //weight change formula
        //this formula is the same as single neuron except that it has
        //the last weight change and momentum as a part of the formula
        double Weight_Change = 
        Neuron->Inputs[Kndex]*Neuron->Error*Learning_Rate +
        Neuron->Changes[Kndex]*Momentum;

        Neuron->Weights[Kndex] += Weight_Change;
        Neuron->Changes[Kndex]  = Weight_Change;
      }
    }//neuron loop
  }//layer loop

  //sum of errors on the last layer
  return Error_Sum;
}

/**
 * Train weights using a list of samples
 * @return Number of iterations over the whole sample set
 */
long neunet::train_weights(vector<sample> Samples,
double Learning_Rate,double Momentum,double Acceptable_Average_Error){

  //loop until error sum is in acceptable range
  long Set_Iteration_Count = 0;
  while (true) {

    //total error over the whole sample set
    double Error_Total = 0;

    //loop over every sample
    for (long Index=0; Index<(long)Samples.size(); Index++) {
      sample& Sample = Samples[Index];

      //feedforward to get final outputs
      this->feedforward(Sample.first);

      //backpropagate to calculate errors and update weights
      double Error_Sum = this->backpropagate(
        Sample.second,Learning_Rate,Momentum
      );

      Error_Total += Error_Sum;
    }//for

    //finished 1 training set iteration
    Set_Iteration_Count++;

    //average error on all samples
    double Average_Error = Error_Total/(double)Samples.size();

    //training result is acceptable? stop looping
    if (Average_Error<Acceptable_Average_Error)
      break;
  }

  //number of iterations over the whole sample set
  return Set_Iteration_Count;
}

//end of file