/// BOAL Верфь
#define sailCostPercent		10
#define hullCostPercent		20
#define paintCostPercent 0.75
int nCurScrollNum;
ref xi_refCharacter;
ref refNPCShipyard;
int shipIndex;
int upgradevalue;
int uptype = 0;
string SellState = "Max";
string sCabins[10] = {"Cabin_Small","Cabin_Small2","New_Cabin1","Cabin_Medium1","Cabin_Medium","Cabin_Medium2","New_Cabin2","Cabin_Quest","Cabin","Cabin_Huge"};//берём из SetCabinTypeEx
string chosenUpgrades[6] = {"","","","","",""};
// Complete CSP refit catalogue.  The original game declared this array in
// its global ship utilities; Caribbean Legend does not, so the Workshop
// port owns the catalogue locally.
#define CSP_SHIP_UPGRADE_COUNT 14
string ShipsUpgrades[14] = {"SUP_TRADEARMOUR","SUP_HULLKRENG","SUP_CAPACITY","SUP_HAMMOCKS","SUP_PUMPS","SUP_FLANGES","SUP_SAIL","SUP_FALCONETS","SUP_REINFORCEDCONSTRUCTION","SUP_HELM","SUP_GUNS","SUP_BIGSIDES","SUP_BARRICADES","SUP_ADDITIONALARSENAL"};
object UpgradeGoods;
int iCabinsNum = 10;//число видов кают
int iCurCabin = 0;
int upsq = 0;
int startupsq = 0;
int goodsnum = 0;
int allowedupgrades = 3;

int nCurScrollOfficerNum;

string CurTable, CurRow;
int iSelected; // курсор в таблице

string sMessageMode, sFrom_sea, upgrades;

float shipCostRate;
bool  bShipyardOnTop, bEmptySlot, bPreset, BlockPreset;

int RepairHull, RepairSail;
int timeHull, timeRig, timePaint, timeUpgrade, timePreUpgrade, timeSoil, timeCan;

int iSoilClearCost, iCannonsRepairCost, CannonsQty;

//// {*} BUHO-FIST - ADDED CODE - Fist state variable
#define FIS_ALL		0		// Normal
#define FIS_1		1		// C1
#define FIS_2		2		// C2
#define FIS_3		3		// C3
#define FIS_4		4		// C4
#define FIS_5		5		// C5
#define FIS_6		6		// C6
#define FIS_7		7		// C7
int FIS_FilterState = FIS_ALL;
//// {*} BUHO END ADDITION

void InitInterface_R(string iniName, ref _shipyarder)
{
	GameInterface.title = "titleShipyard";

	xi_refCharacter = pchar;

	refNPCShipyard  = _shipyarder;

	shipCostRate = stf(refNPCShipyard.ShipCostRate);

	int iTest = FindColony(refNPCShipyard.City); // город магазина
	ref rColony;
	sFrom_sea = "";
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
		sFrom_sea = rColony.from_sea; // ветка верфи, в сухопутных верфи нет, значит везде правильная
	}

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("OpenShipUp","OpenShipUp",0);
	SetEventHandler("CloseShipUp", "CloseShipUp",0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("ToggleShipUpgrade", "ToggleShipUpgrade", 0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	//////////////////
	EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",156,40,366,275); // tak from SHIP_BIG_PICTURE
	EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 161,246,361,1, 4);
	EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",20,46,134,161);
	EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,187,147,1, 4);
	EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,166,147,1, 4);

	bShipyardOnTop = false;
	bEmptySlot = false;
	sMessageMode = "";
	timeHull = 0;
	timeRig = 0;
	timePaint = 0;
	timeUpgrade = 0;
	timePreUpgrade = 0;
	timeSoil = 0;
	timeCan = 0;
	OpenShipUp();
	SetCurrentNode("SHIPSUP_TABLE");
	BlockPreset = false;
	
}

