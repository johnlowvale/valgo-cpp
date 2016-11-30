/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Crawler class header file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef TASKS_CRAWLER_HPP
#define TASKS_CRAWLER_HPP

//standard c++ headers
#include <string>

//library headers
#include <htmlcxx/html/ParserDom.h>
#include <mongocxx/client.hpp>

//in-project headers
#include <entities/webloc.hpp>

//standard c++ namespace
using namespace std;

//library namespaces
using namespace htmlcxx;
using namespace htmlcxx::HTML;
using namespace mongocxx;

//in-project namespaces being used
using namespace Entities;

//namespace path
namespace Tasks {

  /**
   * Crawler class
   */
  class crawler {

    //public properties
    public:
      long                Thread_Index;
      client              Db_Client;
      map<string,webloc*> Queue;
      bool                Is_Queuing;

    //public constructors and methods
    public:
      crawler();
      crawler(long Thread_Index);

      void   clear_queue();
      bool   is_not_queued(webloc* Webloc);
      void   reschedule_webloc(webloc* Webloc);
      string get_inner_text(const tree<Node>& Dom,
             const tree<Node>::iterator& Parent);
      void   crawl_the_queue();
      void   run();
  };
}//namespace path

//include guard end
#endif

//end of file