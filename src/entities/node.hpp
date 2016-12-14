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

//library headers
#include <mongocxx/client.hpp>

//in-project headers
#include <types.hpp>
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
      node(string Id);
      ~node();

      void add_relation(relation* Relation);
      void save_to_db(client& Db_Client);
  };
}//namespace path

//incluce guard end
#endif

//end of file