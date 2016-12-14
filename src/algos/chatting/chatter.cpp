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
#include <mongocxx/exception/operation_exception.hpp>

//in-project headers
#include <consts.hpp>
#include <types.hpp>
#include <algos/learning/neunet.hpp>
#include <algos/chatting/chatter.hpp>
#include <entities/node.hpp>
#include <entities/relation.hpp>
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
using bsoncxx::from_json;
using bsoncxx::to_json;

//standard namespaces
using namespace std;

//in-project namespaces being used
using namespace Algos::Chatting;
using namespace Algos::Learning;
using namespace Entities;
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
 * Split text into sentences based on sentence punctuations
 */
vector<string> chatter::split_text_into_sentences(string Text) {
  replace_all(Text,"!",".");
  replace_all(Text,"?",".");

  vector<string> Sentences;
  split(Sentences,Text,is_any_of("."));

  return Sentences;
}

/**
 * Parse to get terms in a sentence component
 * A term may contain multiple words, eg. AO Smith
 */
vector<string> chatter::get_terms_in_component(string Component) {

  //???
  return vector<string>{};
}

/**
 * Get reply for a sentence
 */
string chatter::get_reply_for_sentence(string Sentence) {
  replace_all(Sentence,":",",");
  replace_all(Sentence,";",",");

  //split sentence into phrases and clauses
  vector<string> Temps;
  split(Temps,Sentence,is_any_of(","));

  vector<string> Components;
  for (string Item: Temps) {
    trim(Item);
    if (Item.length()>0)
      Components.push_back(Item);
  }

  //???
  return string("x.");
}

/**
 * Get a reply for an input text
 */
string chatter::get_reply_for_text(string Text) {
  vector<string> Sentences = this->split_text_into_sentences(Text);
  string         Full_Reply;

  //loop thru' all sentences to get replies
  for (string Sentence: Sentences) {
    trim(Sentence);

    //skip empty sentence
    if (Sentence.length()==0)
      continue;

    //get reply
    string Reply = this->get_reply_for_sentence(Sentence);
    Full_Reply  += " "+Reply;
  }

  return Full_Reply;
}

/**
 * Add relations for subject-verb-object
 */
void chatter::add_svo(string Fragment) {
  vector<string> Tokens;
  split(Tokens,Fragment,is_any_of(">"));

  //get values
  string Subject = Tokens[0];
  string Verb    = Tokens[1];
  string Object  = Tokens[2];
  trim(Subject);
  trim(Verb);
  trim(Object);

  //skip if there is any empty value
  if (Subject.length()==0 || Verb.length()==0 || Object.length()==0)
    return;

  //format values
  to_lower(Subject);
  to_lower(Verb);
  to_lower(Object);
  Subject = utils::tidy_up(Subject);
  Verb    = utils::tidy_up(Verb);
  Object  = utils::tidy_up(Object);

  //create nodes & relation
  node     Node1(Subject);
  node     Node2(Object);
  relation Relation(&Node1,&Node2,Verb);

  Node1.add_relation(&Relation);
  Node2.add_relation(&Relation);

  try {
    Node1.save_to_db(this->Db_Client);
    Node2.save_to_db(this->Db_Client);
  }
  catch (operation_exception& Exception) {
    stringstream Out;
    Out <<"Exception:" <<endl;
    Out <<to_json(Exception.raw_server_error().value()) <<endl;
    cout <<Out.str();
    cout.flush();
  }
}

/**
 * Add relations for subject-verb
 */
void chatter::add_sv(string Fragment) {
  vector<string> Tokens;
  split(Tokens,Fragment,is_any_of(">"));

  //get values
  string Subject = Tokens[0];
  string Verb    = Tokens[1];
  trim(Subject);
  trim(Verb);

  //skip if there is any empty value
  if (Subject.length()==0 || Verb.length()==0)
    return;

  //format values
  to_lower(Subject);
  to_lower(Verb);
  Subject = utils::tidy_up(Subject);
  Verb    = utils::tidy_up(Verb);

  //create nodes & relation
  node     Node1(Subject);
  node     Node2("");
  relation Relation(&Node1,&Node2,Verb);

  Node1.add_relation(&Relation);
  Node2.add_relation(&Relation);

  try {
    Node1.save_to_db(this->Db_Client);
  }
  catch (operation_exception& Exception) {
    stringstream Out;
    Out <<"Exception:" <<endl;
    Out <<to_json(Exception.raw_server_error().value()) <<endl;
    cout <<Out.str();
    cout.flush();
  }
}

/**
 * Add relations for terms in compounds
 */
void chatter::add_compounds(string Fragment) {
}

/**
 * Add terms
 */
void chatter::add_terms(string Fragment) {
}

/**
 * Add terms and relations to db
 */
void chatter::add_terms_and_relations(string Text) {
  Text = Text.substr(5);

  //split text into knowledge fragments, fragments are separated by ';'
  //term list:           term,term,term,...
  //compound list:       term+term+term+...
  //subject-verb:        term>term
  //subject-verb-object: term>term>term
  vector<string> Fragments;
  split(Fragments,Text,is_any_of(";"));

  //parse each fragment
  for (string Fragment: Fragments) {
    trim(Fragment);
    long Greaterthan_Count = count(Fragment.begin(),Fragment.end(),'>');
    long Plus_Count        = count(Fragment.begin(),Fragment.end(),'+');
    long Comma_Count       = count(Fragment.begin(),Fragment.end(),',');

    //skip bad fragment
    if (Greaterthan_Count>2)
      continue;

    //svo
    if (Greaterthan_Count==2)
      this->add_svo(Fragment);
    else
    if (Greaterthan_Count==1)
      this->add_sv(Fragment);
    else
    if (Plus_Count>0)
      this->add_compounds(Fragment);
    else
    if (Comma_Count>0)
      this->add_terms(Fragment);
    else
      this->add_terms(Fragment);
  }//for
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
  if (Text.substr(0,5)==string("/add")+" ") { //the space is important
    this->add_terms_and_relations(Text);
    Result.put("Reply","Added terms and relations!");
  }
  else
    Result.put("Reply",this->get_reply_for_text(Text));

  //respond
  string Json_Str = utils::dump_to_json_str(Result);
  utils::send_json(Response,Json_Str);
}

//end of file