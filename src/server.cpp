/**
 * vAlgo Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Main server class file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//standard c++ headers
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>

//library headers
#include <server_http.hpp>
#include <boost/any.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/operation_exception.hpp>

//in-project headers
#include <consts.hpp>
#include <server.hpp>
#include <types.hpp>
#include <entities/webloc.hpp>
#include <miscs/db.hpp>
#include <miscs/utils.hpp>
#include <tasks/crawler.hpp>

//standard c++ namespaces
using namespace std;
using namespace std::chrono;

//library namespaces
using namespace boost;
using namespace boost::property_tree;
using namespace bsoncxx::document;
using namespace mongocxx;
using namespace SimpleWeb;
using bsoncxx::from_json;
using bsoncxx::to_json;
using bsoncxx::types::b_date;

//in-project namespaces
using namespace Entities;
using namespace Miscs;
using namespace Tasks;

//static members
string  server::VERSION       = VERSION__;
long    server::PORT          = PORT__;
long    server::CRAWLER_COUNT = CRAWLER_COUNT__;
server* server::Singleton     = nullptr;

/**
 * Server constructor 
 */
server::server() {
  server::Singleton = this;
  this->Http_Server = new http_server(server::PORT,1);
}

/**
 * Destructor 
 */
server::~server() {
  delete this->Http_Server;
}

/**
 * Handle root URL
 */
void server::handle_get_root(response Response,request Request) {
  utils::print_request(Request);
  
  //response text
  string Content = "vAlgo++ Server v"+server::VERSION;
  cout <<Content <<endl;

  //respond
  utils::send_text(Response,Content);
}

/**
 * Handle /webloc/add URL
 */
void server::handle_post_webloc_add(response Response,request Request) {
  utils::print_request(Request);

  //get request data
  ptree  Content      = utils::get_request_content_ptree(Request);
  string Full_Url     = Content.get<string>("Full_Url");
  float  Revisit_Time = Content.get<float>("Revisit_Time");

  //log
  cout <<"Full URL: " <<Full_Url <<endl;
  cout <<"Revisit Time: " <<Revisit_Time <<" hour(s)" <<endl;
  cout.flush();
  
  //add to db
  webloc Webloc(Full_Url,Revisit_Time);
  string Result = Webloc.save_to_db(server::Singleton->Db_Client);

  //check result
  if (Result==OK) {
    string Json_Str = JSON_MESSAGE_OK;
    cout <<OK <<endl;
    cout.flush();
    utils::send_json(Response,Json_Str);
  }
  else {
    string Json_Str = utils::make_json_str("Error",Result);
    cout <<Result <<endl;
    cout.flush();
    utils::send_json(Response,Json_Str);
  }
}

/**
 * Handle /crawlers/statuses URL
 */
void server::handle_post_crawlers_statuses(response Response,request Request) {
  utils::print_request(Request);

  //get request data
  ptree Content = utils::get_request_content_ptree(Request);

  //result
  ptree Result;
  ptree Statuses;

  for (long Index=0; Index<server::CRAWLER_COUNT; Index++) {
    crawler* Crawler = server::Singleton->Crawlers[Index];
    ptree    Status;

    Status.put("Index",       Index);
    Status.put("Queue_Length",Crawler->Queue.size()-Crawler->Current_Index);
    Status.put("Current_Url", Crawler->Current_Url);

    Statuses.push_back(make_pair("",Status));
  }

  Result.add_child("Crawlers",Statuses);

  //log
  string Json_Str = utils::dump_to_json_str(Result);
  cout <<"\nCrawlers' statuses:" <<endl;
  cout <<Json_Str <<endl;

  utils::send_json(Response,Json_Str);
}

/**
 * Create indices in db
 */
void server::create_indices() {
  cout <<"\nCreating indices in DB..." <<endl;

  //weblocs
  cout <<"weblocs.Revisit_At" <<endl;
  db::create_index(this->Db_Client,"weblocs","Revisit_At",R"({
    "Revisit_At": 1
  })");

  //contents
  cout <<"contents.*" <<endl;
  db::create_index(this->Db_Client,"contents","Wildcard",R"({
    "$**": "text"
  })");  

  cout <<"Indices created." <<endl <<endl;
}

/**
 * Initialise server
 */
void server::initialise() {

  //info
  cout <<"vAlgo++ Server v" <<server::VERSION <<"\n";
  cout <<"Copyright (c) Abivin JSC\n";

  //create db connection
  this->Db_Client = db::get_client();

  //create indices
  this->create_indices();

  //web server urls
  this->Http_Server->resource["^/$"]["GET"] = 
  server::handle_get_root;

  this->Http_Server->resource["^/webloc/add$"]["POST"] = 
  server::handle_post_webloc_add;

  this->Http_Server->resource["^/crawlers/statuses$"]["POST"] = 
  server::handle_post_crawlers_statuses;
}

/**
 * Update Revisit_At of the past web locations.
 * When server goes offline, the Revisit_At field of some of the weblocs
 * might have turned into time of the past. The value of this Revisit_At field
 * must always be a time in the future.
 */