void ProcessExitCancel()
{
	// LEO: бесшлюпочному не сбрасывать флаг на базовый, если он легитимно поднят (есть перк-флаг нации)
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("OpenShipUp","OpenShipUp");
	DelEventHandler("CloseShipUp","CloseShipUp");
	DelEventHandler("OnTableClick", "OnTableClick");
	DelEventHandler("ToggleShipUpgrade", "ToggleShipUpgrade");
	DelEventHandler("MouseRClickUp","HideInfoWindow");

	interfaceResultCommand = exitCode;
	if( CheckAttribute(&InterfaceStates,"ReloadMenuExit"))
	{
		DeleteAttribute(&InterfaceStates,"ReloadMenuExit");
		EndCancelInterface(false);
	}
	else
	{
		EndCancelInterface(true);
	}
}

void HideInfoWindow()
{
	CloseTooltip();
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	int i;
	string row;
	ref rShip = &RealShips[sti(xi_refCharacter.Ship.Type)];
	
	switch(nodName)
	{
		case "SHIPSUP_ADD":
			if (comName=="click" || comName=="activate")
			{
			    PutUpgradeInLine();
				if (upsq != startupsq) BlockPreset = true;
				else BlockPreset = false;
			}
		break;
		case "SHIPSUP_DOUP":
			if (comName=="click" || comName=="activate")
			{
			    DoUpgrade();
			}
		break;
		case "SHIPSUP_BUTTON_TRADE_TEMPLATE":
			if (comName=="click" || comName=="activate")
			{
				if (startupsq > 0 || BlockPreset) break;
				bPreset = true;
				if (CheckAttribute(rShip,"Blueprint")) upgrades = "SUP_TRADEARMOUR|SUP_HULLKRENG|SUP_CAPACITY|SUP_HAMMOCKS|SUP_PUMPS|SUP_FLANGES";
				else if (CheckAttribute(rShip,"Ordered")) upgrades = "SUP_TRADEARMOUR|SUP_HULLKRENG|SUP_CAPACITY|SUP_HAMMOCKS|SUP_PUMPS";
				else upgrades = "SUP_TRADEARMOUR|SUP_HULLKRENG|SUP_CAPACITY|SUP_HAMMOCKS";
				for (i = 0; i < CSP_SHIP_UPGRADE_COUNT; i++)
				{
					row = "tr" + (i+1);
					if (HasSubStr(upgrades, GameInterface.SHIPSUP_TABLE.(row).td2.up) && CSP_CheckShipUpgradeType(rShip,GetConvertStr(GameInterface.SHIPSUP_TABLE.(row).td2.up+"_TYPE","ShipsUpgrades.txt")))
					{
						iSelected = i+1;
						PutUpgradeInLine();
					}
				}
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_TRADE_TEMPLATE",0, "#Merchant");
				if (chosenUpgrades[0] == "") 
				{
					bPreset = false;
					SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_TRADE_TEMPLATE",0, "#Merchant");
				}
			}
			SetSelectable("SHIPSUP_ADD", true);
		break;
		case "SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE":
			if (comName=="click" || comName=="activate")
			{
				if (startupsq > 0 || BlockPreset) break;
				bPreset = true;
				if (CheckAttribute(rShip,"Blueprint")) upgrades = "SUP_SAIL|SUP_HULLKRENG|SUP_FALCONETS|SUP_REINFORCEDCONSTRUCTION|SUP_FLANGES|SUP_HELM";
				else if (CheckAttribute(rShip,"Ordered")) upgrades = "SUP_SAIL|SUP_HULLKRENG|SUP_FALCONETS|SUP_REINFORCEDCONSTRUCTION|SUP_FLANGES";
				else upgrades = "SUP_SAIL|SUP_HULLKRENG|SUP_FALCONETS|SUP_REINFORCEDCONSTRUCTION";
				for (i = 0; i < CSP_SHIP_UPGRADE_COUNT; i++)
				{
					row = "tr" + (i+1);
					if (HasSubStr(upgrades, GameInterface.SHIPSUP_TABLE.(row).td2.up) && CSP_CheckShipUpgradeType(rShip,GetConvertStr(GameInterface.SHIPSUP_TABLE.(row).td2.up+"_TYPE","ShipsUpgrades.txt")))
					{
						iSelected = i+1;
						PutUpgradeInLine();
					}
				}
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",0, "#Universal");
				if (chosenUpgrades[0] == "") 
				{
					bPreset = false;
					SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",0, "#Universal");
				}
			}
			SetSelectable("SHIPSUP_ADD", true);
		break;
		case "SHIPSUP_BUTTON_WAR_TEMPLATE":
			if (comName=="click" || comName=="activate")
			{
				if (startupsq > 0 || BlockPreset) break;
				bPreset = true;
				if (CheckAttribute(rShip,"Blueprint")) upgrades = "SUP_GUNS|SUP_HULLKRENG|SUP_BIGSIDES|SUP_BARRICADES|SUP_ADDITIONALARSENAL|SUP_PUMPS";
				else if (CheckAttribute(rShip,"Ordered")) upgrades = "SUP_GUNS|SUP_HULLKRENG|SUP_BIGSIDES|SUP_BARRICADES|SUP_ADDITIONALARSENAL";
				else upgrades = "SUP_GUNS|SUP_HULLKRENG|SUP_BIGSIDES|SUP_BARRICADES";
				for (i = 0; i < CSP_SHIP_UPGRADE_COUNT; i++)
				{
					row = "tr" + (i+1);
					if (HasSubStr(upgrades, GameInterface.SHIPSUP_TABLE.(row).td2.up) && CSP_CheckShipUpgradeType(rShip,GetConvertStr(GameInterface.SHIPSUP_TABLE.(row).td2.up+"_TYPE","ShipsUpgrades.txt")))
					{
						iSelected = i+1;
						PutUpgradeInLine();
					}
				}
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_WAR_TEMPLATE",0, "#Combat refit");
				if (chosenUpgrades[0] == "") 
				{
					bPreset = false;
					SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_WAR_TEMPLATE",0, "#Combat refit");
				}
			}
			SetSelectable("SHIPSUP_ADD", true);
		break;
	}
}

