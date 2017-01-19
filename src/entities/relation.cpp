/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Relation entity class implemtation file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//library headers
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>

//in-project headers
#include <types.hpp>
#include <entities/node.hpp>
#include <entities/relation.hpp>
#include <miscs/db.hpp>

//in-project namespace being used
using namespace Entities;
using namespace Miscs;

/**
 * Constructor
 */
relation::relation(node* Node1,node* Node2,string Name,string Language) {
  this->Id       = Node1->Id+" "+Name+" "+Node2->Id;
  this->Left     = Node1;
  this->Right    = Node2;
  this->Name     = Name;
  this->Language = Language;
}

/**
 * Destructor 
 */
relation::~relation() {
  //
}

/**
 * Save to db
 */
void relation::save_to_db(client& Db_Client) {
  document Doc;
  Doc
  <<"_id"      <<this->Id
  <<"Id"       <<this->Id
  <<"Language" <<this->Language
  <<"Left"     <<this->Left->Id
  <<"Right"    <<this->Right->Id
  <<"Name"     <<this->Name;

  db::insert_one(Db_Client,"relations",Doc);
}

/**
 * Find relations containing a term
 */
vector<vector<string>> relation::find_term(client& Db_Client,
string Language,string Term) {
  vector<vector<string>> Results;

  //find in left values
  value Find_Val = document{}
  <<"Language" <<Language
  <<"Left"     <<Term
  <<finalize;

  cursor Cursor = db::find(Db_Client,"relations",Find_Val);
  for (view View: Cursor) {
    vector<string> Entry;

    Entry.push_back(db::get_string(View["Left"]));
    Entry.push_back(db::get_string(View["Name"]));
    Entry.push_back(db::get_string(View["Right"]));

    Results.push_back(Entry);
  }

  //find in name values
  Find_Val = document{}
  <<"Language" <<Language
  <<"Name"     <<Term
  <<finalize;

  Cursor = db::find(Db_Client,"relations",Find_Val);
  for (view View: Cursor) {
    vector<string> Entry;

    Entry.push_back(db::get_string(View["Left"]));
    Entry.push_back(db::get_string(View["Name"]));
    Entry.push_back(db::get_string(View["Right"]));

    Results.push_back(Entry);
  }

  //find in right values
  Find_Val = document{}
  <<"Language" <<Language
  <<"Right"    <<Term
  <<finalize;

  Cursor = db::find(Db_Client,"relations",Find_Val);
  for (view View: Cursor) {
    vector<string> Entry;

    Entry.push_back(db::get_string(View["Left"]));
    Entry.push_back(db::get_string(View["Name"]));
    Entry.push_back(db::get_string(View["Right"]));

    Results.push_back(Entry);
  }

  return Results;
}

//end of file