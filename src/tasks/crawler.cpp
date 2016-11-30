/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Crawler class implementation file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//standard c++ headers
#include <chrono>
#include <iostream>
#include <thread>
#include <typeinfo>
#include <vector>

//library headers
#include <client_http.hpp>
#include <bsoncxx/json.hpp>
#include <htmlcxx/html/ParserDom.h>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/operation_exception.hpp>

//in-project headers
#include <consts.hpp>
#include <types.hpp>
#include <server.hpp>
#include <entities/moment.hpp>
#include <entities/webloc.hpp>
#include <miscs/db.hpp>
#include <miscs/utils.hpp>
#include <tasks/crawler.hpp>

//standard c++ namespaces
using namespace std;
using namespace std::chrono;

//library namespaces
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::document;
using namespace mongocxx;
using namespace mongocxx::result;
using bsoncxx::from_json;
using bsoncxx::to_json;
using bsoncxx::types::b_date;

//namespaces being used
using namespace Entities;
using namespace Miscs;
using namespace Tasks;

/**
 * Default constructor
 */
crawler::crawler() {
  //
}

/**
 * Constructor with the index of thread
 */
crawler::crawler(long Thread_Index) {
  this->Thread_Index = Thread_Index;
  this->Db_Client    = db::get_client();
  this->Is_Queuing   = false;
}

/**
 * Empty webloc queue
 */
void crawler::clear_queue() {
  for (auto& Iterator: this->Queue) {
    delete Iterator.second;
  }

  this->Queue.clear();
}

/**
 * Check if a web location is not yet queued in any crawler
 */
bool crawler::is_not_queued(webloc* Webloc) {
  return server::Singleton->is_not_queued(Webloc);
}

/**
 * Reschedule a webloc
 */
void crawler::reschedule_webloc(webloc* Webloc) {
  int64 New_Revisit_At = Webloc->Revisit_At + Webloc->Revisit_Time*60*60*1000;

  //document to find
  value Find_Value = document{}
  <<"_id" <<Webloc->Id
  <<finalize;

  //update as
  value Update_Value = document{} 
  <<"$set"
  <<open_document
    <<"Revisit_At" <<b_date(milliseconds(New_Revisit_At))
  <<close_document
  <<finalize;

  //do updating
  try {
    db::update_one(this->Db_Client,"weblocs",Find_Value,Update_Value);
  }
  catch (operation_exception& Exception) {
    cout <<"\nFailed to update " <<Webloc->Id <<endl;
  }
}

/**
 * Crawl the current queue
 */
void crawler::crawl_the_queue() {
  
  //make a vector of web locations of the queue
  vector<webloc*> Weblocs;
  for (auto& Iterator: this->Queue)
    Weblocs.push_back(Iterator.second);

  //sort the queue by Revisit_At
  sort(Weblocs.begin(),Weblocs.end(),[](webloc* A,webloc* B)->bool{
    return A->Revisit_At < B->Revisit_At;
  });

  //crawl all web locations in queue
  for (webloc* Webloc: Weblocs) {
    string Html = utils::http_get(
      Webloc->Domain_Name+":"+to_string(Webloc->Port),Webloc->Path
    );

    cout <<Html.length() <<endl;
    cout.flush();
  }

  this->clear_queue();
}

/**
 * Thread method
 */
void crawler::run() {

  //run forever
  while (true) {

    //wait for something in the queue
    while (this->Queue.empty() || this->Is_Queuing) {
      //do nothing
    }

    //bulk printing thru' stringstream
    stringstream Out;
    Out <<"\nCrawler thread index " <<this->Thread_Index <<endl;
    Out <<"Crawling..." <<endl;
    cout <<Out.str();
    cout.flush();

    //do the real crawling
    this->crawl_the_queue();

    //log
    Out.str("");
    Out <<"\nCrawler thread index " <<this->Thread_Index <<endl;
    Out <<"Crawling done." <<endl;
    cout <<Out.str();
    cout.flush();
  }//while
}//run

//end of file