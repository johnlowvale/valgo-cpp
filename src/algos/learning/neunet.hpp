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
      vector<vector<double>>         Outputs; //[layer][neuron]
      vector<vector<vector<double>>> Weights; //[layer][neuron][input]
      vector<double>                 Wds;     //weight deltas

    //public constructor, destructor, and methods
    public:
      neunet(vector<long> Neuron_Nums);
      ~neunet();

      double         sigmoid(double X);
      double         dsigmoid(double X);
      void           randomise_weights();
      double         scalar_product(vector<double> Left,vector<double> Right);
      vector<double> propagate_forwards(vector<double> One_Sample);
      double         propagate_backwards(double Expected_Result,
                     double Learning_Rate=0.1,double Momentum=0.1);
  };

//namespace path
}}

//include guard end
#endif

//end of file