void OpenShipUp()
{
	int clearIndex;
	int i;
	upsq = 0;
	startupsq = 0;
	goodsnum = 0;
	allowedupgrades = 3;
	bPreset = false;
	BlockPreset = false;
	for (clearIndex = 0; clearIndex < 6; clearIndex++) chosenUpgrades[clearIndex] = "";
	XI_WindowShow("SHIPSUP_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", true);
	XI_WindowDisable("SHIPSUP_WINDOW", false);
	SetNodeUsing("MAIN_WINDOW", false);
	SetNodeUsing("SHIPSUP_WINDOW", true);
	upgradevalue = 0;
	int iShip = sti(xi_refCharacter.Ship.Type);
	SetShipTypePictures(RealShips[iShip]);
	if (CheckAttribute(&RealShips[iShip],"Ordered")) allowedupgrades = 5;
	if (CheckAttribute(&RealShips[iShip],"Blueprint")) allowedupgrades = 6;	// Gregg: чертёжные корабли — 6 слотов улучшений по умолчанию
	if (CheckAttribute(&RealShips[iShip],"QuestShip")) allowedupgrades = 6;
	
	for (i = 0; i < CSP_SHIP_UPGRADE_COUNT; i++)
	{
		string row = "tr" + (i+1);
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.group = "SHIP_UPGRADES_MAX";
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.width = 128;
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.height = 128;
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.offset = "-3, 0";
		GameInterface.SHIPSUP_TABLE.(row).td1.textoffset = "65,0";
		GameInterface.SHIPSUP_TABLE.(row).td1.str = "";
		GameInterface.SHIPSUP_TABLE.(row).td1.align = "left";
		GameInterface.SHIPSUP_TABLE.(row).td1.scale = 0.7;
		GameInterface.SHIPSUP_TABLE.(row).td2.scale = 0.7;
		GameInterface.SHIPSUP_TABLE.(row).td3.scale = 0.7;
		
		string tempstring = ShipsUpgrades[i];
		GameInterface.SHIPSUP_TABLE.(row).td2.up = tempstring;
		GameInterface.SHIPSUP_TABLE.(row).td2.str = GetConvertStr(tempstring+"_DESCR","ShipsUpgrades.txt");
		if (CheckAttribute(RealShips[iShip],"Upgrade."+tempstring))
		{
			GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = GetConvertStr(tempstring+"_ICON","ShipsUpgrades.txt")+"On";
			GameInterface.SHIPSUP_TABLE.(row).td3.str = "Installed";
			upgradevalue = 0;
			upsq++;
		}
		else
		{
			GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = GetConvertStr(tempstring+"_ICON","ShipsUpgrades.txt")+"Off";
			if (CSP_CheckShipUpgradeType(&RealShips[iShip],GetConvertStr(tempstring+"_TYPE","ShipsUpgrades.txt")))
			{
				GameInterface.SHIPSUP_TABLE.(row).td3.color = argb(255,155,255,155);
				GameInterface.SHIPSUP_TABLE.(row).td3.str = "Available";
			}
			else 
			{
				GameInterface.SHIPSUP_TABLE.(row).td3.color = argb(255,255,155,155);
				GameInterface.SHIPSUP_TABLE.(row).td3.str = "Incompatible";
			}
		}
	}
	startupsq = upsq;
	SetFormatedText("SHIPSUP_VALUE", FindMoneyString(sti(pchar.money)));
	SetFormatedText("SHIPSUP_CAPTION", XI_ConvertString(RealShips[iShip].BaseName)+" - Rate "+RealShips[iShip].Class);
	SetFormatedText("SHIPSUP_Q", GetConvertStr("Variable_shipyard_76", "Interface.txt")+upsq+"/"+allowedupgrades);
	SetSelectable("SHIPSUP_DOUP", false);
	SetSelectable("SHIPSUP_ADD", false);
	SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#Add upgrade");
	SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_DOUP",0, "#Begin refit");
	GameInterface.SHIPSUP_TABLE.select = 1;
	Table_UpdateWindow("SHIPSUP_TABLE");
}

