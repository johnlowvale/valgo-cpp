/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Node entity class header file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_NODE_HPP
#define ENTITIES_NODE_HPP

//standard c++ headers
#include <string>
#include <vector>

//in-project headers
#include <entities/content.hpp>

//standard c++ namespaces being used
using namespace std;

//in-project namespaces being used
using namespace Entities;

//namespace path
namespace Entities {

  //forward declarations
  class relation;

  /**
   * Node class
   */
  class node {

    //public properties
    public:
      string            Id;
      content*          Content;
      vector<relation*> Relations;

    //public constructor and methods
    public:
      node();
      ~node();
  };
}//namespace path

//incluce guard end
#endif

//end of file