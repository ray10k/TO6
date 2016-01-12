function Login() 
{
	window.open("Main.html", "_self");
}

function Login_Message()
{
	var text = "Hallo " + "ws.getUsername()";
	Print(text, "Main");
	Print("ws.getPassword()", "Main");
}
  
function Print(message, id, type) 
{
	if(type == undefined)
	{
		type = "div";
	}
	var node=document.createElement(type);
	var textnode=document.createTextNode(message);
	node.appendChild(textnode);
	document.getElementById(id).appendChild(node);
}
