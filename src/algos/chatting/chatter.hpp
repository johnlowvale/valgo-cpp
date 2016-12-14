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

      //thread methods
      bool           is_reply_for_motivation(string Reply);
      vector<string> split_text_into_sentences(string Text);
      vector<string> get_terms_in_component(string Component);
      string         get_reply_for_sentence(string Sentence);
      string         get_reply_for_text(string Text);
      void           add_svo(string Fragment);
      void           add_sv(string Fragment);
      void           add_compounds(string Fragment);
      void           add_terms(string Fragment);
      void           add_terms_and_relations(string Text);
      void           run();
  };

//namespace path
}}

//include guard end
#endif

//end of file