/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Neural network header file
 * @version 0.1
 * @author  Dat Dinhquoc
 *
 * Reference:
 * http://www.labri.fr/perso/nrougier/downloads/mlp.py
 */

//include guard begin
#ifndef ALGOS_LEARNING_NEUNET_HPP
#define ALGOS_LEARNING_NEUNET_HPP

//standard c++ headers
#include <vector>

//in-project headers
#include <types.hpp>
#include <algos/learning/neuron.hpp>

//standard c++ namespaces
using namespace std;

//namespace path
namespace Algos {
namespace Learning {

  /**
   * Neural network class
   */
  class neunet {

    //public properties
    public:
      vector<double>          Inputs;
      vector<vector<neuron*>> Layers;

    //public methods
    public:
      neunet(long Input_Count,vector<long> Neuron_Nums);
      ~neunet();

      void           randomise_weights();
      vector<double> get_layer_outputs(long Layer_Index);
      vector<double> feedforward(vector<double> Training_Inputs);
      double         backpropagate(vector<double> Expected_Outputs,
                     double Learning_Rate,double Momentum);
      void           train_weights(vector<sample> Samples,
                     double Learning_Rate,double Momentum);
  };

//namespace path
}}

//include guard end
#endif

//end of file