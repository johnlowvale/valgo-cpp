/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Neuron header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef ALGOS_LEARNING_NEURON_HPP
#define ALGOS_LEARNING_NEURON_HPP

//standard c++ headers
#include <vector>

//in-project headers
#include <miscs/utils.hpp>

//standard c++ namespaces
using namespace std;

//namespace path
namespace Algos {
namespace Learning {

  /**
   * Neuron class
   */
  class neuron {

    //public properties
    public:
      vector<double> Inputs;
      vector<double> Weights;
      vector<double> Changes; //last changes in weights
      double         Output;

    //public methods
    public:
      neuron(long Input_Count);
      ~neuron();

      void   set_inputs(vector<double> Inputs);
      void   randomise_weights();
      void   compute_output();
      double get_output();
  };

//namespace path
}}

//include guard end
#endif

//end of file