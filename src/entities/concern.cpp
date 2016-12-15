/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Node concern entity class implementation file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//library headers
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>

//in-project headers
#include <types.hpp>
#include <entities/node.hpp>
#include <entities/concern.hpp>
#include <miscs/db.hpp>

//library namespaces
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::document;
using namespace mongocxx;
using namespace mongocxx::result;

//in-project namespaces used
using namespace Entities;
using namespace Miscs;

/**
 * Constructor
 */
concern::concern(string Ai_Name,node* Node,double Importance) {
  this->Id         = Ai_Name+" "+Node->Id;
  this->Ai_Name    = Ai_Name;
  this->Node       = Node;
  this->Importance = Importance;
}

/** 
 * Destructor
 */
concern::~concern() {
  //
}

/**
 * Save to db
 */
void concern::save_to_db(client& Db_Client) {
  document Document;
  Document 
  <<"_id"        <<this->Id
  <<"Id"         <<this->Id
  <<"Ai_Name"    <<this->Ai_Name
  <<"Node"       <<this->Node->Id
  <<"Importance" <<this->Importance;

  db::insert_one(Db_Client,"concerns",Document);
}

/**
 * Save or update to db
 */
void concern::save_or_update_to_db(client& Db_Client) {
  value Find_Val = document{}
  <<"_id" <<this->Id
  <<finalize;

  value Upsert_Val = document{}
  <<"$set"
  <<open_document
    <<"Id"         <<this->Id
    <<"Ai_Name"    <<this->Ai_Name
    <<"Node"       <<this->Node->Id
    <<"Importance" <<this->Importance
  <<close_document
  <<finalize;

  db::upsert_one(Db_Client,"concerns",Find_Val,Upsert_Val);
}

//end of file