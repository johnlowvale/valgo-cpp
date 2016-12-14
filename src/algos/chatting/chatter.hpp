/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Chatter class header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef ALGOS_CHATTING_CHATTER_HPP
#define ALGOS_CHATTING_CHATTER_HPP

//library headers
#include <mongocxx/client.hpp>

//in-project headers
#include <types.hpp>

//library namespaces
using namespace mongocxx;

//namespace path
namespace Algos {
namespace Chatting {

  /**
   * Chatter class
   */
  class chatter {

    //constants
    public:
      static string INFORMATION_SUPPORT;

    //public reference properties
    public:
      client&   Db_Client;
      request&  Request;
      response& Response;

    //public properties
    public:
      string Motivation;

    //public constructor, destructor and methods
    public:
      chatter(client& Db_Client_,request& Request_,response& Response_);
      ~chatter();

      //thread method
      bool is_reply_for_motivation(string Reply);
      void run();
  };

//namespace path
}}

//include guard end
#endif

//end of file