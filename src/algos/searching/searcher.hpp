/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Searcher class header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef ALGOS_SEARCHING_SEARCHER_HPP
#define ALGOS_SEARCHING_SEARCHER_HPP

//library headers
#include <mongocxx/client.hpp>

//in-project headers
#include <types.hpp>

//library namespaces
using namespace mongocxx;

//namespace path
namespace Algos {
namespace Searching {

  /**
   * Searcher class
   */
  class searcher {

    //public properties
    public:
      client&   Db_Client;
      request&  Request;
      response& Response;

    //public constructor, destructor and methods
    public:
      searcher(client& Db_Client_,request& Request_,response& Response_);
      ~searcher();

      //thread method
      void run();
  };

//namespace path
}}

//include guard end
#endif

//end of file