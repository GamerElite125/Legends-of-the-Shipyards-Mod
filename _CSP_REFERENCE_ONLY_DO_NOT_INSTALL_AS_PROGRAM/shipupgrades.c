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
string chosenUpgrades[6] = {"","","","","",""}
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
	
	if(LanguageGetLanguage() != "RUSSIAN")
	{
		SetNewGroupPicture("SHIPSUP_TYPE_TRADE", "ICONS_SPEC", "english trader ship icon");
		SetNewGroupPicture("SHIPSUP_TYPE_WAR", "ICONS_SPEC", "english battle ship icon");
		SetNewGroupPicture("SHIPSUP_TYPE_UNIVERSAL", "ICONS_SPEC", "english universal ship icon");
	}
}

void ProcessExitCancel()
{
	// LEO: бесшлюпочному не сбрасывать флаг на базовый, если он легитимно поднят (есть перк-флаг нации)
	if (sti(pchar.ship.type) == SHIP_NOTUSED && !HeroHasFlagPerk(sti(pchar.nation))) PChar.nation = GetBaseHeroNation();
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
				if (CheckAttribute(rShip,"Blueprint") || CheckAttribute(rShip,"QuestShip")) break;	// Gregg: чертёжные/квест-корабли — пресетный набор не выбирается
				bPreset = true;
				if (CheckAttribute(rShip,"Blueprint")) upgrades = "SUP_TRADEARMOUR|SUP_HULLKRENG|SUP_CAPACITY|SUP_HAMMOCKS|SUP_PUMPS|SUP_FLANGES";
				else if (CheckAttribute(rShip,"Ordered")) upgrades = "SUP_TRADEARMOUR|SUP_HULLKRENG|SUP_CAPACITY|SUP_HAMMOCKS|SUP_PUMPS";
				else upgrades = "SUP_TRADEARMOUR|SUP_HULLKRENG|SUP_CAPACITY|SUP_HAMMOCKS";
				for (i = 0; i < 18; i++)
				{
					row = "tr" + (i+1);
					if (HasSubStr(upgrades, GameInterface.SHIPSUP_TABLE.(row).td2.up))
					{
						iSelected = i+1;
						PutUpgradeInLine();
					}
				}
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_TRADE_TEMPLATE",0, "#-");
				if (chosenUpgrades[0] == "") 
				{
					bPreset = false;
					SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_TRADE_TEMPLATE",0, "#+");
				}
			}
			SetSelectable("SHIPSUP_ADD", false);
		break;
		case "SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE":
			if (comName=="click" || comName=="activate")
			{
				if (startupsq > 0 || BlockPreset) break;
				if (CheckAttribute(rShip,"Blueprint") || CheckAttribute(rShip,"QuestShip")) break;	// Gregg: чертёжные/квест-корабли — пресетный набор не выбирается
				bPreset = true;
				if (CheckAttribute(rShip,"Blueprint")) upgrades = "SUP_SAIL|SUP_HULLKRENG|SUP_FALCONETS|SUP_REINFORCEDCONSTRUCTION|SUP_FLANGES|SUP_HELM";
				else if (CheckAttribute(rShip,"Ordered")) upgrades = "SUP_SAIL|SUP_HULLKRENG|SUP_FALCONETS|SUP_REINFORCEDCONSTRUCTION|SUP_FLANGES";
				else upgrades = "SUP_SAIL|SUP_HULLKRENG|SUP_FALCONETS|SUP_REINFORCEDCONSTRUCTION";
				for (i = 0; i < 18; i++)
				{
					row = "tr" + (i+1);
					if (HasSubStr(upgrades, GameInterface.SHIPSUP_TABLE.(row).td2.up))
					{
						iSelected = i+1;
						PutUpgradeInLine();
					}
				}
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",0, "#-");
				if (chosenUpgrades[0] == "") 
				{
					bPreset = false;
					SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",0, "#+");
				}
			}
			SetSelectable("SHIPSUP_ADD", false);
		break;
		case "SHIPSUP_BUTTON_WAR_TEMPLATE":
			if (comName=="click" || comName=="activate")
			{
				if (startupsq > 0 || BlockPreset) break;
				if (CheckAttribute(rShip,"Blueprint") || CheckAttribute(rShip,"QuestShip")) break;	// Gregg: чертёжные/квест-корабли — пресетный набор не выбирается
				bPreset = true;
				if (CheckAttribute(rShip,"Blueprint")) upgrades = "SUP_GUNS|SUP_HULLKRENG|SUP_BIGSIDES|SUP_BARRICADES|SUP_ADDITIONALARSENAL|SUP_PUMPS";
				else if (CheckAttribute(rShip,"Ordered")) upgrades = "SUP_GUNS|SUP_HULLKRENG|SUP_BIGSIDES|SUP_BARRICADES|SUP_ADDITIONALARSENAL";
				else upgrades = "SUP_GUNS|SUP_HULLKRENG|SUP_BIGSIDES|SUP_BARRICADES";
				for (i = 0; i < 18; i++)
				{
					row = "tr" + (i+1);
					if (HasSubStr(upgrades, GameInterface.SHIPSUP_TABLE.(row).td2.up))
					{
						iSelected = i+1;
						PutUpgradeInLine();
					}
				}
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_WAR_TEMPLATE",0, "#-");
				if (chosenUpgrades[0] == "") 
				{
					bPreset = false;
					SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_BUTTON_WAR_TEMPLATE",0, "#+");
				}
			}
			SetSelectable("SHIPSUP_ADD", false);
		break;
	}
}

