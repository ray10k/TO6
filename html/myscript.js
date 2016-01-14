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

var FaseCount = 3;
function CreateWashingCycle(html)
{
	var WashingCycleName = document.getElementById("CreateWashingCycleName").value;
	Print("Creating " + WashingCycleName + "...", "CreateWashingCycle");
	for(var fase = 1; fase <= FaseCount; fase++)
	{
		var FaseOpdracht = document.getElementById("Fase" + fase + "Opdracht").value;
		var FaseTime = document.getElementById("Fase" + fase + "Time").value;
		var Message = FaseOpdracht + ", duur: " + FaseTime + " Minuten";
		var extra = "";
		switch(FaseOpdracht)
		{
			case "WaterToevoegen"+fase:
				var WaterHoeveelheid = document.getElementById("Fase" + fase + "WaterHoeveelheid").value;
				extra = ", WaterHoeveelheid: " + WaterHoeveelheid;
				break;
			case "ZeepToevoegen"+fase:
				var ZeepSoort = document.getElementById("Fase" + fase + "Zeep").value;
				extra = ", Zeep voor: " + ZeepSoort;
				break;
			case "WaterVerwarmen"+fase:
				var Temperatuur = document.getElementById("Fase" + fase + "Temperatuur").value;
				extra = ", Temperatuur: " + Temperatuur;
				break;
		}
		Print(Message + extra, "CreateWashingCycle");
	}
	setTimeout(function(){Load(html);},5000);
}

function AddFase()
{
	if(FaseCount < 8)
	{
		FaseCount++;
		document.getElementById("Fase" + FaseCount).style.display = "block";
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

function DropDownChange(fase)
{
	var FaseOpdracht = document.getElementById("Fase" + fase + "Opdracht").value;
	var disabledBool, value;
	
	if(FaseOpdracht != "Empty"+fase) { disabledBool = false; value = 1; } 
	else { disabledBool = true; value = 0; }
	document.getElementById("Fase" + fase + "Time").disabled = disabledBool;
	document.getElementById("Fase" + fase + "Time").value = value;
	
	switch(FaseOpdracht)
	{
		case "Empty"+fase:
			//Print("Empty"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "...";
			break;
		case "WaterToevoegen"+fase:
			//Print("WaterToevoegen"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "Hoeveelheid: <select id='Fase" + fase + "WaterHoeveelheid'><option value='40Liter" + fase + "'>40</option><option value='45Liter" + fase + "'>45</option><option value='50Liter" + fase + "'>50</option><option value='55Liter" + fase + "'>55</option><option value='60Liter" + fase + "'>60</option></select> Liter";
			break;
		case "ZeepToevoegen"+fase:
			//Print("ZeepToevoegen"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "Zeep voor: <select id='Fase" + fase + "Zeep'><option value='WitteWas" + fase + "'>Witte Was</option><option value='BonteWas" + fase + "'>Gekleurde (Bonte) Was</option><option value='Fijn/WolWas" + fase + "'>Fijn en Wol Was</option><option value='JeansWas" + fase + "'>Jeans Was (Spijkerstof)</option></select>";
			break;
		case "WaterVerwarmen"+fase:
			//Print("WaterVerwarmen"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "Temperatuur: <select id='Fase" + fase + "Temperatuur'><option value='30Graden" + fase + "'>30</option><option value='40Graden" + fase + "'>40</option><option value='60Graden" + fase + "'>60</option><option value='95Graden" + fase + "'>95</option></select> Graden Celcius";
			break;
		case "Draaien"+fase:
			//Print("Draaien"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "...";
			break;
		case "Spoelen"+fase:
			//Print("Spoelen"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "...";
			break;
		case "WaterWegpompen"+fase:
			//Print("WaterWegpompen"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "...";
			break;
		default:
			//Print("Default"+fase, "CreateWashingCycle");
			document.getElementById("Fase" + fase + "Extra").innerHTML = "...";
			break;
	}
}

function CheckIfTime_1_60(fase)
{
	var FaseTime = document.getElementById("Fase" + fase + "Time").value;
	if(FaseTime < 1){FaseTime = 1;}
	if(FaseTime > 60){FaseTime = 60;}
	document.getElementById("Fase" + fase + "Time").value = FaseTime;
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
