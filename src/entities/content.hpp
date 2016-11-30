/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Content entity class header file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_CONTENT_HPP
#define ENTITIES_CONTENT_HPP

//standard c++ headers
#include <string>
#include <vector>

//library headers
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>

//standard namespaces used
using namespace std;

//library namespaces
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::document;
using namespace mongocxx;
using namespace mongocxx::result;

//namespace path
namespace Entities {

  /**
   * Content entity class
   */
  class content {

    //public properties
    public:
      string           Id;
      string           Url;
      string           Name;    //short name
      string           Title;   //long name
      string           Extract; //html extract
      string           Html;    //full html
      vector<string>   Links;
      vector<content*> Children;

    //public constructor and methods
    public:
      content();
      ~content();

      //db operations
      void save_to_db(client& Db_Client);
  };
}//namespace path

//incluce guard end
#endif

//end of file