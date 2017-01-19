/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Tipper class header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef ALGOS_TIPPING_TIPPER_HPP
#define ALGOS_TIPPING_TIPPER_HPP

//library headers
#include <mongocxx/client.hpp>

//in-project headers
#include <types.hpp>

//library namespaces
using namespace mongocxx;

//namespace path
namespace Algos {
namespace Tipping {

  /**
   * Tipper class
   */
  class tipper {

    //public reference properties
    public:
      client&   Db_Client;
      request&  Request;
      response& Response;

    //public properties
    public:
      long Tip_Count;

    //public constructor, destructor and methods
    public:
      tipper(client& Db_Client_,request& Request_,response& Response_);
      ~tipper();

      //thread method
      vector<string> get_facts_from_text(string Term,string Text);
      vector<string> get_facts_from_html(string Term,string Html);
      void           run();
  };

//namespace path
}}

//include guard end
#endif

//end of file