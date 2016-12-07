/**
 * vAlgo++ Smart Algorithm Service
 * Copyright (c) Abivin JSC
 * @file    Chatbot embed javascript file
 * @version 0.1
 * @author  Dat Dinhquoc
 */

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
  var Logbox = $("#Chat-Log");
  var Html   = Logbox.html()+"Visitor: "+Text+"<br/>";

  Logbox.html(Html);
  Logbox.get(0).scrollTop = Number.MAX_VALUE;
  Textbox.val("");
  Textbox.focus();

  //send to server
  //???
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