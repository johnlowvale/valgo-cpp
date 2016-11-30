/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    URI parser class implementation file
 * @version 0.1
 * @author  http://stackoverflow.com/users/882436/tom
 * @updater Dat Dinhquoc
 */

//standard c++ headers
#include <string>
#include <algorithm>

//in-project headers
#include <miscs/uri.hpp>
#include <miscs/utils.hpp>

//standard c++ namespaces
using namespace std;

//in-project namespaces
using namespace Miscs;

/**
 * Parse a URI into parts  (wide string)
 */
uri uri::parse(const wstring& Full_Uri) { 
  uri result;

  typedef std::wstring::const_iterator iterator_t;

  if (Full_Uri.length() == 0)
    return result;

  iterator_t uriEnd = Full_Uri.end();

  //get query start
  iterator_t queryStart = std::find(Full_Uri.begin(), uriEnd, L'?');

  //protocol
  iterator_t protocolStart = Full_Uri.begin();
  iterator_t protocolEnd = std::find(protocolStart, uriEnd, L':'); //"://");

  if (protocolEnd != uriEnd) {
    std::wstring prot = &*(protocolEnd);
    if ((prot.length() > 3) && (prot.substr(0, 3) == L"://")) {
      result.Protocol_W = std::wstring(protocolStart, protocolEnd);
      protocolEnd += 3; //://
    }
    else
      protocolEnd = Full_Uri.begin(); //no protocol
  }
  else
    protocolEnd = Full_Uri.begin(); //no protocol

  //host
  iterator_t hostStart = protocolEnd;
  iterator_t pathStart = std::find(hostStart, uriEnd, L'/'); //get pathStart

  iterator_t hostEnd = std::find(protocolEnd, 
    (pathStart != uriEnd) ? pathStart : queryStart,
    L':'
  );  //check for port

  result.Host_W = std::wstring(hostStart, hostEnd);

  //port
  if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == L':')) {
    hostEnd++;
    iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
    result.Port_W = std::wstring(hostEnd, portEnd);
  }

  //path
  if (pathStart != uriEnd)
    result.Path_W = std::wstring(pathStart, queryStart);

  //query
  if (queryStart != uriEnd)
    result.Query_String_W = std::wstring(queryStart, Full_Uri.end());

  return result;
}

/**
 * Parse URI into parts (single-byte string)
 */
uri uri::parse(string& Full_Uri) {
  uri Uri = uri::parse(utils::string_to_wstring(Full_Uri));

  Uri.Protocol     = utils::wstring_to_string(Uri.Protocol_W);
  Uri.Host         = utils::wstring_to_string(Uri.Host_W);
  Uri.Port         = utils::wstring_to_string(Uri.Port_W);
  Uri.Path         = utils::wstring_to_string(Uri.Path_W);
  Uri.Query_String = utils::wstring_to_string(Uri.Query_String_W);

  return Uri;
}

//end of file