/**
 * vAlgo Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Miscellaneous utilities class file
 * @version 0.1
 * @author  Dat Dinhquoc
 */
#ifndef _GNU_SOURCE 
#define _GNU_SOURCE //for tm_gmtoff and tm_zone
#endif

//standard c++ headers
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>

//library headers
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <bsoncxx/types.hpp>
#include <uriparser/Uri.h>

//in-project headers
#include <entities/moment.hpp>
#include <miscs/utils.hpp>

//standard c++ namespaces
using namespace std;
using namespace std::chrono;

//library namespaces
using namespace boost;
using namespace boost::property_tree;
using bsoncxx::types::b_date;

//in-project namespaces
using namespace Entities;
using namespace Miscs;

/**
 * Parse a JSON string into Boost property tree
 */
ptree utils::parse_json_str(string Json_Str) {

  //create a string stream of Json_Str
  stringstream Str_Stream;
  Str_Stream <<Json_Str.c_str();

  //use Boost to parse json string
  ptree Prop_Tree;
  read_json(Str_Stream,Prop_Tree);

  return Prop_Tree;
}

/**
 * Dump property tree to JSON string
 */
string utils::dump_to_json_str(ptree Prop_Tree) {

  //create a string stream to hold result of write_json
  stringstream Str_Stream;
  
  //use Boost to generate json string from property tree
  write_json(Str_Stream,Prop_Tree,true);

  return Str_Stream.str();
}

/**
 * Make json string {"Left":"Right"}
 */
string utils::make_json_str(const char* Left,string Right) {
  string Key(Left);

  if (Right[0]=='{')
    return string("{\"")+Key+string("\":")+Right+string("}");
  else {    
    replace_all(Right,"\"","\\\"");
    return string("{\"")+Key+string("\":\"")+Right+string("\"}");
  }
}

/**
 * Check if a ptree is empty
 */
bool utils::is_empty_node(ptree Ptree) {
  return Ptree.empty();
}

/**
 * Check if a ptree is parent node
 */
bool utils::is_parent_node(ptree Ptree) {
  if (!Ptree.empty())
    return true;
  else
    return false;
}

/**
 * Check if a ptree is object node
 */
bool utils::is_object_node(ptree Ptree) {
  if (!utils::is_parent_node(Ptree))
    return false;

  //get first key
  string Key;
  for (auto Iterator: Ptree) {
    Key = Iterator.first;
    break;
  }

  //non-empty key means an object
  if (Key.length()>0)
    return true;
  else
    return false;
}

/**
 * Check if a ptree is array node
 */
bool utils::is_array_node(ptree Ptree) {
  if (!utils::is_parent_node(Ptree))
    return false;

  //get first key
  string Key;
  for (auto Iterator: Ptree) {
    Key = Iterator.first;
    break;
  }

  //empty key means an array
  if (Key.length()==0)
    return true;
  else
    return false;
}

/**
 * Check if a ptree is leaf node 
 */
bool utils::is_leaf_node(ptree Ptree) {
  if (Ptree.empty())
    return true;
  else
    return false;
}

/**
 * Get time now as bsoncxx b_date
 */
b_date utils::now_as_bdate() {
  return b_date(system_clock::now());
}

/**
 * Get time now (using GNU tm struct with tm_gmtoff, tm_zone)
 */
moment utils::now() {
  time_t    Time        = time(NULL);
  struct tm Time_Struct = {0};

  //get local time
  localtime_r(&Time,&Time_Struct);

  //create moment instance
  moment Moment;
  Moment.Year            = Time_Struct.tm_year+1900;
  Moment.Month           = Time_Struct.tm_mon+1;
  Moment.Day             = Time_Struct.tm_mday;
  Moment.Year_Day        = Time_Struct.tm_yday+1;
  Moment.Hour            = Time_Struct.tm_hour;
  Moment.Minute          = Time_Struct.tm_min;
  Moment.Second          = Time_Struct.tm_sec;
  Moment.Millisecond     = 0;
  Moment.Daylight_Saving = Time_Struct.tm_isdst;
  Moment.Timezone_Offset = Time_Struct.tm_gmtoff;
  Moment.Timezone_Name   = string(Time_Struct.tm_zone);

  //week day names
  if (Time_Struct.tm_wday==0) 
    Moment.Week_Day = string("Sunday");
  else
  if (Time_Struct.tm_wday==1)
    Moment.Week_Day = string("Monday");
  else
  if (Time_Struct.tm_wday==2)
    Moment.Week_Day = string("Tuesday");
  else
  if (Time_Struct.tm_wday==3)
    Moment.Week_Day = string("Wednesday");
  else
  if (Time_Struct.tm_wday==4)
    Moment.Week_Day = string("Thursday");
  else
  if (Time_Struct.tm_wday==5)
    Moment.Week_Day = string("Friday");
  else
  if (Time_Struct.tm_wday==6)
    Moment.Week_Day = string("Saturday");
  else
    Moment.Week_Day = string("?");

  return Moment;
}

/**
 * Get system timezone offset in seconds from GMT
 */
long utils::get_timezone_offset() {
  time_t    Time = time(NULL);
  struct tm Time_Struct = {0};

  //get local time
  localtime_r(&Time,&Time_Struct);

  //timezone offset in seconds
  return Time_Struct.tm_gmtoff;
}

/**
 * Get milliseconds from unix epoch 0:0:0 1-Jan-1970 GMT
 */
int64 utils::milliseconds_since_epoch() {
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).
         count();
}


