/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Chatbot embed javascript file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

/**
 * Append somthing to chatlog
 */
function append_chatlog(Who,Sentence,Colour) {
  var Logbox = $("#Chat-Log");
  var Html   = Logbox.html()+
  "<div style='color:"+Colour+"'>"+Who+": "+Sentence+"</div>";

  Logbox.html(Html);
  Logbox.get(0).scrollTop = Number.MAX_VALUE;
}

/**
 * Send text to server
 */
function send_text(event) {

  //not enter? just return
  if (event.keyCode!=13)
    return;

  //get text input
  var Textbox = $("#Chat-Text");
  var Text    = Textbox.val().trim();

  //skip empty input
  if (Text.length==0) {
    Textbox.val("");
    return;
  }

  //log the input text
  append_chatlog("Visitor",Text,"gray");
  Textbox.val("");
  Textbox.focus();

  //send to server
  $.post("http://"+location.host+"/chat",JSON.stringify({
    Text:     Text,
    Language: "English"
  })).
  done(function(Data){
    append_chatlog("Chatbot",Data.Reply,"black");
  }).
  fail(function(Data){
    console.log(Data);
    append_chatlog("System","FAILED_TO_GET_REPLY","red");
  });
}

/**
 * Toggle chatbox 
 */
function toggle_chatbox() {
  var Box     = $("#Chat-Box");
  var Button  = $("#Chat-Button");
  var Display = Box.css("display");

  //toggle css display property
  if (Display=="none") {
    Box.css("display","block");
    Button.css("display","none");
  }
  else {
    Box.css("display","none");
    Button.css("display","block");
  }
}

//end of file