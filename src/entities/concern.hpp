/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Node concern entity class header file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_CONCERN_HPP
#define ENTITIES_CONCERN_HPP

//standard c++ headers
#include <string>
#include <vector>

//library headers
#include <mongocxx/client.hpp>

//in-project headers
#include <types.hpp>
#include <entities/node.hpp>

//standard c++ namespaces being used
using namespace std;

//in-project namespaces being used
using namespace Entities;

//namespace path
namespace Entities {

  /**
   * Concern class
   */
  class concern {

    //public properties
    public:
      string Id;
      string Ai_Name;
      node*  Node;
      double Importance;     

    //public constructor and methods
    public:
      concern(string Ai_Name,node* Node,double Importance);
      ~concern();

      void save_to_db(client& Db_Client);
      void save_or_update_to_db(client& Db_Client);
  };
}//namespace path

//incluce guard end
#endif

//end of file