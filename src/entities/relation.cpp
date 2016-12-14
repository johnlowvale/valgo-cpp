/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Relation entity class implemtation file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//in-project headers
#include <entities/node.hpp>
#include <entities/relation.hpp>

//in-project namespace being used
using namespace Entities;

/**
 * Constructor
 */
relation::relation(node* Node1,node* Node2,string Id) {
  this->Left  = Node1;
  this->Right = Node2;
  this->Id    = Id;
}

/**
 * Destructor 
 */
relation::~relation() {
  //
}

//end of file