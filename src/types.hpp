/**
 * vAlgo Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Elemental types definitions file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

//include guard begin
#ifndef TYPES_HPP
#define TYPES_HPP

//standard c++ headers
#include <map>
#include <string>
#include <vector>

//library headers
#include <boost/any.hpp>
#include <client_http.hpp>
#include <server_http.hpp>

//namespaces being used
using namespace boost;
using namespace SimpleWeb;
using namespace std;

//positive integer types
typedef unsigned char      byte;
typedef unsigned short     word;
typedef unsigned long      dword;
typedef unsigned long long qword;
typedef unsigned __int128  oword;

//integer types
typedef char      char__;
typedef short     short__;
typedef long      long__;
typedef long long int64;
typedef __int128  int128;

//float types
typedef float      float__;
typedef double     double__;
typedef __float128 float128;

//shortcuts for templated types
typedef map<string,any> mapsa;
typedef vector<any>     veca;

//Simple-Web-Server types
typedef Server<HTTP> http_server;
typedef Client<HTTP> http_client;
typedef std::shared_ptr<http_server::Response> response;
typedef std::shared_ptr<http_server::Request>  request;
typedef std::shared_ptr<http_client::Response> http_response;

//include guard end
#endif

//end of file