void OpenShipUp()
{
	XI_WindowShow("SHIPSUP_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", true);
	XI_WindowDisable("SHIPSUP_WINDOW", false);
	SetNodeUsing("MAIN_WINDOW", false);
	SetNodeUsing("SHIPSUP_WINDOW", true);
	upgradevalue = 0;
	int iShip = sti(xi_refCharacter.Ship.Type);
	SetShipTypePictures(RealShips[iShip]);
	if (CheckAttribute(&RealShips[iShip],"Blueprint")) allowedupgrades = 6;	// Gregg: чертёжные корабли — 6 слотов улучшений по умолчанию
	if (CheckAttribute(&RealShips[iShip],"QuestShip")) allowedupgrades = 6;
	
	for (int i = 0; i < 18; i++)
	{
		string row = "tr" + (i+1);
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.group = "SHIP_UPGRADES_MAX";
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.width = 64;
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.height = 64;
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
			GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_56", "Interface.txt", "#space#", " ");
			upgradevalue = 0;
			upsq++;
		}
		else
		{
			GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = GetConvertStr(tempstring+"_ICON","ShipsUpgrades.txt")+"Off";
			if (CheckShipType(RealShips[iShip],GetConvertStr(tempstring+"_TYPE","ShipsUpgrades.txt"))) 
			{
				GameInterface.SHIPSUP_TABLE.(row).td3.color = argb(255,155,255,155);
				GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_57", "Interface.txt", "#space#", " ");
			}
			else 
			{
				GameInterface.SHIPSUP_TABLE.(row).td3.color = argb(255,255,155,155);
				GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_58", "Interface.txt", "#space#", " ");
			}
		}
	}
	startupsq = upsq;
	SetFormatedText("SHIPSUP_VALUE", FindRussianMoneyString(sti(pchar.money)));
	SetFormatedText("SHIPSUP_CAPTION", GetConvertStrWithReplace("Variable_shipyard_75", "Interface.txt", "#space#", " "));
	SetFormatedText("SHIPSUP_Q", GetConvertStrWithReplace("Variable_shipyard_76", "Interface.txt", "#space#", " ")+upsq+"/"+allowedupgrades);
	SetSelectable("SHIPSUP_DOUP", false);
	SetSelectable("SHIPSUP_ADD", false);
	GameInterface.SHIPSUP_TABLE.select = 1;
	Table_UpdateWindow("SHIPSUP_TABLE");
}

