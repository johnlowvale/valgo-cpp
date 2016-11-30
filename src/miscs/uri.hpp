/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    URI parser class header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef MISCS_URI_HPP
#define MISCS_URI_HPP

//standard c++ headers
#include <string>

//standard c++ namespaces
using namespace std;

//namespace path
namespace Miscs {

  /**
   * URI (having or not having protocol) class 
   */
  class uri {

    //public properties
    public:
      wstring Protocol_W, Host_W, Port_W, Path_W, Query_String_W;
      string  Protocol, Host, Port, Path, Query_String;

    //public static methods
    public:
      static uri parse(const wstring& Full_Uri);
      static uri parse(string& Full_Uri);
  };//uri

//namespace path
}

//include guard end
#endif

//end of file