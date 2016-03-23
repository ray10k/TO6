var PasswordAttempts = 0;
var FaseCount = 3;
var ws;
function Load(html) 
{
	PasswordAttempts = 0;
	window.open(html, "_self");
}

function onMessage (evt)
{
	console.log(evt.data);
	
	var obj = JSON.parse(evt.data);
	
	if (obj.type == "cycle")
	{
		var innerHtml = document.getElementById('CycleInfo').innerHTML = "<p></p>";
		innerHtml += "Current Cycle Name: " + obj.name;
		innerHtml += "<BR/>Progress: " + obj.currentStep + "/" + obj.totalSteps + " Steps";
		innerHtml += "<BR/>Current Step Name: " + obj.stepName;
		innerHtml += "<BR/>Cycle State: " + obj.state;
		document.getElementById('CycleInfo').innerHTML = innerHtml;
	}
	
	if (obj.type == "machine")
	{
		var innerHtml = document.getElementById('MachineInfo').innerHTML = "<p></p>";
		innerHtml += "<BR/><BR/>Temperatuur: " + obj.temperature;
		innerHtml += "<BR/>Water Level: " + obj.water;
		innerHtml += "<BR/>Drum: " + obj.RPM;
		innerHtml += "<BR/>Drum Clockwise: " + obj.clockwise;
		innerHtml += "<BR/>Soap: " + obj.soap;
		innerHtml += "<BR/>Door Lock: " + obj.lock;
		innerHtml += "<BR/>Water Valve: " + obj.valve;
		innerHtml += "<BR/>Pump: " + obj.pump;
		innerHtml += "<BR/>Heater: " + obj.heater;
		innerHtml += "<BR/>Signal: " + obj.signal;
		document.getElementById('MachineInfo').innerHTML = innerHtml;
	}
	
	
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

function WebSocketConnect() {
  if ("WebSocket" in window) {
     ws = new WebSocket("ws://192.168.137.111:2000");
     //ws = new WebSocket("ws://"+location.hostname+":8008");
     ws.onopen = function(evt) { onOpen(evt) };
	 ws.onmessage = function(evt) { onMessage(evt) };
     ws.onclose = function(evt) { onClose(evt) };
     ws.onerror = function(evt) { onError(evt) };
  }
  else  {
     alert("WebSocket NOT supported by your Browser!");
  }
}

function closeConnection() {
	ws.close();
}


function onClose (evt){
	console.log("connection closed");
}

function onError (evt){
 	console.log("websocket error" + evt.data);
 }

function Login(html) 
{
	var EnteredUserName = document.getElementById("LoginUsername").value;

	if( EnteredUserName != "Admin") //if(ws.checkUserName(EnteredUserName))
	{
		Print("Username Does Not Exist...", "Login");
	}
	else
	{
		var CurrentPassword = "0"; 
		var EnteredPassword = document.getElementById("LoginPassword").value;
	
		if(EnteredPassword == CurrentPassword) //if(ws.checkPassword(EnteredUserName, EnteredPassword))
		{
			Print("Loging in as " + EnteredUserName + "...", "Login");
			//ws.send("Logged in!");	
			//ws.Login(EnteredUserName);
			setTimeout(function(){Load(html);},1000);
		}
		else
		{
			PasswordAttempts++;
			Print("Wrong Password Entered, " + (3-PasswordAttempts) + " Attempts Left", "Login");
			if(PasswordAttempts >= 3)
			{
				Print("No Attempts Left, Closing...", "Login");
				setTimeout(function(){open(location, '_self').close();},1000);
			}
		}
	}
}

function Logout(html, CurrentFile) 
{
	Print("Logout...", CurrentFile);
	//ws.Logout();
	setTimeout(function(){Load(html);},500);
}

function Login_Message()
{

	Print("Welkom: Admin", "Main");
}

function SavePassword(html)
{
	var CurrentPassword = "test"; //ws.getCurrentUserPassword();
	var EnteredCurrentPassword = document.getElementById("CurrentPassword").value;
	var EnteredNewPassword = document.getElementById("NewPassword").value;
	var EnteredRepeatPassword = document.getElementById("RepeatPassword").value;
	
	if(EnteredCurrentPassword == CurrentPassword)
	{
		if(CheckPassword(EnteredNewPassword, EnteredRepeatPassword, "ChangePassword"))
		{
			Print("Saving Password...", "ChangePassword");
			//ws.changeCurrentUserPassword(EnteredNewPassword);
			setTimeout(function(){Load(html);},500);
		}
	}
	else
	{
		PasswordAttempts++;
		Print("Wrong Current Password Entered, " + (3-PasswordAttempts) + " Attempts Left", "ChangePassword");
		if(PasswordAttempts >= 3)
		{
			Print("No Attempts Left, Loging out...", "ChangePassword");
			setTimeout(function(){Logout("Login.html", "ChangePassword");},1000);
		}
	}
}

function CreateUserProfile(html)
{
	var EnteredUserName = document.getElementById("ProfileUsername").value;
	if(EnteredUserName == "")
	{
		Print("You must enter a username!", "CreateUserProfile");
	}
	else if(isAlphaNumeric(EnteredUserName) != true)
	{
		Print("You can only use Numbers and Letters in your username!", "CreateUserProfile");
	}
	else if(EnteredUserName == "Admin")//if(ws.checkUserName(EnteredUserName) == true)
	{
		Print("Username: " + EnteredUserName + " Already Exist...", "CreateUserProfile");
	}
	else
	{
		var EnteredPassword = document.getElementById("ProfilePassword").value;
		var EnteredRepeatPassword = document.getElementById("ProfileRepeatPassword").value;
	
		if(CheckPassword(EnteredPassword, EnteredRepeatPassword, "CreateUserProfile"))
		{
			Print("Creating Profile: " + EnteredUserName + "...", "CreateUserProfile");
			//ws.addUser({EnteredUserName, EnteredPassword});
			setTimeout(function(){Load(html);},1000);
		}
	}
}

function CheckPassword(EnteredPassword, EnteredRepeatPassword, CurrentFile)
{
	if(EnteredPassword == "")
	{
		Print("You must enter a password!", CurrentFile);
	}
	else if(EnteredPassword.length < 6)
	{
		Print("You must enter a password with at least 6 characters!", CurrentFile);
	}
	else if(isAlphaNumeric(EnteredPassword) != true)
	{
		Print("You can only use Numbers and Letters in your password!", CurrentFile);
	}
	else if(EnteredPassword == EnteredRepeatPassword)
	{
		return true;
	}
	else
	{
		Print("New Password and Repeat Password do not match!", CurrentFile);
	}
	return false;
}

function isAlphaNumeric(str) 
{
  return /^[a-zA-Z0-9]+$/.test(str);
}

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
	var WashingCycleName = document.getElementById("WashingCycleName").value; //ws.CreateWashingCycle(WashingCycleName);
	if(WashingCycleName == "")
	{
		Print("You must enter a washing cycle name!", CurrentFile);
		return false;
	}
	else if(isAlphaNumeric(WashingCycleName) != true)
	{
		Print("You can only use Numbers and Letters in your Washing Cycle Name!", CurrentFile);
		return false;
	}
	
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
				//ws.AddFase(WashingCycleName, FaseOpdracht, FaseTime, WaterHoeveelheid);
				break;
			case "ZeepToevoegen"+fase:
				var ZeepSoort = document.getElementById("Fase" + fase + "Zeep").value;
				extra = ", Zeep voor: " + ZeepSoort;
				//ws.AddFase(WashingCycleName, FaseOpdracht, FaseTime, ZeepSoort);
				break;
			case "WaterVerwarmen"+fase:
				var Temperatuur = document.getElementById("Fase" + fase + "Temperatuur").value;
				extra = ", Temperatuur: " + Temperatuur;
				//ws.AddFase(WashingCycleName, FaseOpdracht, FaseTime, Temperatuur);
				break;
			default:
				//ws.AddFase(WashingCycleName, FaseOpdracht, FaseTime);
				break;
		}
		Print(Message + extra, CurrentFile);
	}
	setTimeout(function(){Load(html);},5000);
}

