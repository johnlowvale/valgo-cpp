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

map<string,mapss> chatter::REFLECTS = {
  {"English",{
    {"i",           "you"},
    {"me",          "you"},    
    {"my",          "your"},
    {"mine",        "yours"},
    {"myself",      "yourself"},
    {"you-subject", "i"},
    {"you-object",  "me"},
    {"your",        "my"},
    {"yours",       "mine"},
    {"yourself",    "myself"}
  }}
};

map<string,mapss> chatter::WHATS = {
  {"English",{
    {"what", "what?"}
  }}
};

string chatter::SUBJECT  = "subject";
string chatter::RELATION = "relation";
string chatter::OBJECT   = "object";
string chatter::TERM     = "term";

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
 * Join a list of tokens to make a term
 * @param Tokens    List of tokens (words or syllables)
 * @param To_Before Join tokens to before this index in Tokens vector
 */
string chatter::tokens_to_term(vector<string> Tokens,long To_Before) {
  string Term("");

  for (long Index=0; Index<To_Before; Index++) {
    string Token = this->check_reflection(Tokens[Index],chatter::TERM);

    if (Index==0)
      Term += Token;
    else
      Term += " "+Token;
  }

  return Term;
}

/**
 * Check if a term is in db
 */
bool chatter::term_is_in_db(string Term) {

  //check left side of relation
  value Find_Val = document{}
  <<"Language" <<this->Language
  <<"Left"     <<Term
  <<finalize;

  long Count = db::count(this->Db_Client,"relations",Find_Val,1);
  if (Count==1)
    return true;

  //check name of relation
  Find_Val = document{}
  <<"Language" <<this->Language
  <<"Name"     <<Term
  <<finalize;

  Count = db::count(this->Db_Client,"relations",Find_Val,1);
  if (Count==1)
    return true;

  //check right side of relation
  Find_Val = document{}
  <<"Language" <<this->Language
  <<"Right"    <<Term
  <<finalize;

  Count = db::count(this->Db_Client,"relations",Find_Val,1);
  if (Count==1)
    return true;

  //not found in db
  return false;
}

/**
 * Parse to get terms in a sentence component
 * A term may contain multiple words, eg. AO Smith
 * A term is a node in database
 */
