/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file     Content entity class header file
 * @version  0.1
 * @author   Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_CONTENT_HPP
#define ENTITIES_CONTENT_HPP

//standard c++ headers
#include <string>
#include <vector>

//standard namespaces used
using namespace std;

//namespace path
namespace Entities {

  /**
   * Content entity class
   */
  class content {

    //public properties
    public:
      string           Id;
      string           Url;
      string           Name;    //short name
      string           Title;   //long name
      string           Extract; //html extract
      string           Html;    //full html
      vector<content*> Children;

    //public constructor and methods
    public:
      content();
      ~content();
  };
}//namespace path

//incluce guard end
#endif

//end of file