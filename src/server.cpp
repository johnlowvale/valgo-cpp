/**
 * vAlgo Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Main server class file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//standard c++ headers
#include <climits>
#include <ios>
#include <iostream>
#include <fstream>
#include <map>
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>

//library headers
#include <server_http.hpp>
#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/exception/operation_exception.hpp>

//in-project headers
#include <consts.hpp>
#include <server.hpp>
#include <types.hpp>
#include <algos/chatting/chatter.hpp>
#include <algos/learning/neunet.hpp>
#include <algos/searching/searcher.hpp>
#include <algos/tipping/tipper.hpp>
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
using namespace Algos::Chatting;
using namespace Algos::Learning;
using namespace Algos::Searching;
using namespace Algos::Tipping;
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

  //instantial properties
  this->Http_Server             = new http_server(server::PORT,1);
  this->Last_Revive_Count       = 0;
  this->Last_Distribution_Count = 0;
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
 * Handle admin URL
 */
void server::handle_get_admin(response Response,request Request) {
  utils::print_request(Request);
  
  //read js/css/html files
  string Js   = utils::read_file("../ui/index.js");
  string Css  = utils::read_file("../ui/index.css");
  string Html = utils::read_file("../ui/index.html");

  //put js/css into html
  replace_first(
    Html,
    "<script src=\"index.js\"></script>",
    "<script>"+Js+"</script>"
  );

  replace_first(
    Html,
    "<link rel=\"stylesheet\" href=\"index.css\"/>",
    "<style>"+Css+"</style>"
  );

  //respond
  utils::send_text(Response,Html);
}

/**
 * Handle file URL
 */
void server::handle_get_file(response Response,request Request) {
  utils::print_request(Request);
  
  //get file path from url
  stringstream Stream;
  Stream <<Request->path;
  string Path = Stream.str();

  //remove the heading
  replace_first(Path,"/file/","");

  //get file content
  string Content = utils::read_file("../files/"+Path);
  stringstream Out;
  Out <<"Serving file " <<Path <<endl;
  Out <<"File length: " <<Content.length() <<endl;
  cout <<Out.str();
  cout.flush();

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
 * Handle /webloc/crawl URL
 */
void server::handle_post_webloc_crawl(response Response,request Request) {
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
  webloc* Webloc = new webloc(Full_Url,Revisit_Time);
  string Result = Webloc->save_to_db(server::Singleton->Db_Client,0);

  //update Revisit_At
  value Find_Val = document{}
  <<"_id" <<Full_Url
  <<finalize;

  value Update_Val = document{}
  <<"$set"
  <<open_document
    <<"Revisit_At" <<utils::now_as_bdate()
  <<close_document
  <<finalize;

  db::update_one(server::Singleton->Db_Client,"weblocs",Find_Val,Update_Val);

  //tell distributor thread to queue this webloc to certain crawler
  server::Singleton->queue_single_webloc(Webloc);

  utils::send_json(Response,JSON_MESSAGE_OK);
}

/**
 * Handle /crawlers/statuses URL
 */
void server::handle_post_crawlers_statuses(response Response,request Request) {
  utils::print_request(Request);

  //get request data
  ptree Content = utils::get_request_content_ptree(Request);

  //statuses of crawlers
  ptree Statuses;
  for (long Index=0; Index<server::CRAWLER_COUNT; Index++) {
    crawler* Crawler = server::Singleton->Crawlers[Index];
    ptree    Status;

    Status.put("Index",       Index);
    Status.put("Queue_Length",Crawler->Queue.size());
    Status.put("Current_Url", Crawler->Current_Url);

    Statuses.push_back(make_pair("",Status));
  }

  //reviver infos
  ptree Reviver;
  Reviver.put("Last_Count",server::Singleton->Last_Revive_Count);

  //url distributor infos
  ptree Distributor;
  Distributor.put("Last_Count",server::Singleton->Last_Distribution_Count);

  //result
  ptree Result;  
  Result.add_child("Crawlers",   Statuses);
  Result.add_child("Reviver",    Reviver);
  Result.add_child("Distributor",Distributor);

  //log
  string Json_Str = utils::dump_to_json_str(Result);
  cout <<"\nCrawlers' statuses:" <<endl;
  cout <<Json_Str <<endl;

  utils::send_json(Response,Json_Str);
}

/**
 * Handle /crawlers/queues/clear URLs
 */
void server::handle_post_crawlers_queues_clear(
response Response,request Request) {
  utils::print_request(Request);

  //get request data
  ptree Content = utils::get_request_content_ptree(Request);

  //log
  stringstream Out;

  //clear all crawlers' queues
  for (long Index=0; Index<server::CRAWLER_COUNT; Index++) {
    crawler* Crawler = server::Singleton->Crawlers[Index];

    Out <<"Clearing " <<Crawler->Queue.size() <<" URL(s) from crawler "
    <<Index <<endl;

    if (Crawler->Queue.size()>0)
      Crawler->Need_To_Clear_Queue = true;
  }

  //log
  Out <<"Clearing messages sent." <<endl;
  cout <<Out.str();
  cout.flush();

  utils::send_json(Response,JSON_MESSAGE_OK);
}

/**
 * Handle /search URL
 */
void server::handle_post_search(response Response,request Request) {
  searcher Searcher(
    server::Singleton->Db_Client,Request,Response
  );

  //start new thread
  thread Searcher_Thread(&searcher::run,&Searcher);
  Searcher_Thread.join();

  //not to detach here coz detaching the thread means running out of this
  //method and the 2 variables Response, Request will be disposed.
}

/**
 * Handle /tip URL
 */
void server::handle_post_tip(response Response,request Request) {
  tipper Tipper(
    server::Singleton->Db_Client,Request,Response
  );

  //start new thread
  thread Tipper_Thread(&tipper::run,&Tipper);
  Tipper_Thread.join();

  //not to detach here coz detaching the thread means running out of this
  //method and the 2 variables Response, Request will be disposed.
}

/**
 * Handle /chat URL
 */
void server::handle_post_chat(response Response,request Request) {
  chatter Chatter(
    server::Singleton->Db_Client,Request,Response
  );

  //start new thread
  thread Chatter_Thread(&chatter::run,&Chatter);
  Chatter_Thread.join();

  //not to detach here coz detaching the thread means running out of this
  //method and the 2 variables Response, Request will be disposed.
}

/**
 * Create indices in db
 */
void server::create_indices() {
  cout <<"\nCreating indices in DB..." <<endl;

  //weblocs (web locations for crawlers)
  cout <<"weblocs.Revisit_At" <<endl;
  db::create_index(this->Db_Client,"weblocs","Revisit_At",R"({
    "Revisit_At": 1
  })");

  //contents (contents saved by crawlers)
  cout <<"contents.*" <<endl;
  db::create_index(this->Db_Client,"contents","Wildcard",R"({
    "$**": "text"
  })");  

  //relations (relations between nodes for chatting, reasoning)
  cout <<"relations.Language+Left+Name+Right" <<endl;
  db::create_index(this->Db_Client,"relations","Language_Left_Name_Right",R"({
    "Language": 1,
    "Left":     1,
    "Name":     1,
    "Right":    1
  })");

  //concerns (concerning terms of different AI beings, or chatbots)
  cout <<"concerns.Ai_Name" <<endl;
  db::create_index(this->Db_Client,"concerns","Ai_Name",R"({
    "Ai_Name": 1
  })");  

  cout <<"Indices created." <<endl <<endl;
}