vector<string> chatter::get_terms_in_component(string Component) {

  //format the sentence component
  to_lower(Component);
  Component = utils::tidy_up(Component);

  //get the tokens (words or syllables) in the sentence component
  vector<string> Tokens;
  split(Tokens,Component,is_any_of(" "));

  //create list of terms from tokens
  vector<string> Terms;
  while (Tokens.size()>0) {

    //try to make a term with fewest tokens
    string Term;
    long   Index;
    for (Index=0; Index<(long)Tokens.size(); Index++) {
      Term = this->tokens_to_term(Tokens,Index+1);
      
      //term found in db
      if (this->term_is_in_db(Term)) 
        break;
    }

    //no possible combinations of tokens to make a known term
    if (Index==(long)Tokens.size()) {
      Terms.push_back(this->what());
      Tokens.erase(Tokens.begin());
      continue;
    }

    //add the found term to list
    Terms.push_back(Term);

    //remove the componential tokens
    for (long Jndex=0; Jndex<Index+1; Jndex++)
      Tokens.erase(Tokens.begin());
  }//while

  return Terms;
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

  //each component is a phrase or clause (svo, sv)
  vector<string> Components;
  for (string Item: Temps) {
    trim(Item);
    if (Item.length()>0)
      Components.push_back(Item);
  }

  //make reply
  string Reply("");
  for (long Index=0; Index<(long)Components.size(); Index++) {
    string         Component = Components[Index];
    vector<string> Terms     = this->get_terms_in_component(Component);

    //??? ???
    for (long Jndex=0; Jndex<(long)Terms.size(); Jndex++)
      if (Jndex==0)
        Reply += Terms[Jndex];
      else
        Reply += "+"+Terms[Jndex];

    Reply += "; ";
  }

  return Reply;
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
 * Check term reflection, eg. i->you, you->i
 */
string chatter::check_reflection(string Term,string Type) {
  mapss Reflects = chatter::REFLECTS[this->Language];

  if (Reflects.count(Term+"-"+Type)==1)
    return Reflects[Term+"-"+Type];
  else { 
    if (Reflects.count(Term)==1)
      return Reflects[Term];
    else
      return Term;
  }
}

/**
 * Word used when a term is not understood
 */
string chatter::what() {
  return chatter::WHATS[this->Language]["what"];
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
  Subject = this->check_reflection(Subject,chatter::SUBJECT);
  Object  = this->check_reflection(Object,chatter::OBJECT);

  //create nodes & relation
  node     Node1(Subject,this->Language);
  node     Node2(Object,this->Language);
  relation Relation(&Node1,&Node2,Verb,this->Language);

  try {
    Node1.save_to_db(this->Db_Client);
  }
  catch (operation_exception& Exception) {
    utils::print_db_exception(Exception);
  }

  try {
    Node2.save_to_db(this->Db_Client);
  }
  catch (operation_exception& Exception) {
    utils::print_db_exception(Exception);
  }

  try {
    Relation.save_to_db(this->Db_Client);
  }
  catch (operation_exception& Exception) {
    utils::print_db_exception(Exception);
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
  Subject = this->check_reflection(Subject,chatter::SUBJECT);

  //create nodes & relation
  node     Node1(Subject,this->Language);
  node     Node2("",this->Language);
  relation Relation(&Node1,&Node2,Verb,this->Language);

  try {
    Node1.save_to_db(this->Db_Client);
  }
  catch (operation_exception& Exception) {
    utils::print_db_exception(Exception);
  }

  try {
    Relation.save_to_db(this->Db_Client);
  }
  catch (operation_exception& Exception) {
    utils::print_db_exception(Exception);
  }
}

/**
 * Add relations for terms in compounds
 */
void chatter::add_compounds(string Fragment) {
  vector<string> Temps;
  split(Temps,Fragment,is_any_of("+"));

  //remove empty tokens
  vector<string> Tokens;
  for (string Temp: Temps) {
    trim(Temp);

    if (Temp.length()>0) {
      to_lower(Temp);
      Temp = utils::tidy_up(Temp);
      Tokens.push_back(Temp);
    }
  }//for

  //compound needs at least 2 terms
  if (Tokens.size()<2)
    return;

  //add compounds
  for (long Index=0; Index<(long)Tokens.size()-1; Index++) {
    string Token1 = Tokens[Index];
    string Token2 = Tokens[Index+1];
    Token1 = this->check_reflection(Token1,chatter::TERM);
    Token2 = this->check_reflection(Token2,chatter::TERM);

    node     Node1(Token1,this->Language);
    node     Node2(Token2,this->Language);
    relation Relation(&Node1,&Node2,string("-"),this->Language);

    try {
      Node1.save_to_db(this->Db_Client);
    }
    catch (operation_exception& Exception) {
      utils::print_db_exception(Exception);
    }

    try {
      Node2.save_to_db(this->Db_Client);
    }
    catch (operation_exception& Exception) {
      utils::print_db_exception(Exception);
    }

    try {
      Relation.save_to_db(this->Db_Client);
    }
    catch (operation_exception& Exception) {
      utils::print_db_exception(Exception);
    }
  }//for
}

/**
 * Add terms
 */
void chatter::add_terms(string Fragment) {
  vector<string> Temps;
  split(Temps,Fragment,is_any_of(","));

  //remove empty tokens
  vector<string> Tokens;
  for (string Temp: Temps) {
    trim(Temp);

    if (Temp.length()>0) {
      to_lower(Temp);
      Temp = utils::tidy_up(Temp);
      Tokens.push_back(Temp);
    }
  }//for

  //nothing to save
  if (Tokens.size()<1)
    return;

  //add nodes
  for (long Index=0; Index<(long)Tokens.size(); Index++) {
    string Token = Tokens[Index];

    //create node instance
    Token = this->check_reflection(Token,chatter::TERM);
    node Node(Token,this->Language);

    try {
      Node.save_to_db(this->Db_Client);
    }
    catch (operation_exception& Exception) {
      utils::print_db_exception(Exception);
    }
  }//for
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
  ptree  Content  = utils::get_request_content_ptree(Request);
  string Text     = Content.get<string>("Text");
  string Language = Content.get<string>("Language");
  cout <<"\nA human said " <<Text <<endl;

  //config
  this->Language = Language;

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