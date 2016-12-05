/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Tipper class implementation file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//standard c++ headers
#include <cstdint>
#include <iostream>
#include <vector>

//libary headers
#include <boost/property_tree/json_parser.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

//in-project headers
#include <consts.hpp>
#include <types.hpp>
#include <algos/tipping/tipper.hpp>
#include <miscs/db.hpp>
#include <miscs/utils.hpp>

//library namespaces
using namespace boost;
using namespace boost::property_tree;
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::document;
using namespace mongocxx;
using namespace mongocxx::result;

//standard namespaces
using namespace std;

//in-project namespaces being used
using namespace Algos::Tipping;
using namespace Miscs;

/**
 * Can't assign new object in constructor, using constructor initialiser
 */
tipper::tipper(client& Db_Client_,request& Request_,response& Response_):
Db_Client(Db_Client_),Request(Request_),Response(Response_) {
  //
}

/**
 * Default destructor
 */
tipper::~tipper() {
}

/**
 * Tipper thread
 */
void tipper::run() {
  utils::print_request(Request);

  //get request data
  ptree  Content = utils::get_request_content_ptree(Request);
  string Text    = Content.get<string>("Text");
  cout <<"\nTipping about " <<Text <<endl;

  //value to find
  value Find_Value = document{}
  <<"$text"
  <<open_document
    <<"$search" <<Text
  <<close_document
  <<finalize;

  //get result
  cursor Cursor = db::find(
    Db_Client,"contents",Find_Value,10
  );

  //get results from cursor
  ptree Result;
  ptree Tips;
  long  Count = 0;

  for (view View: Cursor) {
    ptree Tip;
    Tip.put("Url", db::get_string(View["Url"]));
    Tip.put("Fact","FACT");

    Tips.push_back(make_pair("",Tip));
    Count++;
  }

  Result.add_child("Tips",Tips);
  cout <<"Found " <<Count <<" result(s)" <<endl;

  //respond
  string Json_Str = utils::dump_to_json_str(Result);
  utils::send_json(Response,Json_Str);
}

//end of file