void server::update_past_weblocs() {

  //find weblocs with Revisit_At of time value in the past
  value Value = document{} 
  <<"Revisit_At"
  <<open_document
    <<"$lte" <<utils::now_as_bdate()
  <<close_document
  <<finalize;

  //do finding
  cursor Cursor = db::find(this->Db_Client,"weblocs",Value);

  //update Revisit_At to make it a time in future
  long Update_Count = 0;
  for (view View: Cursor) {
    //int64 Revisit_At = View["Revisit_At"].get_date().to_int64();

    //schedule the webloc to be visited next minute
    int64 Revisit_At = utils::milliseconds_since_epoch()+60*1000;

    //update the webloc
    value Find_Value = document{}
    <<"_id" <<View["_id"].get_utf8().value.to_string()
    <<finalize;

    value Update_Value = document{}
    <<"$set"
    <<open_document
      <<"Revisit_At" <<b_date(milliseconds(Revisit_At))
    <<close_document
    <<finalize;

    try {
      db::update_one(this->Db_Client,"weblocs",Find_Value,Update_Value);
      Update_Count++;
      //cout <<"Updated visit schedule for ";
      //cout <<View["_id"].get_utf8().value.to_string() <<endl;
    }
    catch (operation_exception& Exception) {
      cout <<"Update visit schedule failed for ";
      cout <<View["_id"].get_utf8().value.to_string() <<endl;
      cout <<to_json(Exception.raw_server_error().value()) <<endl;
    }
  }

  cout <<"Updated schedules for " <<Update_Count <<" URL(s)" <<endl;
}

/**
 * Check if a webloc is not yet queued in any crawler thread
 */
bool server::is_not_queued(webloc* Webloc) {
  for (crawler* Crawler: this->Crawlers) {
    long Count = Crawler->Queue.count(Webloc->Id);
    if (Count==1)
      return false;
  }

  return true;
}

/**
 * Queue web locations into crawlers' queues
 */
void server::queue_weblocs() {

  //thread loop
  while (true) {

    //clear queues of crawlers
    for (crawler* Crawler: this->Crawlers) {
      Crawler->clear_queue();
      Crawler->Is_Queuing = true;
    }

    //log
    stringstream Out; //for bulk printing to cout
    Out <<"\nDistributor thread..." <<endl;

    //get the weblocs in the time span [present..present+1hour]
    //when the crawler can't process all weblocs of this time range in 1 hour
    //some weblocs turn into missed-to-crawl state
    moment Now          = utils::now();
    int64  Now_In_Milli = utils::milliseconds_since_epoch();
    int64  Lower_Milli  = Now_In_Milli-(Now_In_Milli%ONE_HOUR_MILLI);
    int64  Upper_Milli  = Lower_Milli+ONE_HOUR_MILLI;
    Out <<"Getting web locations..." <<endl;
    Out <<"From " <<Now.Year <<"-" <<Now.Month <<"-" <<Now.Day <<" "
        <<Now.Hour <<":" <<0 <<":" <<0 <<" "
        <<"To +1 hour" <<endl;

    //visit time range to find
    value Find_Value = document{}
    <<"Revisit_At"
    <<open_document
      <<"$gte" <<b_date(milliseconds(Lower_Milli))
      <<"$lte" <<b_date(milliseconds(Upper_Milli))
    <<close_document
    <<finalize;

    //get the result cursor
    cursor Cursor = db::find(this->Db_Client,"weblocs",Find_Value);

    //loop thru' documents found
    long Webloc_Count  = 0;
    long Crawler_Index = 0;
    for (view View: Cursor) {

      //get doc values
      string Full_Url     = db::get_string(View["Full_Url"]);
      double Revisit_Time = db::get_double(View["Revisit_Time"]); 

      //make a webloc instance from the document
      webloc* Webloc     = new webloc(Full_Url,Revisit_Time);
      Webloc->Revisit_At = db::get_date(View["Revisit_At"]);

      //add webloc to queue and reschedule it
      crawler* Crawler = this->Crawlers[Crawler_Index];
      Crawler->Queue[Webloc->Id] = Webloc;
      Crawler->reschedule_webloc(Webloc);

      //count web locations found and round-robin increase crawler index
      Webloc_Count++;
      if (Crawler_Index<server::CRAWLER_COUNT-1)
        Crawler_Index++;
      else
        Crawler_Index = 0;
    }

    //log
    Out <<"Queued " <<Webloc_Count <<" web location(s)" <<endl;
    cout <<Out.str();
    cout.flush();

    //queuing done
    for (crawler* Crawler: this->Crawlers)
      Crawler->Is_Queuing = false;

    //sleep for 1 hour
    this_thread::sleep_for(milliseconds(ONE_HOUR_MILLI));
  }
}

/**
 * Run server
 */
void server::run() {
  this->update_past_weblocs();

  //crawler threads
  for (long Index=0; Index<server::CRAWLER_COUNT; Index++) {

    //create crawler
    crawler* Crawler = new crawler(Index);
    this->Crawlers.push_back(Crawler);

    //create thread
    thread Crawler_Thread(&crawler::run,Crawler);
    Crawler_Thread.detach();

    //log
    cout <<"Created a crawler, thread index: " <<Index <<"\n";
  }

  //create thread for crawling distributor which distributes web locations
  //to crawlers
  thread Distributor_Thread(&server::queue_weblocs,this);
  Distributor_Thread.detach();

  //start web server
  cout <<"\nServer listening at port " <<to_string(server::PORT) <<"...\n";
  this->Http_Server->start();
}

/**
 * Programme entry point
 */
int main(int Argc,char** Args) {
  server Valgo_Server;
  Valgo_Server.initialise();
  Valgo_Server.run();
}

//end of file