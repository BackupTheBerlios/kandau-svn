var httpObject
var xmlHttp
var widgets = new Array

function queryServer()
{
}
function showHint(str)
{
	if (str.length==0) {
		document.getElementById("txtHint").innerHTML=""
		return
	}
	xmlHttp=GetXmlHttpObject()
	if (xmlHttp==null) {
		alert ("Browser does not support HTTP Request")
		return
	}
	var url="http://localhost:8080/list"
	url=url+"?function="+str
	url=url+"&sid="+Math.random()
	xmlHttp.onreadystatechange=stateChanged
	xmlHttp.open("GET",url,true)
	xmlHttp.send(null)
}
function stateChanged()
{
	if (xmlHttp.readyState==4 || xmlHttp.readyState=="complete") {
		document.getElementById("txtHint").innerHTML=xmlHttp.responseText
	}
}
function remoteCall(object, func)
{
	httpObject = GetXmlHttpObject()
	if ( httpObject == null ) {
		alert( "Browser does not support HTTP Request" )
		return
	}
	var url = "http://localhost:8080/"
	url = url + object + "?function=" + func
//	alert( url )
	httpObject.onreadystatechange=remoteAnswer
	httpObject.open( "GET", url, true )
	httpObject.send( null )
}
function remoteAnswer()
{
	if ( httpObject.readyState == 4 || httpObject.readyState == "complete" ) {
		//alert( functions )
//		alert( "remoteAnswer" )
//		alert( "Msg: " + httpObject.responseText )
		functions=eval(httpObject.responseText)
//		alert( "Msg: " + httpObject.responseText )
		for (var f in functions) {
//			alert( "Loop" )
//			alert( f )
//			alert( functions[ f ] )
			eval( functions[ f ] )
		}
		return;
	}
}
function GetXmlHttpObject()
{
	var objXMLHttp=null
	if (window.XMLHttpRequest) {
		objXMLHttp=new XMLHttpRequest()
	} else if (window.ActiveXObject) {
		objXMLHttp=new ActiveXObject("Microsoft.XMLHTTP")
	}
	return objXMLHttp
}