/**
 * Test neural network
 */
void server::test_neunet() {
  cout <<"\nNeunet test..." <<endl;

  //neural network of 2 layers for logic 'and' operation
  //inputs: 2 values
  //first layer: 2 neurons
  //last layer (second layer): 1 neuron
  neunet Neunet(2,(vector<long>){2,1});

  //training data for logic 'and'
  vector<sample> Samples;
  Samples.push_back(sample{ {0,0}, {0} });
  Samples.push_back(sample{ {0,1}, {0} });
  Samples.push_back(sample{ {1,0}, {0} });
  Samples.push_back(sample{ {1,1}, {1} });

  //train weights using the above samples
  double Learning_Rate            = 0.1;
  double Momentum                 = 0.1;
  double Acceptable_Average_Error = 0.01;

  long Set_Iteration_Count = Neunet.train_weights(
    Samples,Learning_Rate,Momentum,Acceptable_Average_Error
  );

  long Sample_Iteration_Count = Set_Iteration_Count*Samples.size();
  cout <<"Acceptable average error:   " <<Acceptable_Average_Error <<endl;
  cout <<"Training set samples:       " <<Samples.size() <<endl;
  cout <<"Training set iterations:    " <<Set_Iteration_Count <<endl;
  cout <<"Training sample iterations: " <<Sample_Iteration_Count <<endl;

  //check using training data
  for (long Index=0; Index<(long)Samples.size(); Index++) {
    sample         Sample = Samples[Index];
    vector<double> Output = Neunet.feedforward(Sample.first);

    //inputs
    cout <<Sample.first[0]<<" and "<<Sample.first[1]<<" --> ";

    //output (close value)
    cout <<fixed <<showpos <<Output[0] <<noshowpos;
    cout.unsetf(ios_base::fixed);

    //output (rounded value)
    cout <<" --> " <<(long)round(Output[0]) <<endl;
  }
}//test neunet

