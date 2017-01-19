/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Database operation class implementation file
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
using namespace Miscs;

//static members
string db::DB_SERVER = DB_SERVER__;
string db::DB_NAME   = DB_NAME__;

/**
 * Constructor
 */
db::db() {
  //
}

/**
 * Destructor
 */
db::~db() {
  //
}

/**
 * Initialise database
 */
client db::get_client() {

  //create connection
  uri Uri(db::DB_SERVER);
  return client(Uri);
}

/**
 * Build document from ptree
 */
value db::build_doc_from_ptree(ptree Ptree) {
  string Json_Str = utils::dump_to_json_str(Ptree);
  return bsoncxx::from_json(Json_Str);
}

/**
 * Get a collection
 */
collection db::get_collection(client& Db_Client,const char* Collection_Name) {
  return Db_Client[db::DB_NAME][Collection_Name];
}

/**
 * Get a string from mongodb element
 */
string db::get_string(element Element) {
  return Element.get_utf8().value.to_string();
}

/**
 * Get an int64 from mongodb element
 */
int64 db::get_int64(element Element) {
  return Element.get_int64().value;
}

/**
 * Get a double from mongodb element
 */
double db::get_double(element Element) {
  return Element.get_double().value;
}

/**
 * Get a date value from mongodb element
 */
int64 db::get_date(element Element) {
  return Element.get_date().to_int64();
}

/**
 * Insert one document into database
 * Notes:
 * For Couchbase:
 * - Parameter Collection_Name is NULL or optional plural form of type 
 * - Ptree contains a field named 'Type'
 */
string db::insert_one(client& Db_Client,
const char* Collection_Name,ptree Ptree) {

  //get collection
  collection Collection = Db_Client[db::DB_NAME][Collection_Name];

  //build document
  value Document = db::build_doc_from_ptree(Ptree);

  //insert into db
  auto Result = Collection.insert_one(Document.view());
  return Result->inserted_id().get_oid().value.to_string();
}

/**
 * Insert into database using mongocxx document instead of ptree
 */
string db::insert_one(client& Db_Client,
const char* Collection_Name,document& Document) {

  //get collection
  collection Collection = Db_Client[db::DB_NAME][Collection_Name];

  //insert
  value Value = Document <<finalize;

  //insert into db
  auto Result = Collection.insert_one(Value.view());
  return Result->inserted_id().get_oid().value.to_string();
}

/**
 * Create index in a collection
 */
string db::create_index(client& Db_Client,const char* Collection_Name,
const char* Index_Name,const char* Index_Info) {

  //get collection
  collection Collection = Db_Client[db::DB_NAME][Collection_Name];

  //index info doc
  value Index_Info_Doc = bsoncxx::from_json(string(Index_Info));

  //index options doc
  options::index Index_Options;
  Index_Options.name(
    bsoncxx::string::view_or_value(Index_Name)
  );

  //create index
  try {
    Collection.create_index(move(Index_Info_Doc),Index_Options);
    return OK;
  }
  catch (system_error& Error) {
    return Error.code().message();
  }
}

/**
 * Find documents in collection
 */
cursor db::find(client& Db_Client,const char* Collection_Name,
value& Value,long Limit) {
  if (Limit==0)
    return Db_Client[db::DB_NAME][Collection_Name].find(Value.view());
  else {
    options::find Options;
    Options.limit(Limit);

    return Db_Client[db::DB_NAME][Collection_Name].find(
      Value.view(),
      Options
    );
  }//else
}

/**
 * Count documents in collection
 */
int64 db::count(client& Db_Client,const char* Collection_Name,
value& Value,long Limit) {
  if (Limit==0)
    return Db_Client[db::DB_NAME][Collection_Name].count(Value.view());
  else {
    options::count Options;
    Options.limit(Limit);

    return Db_Client[db::DB_NAME][Collection_Name].count(
      Value.view(),
      Options
    );
  }//else
}

/**
 * Update one document
 */
void db::update_one(client& Db_Client,const char* Collection_Name,
value& Find_Value,value& Update_Value) {
  Db_Client[db::DB_NAME][Collection_Name].update_one(
    Find_Value.view(),Update_Value.view()
  );
}

/**
 * Upsert one document
 */
void db::upsert_one(client& Db_Client,const char* Collection_Name,
value& Find_Value,value& Update_Value) {
  options::update Options;
  Options.upsert(true);

  Db_Client[db::DB_NAME][Collection_Name].update_one(
    Find_Value.view(),Update_Value.view(),Options
  );
}

/**
 * Test inserting into db
 */
void db::test() {

  //create connection
  uri    Uri("mongodb://localhost:27017");
  client Client(Uri);

  //use a database
  database Db = Client["valgo"];

  //use a collection in db
  collection Collection = Db["tests"];

  //create a test document
  document Document = document{};
  value Value = Document
  <<"name"  <<"MongoDB"
  <<"type"  <<"database"
  <<"count" <<1
  <<"versions" 
  <<open_array
    <<"v3.2" <<"v3.0" <<"v2.6"
  <<close_array
  <<"info" 
  <<open_document
    <<"x" <<203
    <<"y" <<102
  <<close_document
  <<finalize;

  //test printing 
  view    View    = Value.view();
  element Element = View["name"];
  cout <<"\ndb::test()\n";
  cout <<"Name: " <<Element.get_utf8().value.to_string() <<"\n";

  //test insert
  auto Result = Collection.insert_one(Value.view());

  //print result
  cout <<"Id: " <<Result->inserted_id().get_oid().value.to_string() <<"\n";
  cout.flush();
}

//end of file