void TableSelectChange()
{
	int i;
	string sControl = GetEventData();
	if (sControl == "SHIPSUP_TABLE") iSelected = GetEventData();
	CurTable = sControl;
	CurRow   =  "tr" + (iSelected);
	if (CurTable == "SHIPSUP_TABLE")
	{
		string tempstring = ShipsUpgrades[iSelected-1];
		int iShip = sti(xi_refCharacter.Ship.Type);
		ref rship = &RealShips[iShip];
		/*switch (sti(rship.class))
		{
			case 7:
				timePreUpgrade = 2+drand(1);
			break;
			case 6:
				timePreUpgrade = 2+drand(2);
			break;
			case 5:
				timePreUpgrade = 3+drand(3);
			break;
			case 4:
				timePreUpgrade = 4+drand(4);
			break;
			case 3:
				timePreUpgrade = 4+drand(6);
			break;
			case 2:
				timePreUpgrade = 6+drand(6);
			break;
			case 1:
				timePreUpgrade = 10+drand(10);
			break;
		}*/
		if (CSP_CheckShipUpgradeType(&RealShips[iShip],GetConvertStr(tempstring+"_TYPE","ShipsUpgrades.txt"))) SetSelectable("SHIPSUP_ADD", true);
		else SetSelectable("SHIPSUP_ADD", false);
		for (i = 0; i < 6; i++)
		{
			if (chosenUpgrades[i] == "" && i < 5) continue;
			if (chosenUpgrades[i] == ShipsUpgrades[iSelected-1])
			{
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#"+GetConvertStr("Variable_shipyard_21", "Interface.txt"));
				SetSelectable("SHIPSUP_ADD",true);
				break;
			}
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#"+XI_Convertstring("TransferFoodToMine"));
			if (upsq >= allowedupgrades) 
			{
				SetSelectable("SHIPSUP_ADD",false);
			}
			if (CheckAttribute(&RealShips[iShip],"Upgrade."+ShipsUpgrades[iSelected-1])) SetSelectable("SHIPSUP_ADD",false);
		}
	}
}


