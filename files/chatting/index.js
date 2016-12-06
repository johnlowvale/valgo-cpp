
function send_text(event) {
  if (event.keyCode!=13)
    return;

  var Textbox = $("#Chat-Text");
  var Text    = Textbox.val().trim();

  if (Text.length==0) {
    Textbox.val("");
    return;
  }

  var Logbox = $("#Log-Box");
  var Html   = Logbox.html();

  Html += "Visitor: "+Text+"<br/>";

  Logbox.html(Html);
  Logbox.get(0).scrollTop = Number.MAX_VALUE;
  Textbox.val("");
  Textbox.focus();
}

function toggle_chatbox() {
  var Box     = $("#Chat-Box");
  var Display = Box.css("display");

  if (Display=="none")
    Box.css("display","block");
  else
    Box.css("display","none");
}

//end of file