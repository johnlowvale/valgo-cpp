/**
 * vAlgo Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Miscellaneous utilities class file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef MISCS_UTILS_HPP
#define MISCS_UTILS_HPP

//standard c++ headers
#include <iostream>
#include <string>
#include <time.h>

//library headers
#include <client_http.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <bsoncxx/types.hpp>
#include <uriparser/Uri.h>

//in-project headers
#include <types.hpp>
#include <entities/moment.hpp>

//standard c++ namespaces being used
using namespace std;

//library namespaces being used
using namespace boost;
using namespace boost::property_tree;
using namespace SimpleWeb;
using bsoncxx::types::b_date;

//in-project namespaces
using namespace Entities;

//namespace path
namespace Miscs {

  /**
   * Miscellaneous utilities class
   */
  class utils {

    //public methods
    public:

      //parse and dump json
      static ptree  parse_json_str(string Json_Str);
      static string dump_to_json_str(ptree Prop_Tree);
      static string make_json_str(const char* Left,string Right);

      //boost property tree
      static bool is_empty_node(ptree Ptree);
      static bool is_parent_node(ptree Ptree);
      static bool is_object_node(ptree Ptree);
      static bool is_array_node(ptree Ptree);
      static bool is_leaf_node(ptree Ptree);

      //mongodb related
      static b_date now_as_bdate();
      static moment now();
      static long   get_timezone_offset();
      static int64  milliseconds_since_epoch();

      //http request and response related
      static string get_request_header(request Request,const char* Header);
      static string get_request_content(request Request);
      static ptree  get_request_content_ptree(request Request);
      static void   send_text(response Response,string Text);
      static void   send_json(response Response,string Json_Str);
      static string read_buffer(http_response Response);
      static string http_get(string Server,string Path);
      static string http_get(const char* Server,const char* Path);
      static string http_post(string Server,string Path,string Body);

      //url & html
      static void    to_lower(string& Str);
      static string  wstring_to_string(wstring& Wstring);
      static wstring string_to_wstring(string& String);
      static string  path_segments_to_str(UriPathSegmentA* Segment);
      static void    parse_url(string Full_Url,string& Protocol,
                     string& Domain_Name,long& Port,string& Path,
                     string& Query_String);
      static ptree   parse_html(string Html);

      //miscellaneous methods
      static void   print(char const* Text);
      static void   print(string Text);
      static void   print_request(request Request);
      static double random();

      //file operations
      static string read_file(string Path);
      static string get_current_dir();
  };//class

}//namespace path

//include guard end
#endif

//end of file