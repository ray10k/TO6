function Load(html) 
{
	window.open(html, "_self");
};

function Login(html) 
{
	Print("Login...", "Login");
	setTimeout(function(){Load(html);},1000);
};

function Logout(html) 
{
	Print("Logout...", "Main");
	setTimeout(function(){Load(html);},500);
}

function CreateUserProfile(html)
{
	Print("Creating...", "CreateUserProfile");
	setTimeout(function(){Load(html);},1000);
}

function CreateWashingCycle(html)
{
	var WashingCycleName = document.getElementById("CreateWashingCycleName").value;
	var Fase1Opdracht = document.getElementById("Fase1").value;
	var Fase1Time = document.getElementById("CreateFase1Time").value;
	var Fase2Opdracht = document.getElementById("Fase2").value;
	var Fase2Time = document.getElementById("CreateFase2Time").value;
	var Fase3Opdracht = document.getElementById("Fase3").value;
	var Fase3Time = document.getElementById("CreateFase3Time").value;
	var Fase4Opdracht = document.getElementById("Fase4").value;
	var Fase4Time = document.getElementById("CreateFase4Time").value;
	var Fase5Opdracht = document.getElementById("Fase4").value;
	var Fase5Time = document.getElementById("CreateFase4Time").value;
	
	Print("Creating... " + WashingCycleName, "CreateWashingCycle");
	Print(Fase1Opdracht + ", duur: " + Fase1Time + " Min", "CreateWashingCycle");
	Print(Fase2Opdracht + ", duur: " + Fase2Time + " Min", "CreateWashingCycle");
	Print(Fase3Opdracht + ", duur: " + Fase3Time + " Min", "CreateWashingCycle");
	Print(Fase4Opdracht + ", duur: " + Fase4Time + " Min", "CreateWashingCycle");
	Print(Fase5Opdracht + ", duur: " + Fase5Time + " Min", "CreateWashingCycle");
	setTimeout(function(){Load(html);},2000);
}

var FaseCount = 1;
function AddFase()
{
	if(FaseCount < 5)
	{
		FaseCount++;
		document.getElementById("Fase" + FaseCount).style.display = "block";
		var part1 = document.getElementById('Fase' + FaseCount).innerHTML = " Fase " + FaseCount + " &nbsp; <select id='Fase" + FaseCount + "Opdracht'><option value='Empty" + FaseCount + "'>...</option><option ";
		var part2 = document.getElementById('Fase' + FaseCount).innerHTML = " value='WaterToevoegen" + FaseCount + "'>Water Toevoegen</option><option value='ZeepToevoegen" + FaseCount + "'>Zeep Toevoegen</option></select><label> ";
		document.getElementById('Fase' + FaseCount).innerHTML = part1 + part2 + " &nbsp; Duur: </label> <input type = 'text' id = 'CreateFase" + FaseCount + "Time'/> Min &nbsp (tussen 5 en 60) ";
	}
}

function RemoveFase()
{
	if(FaseCount > 1)
	{
		document.getElementById("Fase" + FaseCount).style.display = "none";
		FaseCount--;
	}
}

function SavePassword(html)
{
	Print("Saving...", "ChangePassword");
	setTimeout(function(){Load(html);},500);
}

function LoadWashingCycle(html)
{
	Print("Loading...", "LoadWashingCycle");
	setTimeout(function(){Load(html);},500);
}

function Login_Message()
{
	var text = "Hallo " + "ws.getUsername()";
	Print(text, "Main");
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
