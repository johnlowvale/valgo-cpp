/**
 * vAlgo++ UI (for testing purpose)
 * Copyright (c) Abivin JSC
 * @file    Main browser-side code file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

/**
 * Check valid url
 * http://stackoverflow.com/questions/8667070/
 * javascript-regular-expression-to-validate-url
 */
function is_valid_url(Url) {
  var Str = 
  "^(https?|ftp):\\/\\/(((([a-z]|\\d|-|\\.|_|~|[\\u00A0-\\uD7FF"+
  "\\uF900-\\uFDCF\\uFDF0-\\uFFEF])|(%[\\da-f]{2})|[!\\$&'\\(\\)"+
  "\\*\\+,;=]|:)*@)?(((\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])"+
  "\\.(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|[1-9]\\d|1"+
  "\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|[1-9]\\d|1\\d\\d|2[0-4]"+
  "\\d|25[0-5]))|((([a-z]|\\d|[\\u00A0-\\uD7FF\\uF900-\\uFDCF"+
  "\\uFDF0-\\uFFEF])|(([a-z]|\\d|[\\u00A0-\\uD7FF\\uF900-\\uFDCF"+
  "\\uFDF0-\\uFFEF])([a-z]|\\d|-|\\.|_|~|[\\u00A0-\\uD7FF\\uF900-"+
  "\\uFDCF\\uFDF0-\\uFFEF])*([a-z]|\\d|[\\u00A0-\\uD7FF\\uF900-"+
  "\\uFDCF\\uFDF0-\\uFFEF])))\\.)+(([a-z]|[\\u00A0-\\uD7FF\\uF900-"+
  "\\uFDCF\\uFDF0-\\uFFEF])|(([a-z]|[\\u00A0-\\uD7FF\\uF900-"+
  "\\uFDCF\\uFDF0-\\uFFEF])([a-z]|\\d|-|\\.|_|~|[\\u00A0-\\uD7FF"+
  "\\uF900-\\uFDCF\\uFDF0-\\uFFEF])*([a-z]|[\\u00A0-\\uD7FF"+
  "\\uF900-\\uFDCF\\uFDF0-\\uFFEF])))\\.?)(:\\d*)?)(\\/((([a-z]|"+
  "\\d|-|\\.|_|~|[\\u00A0-\\uD7FF\\uF900-\\uFDCF\\uFDF0-"+
  "\\uFFEF])|(%[\\da-f]{2})|[!\\$&'\\(\\)\\*\\+,;=]|:|@)+("+
  "\\/(([a-z]|\\d|-|\\.|_|~|[\\u00A0-\\uD7FF\\uF900-\\uFDCF"+
  "\\uFDF0-\\uFFEF])|(%[\\da-f]{2})|[!\\$&'\\(\\)\\*"+
  "\\+,;=]|:|@)*)*)?)?(\\?((([a-z]|\\d|-|\\.|_|~|[\\u00A0-"+
  "\\uD7FF\\uF900-\\uFDCF\\uFDF0-\\uFFEF])|(%[\\da-f]{2})|[!"+
  "\\$&'\\(\\)\\*\\+,;=]|:|@)|[\\uE000-\\uF8FF]|\\/|\\?)*)?("+
  "\\#((([a-z]|\\d|-|\\.|_|~|[\\u00A0-\\uD7FF\\uF900-\\uFDCF"+
  "\\uFDF0-\\uFFEF])|(%[\\da-f]{2})|[!\\$&'\\(\\)\\*\\+,;=]|:|@)|"+
  "\\/|\\?)*)?$";

  var Regex = new RegExp(Str,"i");
  return Regex.test(Url);
}

/**
 * Add new web location
 */
function add_web_location() {
  var Full_Url     = $("#Nwl-Url").val();
  var Revisit_Time = $("#Nwl-Revisit-Time").val();

  //check if url is valid
  if (!is_valid_url(Full_Url)) {
    alert("URL is invalid!");
    return;
  }

  //check if revisit time is number
  if (!$.isNumeric(Revisit_Time) || Revisit_Time<=0) {
    alert("Revisit time must be positive number");
    return;
  }

  //send to server
  $.post("http://localhost:8891/webloc/add",JSON.stringify({
    Full_Url:     Full_Url,
    Revisit_Time: parseInt(Revisit_Time)
  })).
  done(function(Data){
    if (Data.Error) {
      alert("Error: "+JSON.stringify(Data.Error));
      return;
    }

    alert("Web location added successfully!");
  }).
  fail(function(Data){
    alert("Error: "+JSON.stringify(Data));
  });
}

/**
 * Crawl a web location immediately
 */
function crawl_web_location() {
  var Full_Url     = $("#Nwl-Url").val();
  var Revisit_Time = $("#Nwl-Revisit-Time").val();

  //check if url is valid
  if (!is_valid_url(Full_Url)) {
    alert("URL is invalid!");
    return;
  }

  //check if revisit time is number
  if (!$.isNumeric(Revisit_Time) || Revisit_Time<=0) {
    alert("Revisit time must be positive number");
    return;
  }

  //send to server
  $.post("http://localhost:8891/webloc/crawl",JSON.stringify({
    Full_Url:     Full_Url,
    Revisit_Time: parseInt(Revisit_Time)
  })).
  done(function(Data){
    if (Data.Error) {
      alert("Error: "+JSON.stringify(Data.Error));
      return;
    }

    alert("Web location queued!");
  }).
  fail(function(Data){
    alert("Error: "+JSON.stringify(Data));
  });
}

