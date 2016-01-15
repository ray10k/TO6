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
function FillWashingCycleFases()
{
	for(var i = 1; i <= 8; i++)
	{
		var part1 = document.getElementById('Fase' + i).innerHTML = " Fase " + i + " &nbsp; <select id='Fase" + i + "Opdracht'onchange='DropDownChange("+ i +")'><option value='Empty" + i + "'>...</option>";
		var part2;
		switch(i)
		{
			default:
			part2 = document.getElementById('Fase' + i).innerHTML = "<option value='WaterToevoegen" + i + "'>Water Toevoegen</option><option value='ZeepToevoegen" + i + "'>Zeep Toevoegen</option><option value='WaterVerwarmen" + i + "'>Water Verwarmen</option><option value='Draaien" + i + "'>Draaien</option><option value='Spoelen" + i + "'>Spoelen</option><option value='WaterWegpompen" + i + "'>Water Wegpompen</option>";
			break;
		}
		document.getElementById('Fase' + i).innerHTML = part1 + part2 + "</select><label> &nbsp; Duur: </label> <input type = 'number' value='0' min='1' max='60' id = 'Fase" + i + "Time' oninput='CheckIfTime_1_60(" + i + ")' disabled=''/> Minuten <div id='Fase" + i + "Extra'>...</div>";
	}
}

function SaveWashingCycle(html, CurrentFile)
{
	var WashingCycleName = document.getElementById("WashingCycleName").value;
	Print("Saving " + WashingCycleName + "...", CurrentFile);
	Print(WashingCycleName + " bevat " + FaseCount + " fases", CurrentFile);
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
		Print(Message + extra, CurrentFile);
	}
	setTimeout(function(){Load(html);},5000);
}

function DeleteWashingCycle(html)
{
	var WashingCycleName = document.getElementById("EditWashingCycleName").value;
	Print("Deleting " + WashingCycleName + "...", "EditWashingCycle");
	
	//ws.DeleteWashingCycle(WashingCycleName);
	
	setTimeout(function(){Load(html);},1000);
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
		document.getElementById("Fase" + FaseCount + "Opdracht").value = "Empty" + FaseCount;
		document.getElementById("Fase" + FaseCount + "Time").disabled = true;
		document.getElementById("Fase" + FaseCount + "Time").value = 0;
		document.getElementById("Fase" + FaseCount + "Extra").innerHTML = "...";
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
	
	var InnerHtml = "..."
	switch(FaseOpdracht)
	{
		case "WaterToevoegen"+fase:
			//Print("WaterToevoegen"+fase, "CreateWashingCycle");
			InnerHtml = "Hoeveelheid: <select id='Fase" + fase + "WaterHoeveelheid'><option value='40Liter" + fase + "'>40</option><option value='45Liter" + fase + "'>45</option><option value='50Liter" + fase + "'>50</option><option value='55Liter" + fase + "'>55</option><option value='60Liter" + fase + "'>60</option></select> Liter";
			break;
		case "ZeepToevoegen"+fase:
			//Print("ZeepToevoegen"+fase, "CreateWashingCycle");
			InnerHtml = "Zeep voor: <select id='Fase" + fase + "Zeep'><option value='WitteWas" + fase + "'>Witte Was</option><option value='BonteWas" + fase + "'>Gekleurde (Bonte) Was</option><option value='Fijn/WolWas" + fase + "'>Fijn en Wol Was</option><option value='JeansWas" + fase + "'>Jeans Was (Spijkerstof)</option></select>";
			break;
		case "WaterVerwarmen"+fase:
			//Print("WaterVerwarmen"+fase, "CreateWashingCycle");
			InnerHtml = "Temperatuur: <select id='Fase" + fase + "Temperatuur'><option value='30Graden" + fase + "'>30</option><option value='40Graden" + fase + "'>40</option><option value='60Graden" + fase + "'>60</option><option value='95Graden" + fase + "'>95</option></select> Graden Celcius";
			break;
	}
	document.getElementById("Fase" + fase + "Extra").innerHTML = InnerHtml;
}

