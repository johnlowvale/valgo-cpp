/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Moment of time class header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef ENTITIES_MOMENT_HPP
#define ENTITIES_MOMENT_HPP

//standard c++ headers
#include <string>

//standard c++ namespace
using namespace std;

//namespace path
namespace Entities {

  /**
   * Moment of time class
   */
  class moment {

    //public properties
    public:
      long   Year;
      long   Month;
      long   Day;
      long   Year_Day; //since 1 Jan
      long   Hour;
      long   Minute;
      long   Second;
      long   Millisecond;
      long   Daylight_Saving; 
      long   Timezone_Offset; //in seconds      
      string Timezone_Name;
      string Week_Day;

    //public methods
    public:
      moment();
      ~moment();
  };

//namespace path  
}

//include guard end
#endif

//end of file