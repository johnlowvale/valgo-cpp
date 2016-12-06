
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

//end of file