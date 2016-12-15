/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Database operation class header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef MISCS_DB_HPP
#define MISCS_DB_HPP

//standard c++ headers
#include <cstdint>
#include <iostream>
#include <vector>

//libary headers
#include <boost/property_tree/json_parser.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

//in-project headers
#include <types.hpp>
#include <miscs/db.hpp>

//library namespaces
using namespace boost;
using namespace boost::property_tree;
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::document;
using namespace mongocxx;
using namespace mongocxx::result;

//standard namespaces
using namespace std;

//namespace path
namespace Miscs {

  /**
   * Database operation class
   */
  class db {

    //public constants
    public:
      static string DB_SERVER;
      static string DB_NAME;

    //public constructor and methods
    public:
      db();  //unused
      ~db(); //unused

      //test method      
      static void test();

      //other static methods
      static value      build_doc_from_ptree(ptree Ptree);
      static client     get_client();      
      static collection get_collection(client& Db_Client,
                        const char* Collection_Name);
      static string     get_string(element Element);
      static int64      get_int64(element Element);
      static double     get_double(element Element);
      static int64      get_date(element Element);

      //all db methods would use Boost ptree thus another db interface class
      //like this for Couchbase can work with the same method parameters;
      //Collection_Name is just optional when using Couchbase, coz it's unused;
      //Collection_Name is required when using MongoDB.
      static string insert_one(client& Db_Client,
                    const char* Collection_Name,ptree Ptree);
      static string insert_one(client& Db_Client,
                    const char* Collection_Name,document& Document);
      static string create_index(client& Db_Client,const char* Collection_Name,
                    const char* Index_Name,const char* Index_Info);
      static cursor find(client& Db_Client,const char* Collection_Name,
                    value& Value,long Limit=0);
      static int64  count(client& Db_Client,const char* Collection_Name,
                    value& Value,long Limit=0);
      static void   update_one(client& Db_Client,const char* Collection_Name,
                    value& Find_Value,value& Update_Value);
      static void   upsert_one(client& Db_Client,const char* Collection_Name,
                    value& Find_Value,value& Update_Value);
  };
}//namespace path

//include guard end
#endif

//end of file