void PutUpgradeInLine()
{
	int i;
	int j;
	int z = allowedupgrades;
	ref rShip = &RealShips[sti(xi_refCharacter.Ship.Type)];
	string selectedUpgrade = ShipsUpgrades[iSelected-1];
	if (CheckAttribute(rShip,"Upgrade."+selectedUpgrade)) return;
	if (!CheckChosenForUpgrade(selectedUpgrade) && !CSP_CheckShipUpgradeType(rShip,GetConvertStr(selectedUpgrade+"_TYPE","ShipsUpgrades.txt"))) return;
	if (bPreset)
	{
		if (CheckAttribute(rShip,"Blueprint")) z = 6;
		else if (CheckAttribute(rShip,"Ordered")) z = 5;
		else z = 4;
	}
	SetCurrentNode("SHIPSUP_TABLE");
	string row;
	for (i = 0; i < z; i++)
	{
		row = "tr" + (iSelected);
		if (chosenUpgrades[i] == "" && !CheckChosenForUpgrade(ShipsUpgrades[iSelected-1])) 
		{
			chosenUpgrades[i] = ShipsUpgrades[iSelected-1];
			GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = GetConvertStr(chosenUpgrades[i]+"_ICON","ShipsUpgrades.txt")+"On";
			GameInterface.SHIPSUP_TABLE.(row).td3.str = "Selected";
			upsq++;
			SetFormatedText("SHIPSUP_Q", GetConvertStr("Variable_shipyard_76", "Interface.txt")+upsq+"/"+z);
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#"+GetConvertStr("Variable_shipyard_21", "Interface.txt"));
			Table_UpdateWindow("SHIPSUP_TABLE");
			break;
		}
		else if (chosenUpgrades[i] == ShipsUpgrades[iSelected-1])
		{
			GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = GetConvertStr(chosenUpgrades[i]+"_ICON","ShipsUpgrades.txt")+"Off";
			chosenUpgrades[i] = "";
			GameInterface.SHIPSUP_TABLE.(row).td3.str = "Available";
			upsq--;
			SetFormatedText("SHIPSUP_Q", GetConvertStr("Variable_shipyard_76", "Interface.txt")+upsq+"/"+z);	// Gregg: фактический лимит слотов, а не жёсткие 3
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#"+XI_Convertstring("TransferFoodToMine"));
			Table_UpdateWindow("SHIPSUP_TABLE");
			break;
		}
	}
	
	Table_Clear("SHIPSUP_TABLE_GOODS", false, true, false);
	DeleteAttribute(&UpgradeGoods, "");
	string sGood;
	for (i = 0; i < z; i++)
	{
		if (chosenUpgrades[i] != "")
		{
			for (j = 1; j <= 4; j++)
			{
				if (sti(GetConvertStr(chosenUpgrades[i]+"_GOODA","ShipsUpgrades.txt"))>=j)
				{
					sGood = GetConvertStr(chosenUpgrades[i]+"_GOOD"+j,"ShipsUpgrades.txt");
					if (!CheckAttribute(UpgradeGoods,"type."+sGood)) UpgradeGoods.type.(sGood) = 1;
					else UpgradeGoods.type.(sGood) = sti(UpgradeGoods.type.(sGood)) + 1;
				}
			}
		}
	}
	aref aroot, arcur;
	makearef(aroot,UpgradeGoods.type);
	int num = GetAttributesNum(aroot);
	for (i = 0; i < num; i++)
	{
		row = "tr" + (i+1);
		arcur = GetAttributeN(aroot,i);
		sGood = GetAttributeName(arcur);
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.image = sGood;
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.group = "GOODS";
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.width = 32;
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.height = 32;
    	GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.offset = "-2, -1";
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.str = "";
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.align = "center";
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td2.str = XI_ConvertString(sGood);
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td2.scale = 0.8;
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td3.scale = 0.8;
		GameInterface.SHIPSUP_TABLE_GOODS.(row).td3.str = CalculateGoodAmount(&RealShips[sti(xi_refCharacter.Ship.Type)],sGood)*(sti(UpgradeGoods.type.(sGood)));
	}
	goodsnum = num;
	Table_UpdateWindow("SHIPSUP_TABLE_GOODS");
	// Keep refits meaningful without charging the price of another ship.
	// Each selected upgrade costs two percent of the vessel's base value.
	upgradevalue = makeint(sti(RealShips[sti(xi_refCharacter.Ship.Type)].price)/50) * (upsq-startupsq);
	timeUpgrade = (upsq-startupsq) * (4 + (7-sti(RealShips[sti(xi_refCharacter.Ship.Type)].class)));
	if (upgradevalue > 0) SetFormatedText("SHIPSUP_SELECTED", XI_ConvertString("UpgradeTime")+": "+timeUpgrade + "\n"+XI_ConvertString("Prepay") + ": " + FindMoneyString(upgradevalue));
	else SetFormatedText("SHIPSUP_SELECTED", "");
	if (upgradevalue > 0 && sti(pchar.money) >= upgradevalue) SetSelectable("SHIPSUP_DOUP", true);
	else SetSelectable("SHIPSUP_DOUP", false);
	SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_DOUP",0, "#Begin refit");
}