function DeleteWashingCycle(html)
{
	var WashingCycleName = document.getElementById("WashingCycleNameList").value;
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
	
	if(FaseOpdracht == "Empty"+fase || FaseOpdracht == "WaterVerwarmen"+fase) { disabledBool = true; value = 0; } 
	else { disabledBool = false; value = 1; }
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
	
	var WashingCycleNames = ["Kort Maar Krachtig", "Hot Jeans"]; //ws.loadWashingCycleNames();
	var NamesToHtml = "";
	for(var i=0; i<WashingCycleNames.length; i++)
	{
		NamesToHtml += "<option value='" + WashingCycleNames[i] + "'>" + WashingCycleNames[i] + "</option>"
	}
	document.getElementById("WashingCycleNameList").innerHTML = NamesToHtml;
}

function LoadWashingCycle()
{
	var WashingCycleName = document.getElementById("WashingCycleNameList").value;
	document.getElementById("WashingCycleName").value = WashingCycleName;
	//Print("Loading WashingCycle: " + WashingCycleName + "...", "EditWashingCycle");
	
	FaseCount = 5; 
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
		
		var disabledBool;	
		if(FaseOpdracht == "Empty"+fase || FaseOpdracht == "WaterVerwarmen"+fase) { disabledBool = true; } 
		else { disabledBool = false; }
		document.getElementById("Fase" + fase + "Time").disabled = disabledBool;
		document.getElementById("Fase" + fase + "Time").value = 30; //ws.LoadWashingCycleFaseTime(WashingCycleName, fase);
	}
	
	document.getElementById("EditWashingCycleButtons").style.display = "block";
}

var WashingCycleState = "stop";
var LastWashingCycleState = "stop";
function SetMainBotButtonColor()
{
	var buttons = ["RunButton", "PauseButton", "StopButton"];
	for(var i=0; i<buttons.length; i++)
	{
		document.getElementById(buttons[i]).style.background = document.getElementById("LogoutButton").style.background;
	}
	switch(WashingCycleState)
	{
		case "run":
			document.getElementById("RunButton").style.background = "lime";
			break;
		case "pause":
			document.getElementById("PauseButton").style.background = "orange";
			break;
		case "stop":
			document.getElementById("StopButton").style.background = "red";
			break;
	}
}

function SetWashingCycleState(State)
{
	
	var WashingCycleName = document.getElementById("WashingCycleNameList").value;
	WashingCycleState = State;
	SetMainBotButtonColor();
	if(State == "run")
	{
		if(LastWashingCycleState == "pause")
		{ 	
			var message = {type:"command",command:"resume"};
			ws.send(JSON.stringify(message));
		}
		else
		{
			var message = {type:"command",command:"start",user:"Admin",cycle:WashingCycleName};
			ws.send(JSON.stringify(message));
		}
	}
	else
	{
		var message = {type:"command",command:State};
		ws.send(JSON.stringify(message));
	}
	LastWashingCycleState = State;
}

function LoadWashingCycleState()
{
	var WashingCycleName = document.getElementById("WashingCycleNameList").value;
	SetWashingCycleState("stop"); //SetWashingCycleState(ws.GetWashingCycleState(WashingCycleName));
}