/**
 * Get crawlers' statuses
 */
function get_crawlers_statuses() {
  $.post("http://localhost:8891/crawlers/statuses",JSON.stringify({})).
  done(function(Data){
    if (Data.Error) {
      alert("Error: "+JSON.stringify(Data.Error));
      return;
    }

    var Crawlers = Data.Crawlers;
    var Html     = "";
    for (var Index=0; Index<Crawlers.length; Index++) {
      var Crawler = Crawlers[Index];
      var Tr      = "";

      Tr += "<tr>";
      Tr += "<td>"+Crawler.Index+"</td>";
      Tr += "<td>"+Crawler.Queue_Length+"</td>";
      Tr += "<td><a target='_blank' href='"+Crawler.Current_Url+"'>"+
            Crawler.Current_Url+"</a></td>";
      Tr += "</tr>";

      Html += Tr;
    }

    $("#Last-Revive-Count").html(Data.Reviver.Last_Count);
    $("#Last-Dist-Count").html(Data.Distributor.Last_Count);
    $("#Crawler-Statuses").html(Html);
  }).
  fail(function(Data){
    alert("Error: "+JSON.stringify(Data));
  });
}

/**
 * Clear crawlers' queues
 */
function clear_crawlers_queues() {
  $.post("http://localhost:8891/crawlers/queues/clear",JSON.stringify({})).
  done(function(Data){
    if (Data.Error) {
      alert("Error: "+JSON.stringify(Data.Error));
      return;
    }

    alert("All crawlers' queues cleared!");    
  }).
  fail(function(Data){
    alert("Error: "+JSON.stringify(Data));
  });
}

/**
 * Search for text
 */
function search_for_text() {
  var Text = $("#Search-Text").val().trim();
  if (Text.length==0) {
    alert("Please type something to search!");
    return;
  }

  $.post("http://localhost:8891/search",JSON.stringify({
    Text: Text
  })).
  done(function(Data){
    if (Data.Error) {
      alert("Error: "+JSON.stringify(Data.Error));
      return;
    }

    //build results html
    var Contents = Data.Contents;
    var Box      = $("#Search-Result-Box");
    Box.html("");

    if (Contents.length==0)
      Box.html("No results");

    //make results
    for (var Index=0; Index<Contents.length; Index++) {
      var Content = Contents[Index];
      var Title   = Content.Title;
      var Url     = Content.Url;
      var Extract = Content.Extract;
      var Html    = Content.Html;
      var Hilight = "";

      try {
        Hilight = $(Html).text();
      }
      catch (Error) {
        Hilight = Html;
      }
    
      //words in search text
      var Words  = [];
      var Tokens = Text.split(" ");
      for (var Jndex=0; Jndex<Tokens.length; Jndex++) 
        if (Tokens[Jndex].length>0)
          Words.push(Tokens[Jndex]);

      //highlight words in results
      for (var Jndex=0; Jndex<Words.length; Jndex++) {
        var Word = Words[Jndex];
        Hilight = Hilight.replace(
          new RegExp(Word,"gi"),
          "<b style='color:red;'>"+Word+"</b>"
        );
      }  

      //shorten highlight text
      var Shortened_Hilight = "";
      for (var Jndex=0; Jndex<Words.length; Jndex++) {
        var Word = Words[Jndex];

        for (var Kndex=0; Kndex<Hilight.length; Kndex++)
          if (Hilight[Kndex]=='<' && Hilight[Kndex+1]=='b')
            Shortened_Hilight += "..."+Hilight.substr(Kndex-50,100)+"...";
      }

      //no title, extract
      if (Title.length==0)
        Title = Url;
      if (Extract.length==0)
        Extract = "(no extract)";

      //html entry
      var Result = "";
      Result += "<div style='margin-bottom:20px; line-height:20px;'>";
      Result += "<a target='_blank' href='"+Url+"'>"+Title+"</a><br/>";
      Result += Extract+"<br/>";
      Result += Shortened_Hilight;
      Result += "</div>";

      Box.html(Box.html()+Result);
    }
  }).
  fail(function(Data){
    alert("Error: "+JSON.stringify(Data));
  });
}

/**
 * Clear search result
 */
function clear_search_results() {
  $("#Search-Result-Box").
  html("Type some text to search and get results here!");
}

/**
 * Check enter key to do searching
 */
function check_enter_key(Event) {
  if (Event.keyCode==13)
    search_for_text();
}

/**
 * Entry point of web application
 */
$(function(){

  //get server status
  $.get("http://localhost:8891").
  done(function(Data){
    $("#Server-Status").html(Data);

    //get crawlers' statuses after every 5s
    setInterval(get_crawlers_statuses,5*1000);
  }).
  fail(function(Data){
    //$("#Server-Status").html(JSON.stringify(Data));
    $("#Server-Status").html("FAILED_TO_CONNECT_TO_VALGO_CPP");
  });
});

//end of file