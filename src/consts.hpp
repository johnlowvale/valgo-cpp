/**
 * vAlgo++ Smart Algorithm Service Server
 * Copyright (c) Abivin JSC
 * @file    Constants file
 * @version 0.1
 * @author  Dat Dinhquoc
 *
 * Version info:
 * 0.1 Preliminary code 
 */

//include guard begin
#ifndef CONSTS_HPP
#define CONSTS_HPP

//server constants
//todo: fix g++ error 'expected unqualified-id' when these constants
//have the same names with static properties in server.hpp/server.cpp
#define VERSION__       "0.1"
#define PORT__          8891
#define CRAWLER_COUNT__ 2
#define DB_SERVER__     "mongodb://localhost:27017"
#define DB_NAME__       "valgo"

//number constants
#define ONE_HOUR_MILLI (60*60*1000)
#define EXTRACT_LENGTH 100

//string constants
#define OK                     string("OK")
#define FAILED                 string("FAILED")
#define FAILED_TO_ADD_DOCUMENT string("FAILED_TO_ADD_DOCUMENT")
#define INVALID                string("INVALID")

//json constants
#define JSON_MESSAGE_OK    string("{\"Message\":\"OK\"}")
#define JSON_ERROR_GENERIC string("{\"Error\":\"GENERIC_ERROR\"}")

//include guard end
#endif

//end of file