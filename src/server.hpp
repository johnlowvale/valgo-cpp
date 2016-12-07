/**
 * vAlgo Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Main server class file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef SERVER_HPP
#define SERVER_HPP

//standard c++ headers
#include <string>
#include <vector>

//library headers
#include <server_http.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

//in-project headers
#include <types.hpp>
#include <entities/webloc.hpp>
#include <tasks/crawler.hpp>

//standard c++ namespace
using namespace std;

//library namespaces
using namespace mongocxx;

//in-project namespaces
using namespace Entities;
using namespace Tasks;

/**
 * Main server class
 */
class server {

  //public constants
  public:
    static string VERSION;       //server version
    static long   PORT;          //server port
    static long   CRAWLER_COUNT; //number of crawlers

  //public static properties
  public:
    static server* Singleton;  

  //public properties
  public:
    http_server*     Http_Server;
    client           Db_Client;
    client           Db_Client_Reviver;
    client           Db_Client_Distributor;
    vector<crawler*> Crawlers;
    long             Last_Revive_Count;
    long             Last_Distribution_Count;

  //public constructor and methods
  public:
    server();
    ~server();

    //url handlers
    static void handle_get_root(response Response,request Request);
    static void handle_get_admin(response Response,request Request);
    static void handle_get_file(response Response,request Request);
    static void handle_post_webloc_add(response Response,request Request);
    static void handle_post_webloc_crawl(response Response,request Request);
    static void handle_post_crawlers_statuses(
                response Response,request Request);
    static void handle_post_crawlers_queues_clear(
                response Response,request Request);
    static void handle_post_search(response Response,request Request);
    static void handle_post_tip(response Response,request Request);

    //methods
    void create_indices();
    void test_neunet();
    void initialise();
    void update_past_weblocs();
    bool is_not_queued(webloc* Webloc);
    void queue_weblocs();
    void queue_single_webloc(webloc* Webloc);
    void run();
};

//include guard end
#endif

//end of file