/**
 * Initialise server
 */
void server::initialise() {

  //info
  cout <<"vAlgo++ Server v" <<server::VERSION <<"\n";
  cout <<"Copyright (c) Abivin JSC\n";

  //test neural net
  this->test_neunet();

  //create db connection
  this->Db_Client             = db::get_client();
  this->Db_Client_Reviver     = db::get_client();
  this->Db_Client_Distributor = db::get_client();

  //create indices
  this->create_indices();

  //web server urls
  this->Http_Server->resource["^/$"]["GET"] = 
  server::handle_get_root;

  this->Http_Server->resource["^/admin$"]["GET"] = 
  server::handle_get_admin;

  this->Http_Server->resource["^/file/.+$"]["GET"] = 
  server::handle_get_file;

  this->Http_Server->resource["^/webloc/add$"]["POST"] = 
  server::handle_post_webloc_add;

  this->Http_Server->resource["^/webloc/crawl$"]["POST"] = 
  server::handle_post_webloc_crawl;

  this->Http_Server->resource["^/crawlers/statuses$"]["POST"] = 
  server::handle_post_crawlers_statuses;

  this->Http_Server->resource["^/crawlers/queues/clear$"]["POST"] = 
  server::handle_post_crawlers_queues_clear;

  this->Http_Server->resource["^/search$"]["POST"] = 
  server::handle_post_search;

  this->Http_Server->resource["^/tip$"]["POST"] = 
  server::handle_post_tip;

  this->Http_Server->resource["^/chat$"]["POST"] = 
  server::handle_post_chat;
}

/**
 * Update Revisit_At of the past web locations.
 * When server goes offline, the Revisit_At field of some of the weblocs
 * might have turned into time of the past. The value of this Revisit_At field
 * must always be a time in the future.
 */
void server::update_past_weblocs() {
  while (true) {

    //find weblocs with Revisit_At of time value in the past
    value Value = document{} 
    <<"Revisit_At"
    <<open_document
      <<"$lte" <<utils::now_as_bdate()
    <<close_document
    <<finalize;

    //do finding
    cursor Cursor = db::find(
      this->Db_Client_Reviver,"weblocs",Value,REVIVES_PER_MINUTE
    );

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
        db::update_one(
          this->Db_Client_Reviver,"weblocs",Find_Value,Update_Value
        );

        Update_Count++;
        //cout <<"Updated visit schedule for ";
        //cout <<View["_id"].get_utf8().value.to_string() <<endl;
      }
      catch (operation_exception& Exception) {
        cout <<"\nUpdate visit schedule failed for ";
        cout <<View["_id"].get_utf8().value.to_string() <<endl;
        cout <<to_json(Exception.raw_server_error().value()) <<endl;
      }
    }

    //log
    this->Last_Revive_Count = Update_Count;
    cout <<"\nUpdated schedules for " <<Update_Count <<" URL(s)" <<endl;

    //sleep for a minute
    this_thread::sleep_for(milliseconds(ONE_HOUR_MILLI/60));
  }//while
}//update_past_weblocs

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
    cursor Cursor = db::find(this->Db_Client_Distributor,"weblocs",Find_Value);

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
    this->Last_Distribution_Count = Webloc_Count;
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
 * Queue a single webloc to a crawler
 */
void server::queue_single_webloc(webloc* Webloc) {
  long     Min_Queue_Length = LONG_MAX;
  crawler* Min_Crawler      = nullptr; 

  //find crawler with fewest weblocs in queue
  for (long Index=0; Index<server::CRAWLER_COUNT; Index++) {
    if (this->Crawlers[Index]->Queue.size()<(unsigned long)Min_Queue_Length) {
      Min_Queue_Length = this->Crawlers[Index]->Queue.size();
      Min_Crawler      = this->Crawlers[Index];
    }
  }//for

  //enqueue to the crawler with min queue length
  Min_Crawler->Queue[Webloc->Id] = Webloc;
}

/**
 * Run server
 */
void server::run() {

  //thread for reviving webloc with Revisit_At of time in the past
  thread Webloc_Revive_Thread(&server::update_past_weblocs,this);
  Webloc_Revive_Thread.detach();

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

  //free resources
  for (long Index=0; Index<server::CRAWLER_COUNT; Index++) {

    //weblocs
    for (auto Iter: this->Crawlers[Index]->Queue) {
      delete Iter.second;
    }

    //crawler
    delete this->Crawlers[Index];
  }

  //http server
  delete this->Http_Server;
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