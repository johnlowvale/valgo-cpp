/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Tipper class implementation file
 * @version 0.1
 * @author  Dat Dinhquoc
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
#include <algos/tipping/tipper.hpp>
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
using namespace Algos::Tipping;
using namespace Miscs;

/**
 * Can't assign new object in constructor, using constructor initialiser
 */
tipper::tipper(client& Db_Client_,request& Request_,response& Response_):
Db_Client(Db_Client_),Request(Request_),Response(Response_) {
  this->Tip_Count = 0;
}

/**
 * Default destructor
 */
tipper::~tipper() {
}

/**
 * Get a list of facts from text 
 */
vector<string> tipper::get_facts_from_text(string Term,string Text) {
  vector<string> Facts;

  //enough tips
  if (this->Tip_Count>=TIP_MAX)
    return Facts;

  //remove doublequotes
  replace_all(Term,"\""," ");
  trim(Term);

  //find occurrences of Term
  long Pos;
  while ((Pos=Text.find(Term))!=-1) {
    long Left_Pos  = Pos; //first char of Term in Text
    long Right_Pos = Pos+Term.length()-1; //last char of Term in Text

    //find sentence separator at left side
    while (Left_Pos>=0 && Text[Left_Pos]!='.' && Text[Left_Pos]!='?')
      Left_Pos--;

    //find sentence separator at right side
    while (Right_Pos<=(long)Text.length()-1 && Text[Right_Pos]!='.' &&
    Text[Right_Pos]!='?')
      Right_Pos++;

    //extract the sentence found
    string Sentence = Text.substr(Left_Pos+1,Right_Pos-Left_Pos-1);
    trim(Sentence);

    //add to fact list
    Facts.push_back(Sentence+"...");
    this->Tip_Count++;

    //remove the text before and including Right_Pos from Text
    Text = Text.substr(Right_Pos);  
  }

  return Facts;
}

/**
 * Get a list of facts from html
 */
vector<string> tipper::get_facts_from_html(string Term,string Html) {
  vector<string> Facts;

  //parse html
  ParserDom  Parser;
  tree<Node> Dom = Parser.parseTree(Html);

  //find all text nodes
  for (auto Iter=Dom.begin(); Iter!=Dom.end(); Iter++) {
    if (Iter->isTag() || Iter->isComment())
      continue;

    //get facts from text
    string         Text          = Iter->text();
    vector<string> Partial_Facts = this->get_facts_from_text(Term,Text);

    //add facts to list
    for (string Fact: Partial_Facts)
      Facts.push_back(Fact);

    //enough tips
    if (this->Tip_Count>=TIP_MAX)
      return Facts;
  }

  return Facts;
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

  //loop thru' docs found in db
  for (view View: Cursor) {
    ptree          Tip;
    string         Url   = db::get_string(View["Url"]);
    vector<string> Facts = this->get_facts_from_html(
      Text,db::get_string(View["Html"])
    );

    for (string Fact: Facts) {
      Tip.put("Url", Url);
      Tip.put("Fact",Fact);
      Tips.push_back(make_pair("",Tip));
    }

    //url count
    Count++;

    //enough tips
    if (this->Tip_Count>=TIP_MAX)
      break;
  }

  Result.add_child("Tips",Tips);
  cout <<"Tip limit: " <<TIP_MAX <<endl;
  cout <<"Parsed " <<Count <<" related URL(s)" <<endl;
  cout <<"Found " <<this->Tip_Count <<" tips" <<endl;

  //respond
  string Json_Str = utils::dump_to_json_str(Result);
  utils::send_json(Response,Json_Str);
}

//end of file