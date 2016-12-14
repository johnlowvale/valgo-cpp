/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Relation entity class header file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_RELATION_HPP
#define ENTITIES_RELATION_HPP

//standard c++ headers
#include <string>

//in-project headers
#include <entities/node.hpp>

//standard c++ namespaces being used
using namespace std;

//in-project namespaces used
using namespace Entities;

//namespace path
namespace Entities {

  /**
   * Node relation class 
   * (directed relation left to right, no the other way round)
   * eg.
   * 'galaxy-note-3' -- 'is-created-by' -- 'samsung'
   * 'samsung' -- 'creates' -- 'galaxy-note-3'
   */
  class relation {

    //public properties
    public:
      string Id;
      node*  Left;  //eg. galaxy-note-3
      node*  Right; //eg. samsung
      string Name;  //eg. is-created-by, is-a-child-of,...

    //public constructor and methods
    public:
      relation(node* Left,node* Right,string Id);
      ~relation();
  };
}//namespace path

//incluce guard end
#endif

//end of file