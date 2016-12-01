/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Web location entity class header file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_WEBLOC_HPP
#define ENTITIES_WEBLOC_HPP

//standard c++ headers
#include <map>
#include <string>
#include <vector>

//library headers
#include <mongocxx/client.hpp>

//in-project headers
#include <consts.hpp>
#include <types.hpp>

//standard c++ namespaces being used
using namespace std;

//library namespaces
using namespace mongocxx;

//namespace path
namespace Entities {

  /**
   * Web location class
   */
  class webloc {

    //public properties
    public:
      string Id;
      string Protocol;
      string Domain_Name;
      long   Port;
      string Path;
      string Query_String;
      string Full_Url; //to be used as _id as it is unique  
      double Revisit_Time; //to be visited after this amount of hours   
      int64  Revisit_At;
      int64  Added_At;
      long   Visit_Count;

      //not to store in db
      bool   Is_Valid;

      //map time to html, not to add more into this map
      //if current html of the web page stay unchanged
      //THIS PROPERTY IS UNUSED, TOO MANY ENTRIES OF HTML CONTENTS.
      //HTML CONTENTS ARE PUT IN ANOTHER COLLECTION
      map<int64,string> Htmls; 

    //public constructor and methods
    public:
      webloc();
      webloc(string Full_Url,double Revisit_Time);
      ~webloc();

      //db operations
      string save_to_db(client& Db_Client,int64 Crawl_At=ONE_HOUR_MILLI);
  };
}//namespace path

//incluce guard end
#endif

//end of file