/**
 * Print and flush
 */
void utils::print(char const* Text) {
  cout <<Text;
  cout.flush();
}

/**
 * Print and flush (overload)
 */
void utils::print(string Text) {
  cout <<Text;
  cout.flush();
}

/**
 * Print request info
 */
void utils::print_request(request Request) {
  cout <<"\n" <<Request->remote_endpoint_address <<" ";
  cout <<Request->method <<" " <<Request->path <<"\n";
  cout.flush();
}

/**
 * Get http request header
 */
string utils::get_request_header(request Request,const char* Header) {
  return Request->header.find(Header)->second;
}

/**
 * Get http request content
 */
string utils::get_request_content(request Request) {
  return Request->content.string();
}

/**
 * Get http request content as ptree
 */
ptree utils::get_request_content_ptree(request Request) {
  string Content = Request->content.string();
  return utils::parse_json_str(Content);
}

/**
 * Send a text as response
 */
void utils::send_text(response Response,string Text) {
  *Response <<"HTTP/1.1 200 OK\r\n";
  *Response <<"Access-Control-Allow-Origin: *\r\n";
  *Response <<"Content-Length: " <<Text.length() <<"\r\n";
  *Response <<"\r\n";
  *Response <<Text;
}

/**
 * Send a json string as response
 */
void utils::send_json(response Response,string Json_Str) {
  *Response <<"HTTP/1.1 200 OK\r\n";
  *Response <<"Access-Control-Allow-Origin: *\r\n";
  *Response <<"Content-Type: application/json\r\n";
  *Response <<"Content-Length: " <<Json_Str.length() <<"\r\n";
  *Response <<"\r\n";
  *Response <<Json_Str;
}

/**
 * Read buffer to string
 */
string utils::read_buffer(http_response Response) {
  stringstream Stream;
  Stream <<Response->content.rdbuf();

  return Stream.str();
}

/**
 * Do HTTP GET and return content
 */
string utils::http_get(string Server,string Path) {
  http_client Client(Server);
  http_response Response = Client.request("GET",Path);

  return utils::read_buffer(Response);
}

/**
 * HTTP GET (overload)
 */
string utils::http_get(const char* Server,const char* Path) {
  return utils::http_get(string(Server),string(Path));
}

/**
 * Do HTTP POST and return content
 */
string utils::http_post(string Server,string Path,string Body) {
  string Result = "";
  return Result;
}

/**
 * Convert string to lower case
 */
void utils::to_lower(string& Str) {
  to_lower(Str); //boost::to_lower
}

/**
 * Convert wstring to string
 * http://stackoverflow.com/questions/15333259/
 * c-stdwstring-to-stdstring-quick-and-dirty-conversion-for-use-as-key-in
 */
string utils::wstring_to_string(wstring& Wstring) { 
  string Str(
    (const char*)&Wstring[0], 
    sizeof(wchar_t)/sizeof(char)*Wstring.size()
  );

  return Str;
}

/**
 * Convert string to wstring
 * http://stackoverflow.com/questions/15333259/
 * c-stdwstring-to-stdstring-quick-and-dirty-conversion-for-use-as-key-in
 */
wstring utils::string_to_wstring(string& String) {
  wstring Wstr(
    (const wchar_t*)&String[0], 
    sizeof(char)/sizeof(wchar_t)*String.size()
  );

  return Wstr;
}

/**
 * Uri path segments to string
 * See example by Elliot Cameron:
 * http://stackoverflow.com/questions/2616011/
 * easy-way-to-parse-a-url-in-c-cross-platform
 */
string utils::path_segments_to_str(UriPathSegmentA* Segment) {
  string                 Delim("/");
  string                 Accum; 
  UriPathSegmentStructA* Head(Segment);

  while (Head) {
    Accum += Delim+string(Head->text.first,Head->text.afterLast); 
    Head   = Head->next;
  }

  return Accum;
}

/**
 * Parse url (including protocol) into parts
 * See example by Elliot Cameron:
 * http://stackoverflow.com/questions/2616011/
 * easy-way-to-parse-a-url-in-c-cross-platform
 */
void utils::parse_url(string Full_Url,string& Protocol,string& Domain_Name,
long& Port,string& Path,string& Query_String) {
  UriParserStateA State;
  UriUriA         Uri;

  //parse
  State.uri = &Uri;
  if (uriParseUriA(&State,Full_Url.c_str())!=URI_SUCCESS) {
    uriFreeUriMembersA(&Uri);

    Protocol.clear();
    Domain_Name.clear();
    Port = 0;
    Path.clear();
    Query_String.clear();

    return;
  }

  //for conversion to integer
  string Port_Str; 

  //get members
  Protocol     = string(Uri.scheme.first,Uri.scheme.afterLast);
  Domain_Name  = string(Uri.hostText.first,Uri.hostText.afterLast);
  Port_Str     = string(Uri.portText.first,Uri.portText.afterLast);
  Path         = utils::path_segments_to_str(Uri.pathHead);
  Query_String = string(Uri.query.first,Uri.query.afterLast);
  
  if (Port_Str.length()==0) {
    if (Protocol=="http")
      Port = 80;
    else
    if (Protocol=="https")
      Port = 443;
    else
      Port = 0;
  }
  else
    Port = stol(Port_Str);

  //free resources
  uriFreeUriMembersA(&Uri);
}

/**
 * Parse html to Boost ptree
 */
ptree utils::parse_html(string Html) {
  ptree Ptree;
  return Ptree;
}

//end of file