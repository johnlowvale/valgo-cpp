/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Chatter class implementation file
 * @version 0.1
 * @author  Dat Dinhquoc
 *
 * Types of sentences:
 * - Declarative sentence (ends with nothing or period)
 * - Imperative sentence (ends with nothing or !)
 * - Interrogative sentence (ends with nothing or !)
 * - Exclamatory sentence (ends with nothing or ?)
 */

//standard c++ headers
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

//libary headers
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <bsoncxx/json.hpp>
#include <htmlcxx/html/ParserDom.h>
#include <mongocxx/client.hpp>
#include <mongocxx/cursor.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

//in-project headers
#include <consts.hpp>
#include <types.hpp>
#include <algos/learning/neunet.hpp>
#include <algos/chatting/chatter.hpp>
#include <miscs/db.hpp>
#include <miscs/utils.hpp>

//library namespaces
using namespace boost;
using namespace boost::property_tree;
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::document;
using namespace htmlcxx;
using namespace htmlcxx::HTML;
using namespace mongocxx;
using namespace mongocxx::result;

//standard namespaces
using namespace std;

//in-project namespaces being used
using namespace Algos::Chatting;
using namespace Algos::Learning;
using namespace Miscs;

//static properties
string chatter::INFORMATION_SUPPORT = "INFORMATION_SUPPORT";

/**
 * Can't assign new object in constructor, using constructor initialiser
 */
chatter::chatter(client& Db_Client_,request& Request_,response& Response_):
Db_Client(Db_Client_),Request(Request_),Response(Response_) {
  this->Motivation = chatter::INFORMATION_SUPPORT;
}

/**
 * Default destructor
 */
chatter::~chatter() {
}

/**
 * Check if a reply is for the current motivation
 */
bool chatter::is_reply_for_motivation(string Reply) {
  return true;
}

/**
 * Chatter thread (for only a single reply)
 */
void chatter::run() {
  utils::print_request(Request);

  //get request data
  ptree  Content = utils::get_request_content_ptree(Request);
  string Text    = Content.get<string>("Text");
  cout <<"\nA human said " <<Text <<endl;

  //result
  ptree Result;
  Result.put("Reply","I'm chatbot");

  //respond
  string Json_Str = utils::dump_to_json_str(Result);
  utils::send_json(Response,Json_Str);
}

//end of file