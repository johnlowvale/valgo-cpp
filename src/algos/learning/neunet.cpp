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

//standard c++ namespaces
using namespace std;

//in-project namespaces
using namespace Algos::Learning;

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
  for (long Index=1; Index<=Neuron_Nums.size(); Index++) {
    this->Layers[Index].push_back((vector<neuron*>){});

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
  for (long Index=0; Index<this->Layers.size(); Index++)
    for (long Jndex=0; Jndex<this->Layers[Index].size(); Jndex++)
      delete this->Layers[Index][Jndex];
}

/**
 * Randomise weights of all neurons of all layers
 */
void neunet::randomise_weights() {
  for (long Index=0; Index<this->Layers.size(); Index++)
    for (long Jndex=0; Jndex<this->Layers[Index].size(); Jndex++)
      this->Layers[Index][Jndex]->randomise_weights();
}

/**
 * Get outputs of all neurons in a layer
 */
vector<double> neunet::get_layer_outputs(long Layer_Index) {
  vector<double> Layer_Outputs;

  for (long Index=0; Index<this->Layers[Layer_Index].size(); Index++) {
    neuron* Neuron = this->Layers[Layer_Index][Index];
    Layer_Outputs.push_back(Neuron->get_output());
  }

  return Layer_Outputs;
}

/**
 * Propagate forwards
 */
vector<double> neunet::propagate_forwards(vector<double> Training_Inputs) {

  //save inputs
  this->Inputs = Training_Inputs;

  //propagate forwards thru' all layers
  for (long Index=0; Index<this->Layers.size(); Index++) {
    vector<double> Layer_Inputs;

    //inputs for neurons in a layer
    if (Index==0)
      Layer_Inputs = Training_Inputs;
    else
      Layer_Inputs = this->get_layer_outputs(Index-1);

    //set input for all neurons in layer and compute
    for (long Jndex=0; Jndex<this->Layers[Index].size(); Jndex++) {
      neuron* Neuron = this->Layers[Index][Jndex];

      Neuron->set_inputs(Layer_Inputs);
      Neuron->compute_output();
    }//jndex
  }//index

  //return outputs
  long Layer_Count = this->Layers.size();
  return this->get_layer_outputs(Layer_Count-1);
}

/**
 * Propagate backwards
 */
void neunet::propagate_backwards(vector<double> Expected_Outputs,
double Learning_Rate,double Momentum) {
  long Layer_Count = this->Layers.size();

  //deltas (each error value has 1 delta along side)
  vector<vector<double>> Deltas;
  for (long Index=0; Index<Layer_Count; Index++) {
    vector<double> Vec;
    Vec.reserve(this->Layers[Index].size());
    Deltas.push_back(Vec);
  }

  //compute errors on last layer
  //loop thru neurons
  vector<double> Errors;
  for (long Index=0; Index<this->Layers[Layer_Count-1].size(); Index++) {
    neuron* Neuron = this->Layers[Layer_Count-1][Index]; 

    double  Output = Neuron.get_output();
    double  Error  = Expected_Outputs[Index] - Output;
    double  Delta  = Error*utils::dsigmoid(Output);

    Errors.push_back(Error);
    Deltas[Layer_Count-1][Index] = Delta;
  }

  //compute errors on other layers
  //loop thru layers
  for (long Index=Layer_Count-2; Index>=0; Index--) {

    //loop thru' neurons
    for (long Jndex=0; Jndex<this->Layers[Index].size(); Jndex++) {
      neuron* Left_Neuron = this->Layers[Index][Jndex];

      //calculate delta from layer on the right side
      double Delta = 0;
      for (long Kndex=0; Kndex<this->Layers[Index+1].size(); Kndex++) {
        neuron* Right_Neuron = this->Layers[Index+1][Kndex];

        Delta += Deltas[Kndex]*Right_Neuron.Weights[Jndex];
      }

      Deltas[Index][Jndex] = Delta*utils::dsigmoid(Left_Neuron.Output);
    }//jndex
  }//index

  //compute deltas on input layer
  vector<double> Input_Deltas;
  Input_Deltas.reserve(this->Inputs.size());

  for (long Index=0; Index<Input_Deltas.size(); Index++) {
    double Delta = 0;
    for (long Jndex=0; Jndex<this->Layers[0].size(); Jndex++) {
      neuron* Right_Neuron = this->Layers[0][Jndex];

      Delta += Deltas[Jndex]*Right_Neuron.Weights[Index];
    }

    Input_Deltas[Index] = Delta*utils::dsigmoid(this->Inputs[Index]);
  }

  //update weights
  //loop thru' layers
  for (long Index=0; Index<this->Layers.size(); Index++) {
    vector<neuron*> Neurons = this->Layers[Index];

    //left outputs & deltas
    vector<double> Left_Outputs;
    vector<double> Left_Deltas; 
    if (Index==0) {
      Left_Outputs = this->Inputs;
      Left_Deltas  = Input_Deltas;
    }
    else {
      Left_Outputs = this->get_layer_outputs(Index-1);
      Left_Deltas  = Deltas[Index-1];
    }

    //loop thru' neurons
    for (long Jndex=0; Jndex<this->Layers[Index].size(); Jndex++) {
      neuron* Neuron = Neurons[Jndex];

      //update weights (on dendrites)
      for (long Kndex=0; Kndex<Neuron->Weights.size(); Kndex++) {
        double Change = Left_Outputs[Kndex]*Left_Deltas[Kndex];

        Neuron->Weights[Kndex] += 
        Learning_Rate*Change + Momentum*Neuron->Changes[Kndex];

        Neuron->Changes[Kndex] = Change;
      }
    }//jndex
  }//index

  //return error sum
  return utils::scalar_product(Errors,Errors);
}

/**
 * Train weights
 */
void neunet::train_weights(vector<sample> Samples,
double Learning_Rate,double Momentum){

  //loop until error sum is in acceptable range
  while (true) {
    double Error_Sum = 0;

    for (long Index=0; Index<Samples.size(); Index++) {
      sample& Sample = Samples[Index];

      this->propagate_forwards(Sample.first);
      double Error = this->propagate_backwards(
        Sample.second,Learning_Rate,Momentum
      );

      Error_Sum += Error;
    }//for

    //acceptable error sum
    if (Error_Sum<0.1)
      break;
  }//while
}

//end of file