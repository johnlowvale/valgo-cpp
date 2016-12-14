/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Node entity class implementation file
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
node::node(string Id) {
  this->Id = Id;
}

/** 
 * Destructor
 */
node::~node() {
  //
}

/**
 * Add relation
 */
void node::add_relation(relation* Relation) {
  this->Relations.push_back(Relation);
}

/**
 * Save to db
 */
void node::save_to_db(client& Db_Client) {
  document Document;
  Document 
  <<"_id"     <<this->Id
  <<"Id"      <<this->Id
  <<"Content" <<"";

  //relations array
  auto Array = Document <<"Relations" <<open_array;

  //loop thru all relations
  for (relation* Relation: this->Relations) {

    //relation is directed relation from left to right
    //no the other way round
    if (Relation->Left->Id==this->Id) {
      Array
      <<open_document
        <<"Relation" <<Relation->Id
        <<"Node"     <<Relation->Right->Id
      <<close_document;
    }//if
  }//for

  //close array
  Array <<close_array;

  db::insert_one(Db_Client,"nodes",Document);
}

//end of file