bool CheckChosenForUpgrade(string check)
{
	int i;
	int z = allowedupgrades;
	ref rShip = &RealShips[sti(xi_refCharacter.Ship.Type)];
	if (bPreset)
	{
		if (CheckAttribute(rShip,"Blueprint")) z = 6;
		else if (CheckAttribute(rShip,"Ordered")) z = 5;
		else z = 4;
	}
	for (i = 0; i < z; i++)
	{
		if (chosenUpgrades[i] == check) return true;
	}
	return false;
}

int CalculateGoodAmount(ref rship, string sGood)
{
	int amount = 0;
	switch (sti(rship.class))
	{
		case 7:
			amount = 2;
		break;
		case 6:
			amount = 3;
		break;
		case 5:
			amount = 4;
		break;
		case 4:
			amount = 6;
		break;
		case 3:
			amount = 8;
		break;
		case 2:
			amount = 10;
		break;
		case 1:
			amount = 12;
		break;
	}
	float AC = 510/sti(Goods[FindGood(sGood)].cost);
	if (AC > 1.5) AC = 1.5;
	amount = makeint(amount * AC);
	return amount;
}

void CloseShipUp()
{
	ProcessExitCancel();
}

string SetShipTypePictures(ref rship)
{
	string shiptype = "";
	SetSelectable("SHIPSUP_BUTTON_TRADE_TEMPLATE",true);
	SetSelectable("SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",true);
	SetSelectable("SHIPSUP_BUTTON_WAR_TEMPLATE",true);
	if (rship.type.merchant == true && rship.type.war == true)
	{
		shiptype = "Universal";
	}
	else if (rship.type.merchant == true)
	{
		shiptype = "Merchant";
	}
	else if (rship.type.war == true)
	{
		shiptype = "War";
	}
	return shiptype;
}

void DoUpgrade()
{
	int i;
	if (upgradevalue > 0) AddMoneyToCharacter(pchar, -upgradevalue);
	ref shTo = &RealShips[sti(xi_refCharacter.Ship.Type)];
	for (i = 0; i < 6; i++)
	{
		if (chosenUpgrades[i] != "")
		{
			string upgradename = chosenUpgrades[i];
			shTo.UpgradePrepared.(upgradename) = true;
		}
	}
	string sGood = "";
	int goodnum = 0;
	for (i = 1; i <= goodsnum; i++)
	{
		string row = "tr"+i;
		sGood = GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.image;
		goodnum = sti(GameInterface.SHIPSUP_TABLE_GOODS.(row).td3.str);
		shTo.GoodsForUpgrade.(sGood) = goodnum;
	}
	shTo.Shipyarder = refNPCShipyard.index;
	shTo.CSPMinimalRecipe = true;
	ProcessExitCancel();
}

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();
}