function CheckIfTime_1_60(fase)
{
	var FaseTime = document.getElementById("Fase" + fase + "Time").value;
	if(FaseTime < 1){FaseTime = 1;}
	if(FaseTime > 60){FaseTime = 60;}
	document.getElementById("Fase" + fase + "Time").value = FaseTime;
}

function LoadWashingCycleNames()
{
	//Print("Loading Washing Cycle Names...", "EditWashingCycle");
	
	var WashingCycleNames = ["Kort Maar Krachtig", "Hot Jeans"]; //ws.LoadWashingCycleNames();
	var NamesToHtml = "";
	for(var i=0; i<WashingCycleNames.length; i++)
	{
		NamesToHtml += "<option value='" + WashingCycleNames[i] + "'>" + WashingCycleNames[i] + "</option>"
	}
	document.getElementById("EditWashingCycleName").innerHTML = NamesToHtml;
}

function LoadWashingCycle()
{
	var WashingCycleName = document.getElementById("EditWashingCycleName").value;
	document.getElementById("WashingCycleName").value = WashingCycleName;
	//Print("Loading WashingCycle: " + WashingCycleName + "...", "EditWashingCycle");
	
	FaseCount = 5; //ws.LoadWashingCycleFaseCount(WashingCycleName);
	for(var fase=1; fase<=FaseCount; fase++)
	{
		document.getElementById("Fase" + fase).style.display = "block";
		
		var FaseOpdracht = "WaterToevoegen" + fase; //ws.LoadWashingCycleFaseOpdracht(WashingCycleName, fase);
		document.getElementById("Fase" + fase + "Opdracht").value = FaseOpdracht;
		switch(FaseOpdracht)
		{
			case "WaterToevoegen"+fase:
				document.getElementById("Fase" + fase + "Extra").innerHTML = "Hoeveelheid: <select id='Fase" + fase + "WaterHoeveelheid'><option value='40Liter" + fase + "'>40</option><option value='45Liter" + fase + "'>45</option><option value='50Liter" + fase + "'>50</option><option value='55Liter" + fase + "'>55</option><option value='60Liter" + fase + "'>60</option></select> Liter";
				document.getElementById("Fase" + fase + "WaterHoeveelheid").value = "55Liter"+fase; //ws.LoadWashingCycleWaterHoeveelheid(WashingCycleName, fase);
				break;
			case "ZeepToevoegen"+fase:
				document.getElementById("Fase" + fase + "Extra").innerHTML = "Zeep voor: <select id='Fase" + fase + "Zeep'><option value='WitteWas" + fase + "'>Witte Was</option><option value='BonteWas" + fase + "'>Gekleurde (Bonte) Was</option><option value='Fijn/WolWas" + fase + "'>Fijn en Wol Was</option><option value='JeansWas" + fase + "'>Jeans Was (Spijkerstof)</option></select>";
				document.getElementById("Fase" + fase + "Zeep").value = "Fijn/WolWas"+fase; //ws.LoadWashingCycleZeepSoort(WashingCycleName, fase);
				break;
			case "WaterVerwarmen"+fase:
				document.getElementById("Fase" + fase + "Extra").innerHTML = "Temperatuur: <select id='Fase" + fase + "Temperatuur'><option value='30Graden" + fase + "'>30</option><option value='40Graden" + fase + "'>40</option><option value='60Graden" + fase + "'>60</option><option value='95Graden" + fase + "'>95</option></select> Graden Celcius";
				document.getElementById("Fase" + fase + "Temperatuur").value = "60Graden"+fase; //ws.LoadWashingCycleTemperatuur(WashingCycleName, fase);
				break;
		}
		
		document.getElementById("Fase" + fase + "Time").disabled = false;
		document.getElementById("Fase" + fase + "Time").value = 30; //ws.LoadWashingCycleFaseTime(WashingCycleName, fase);
	}
	
	document.getElementById("EditWashingCycleButtons").style.display = "block";
}

function SavePassword(html)
{
	Print("Saving...", "ChangePassword");
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
