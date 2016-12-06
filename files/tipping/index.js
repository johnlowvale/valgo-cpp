
function get_tip() {
  var Topic = $("#Topic").val().trim();

  if (Topic.length==0) {
    $("#Tip").html("Please enter topic");
    return;
  }

  $.post("http://"+location.host+"/tip",JSON.stringify({
    Text: Topic
  })).
  done(function(Data){
    var Index = Math.floor(Data.Tips.length*Math.random());
    var Tip   = Data.Tips[Index];

    $("#Tip").html(
      "<a href='"+Tip.Url+"'>"+Tip.Url+"</a><br/>"+
      Tip.Fact
    );
  }).
  fail(function(Data){
    $("#Tip").html(JSON.stringify(Data));
  });
}

$(function(){
  var Js_Href   = $("#Js-File").attr("href");
  var Css_Href  = $("#Css-File").attr("href");
  var Html_Href = $("#Html-File").attr("href");

  $("#Js-File").attr("href","view-source:"+location.host+Js_Href);
  $("#Css-File").attr("href","view-source:"+location.host+Css_Href);
  $("#Html-File").attr("href","view-source:"+location.host+Html_Href);
});

//end of file