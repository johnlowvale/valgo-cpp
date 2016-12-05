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

    //public properties
    public:
      client&   Db_Client;
      request&  Request;
      response& Response;

    //public constructor, destructor and methods
    public:
      tipper(client& Db_Client_,request& Request_,response& Response_);
      ~tipper();

      //thread method
      void run();
  };

//namespace path
}}

//include guard end
#endif

//end of file