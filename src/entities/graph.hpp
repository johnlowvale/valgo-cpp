/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Graph class header file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_GRAPH_HPP
#define ENTITIES_GRAPH_HPP

//standard c++ headers
#include <string>
#include <vector>

//in-project headers
#include <entities/node.hpp>

//standard c++ namespaces being used
using namespace std;

//namespace path
namespace Entities {

  /**
   * Graph class
   */
  class graph {

    //public properties
    public:
      vector<node*> Nodes;

    //public constructor and methods
    public:
      graph();
      ~graph();
  };
}//namespace path

//incluce guard end
#endif

//end of file