void TableSelectChange()
{
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
		if (CheckShipType(RealShips[iShip],GetConvertStr(tempstring+"_TYPE","ShipsUpgrades.txt"))) SetSelectable("SHIPSUP_ADD", true);
		else SetSelectable("SHIPSUP_ADD", false);
		for (i = 0; i < 6; i++)
		{
			if (chosenUpgrades[i] == "" && i < 5) continue;
			if (chosenUpgrades[i] == ShipsUpgrades[iSelected-1])
			{
				SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#"+GetConvertStrWithReplace("Variable_shipyard_21", "Interface.txt", "#space#", " "));
				if (bPreset) SetSelectable("SHIPSUP_ADD",false);
				else SetSelectable("SHIPSUP_ADD",true);
				break;
			}
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#"+XI_Convertstring("TransferFoodToMine"));
			if (upsq >= allowedupgrades) 
			{
				SetSelectable("SHIPSUP_ADD",false);
			}
			if (CheckForUpgrade(&RealShips[iShip],ShipsUpgrades[iSelected-1])) SetSelectable("SHIPSUP_ADD",false);
		}
	}
}


void PutUpgradeInLine()
{
	int z = allowedupgrades;
	ref rShip = &RealShips[sti(xi_refCharacter.Ship.Type)];
	if (bPreset)
	{
		if (CheckAttribute(rShip,"Blueprint")) z = 6;
		else if (CheckAttribute(rShip,"Ordered")) z = 5;
		else z = 4;
	}
	SetCurrentNode("SHIPSUP_TABLE");
	string row;
	for (int i = 0; i < z; i++)
	{
		row = "tr" + (iSelected);
		if (chosenUpgrades[i] == "" && !CheckChosenForUpgrade(ShipsUpgrades[iSelected-1])) 
		{
			chosenUpgrades[i] = ShipsUpgrades[iSelected-1];
			GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = GetConvertStr(chosenUpgrades[i]+"_ICON","ShipsUpgrades.txt")+"On";
			GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_82", "Interface.txt", "#space#", " ");
			upsq++;
			SetFormatedText("SHIPSUP_Q", GetConvertStrWithReplace("Variable_shipyard_76", "Interface.txt", "#space#", " ")+upsq+"/"+z);
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_ADD",0, "#"+GetConvertStrWithReplace("Variable_shipyard_21", "Interface.txt", "#space#", " "));
			Table_UpdateWindow("SHIPSUP_TABLE");
			break;
		}
		else if (chosenUpgrades[i] == ShipsUpgrades[iSelected-1])
		{
			GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = GetConvertStr(chosenUpgrades[i]+"_ICON","ShipsUpgrades.txt")+"Off";
			chosenUpgrades[i] = "";
			GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_57", "Interface.txt", "#space#", " ");
			upsq--;
			SetFormatedText("SHIPSUP_Q", GetConvertStrWithReplace("Variable_shipyard_76", "Interface.txt", "#space#", " ")+upsq+"/"+z);	// Gregg: фактический лимит слотов, а не жёсткие 3
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
			for (int j = 1; j <= 4; j++)
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
	upgradevalue = makeint(sti(RealShips[sti(xi_refCharacter.Ship.Type)].price)/10) * (upsq-startupsq);
	timeUpgrade = (upsq-startupsq) * (4 + (7-sti(RealShips[sti(xi_refCharacter.Ship.Type)].class)));
	if (upgradevalue > 0) SetFormatedText("SHIPSUP_SELECTED", XI_ConvertString("UpgradeTime")+": "+timeUpgrade + "\n"+XI_ConvertString("Prepay") + ": " + FindRussianMoneyString(upgradevalue));
	else SetFormatedText("SHIPSUP_SELECTED", "");
	if (upsq > 0 && sti(pchar.money) >= upgradevalue) SetSelectable("SHIPSUP_DOUP", true);
	else SetSelectable("SHIPSUP_DOUP", false);
}

bool CheckChosenForUpgrade(string check)
{
	int z = allowedupgrades;
	ref rShip = &RealShips[sti(xi_refCharacter.Ship.Type)];
	if (bPreset)
	{
		if (CheckAttribute(rShip,"Blueprint")) z = 6;
		else if (CheckAttribute(rShip,"Ordered")) z = 5;
		else z = 4;
	}
	for (int i = 0; i < z; i++)
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
			amount = 3;
		break;
		case 6:
			amount = 8;
		break;
		case 5:
			amount = 16;
		break;
		case 4:
			amount = 32;
		break;
		case 3:
			amount = 74;
		break;
		case 2:
			amount = 112;
		break;
		case 1:
			amount = 154;
		break;
	}
	float AC = 510/sti(Goods[FindGood(sGood)].cost);
	if (AC > 3.5) AC = 3.5;
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
	if (rship.type.merchant == true && rship.type.war == true)
	{
		SetNewGroupPicture("SHIPSUP_TRADE", "UP_SHIPS_PICTURES", "UP_Trade_OFF");
		SetNewGroupPicture("SHIPSUP_UNIVERSAL", "UP_SHIPS_PICTURES", "UP_Universal_ON");
		SetNewGroupPicture("SHIPSUP_WAR", "UP_SHIPS_PICTURES", "UP_War_OFF");
		shiptype = "Universal";
		SetSelectable("SHIPSUP_BUTTON_TRADE_TEMPLATE",false);
		SetSelectable("SHIPSUP_BUTTON_WAR_TEMPLATE",false);
		if (CheckAttribute(rship,"QuestShip") || CheckAttribute(rship,"Blueprint")) SetSelectable("SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",false);	// Gregg: чертёжные/квест-корабли — пресет недоступен
	}
	else if (rship.type.merchant == true)
	{
		SetNewGroupPicture("SHIPSUP_TRADE", "UP_SHIPS_PICTURES", "UP_Trade_ON");
		SetNewGroupPicture("SHIPSUP_UNIVERSAL", "UP_SHIPS_PICTURES", "UP_Universal_OFF");
		SetNewGroupPicture("SHIPSUP_WAR", "UP_SHIPS_PICTURES", "UP_War_OFF");
		shiptype = "Merchant";
		SetSelectable("SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",false);
		SetSelectable("SHIPSUP_BUTTON_WAR_TEMPLATE",false);
		if (CheckAttribute(rship,"QuestShip") || CheckAttribute(rship,"Blueprint")) SetSelectable("SHIPSUP_BUTTON_TRADE_TEMPLATE",false);	// Gregg: чертёжные/квест-корабли — пресет недоступен
	}
	else if (rship.type.war == true)
	{
		SetNewGroupPicture("SHIPSUP_TRADE", "UP_SHIPS_PICTURES", "UP_Trade_OFF");
		SetNewGroupPicture("SHIPSUP_UNIVERSAL", "UP_SHIPS_PICTURES", "UP_Universal_OFF");
		SetNewGroupPicture("SHIPSUP_WAR", "UP_SHIPS_PICTURES", "UP_War_ON");
		shiptype = "War";
		SetSelectable("SHIPSUP_BUTTON_TRADE_TEMPLATE",false);
		SetSelectable("SHIPSUP_BUTTON_UNIVERSAL_TEMPLATE",false);
		if (CheckAttribute(rship,"QuestShip") || CheckAttribute(rship,"Blueprint")) SetSelectable("SHIPSUP_BUTTON_WAR_TEMPLATE",false);	// Gregg: чертёжные/квест-корабли — пресет недоступен
	}
	return shiptype;
}

void DoUpgrade()
{
	if (upgradevalue > 0) AddMoneyToCharacter(pchar, -upgradevalue);
	ref shTo = &RealShips[sti(xi_refCharacter.Ship.Type)];
	for (int i = 0; i < 6; i++)
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
		sGood = GameInterface.SHIPSUP_TABLE_GOODS.(row).td1.icon.image
		goodnum = sti(GameInterface.SHIPSUP_TABLE_GOODS.(row).td3.str);
		shTo.GoodsForUpgrade.(sGood) = goodnum;
	}
	shTo.Shipyarder = refNPCShipyard.index;
	timeUpgrade = timePreUpgrade;
	ProcessExitCancel();
}

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sText4, sPicture;
	string sGroup, sGroupPicture;
	int iItem;
	int xx = 256;
	int yy = 256;

	//Boyer fix #20170401-01 Can't load texture -1.tx log errors
	//sPicture = "-1";
	sPicture = "";
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
		    sHeader = XI_Convertstring("SHIPS_UPGRADES");
			sText1  = GetConvertStr("SHIPSUP_TABLE_hint", "ShipsDescribe.txt");
		break;
	}
	if (bShowHint)
	{
		CreateTooltip("#" + sHeader, sText1, argb(255,255,255,255), sText2, argb(255,255,192,192), sText3, argb(255,192,255,192), sText4, argb(255,255,255,255), sPicture, sGroup, sGroupPicture, xx, yy);
	}
}