void ToggleShipUpgrade()
{
	PutUpgradeInLine();
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader = "";
	string sText1 = "";
	string sText2 = "";
	string sText3 = "";
	string sText4 = "";
	string sPicture = "";
	string sGroup = "";
	string sGroupPicture = "";
	int iItem;
	int xx = 256;
	int yy = 256;

	string sAttributeName;
	int nChooseNum = -1;
	int iShip;
	ref refBaseShip;

	bool  bShowHint = true;

	ref rChr;
	if (bShipyardOnTop)
	{
	    rChr = refNPCShipyard;
	}
	else
	{
		rChr = xi_refCharacter;
	}
	switch (sCurrentNode)
	{
		case "SHIPSUP_TABLE" :
			int selectedRow = iSelected;
			if (selectedRow < 1 || selectedRow > CSP_SHIP_UPGRADE_COUNT)
			{
				selectedRow = sti(GameInterface.SHIPSUP_TABLE.select);
			}
			if (selectedRow < 1 || selectedRow > CSP_SHIP_UPGRADE_COUNT) selectedRow = 1;

			string upgradeId = ShipsUpgrades[selectedRow - 1];
			sHeader = GetConvertStr(upgradeId + "_DESCR", "ShipsUpgrades.txt");
			sText1 = CSP_GetShipUpgradeEffectText(upgradeId);
			sText2 = "Refit category: " + GetConvertStr(upgradeId + "_TYPE", "ShipsUpgrades.txt");
			sGroup = "SHIP_UPGRADES_MAX";
			sGroupPicture = GetConvertStr(upgradeId + "_ICON", "ShipsUpgrades.txt") + "On";
			xx = 128;
			yy = 128;
		break;
	}
	if (bShowHint)
	{
		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), sText4, argb(255,255,255,255), sPicture, sGroup, sGroupPicture, xx, yy);
	}
}

string CSP_GetShipUpgradeEffectText(string upgradeId)
{
	if (upgradeId == "SUP_TRADEARMOUR") return "Effect: +10% hull strength, -4% cargo capacity.";
	if (upgradeId == "SUP_HULLKRENG") return "Effect: +5% sailing speed and +5% manoeuvrability.";
	if (upgradeId == "SUP_CAPACITY") return "Effect: +18% cargo capacity, -3% sailing speed.";
	if (upgradeId == "SUP_HAMMOCKS") return "Effect: +15% maximum crew and +8% minimum crew.";
	if (upgradeId == "SUP_PUMPS") return "Effect: +6% hull strength and 15% better damage control.";
	if (upgradeId == "SUP_FLANGES") return "Effect: +15% sail strength and 15% stronger rigging.";
	if (upgradeId == "SUP_SAIL") return "Effect: +8% sailing speed and +10% sail strength.";
	if (upgradeId == "SUP_FALCONETS") return "Effect: +20% boarding firepower and +5% maximum crew.";
	if (upgradeId == "SUP_REINFORCEDCONSTRUCTION") return "Effect: +15% hull strength, -3% sailing speed.";
	if (upgradeId == "SUP_HELM") return "Effect: +12% manoeuvrability and +5% close-hauled performance.";
	if (upgradeId == "SUP_GUNS") return "Effect: raises the maximum cannon calibre by one tier and improves the gun deck by 10%.";
	if (upgradeId == "SUP_BIGSIDES") return "Effect: +20% hull strength, -5% sailing speed.";
	if (upgradeId == "SUP_BARRICADES") return "Effect: +25% boarding defence and +8% maximum crew.";
	if (upgradeId == "SUP_ADDITIONALARSENAL") return "Effect: +6% cargo capacity and 15% greater reload reserves.";
	return "Advanced ship refit.";
}

bool CSP_CheckShipUpgradeType(ref rShip, string upgradeType)
{
	if (upgradeType == "Universal") return true;
	if (upgradeType == "Merchant" && rShip.Type.Merchant == true) return true;
	if (upgradeType == "War" && rShip.Type.War == true) return true;
	return false;
}
