/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Web location entity class implementation file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//library headers
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>

//in-project headers
#include <consts.hpp>
#include <entities/moment.hpp>
#include <entities/webloc.hpp>
#include <miscs/db.hpp>
#include <miscs/utils.hpp>

//standard c++ namespaces
using namespace std;
using namespace std::chrono;

//library namespaces
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::document;
using namespace mongocxx;
using namespace mongocxx::result;
using bsoncxx::from_json;
using bsoncxx::to_json;
using bsoncxx::types::b_date;

//in-project namespaces used
using namespace Entities;
using namespace Miscs;

/**
 * Constructor
 */
webloc::webloc() {
  //
}

/**
 * Constructor with data
 */
webloc::webloc(string Full_Url,double Revisit_Time) {
  this->Id           = Full_Url;
  this->Full_Url     = Full_Url;
  this->Revisit_Time = Revisit_Time;

  utils::parse_url(
    Full_Url,
    this->Protocol,this->Domain_Name,this->Port,this->Path,this->Query_String
  ); 

  if (this->Domain_Name.length()>0)
    this->Is_Valid = true;
  else
    this->Is_Valid = false;
}

/** 
 * Destructor
 */
webloc::~webloc() {
  //
}

/**
 * Save to db
 * @return "OK" if successful, a json string representing the error otherwise
 */
string webloc::save_to_db(client& Db_Client,int64 Crawl_At) {

  //current time
  int64 Millisecond = utils::milliseconds_since_epoch();

  //let it be crawled in the next hour
  Millisecond += Crawl_At;

  //make the webloc document
  document Document;
  Document
  <<"_id"          <<this->Id
  <<"Protocol"     <<this->Protocol
  <<"Domain_Name"  <<this->Domain_Name
  <<"Port"         <<this->Port
  <<"Path"         <<this->Path
  <<"Query_String" <<this->Query_String
  <<"Full_Url"     <<this->Full_Url
  <<"Revisit_Time" <<this->Revisit_Time
  <<"Revisit_At"   <<b_date(milliseconds(Millisecond))
  <<"Added_At"     <<utils::now_as_bdate()
  <<"Visit_Count"  <<0
  <<"Htmls" 
  <<open_document
  <<close_document;

  //insert
  try {
    db::insert_one(Db_Client,"weblocs",Document);
    return OK;
  }
  catch (bulk_write_exception& Exception) {
    //return Exception.code().message();
    //return FAILED_TO_ADD_DOCUMENT;
    return Exception.raw_server_error().value().
           view()["writeErrors"][0]["errmsg"].
           get_utf8().value.to_string();
  }
}

//end of file