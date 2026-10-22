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
int iCabinsNum = 10;//число видов кают
int iCurCabin = 0;

int nCurScrollOfficerNum;

string CurTable, CurRow;
int iSelected; // курсор в таблице

string sMessageMode, sFrom_sea, sShipId;

float shipCostRate;
bool  bShipyardOnTop, bEmptySlot;

int RepairHull, RepairSail;
int timeHull, timeRig, timePaint, timeUpgrade, timePreUpgrade, timeSoil, timeCan;
bool bSquadronRepair = false; // LEO: режим «Общий ремонт эскадры» окна REPAIR_WINDOW; одиночный ремонт не затронут (везде гейт этим флагом)

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
    SetShipyardStore(refNPCShipyard);

    shipCostRate = stf(refNPCShipyard.ShipCostRate);

    int iTest = FindColony(refNPCShipyard.City); // город магазина
    ref rColony;
    sFrom_sea = "";
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
		sFrom_sea = rColony.from_sea; // ветка верфи, в сухопутных верфи нет, значит везде правильная
	}

	FillShipsScroll();

	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
    SetEventHandler("ievnt_command","ProcessCommandExecute",0);
    SetEventHandler("frame","ProcessFrame",1);
    SetEventHandler("confirmShipChangeName","confirmShipChangeName",0);
	SetEventHandler("CheckForRename","CheckForRename",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ExitMsgMenu", "ExitMsgMenu", 0);
	SetEventHandler("ExitRepairMenu", "ExitRepairMenu", 0);
	SetEventHandler("ShowOtherClick", "ShowOtherClick", 0);
	SetEventHandler("ExitCannonsMenu", "ExitCannonsMenu", 0);
    SetEventHandler("ExitOfficerMenu","ExitOfficerMenu",0);
	SetEventHandler("acceptaddofficer","AcceptAddOfficer",0);
	SetEventHandler("BuyShipEvent","BuyShipEvent",0);
	SetEventHandler("ClickREPAIR_Soiling","ClickREPAIR_Soiling",0);
	SetEventHandler("ClickREPAIR_CANNONS","ClickREPAIR_CANNONS",0);
	/* SetEventHandler("OpenShipUp","OpenShipUp",0);
	SetEventHandler("CloseShipUp", "CloseShipUp",0); */
	SetEventHandler("ExitChangeHullMenu", "ExitChangeHullMenu",0);
	SetEventHandler("ExitChangeCabinMenu", "ExitChangeCabinMenu",0);
	SetEventHandler("CheckButtonChange","procCheckBoxChange",0);
	SetEventHandler("SelectShipyard","SelectShipyard",0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
    //////////////////
    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",156,40,366,275); // tak from SHIP_BIG_PICTURE
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 161,246,361,1, 4);
    EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",20,46,134,161);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,187,147,1, 4);
    EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,166,147,1, 4);

    // SetNewGroupPicture("Money_PIC", "ICONS_CHAR", "Money");
    SetNewGroupPicture("REPAIR_Money_PIC", "ICONS_CHAR", "Money");

    SetNewGroupPicture("REPAIR_Hull_PIC", "SHIP_STATE_ICONS", "Hull");
    SetNewGroupPicture("REPAIR_Sails_PIC", "SHIP_STATE_ICONS", "Sails");

    FillShipyardTable();

	SetCurrentNode("SHIPS_SCROLL");
	bShipyardOnTop = false;
	OnShipScrollChange();
	bEmptySlot = false;
	sMessageMode = "";
	SetButtionsAccess();
	timeHull = 0;
	timeRig = 0;
	timePaint = 0;
	timeUpgrade = 0;
	timePreUpgrade = 0;
	timeSoil = 0;
	timeCan = 0;

	ProcessFilter();
	SetNodeUsing("BUTTON_PAINT",false);//отключаем кнопку Покраски, не трогая весь код
	if ( refNPCShipyard.city == "Tortuga" || refNPCShipyard.city == "Villemstad" || refNPCShipyard.city == "PortRoyal" || refNPCShipyard.city == "Havana" || refNPCShipyard.city == "Pirates")
	{
		if (!CheckAttribute(&RealShips[sti(pchar.Ship.Type)],"Shipyarder")) SetNodeUsing("SHIPSUP_BUTTON", true);
		else SetNodeUsing("SHIPSUP_BUTTON", false);
	}
	else SetNodeUsing("SHIPSUP_BUTTON", false);
}

void ProcessExitCancel()
{
	if (S_armor(xi_refCharacter)) // СФ
	{
		timeRig *= 2;
		timeHull *= 2;
	}
	float timeSum;
    // boal время на ремонт -->
    if (timeSoil > 0 || timeRig > 0 || timeHull > 0 || timePaint > 0 || timeUpgrade > 0 || timeCan)
    {
		timeSum = timeSoil;
		timeSum = retMax(timeRig/6.0, timeSum);
		timeSum = retMax(timeHull/4.0, timeSum);
		timeSum = retMax(timePaint/6.0, timeSum);//окраска уже не делается в этом интерфейсе
		timeSum = retMax(timeCan, timeSum);
		timeSum += timeUpgrade;
        LAi_Fade("", "");
        bQuestCheckProcessFreeze = true;
    	WaitDate("",0,0,0, 0, makeint(timeSum * 60.0));
    	RecalculateJumpTable();
    	bQuestCheckProcessFreeze = false;
    	RefreshLandTime();
	}
	// LEO: бесшлюпочному не сбрасывать флаг на базовый, если он легитимно поднят (есть перк-флаг нации)
	if (sti(pchar.ship.type) == SHIP_NOTUSED && !HeroHasFlagPerk(sti(pchar.nation))) PChar.nation = GetBaseHeroNation();
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void ProcessExit()
{
	if (S_armor(xi_refCharacter)) // СФ
	{
		timeRig *= 2;
		timeHull *= 2;
	}
	float timeSum;
    // boal время на ремонт -->
    if (timeSoil > 0 || timeRig > 0 || timeHull > 0 || timePaint > 0 || timeUpgrade > 0 || timeCan)
    {
		timeSum = timeSoil;
		timeSum = retMax(timeRig/6.0, timeSum);
		timeSum = retMax(timeHull/4.0, timeSum);
		timeSum = retMax(timePaint/6.0, timeSum);//окраска уже не делается в этом интерфейсе
		timeSum = retMax(timeCan, timeSum);
		timeSum += timeUpgrade;
        LAi_Fade("", "");
        bQuestCheckProcessFreeze = true;
    	WaitDate("",0,0,0, 0, makeint(timeSum * 60.0));
    	RecalculateJumpTable();
    	bQuestCheckProcessFreeze = false;
    	RefreshLandTime();
	}
	// LEO: бесшлюпочному не сбрасывать флаг на базовый, если он легитимно поднят (есть перк-флаг нации)
	if (sti(pchar.ship.type) == SHIP_NOTUSED && !HeroHasFlagPerk(sti(pchar.nation))) PChar.nation = GetBaseHeroNation();
	IDoExit(RC_INTERFACE_TO_CANNONS);
}

void ProcessExitUpgrades()
{
	if (S_armor(xi_refCharacter)) // СФ
	{
		timeRig *= 2;
		timeHull *= 2;
	}
	float timeSum;
    // boal время на ремонт -->
    if (timeSoil > 0 || timeRig > 0 || timeHull > 0 || timePaint > 0 || timeUpgrade > 0 || timeCan)
    {
		timeSum = timeSoil;
		timeSum = retMax(timeRig/6.0, timeSum);
		timeSum = retMax(timeHull/4.0, timeSum);
		timeSum = retMax(timePaint/6.0, timeSum);//окраска уже не делается в этом интерфейсе
		timeSum = retMax(timeCan, timeSum);
		timeSum += timeUpgrade;
        LAi_Fade("", "");
        bQuestCheckProcessFreeze = true;
    	WaitDate("",0,0,0, 0, makeint(timeSum * 60.0));
    	RecalculateJumpTable();
    	bQuestCheckProcessFreeze = false;
    	RefreshLandTime();
	}
	// LEO: бесшлюпочному не сбрасывать флаг на базовый, если он легитимно поднят (есть перк-флаг нации)
	if (sti(pchar.ship.type) == SHIP_NOTUSED && !HeroHasFlagPerk(sti(pchar.nation))) PChar.nation = GetBaseHeroNation();
	IDoExit(RC_SHIPS_UPGRADES);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
    DelEventHandler("ievnt_command","ProcessCommandExecute");
    DelEventHandler("frame","ProcessFrame");
    DelEventHandler("confirmShipChangeName","confirmShipChangeName");
	DelEventHandler("CheckForRename","CheckForRename");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ExitMsgMenu", "ExitMsgMenu");
	DelEventHandler("ShowOtherClick", "ShowOtherClick");
	DelEventHandler("ExitCannonsMenu", "ExitCannonsMenu");
    DelEventHandler("ExitOfficerMenu","ExitOfficerMenu");
	DelEventHandler("acceptaddofficer","AcceptAddOfficer");
	DelEventHandler("ExitRepairMenu", "ExitRepairMenu");
	DelEventHandler("BuyShipEvent","BuyShipEvent");
	DelEventHandler("ClickREPAIR_Soiling","ClickREPAIR_Soiling");
	DelEventHandler("ClickREPAIR_CANNONS","ClickREPAIR_CANNONS");
	/* DelEventHandler("OpenShipUp","OpenShipUp");
	DelEventHandler("CloseShipUp","CloseShipUp"); */
	DelEventHandler("ExitChangeHullMenu", "ExitChangeHullMenu");
	DelEventHandler("ExitChangeCabinMenu", "ExitChangeCabinMenu");
	DelEventHandler("CheckButtonChange","procCheckBoxChange");
	DelEventHandler("SelectShipyard","SelectShipyard");
	DelEventHandler("OnTableClick", "OnTableClick");

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

void SelectShipyard()
{
	SetCurrentNode("TABLE_SHIPYARD");
}

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();

    switch(nodName)
	{
		case "MSG_CREW_CHECKBOX":
			if (comName=="rclick")
			{
				string sText1, sText2, sText3, sPicture, sGroup, sGroupPicture;
				sText1 = GetConvertStrWithReplace("Variable_shipyard_1", "Interface.txt", "#space#", " ");
				sText2 = GetConvertStrWithReplace("Variable_shipyard_2", "Interface.txt", "#space#", " ");
				sText3 = GetConvertStrWithReplace("Variable_shipyard_3", "Interface.txt", "#space#", " ");
				sPicture = "none";
			    CreateTooltip("#" + GetConvertStrWithReplace("Variable_shipyard_4", "Interface.txt", "#space#", " "), sText1, argb(255,255,255,255), sText2, argb(255,255,255,255), sText3, argb(255,255,255,255), "", argb(255,255,255,255), sPicture, sGroup, sGroupPicture, 256, 256);
			}
		break;
        case "BUTTON_SELL":
			if (comName=="click" || comName=="activate")
			{
			    ShowMessageInfo();
			}
		break;

		case "BUTTON_BUY":
			if (comName=="click" || comName=="activate")
			{
			    ShowMessageInfo();
			}
		break;
		//#20180922-01
		case "BUTTON_PAINT":
			if (comName=="click" || comName=="activate")
			{
			    ShowChangeHullMenu();
			}
		break;

		case "BUTTON_CABIN":
			if (comName=="click" || comName=="activate")
			{
			    ShowChangeCABINMenu();
			}
		break;
		
		case "BUTTON_CANNONS":
			ProcessExit();
		break;

		case "CHANGE_HULL_LEFT":
			if(comName=="click")
			{
				SelectChangeHull(true);
			}
		break;

		case "CHANGE_HULL_RIGHT":
			if(comName=="click")
			{
				SelectChangeHull(false);
			}
		break;

		case "CHANGE_HULL_OK":
			if(comName=="click")
			{
				SetNewHullToCharacterShip();
			}
		break;

		case "CHANGE_CABIN_LEFT":
			if(comName=="click")
			{
				SelectChangeCABIN(true);
			}
		break;

		case "CHANGE_CABIN_RIGHT":
			if(comName=="click")
			{
				SelectChangeCABIN(false);
			}
		break;

		case "CHANGE_CABIN_OK":
			if(comName=="click")
			{
				SetNewCABINToCharacterShip();
			}
		break;

		case "SHIPSUP_BUTTON":
			if (comName=="click" || comName=="activate")
			{
			    ProcessExitUpgrades();
			}
		break;

		/*case "SHIPSUP_DOUP":
			if (comName=="click" || comName=="activate")
			{
			    DoUpgrade();
			}
		break; */

		case "CLASS_ALL":
			if (comName == "click")ProcessFilter();
		break;

		case "MSG_OK":
			if (comName=="click" || comName=="activate")
			{
			    MessageOk();
			}
		break;

		case "BUTTON_REPAIR":
			if (comName=="click" || comName=="activate")
			{
			    ShowRepairMenu();
			}
		break;

		case "BUTTON_SQUADRON_REPAIR":
			if (comName=="click" || comName=="activate")
			{
			    ShowSquadronRepairMenu();
			}
		break;

		case "REPAIR_LEFT_H":
			if (comName=="click" || comName=="activate")
			{
			    ClickRepairArror("hull", -1);
			}
			if (comName=="rclick")
			{
			    ClickRepairArror("hull", -5);
			}
		break;

		case "REPAIR_RIGHT_H":
			if (comName=="click" || comName=="activate")
			{
			    ClickRepairArror("hull", 1);
			}
			if (comName=="rclick")
			{
			    ClickRepairArror("hull", 5);
			}
		break;

		case "REPAIR_LEFT_S":
			if (comName=="click" || comName=="activate")
			{
			    ClickRepairArror("sail", -1);
			}
			if (comName=="rclick")
			{
			    ClickRepairArror("sail", -5);
			}
		break;

		case "REPAIR_RIGHT_S":
			if (comName=="click" || comName=="activate")
			{
			    ClickRepairArror("sail", 1);
			}
			if (comName=="rclick")
			{
			    ClickRepairArror("sail", 5);
			}
		break;

		case "REPAIR_OK":
			if (comName=="click" || comName=="activate")
			{
			    RepairOk();
			}
		break;

		case "REPAIR_ALL":
			if (comName=="click" || comName=="activate")
			{
			    RepairAll();
			}
		break;
	}
}

void procCheckBoxChange()
{
	string sNodName = GetEventData();
	int nBtnIndex = GetEventData();
	int bBtnState = GetEventData();

	if( sNodName == "MSG_CREW_CHECKBOX" ) {
		if( bBtnState==true ) {
			switch( nBtnIndex ) {
			case 1: SellState="Max"; break;
			case 2: SellState="Opt" break;
			case 3: SellState="Fire" break;
			}
		}
		return;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
void FillShipsScroll()
{
    DeleteAttribute(&GameInterface, "SHIPS_SCROLL");
	nCurScrollNum = -1;
	if(!CheckAttribute(&GameInterface,"SHIPS_SCROLL.current"))
	{
		GameInterface.SHIPS_SCROLL.current = 0;
	}

	string attributeName;
	string shipName;
	string potc = "";
	int iShipType, cn;
	GameInterface.SHIPS_SCROLL.ImagesGroup.t0 = "BLANK_SHIP2";

	FillShipList("SHIPS_SCROLL.ImagesGroup", xi_refCharacter);
	GameInterface.SHIPS_SCROLL.BadTex1 = 0;
	GameInterface.SHIPS_SCROLL.BadPic1 = "Not Used2";
	if (InterfaceStates.InterfaceShipString == "interfaces\ships_PotC\") potc = "_POTC";

	int m = 0;
	for(int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(pchar, i);
		if(cn != -1)
		{
			iShipType = sti(characters[cn].ship.type);
			if(iShipType != SHIP_NOTUSED)
			{
				attributeName = "pic" + (m+1);
				ref rBaseShip = GetRealShip(iShipType);
				iShipType = sti(RealShips[iShipType].basetype);
				shipName = ShipsTypes[iShipType].Name;

				GameInterface.SHIPS_SCROLL.(attributeName).character = cn;
				GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#"+GetConvertStrWithReplace("Variable_shipyard_5", "Interface.txt", "#space#", " ")+ShipsTypes[iShipType].Class;
				GameInterface.SHIPS_SCROLL.(attributeName).str4 = shipName;
				GameInterface.SHIPS_SCROLL.(attributeName).str3 = "#" + MakeMoneyShow(GetShipSellPrice(&characters[cn], refNPCShipyard), MONEY_SIGN,MONEY_DELIVER);
				if (sti(rBaseShip.Type.Merchant)) GameInterface.SHIPS_SCROLL.(attributeName).str5 = "#"+"Í";
				if (sti(rBaseShip.Type.War)) GameInterface.SHIPS_SCROLL.(attributeName).str6 = "#"+"É";
				GameInterface.SHIPS_SCROLL.(attributeName).img1 = "ship";
				GameInterface.SHIPS_SCROLL.(attributeName).tex1 = FindFaceGroupNum("SHIPS_SCROLL.ImagesGroup","SHIPS_"+shipName+potc);
				m++;
			}
			else
			{
				attributeName = "pic" + (m+1);
				GameInterface.SHIPS_SCROLL.(attributeName).character = cn;
				GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#";
				GameInterface.SHIPS_SCROLL.(attributeName).str2 = "NoneBoat";
				GameInterface.SHIPS_SCROLL.(attributeName).img1 = "Not Used2";
				GameInterface.SHIPS_SCROLL.(attributeName).tex1 = "BLANK_SHIP2";
				m++;
			}
		}
		else
		{
		    attributeName = "pic" + (m+1);
			GameInterface.SHIPS_SCROLL.(attributeName).character = -1;
			GameInterface.SHIPS_SCROLL.(attributeName).str1 = "#";
			GameInterface.SHIPS_SCROLL.(attributeName).str2 = "";
			GameInterface.SHIPS_SCROLL.(attributeName).img1 = "Not Used2";
			GameInterface.SHIPS_SCROLL.(attributeName).tex1 = "BLANK_SHIP2";
			m++;
		}
	}

	GameInterface.SHIPS_SCROLL.ListSize = m;
	//GameInterface.SHIPS_SCROLL.NotUsed = 5 - m + 1;
	GameInterface.SHIPS_SCROLL.NotUsed = 0;
}

void ProcessFrame()
{
	string attributeName;
	int iCharacter;

	string sNode = GetCurrentNode();

    if (sNode == "PASSENGERSLIST" && sti(GameInterface.PASSENGERSLIST.current)!= nCurScrollOfficerNum)
	{
		nCurScrollOfficerNum = sti(GameInterface.PASSENGERSLIST.current);
		SetOfficersSkills();
	}
	if(sNode == "SHIPS_SCROLL")
	{
		if (sti(GameInterface.SHIPS_SCROLL.current)!= nCurScrollNum || CurTable != "")
		{
            CurTable = "";
			NullSelectTable("TABLE_SHIPYARD"); // убрать скрол
			bShipyardOnTop = false;

			nCurScrollNum = sti(GameInterface.SHIPS_SCROLL.current);

			attributeName = "pic" + (nCurScrollNum+1);

			iCharacter = sti(GameInterface.SHIPS_SCROLL.(attributeName).character);

			if(iCharacter > 0)
			{
				string sChrId = characters[iCharacter].id;

				xi_refCharacter = characterFromID(sChrId);
				bEmptySlot = false;
				shipIndex= nCurScrollNum+ 1;
				if (shipIndex< 0)
				{
					shipIndex= 0;
				}
				if (sti(xi_refCharacter.ship.type) == SHIP_NOTUSED)
				{
					shipIndex= -1;
				}
				// boal оптимизация скилов -->
			    DelBakSkillAttr(xi_refCharacter);
			    ClearCharacterExpRate(xi_refCharacter);
			    RefreshCharacterSkillExpRate(xi_refCharacter);

			    SetEnergyToCharacter(xi_refCharacter);
			    // boal оптимизация скилов <--
				OnShipScrollChange();
			}
			else
			{
				shipIndex= -1;
				bEmptySlot = true;
			}
			SetButtionsAccess();
		}
	}
}
void FillShipParam(ref _chr)
{
    int iShip = sti(_chr.ship.type);
	if (iShip != SHIP_NOTUSED)
	{
        if (CheckAttribute(_chr, "Ship.Cargo.RecalculateCargoLoad") && sti(_chr.Ship.Cargo.RecalculateCargoLoad))
		{
			RecalculateCargoLoad(_chr);
			_chr.Ship.Cargo.RecalculateCargoLoad = 0;
		}
		ref refBaseShip = GetRealShip(iShip);
		string sShip = refBaseShip.BaseName;
		SetNewPicture("SHIP_BIG_PICTURE", InterfaceStates.InterfaceShipString + sShip + ".dds");
		if (!CheckAttribute(refBaseShip,"QuestShip")) SetNodeUsing("SHIP_BIG_PICTURE_VIDEO",false);
		else
		{
			SetNewPicture("SHIP_BIG_PICTURE", InterfaceStates.InterfaceShipString + sShip + ".dds");
			SetNodeUsing("SHIP_BIG_PICTURE_VIDEO",true);
			SetNewVideoPicture("SHIP_BIG_PICTURE_VIDEO","SHIP_"+sShip);
		}

		GameInterface.edit_box.str = _chr.ship.name;
		SetFormatedText("SHIP_RANK", refBaseShip.Class);
		SetFormatedText("CLASS_ARMOR", refBaseShip.HullArmor);
		if (!CheckAttribute(refBaseShip,"Tuning.HullArmor")) SetNewGroupPicture("CLASS_ARMOR_ICON", "ICONS_SPEC", "Normal");
		else SetNewGroupPicture("CLASS_ARMOR_ICON", "ICONS_SPEC", "Upgraded");
		SetShipOTHERTable("TABLE_OTHER", _chr);

		if (CheckAttribute(refBaseShip,"Upgrade")) SetUpgradesNodes(refBaseShip,refBaseShip,false);
		else DisableUpgradeNodes(refBaseShip,refBaseShip,false);

		SetShipQualityTable(_chr, "BAR_HP", "BAR_Mast", "BAR_Speed", "BAR_TurnRate", "BAR_WAS", "BAR_Capacity", "BAR_Crew");
		SetFormatedText("FRAME_INFO_CAPTION", XI_ConvertString(sShip));
		SetFormatedText("INFO_TEXT", GetConvertStr(sShip, "ShipsDescribe.txt"));
	}
	else
	{
		SetNewPicture("SHIP_BIG_PICTURE", "interfaces\blank_ship2.dds");
		SetNodeUsing("SHIP_BIG_PICTURE_VIDEO",false);
		GameInterface.edit_box.str = XI_Convertstring("NoneBoat");
		SetFormatedText("FRAME_INFO_CAPTION","");
		SetFormatedText("INFO_TEXT","");
	}
	Table_UpdateWindow("TABLE_OTHER");
}

void OnShipScrollChange()
{
	SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\128\face_" + xi_refCharacter.FaceId + ".tga");
	SetFormatedText("HERO_NAME", GetFullName(xi_refCharacter));
	SetFormatedText("HERO_RANK",xi_refCharacter.Rank);
	// нулим все формы, тк корабля может не быть
	SetFormatedText("SHIP_RANK","");
	Table_Clear("TABLE_OTHER", false, true, false);
	//Ship info window
	SetFormatedText("Money_TEXT", MakeMoneyShow(sti(pchar.Money), MONEY_SIGN,MONEY_DELIVER));
    FillShipParam(xi_refCharacter);
}

void confirmShipChangeName()
{
	/*
	if (shipIndex== -1) return;
	xi_refcharacter.ship.name = GameInterface.edit_box.str;
	GameInterface.edit_box.str = xi_refcharacter.ship.name;
	SetCurrentNode("SHIP_INFO_TEXT");   */
}

void CheckForRename()
{
	/*if (GetShipRemovable(xi_refCharacter) == true && shipIndex!= -1)
	{
		SetCurrentNode("EDIT_BOX");
	}    */
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
	if (HasSubStr(sCurrentNode,"SHIPSUP_")) {SetUpgradeTooltip(sCurrentNode); return;}
	switch (sCurrentNode)
	{
		case "SHIP_BIG_PICTURE":
			if (shipIndex != -1 || bShipyardOnTop)
			{
			    iShip = sti(rChr.ship.type);
			    refBaseShip = GetRealShip(iShip);
				sHeader = XI_ConvertString(refBaseShip.BaseName);
				sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");
				//--> показ каюты по ПКМ на портрете корабля
				int nLoc = FindLocation(refBaseShip.CabinType);
				if (nLoc >= 0 && CheckAttribute(&Locations[nLoc],"image")) 
				{
					sPicture = Locations[nLoc].image;
					xx = 512;//размер картинки
					yy = 288;
				}
				//<-- показ каюты
			}
			else
			{
				sHeader = XI_Convertstring("NoneBoat");
				sText1  = GetConvertStr("NoneBoat2", "ShipsDescribe.txt");
			}
		break;

		case "SHIPS_SCROLL":
			if (shipIndex != -1)
			{
			    iShip = sti(xi_refCharacter.ship.type);
			    refBaseShip = GetRealShip(iShip);
				sHeader = XI_ConvertString(refBaseShip.BaseName);
				sText1 = GetConvertStr(refBaseShip.BaseName, "ShipsDescribe.txt");
				//sText2 = GetRPGText("shipChoose_hint");
			}
			else
			{
				sHeader = XI_Convertstring("NoneBoat");
				sText1  = GetConvertStr("NoneBoat2", "ShipsDescribe.txt");
			}
		break;

		case "MAIN_CHARACTER_PICTURE":
			// отдельная форма
			bShowHint = false;
			ShowRPGHint();
		break;

		case "TABLE_OTHER":
			sHeader = XI_ConvertString(GameInterface.(CurTable).(CurRow).UserData.ID);
		    sText1  = GetConvertStr(GameInterface.(CurTable).(CurRow).UserData.ID, "ShipsDescribe.txt");
			if (sti(rChr.ship.type) != SHIP_NOTUSED) {
				sText2 = GameInterface.(CurTable).(CurRow).UserData.ID1;
				sText3 = GameInterface.(CurTable).(CurRow).UserData.ID2;
				if (GameInterface.(CurTable).(CurRow).UserData.ID == "Hull") {
					sText4 = AddTextAboutHull(rChr);
				}
			}
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType" && sti(rChr.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON && GetCannonsNum(rChr)){
				sGroup = "GOODS";
				sGroupPicture = GetCannonType(sti(rChr.Ship.Cannons.Type)) + "_" + GetCannonCaliber(sti(rChr.Ship.Cannons.Type));
				xx = 128;
				yy = 128;
			}
			// трюм
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "Capacity" && sti(rChr.ship.type) != SHIP_NOTUSED)
			{
				sText3 = GetConvertStrWithReplace("Variable_shipyard_14", "Interface.txt", "#space#", " ") + FloatToString((stf(GetCargoLoad(rChr))  /  stf(GetCargoMaxSpace(rChr))) * 100.0, 1)+ " %";
				xx = 128;
				yy = 128;
			}
		break;

		case "TABLE_SHIPYARD" :
		    sHeader = XI_Convertstring("Shipyard");
			sText1  = GetConvertStr("Shipyard_hint", "ShipsDescribe.txt");
		break;
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

void HideInfoWindow()
{
	CloseTooltip();
	ExitRPGHint();
}

void TableSelectChange()
{
	string sControl = GetEventData();
	iSelected = GetEventData();
    CurTable = sControl;
    CurRow   =  "tr" + (iSelected);
    NullSelectTable("TABLE_OTHER");
    // не тереть скрол на верфи

    // заполнялка
    if (CurTable == "TABLE_SHIPYARD")
	{
	    sShipId = GameInterface.(CurTable).(CurRow).sShipId;
        FillShipyardShip(refNPCShipyard, sShipId);
		FillShipParam(refNPCShipyard);
		bShipyardOnTop = true;
		SetButtionsAccess();
	}
	/*if (CurTable == "SHIPSUP_TABLE")
	{
		int iShip = sti(xi_refCharacter.Ship.Type);
		int upsq = 0;
		switch(CurRow)
		{
			case "tr1":
				if (CheckAttribute(RealShips[iShip],"Tuning.HullSpecial")) upgradevalue = 0;
				else upgradevalue = makeint(sti(RealShips[iShip].price)/2);
				uptype = 1;
			break;
			case "tr2":
				if (CheckAttribute(RealShips[iShip],"Tuning.SailsSpecial")) upgradevalue = 0;
				else upgradevalue = makeint(sti(RealShips[iShip].price)*0.35);
				uptype = 2;
			break;
			case "tr3":
				if (CheckAttribute(RealShips[iShip],"Tuning.CannonsSpecial")) upgradevalue = 0;
				else upgradevalue = makeint(sti(RealShips[iShip].price)*0.25);
				uptype = 3;
			break;
			case "tr4":
				if (CheckAttribute(RealShips[iShip],"Tuning.CuBot")) upgradevalue = 0;
				else upgradevalue = makeint(sti(RealShips[iShip].price)*0.4);
				uptype = 4;
			break;
			case "tr5":
				if (CheckAttribute(RealShips[iShip],"Tuning.BotPack")) upgradevalue = 0;
				else upgradevalue = makeint(sti(RealShips[iShip].price)*0.5);
				uptype = 5;
			break;
			case "tr6":
				if (CheckAttribute(RealShips[iShip],"Tuning.HighBort")) upgradevalue = 0;
				else upgradevalue = makeint(sti(RealShips[iShip].price)*0.4);
				uptype = 6;
			break;
		}

		if (CheckAttribute(RealShips[iShip],"Tuning.HullSpecial")) upsq++;
		if (CheckAttribute(RealShips[iShip],"Tuning.SailsSpecial")) upsq++;
		if (CheckAttribute(RealShips[iShip],"Tuning.CannonsSpecial")) upsq++;
		if (CheckAttribute(RealShips[iShip],"Tuning.CuBot")) upsq++;
		if (CheckAttribute(RealShips[iShip],"Tuning.BotPack")) upsq++;
		if (CheckAttribute(RealShips[iShip],"Tuning.HighBort")) upsq++;

		switch (sti(RealShips[iShip].class))
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
		}
		if (upgradevalue == 0)
		{
			timePreUpgrade = makeint(timePreUpgrade/2);
			SetFormatedText("SHIPSUP_VALUE",GetConvertStrWithReplace("Variable_shipyard_15", "Interface.txt", "#space#", " ")+timePreUpgrade);
		}
		else SetFormatedText("SHIPSUP_VALUE",FindRussianMoneyString(upgradevalue)+GetConvertStrWithReplace("Variable_shipyard_16", "Interface.txt", "#space#", " ")+timePreUpgrade);

		SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_DOUP",0, "#"+GetConvertStrWithReplace("Variable_shipyard_17", "Interface.txt", "#space#", " "));
		if (upgradevalue == 0) SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_DOUP",0, "#"+GetConvertStrWithReplace("Variable_shipyard_18", "Interface.txt", "#space#", " "));

		if (sti(pchar.money)>=upgradevalue && upsq < 3) SetSelectable("SHIPSUP_DOUP", true);
		if (sti(pchar.money)<upgradevalue) SetSelectable("SHIPSUP_DOUP", false);
		if (upsq >= 3 && upgradevalue > 0)
		{
			SetFormatedText("SHIPSUP_VALUE",GetConvertStrWithReplace("Variable_shipyard_19", "Interface.txt", "#space#", " "));
			SetSelectable("SHIPSUP_DOUP", false);
		}
		if (upsq >= 3 && upgradevalue == 0)
		{
			SetFormatedText("SHIPSUP_VALUE",GetConvertStrWithReplace("Variable_shipyard_20", "Interface.txt", "#space#", " ")+timePreUpgrade);
			SetSelectable("SHIPSUP_DOUP", true);
			SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"SHIPSUP_DOUP",0, "#"+GetConvertStrWithReplace("Variable_shipyard_21", "Interface.txt", "#space#", " "));
		}
		switch(uptype)
		{
			case 1:
				if (RealShips[iShip].Type.War == true && RealShips[iShip].Type.Merchant == false)
				{
					if (sti(pchar.money)>=upgradevalue && upsq < 3) SetSelectable("SHIPSUP_DOUP", true);
				}
				else
				{
					SetFormatedText("SHIPSUP_VALUE",GetConvertStrWithReplace("Variable_shipyard_22", "Interface.txt", "#space#", " "));
					SetSelectable("SHIPSUP_DOUP", false);
				}
			break;
			case 2:

			break;
			case 3:
				if (RealShips[iShip].Type.War == true && RealShips[iShip].Type.Merchant == false)
				{
					if (sti(pchar.money)>=upgradevalue && upsq < 3) SetSelectable("SHIPSUP_DOUP", true);
				}
				else
				{
					SetFormatedText("SHIPSUP_VALUE",GetConvertStrWithReplace("Variable_shipyard_23", "Interface.txt", "#space#", " "));
					SetSelectable("SHIPSUP_DOUP", false);
				}
			break;
			case 4:

			break;
			case 5:
				if (RealShips[iShip].Type.War == false && RealShips[iShip].Type.Merchant == true)
				{
					if (sti(pchar.money)>=upgradevalue && upsq < 3) SetSelectable("SHIPSUP_DOUP", true);
				}
				else
				{
					SetFormatedText("SHIPSUP_VALUE",GetConvertStrWithReplace("Variable_shipyard_24", "Interface.txt", "#space#", " "));
					SetSelectable("SHIPSUP_DOUP", false);
				}
			break;
			case 6:

			break;
		}
	}*/
}

void ExitRPGHint()
{
	if (sMessageMode == "RPG_Hint")
	{
		XI_WindowShow("RPG_WINDOW", false);
		XI_WindowDisable("RPG_WINDOW", true);
		XI_WindowDisable("MAIN_WINDOW", false);
		sMessageMode = "";
	}
}

void ShowRPGHint()
{
	SetSPECIALMiniTable("RPG_TABLE_SMALLSKILL", xi_refCharacter);
    SetOTHERMiniTable("RPG_TABLE_SMALLOTHER", xi_refCharacter);
    SetFormatedText("RPG_OFFICER_NAME", GetFullName(xi_refCharacter));

	XI_WindowShow("RPG_WINDOW", true);
	XI_WindowDisable("RPG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	sMessageMode = "RPG_Hint";
}


void NullSelectTable(string sControl)
{
	if (sControl != CurTable)
	{
	    GameInterface.(sControl).select = 0;
	    Table_UpdateWindow(sControl);
	}
}

void ExitMsgMenu()
{
	XI_WindowShow("MSG_WINDOW", false);
	XI_WindowDisable("MSG_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("SHIPS_SCROLL");
	sMessageMode = "";
}

void ShowMsgMenu(bool change)
{
	XI_WindowShow("MSG_WINDOW", true);
	XI_WindowDisable("MSG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetNodeUsing("MSG_WINDOW_TEXT4",true);
	if (change) SetNodeUsing("MSG_CREW_CHECKBOX",false);
	else SetNodeUsing("MSG_CREW_CHECKBOX",true);

	SetCurrentNode("MSG_CANCEL");
}

void ShowOtherClick()
{
	if (CheckAttribute(GameInterface,CurTable+"."+CurRow+".UserData"))
	{
		if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType")
		{
			ShowCannonsMenu();
		}
	}
}

void ShowCannonsMenu()
{
	XI_WindowShow("CANNONS_WINDOW", true);
	XI_WindowDisable("CANNONS_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetCurrentNode("CANNONS_CANCEL");

    DeleteAttribute(&GameInterface, "CANNONS_TABLE.BackUp")
    CannonsMenuRefresh();
}
void CannonsMenuRefresh()
{
	ref rChr;
	if (bShipyardOnTop)
	{
	    rChr = refNPCShipyard;
	}
	else
	{
		rChr = xi_refCharacter;
	}
	int idx = GetCannonGoodsIdxByType(sti(rChr.Ship.Cannons.Type));
	if (idx != -1)
	{
		int nShipType = GetCharacterShipType(rChr);
		ref refBaseShip = GetRealShip(nShipType);
	    SetNewGroupPicture("CANNONS_PIC", "GOODS", Goods[idx].Name);
		SetFormatedText("CANNONS_TEXT", XI_ConvertString(Goods[idx].Name));

		SetFormatedText("CANNONS_QTY_F", its(GetBortCannonsQty(rChr, "fcannon")) + "/" + refBaseShip.fcannon);
		SetFormatedText("CANNONS_QTY_B", its(GetBortCannonsQty(rChr, "bcannon")) + "/" + refBaseShip.bcannon);
		SetFormatedText("CANNONS_QTY_R", its(GetBortCannonsQty(rChr, "rcannon")) + "/" + refBaseShip.rcannon);
		SetFormatedText("CANNONS_QTY_L", its(GetBortCannonsQty(rChr, "lcannon")) + "/" + refBaseShip.lcannon);
		/// всего GetCannonsNum(xi_refCharacter)
	}
	else
	{
		SetNewGroupPicture("CANNONS_PIC", "GOODS", "");
		SetFormatedText("CANNONS_TEXT", "");
		SetFormatedText("CANNONS_QTY_F", "0");
		SetFormatedText("CANNONS_QTY_B", "0");
		SetFormatedText("CANNONS_QTY_R", "0");
		SetFormatedText("CANNONS_QTY_L", "0");
	}
}
void ExitCannonsMenu()
{
	HideCannonsMenu();
}

void HideCannonsMenu()
{
	XI_WindowShow("CANNONS_WINDOW", false);
	XI_WindowDisable("CANNONS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("TABLE_OTHER");
	sMessageMode = "";
}

void FillShipyardTable()
{
    Table_Clear("TABLE_SHIPYARD", false, true, false);

	GameInterface.TABLE_SHIPYARD.hr.td1.str = GetConvertStr("Variable_shipyard_25", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td1.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.hr.td2.str = GetConvertStr("Variable_shipyard_27", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td2.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td3.str = GetConvertStr("Variable_shipyard_28", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td3.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td4.str = GetConvertStr("Variable_shipyard1_15", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td4.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td5.str = GetConvertStr("Variable_shipyard_30", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td5.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td6.str = GetConvertStr("Variable_shipyard1_18", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td6.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td7.str = GetConvertStr("Variable_ColonyGuardiang_18", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td7.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td8.str = GetConvertStr("Variable_shipyard1_17", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td8.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.select = 0;
	GameInterface.TABLE_SHIPYARD.top = 0;
//--> mod tablesort
	GameInterface.TABLE_SHIPYARD.hr.td1.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td2.sorttype = "string";
	GameInterface.TABLE_SHIPYARD.hr.td3.sorttype = "string";
	GameInterface.TABLE_SHIPYARD.hr.td4.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td5.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td6.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td7.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td8.sorttype = "";
//<-- mod tablesort

	aref   arDest, arImt;
	string sAttr;
	int    iNum, i, iShip;
	ref    refShip;
	string sShip;
	string row;
	int k = 0;

	makearef(arDest, refNPCShipyard.shipyard);
	iNum = GetAttributesNum(arDest);
	for (i = 0; i < iNum; i++)
	{
        row = "tr" + (k+1);

		arImt = GetAttributeN(arDest, i);
		//Log_Info(GetAttributeName(arImt));
		sAttr = GetAttributeName(arImt);
    	FillShipyardShip(refNPCShipyard, sAttr); // скинуть в стандарт корабль из ШипХХ

    	iShip = sti(refNPCShipyard.Ship.Type);
    	refShip = GetRealShip(iShip);
		sShip = refShip.BaseName;

		int nClass = sti(refShip.Class);
		if (FIS_FilterState != FIS_ALL)
		{
			if (sti(FIS_FilterState) != nClass)
			{
				DeleteAttribute(&GameInterface,"TABLE_SHIPYARD.row");
				continue;
			}
		}

		k++;
		GameInterface.TABLE_SHIPYARD.(row).sShipId = sAttr;
		GameInterface.TABLE_SHIPYARD.(row).index = i;
        GameInterface.TABLE_SHIPYARD.(row).td1.icon.texture = InterfaceStates.InterfaceShipString + sShip + ".dds";
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.uv = "0,0,1,1";
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.width = 46;
    	GameInterface.TABLE_SHIPYARD.(row).td1.icon.height = 46;
    	GameInterface.TABLE_SHIPYARD.(row).td1.icon.offset = "0, 1";
		//GameInterface.TABLE_SHIPYARD.(row).td1.scale = 0.9;
		GameInterface.TABLE_SHIPYARD.(row).td1.str = refShip.Class;
		GameInterface.TABLE_SHIPYARD.(row).td1.textoffset = "40,-16";

		GameInterface.TABLE_SHIPYARD.(row).td2.str = XI_Convertstring(sShip)/*  + "\n"+refNPCShipyard.ship.name */;
		GameInterface.TABLE_SHIPYARD.(row).td2.scale = 0.76;
		GameInterface.TABLE_SHIPYARD.(row).td2.align = "left";

		GameInterface.TABLE_SHIPYARD.(row).td2.textoffset = "0,6";//сдвинул текст чуть ниже, для более красивого расположения плашки, по умолчанию пустая строка IG Baron. 
		GameInterface.TABLE_SHIPYARD.(row).td2.icon.width = 51;
		GameInterface.TABLE_SHIPYARD.(row).td2.icon.height = 26;
		GameInterface.TABLE_SHIPYARD.(row).td2.icon.offset = "0, -4";
		if (LanguageGetLanguage() != "russian") sTemp = "english "; else sTemp = "";
		GameInterface.TABLE_SHIPYARD.(row).td2.icon.group = "ICONS_SPEC";
		GameInterface.TABLE_SHIPYARD.(row).td2.icon.image = sTemp + "universal ship icon"; 
		if ( sti(refShip.Type.Merchant) && !sti(refShip.Type.War)) 
			GameInterface.TABLE_SHIPYARD.(row).td2.icon.image = sTemp + "trader ship icon";
		if ( !sti(refShip.Type.Merchant) && sti(refShip.Type.War)) 
			GameInterface.TABLE_SHIPYARD.(row).td2.icon.image = sTemp + "battle ship icon";//плашка для боевого типа корабля (есть ещё для быстрого fast ship icon)

		GameInterface.TABLE_SHIPYARD.(row).td3.str = GetShipBuyPrice(iShip, refNPCShipyard);
		GameInterface.TABLE_SHIPYARD.(row).td3.scale = 0.9;
		GameInterface.TABLE_SHIPYARD.(row).td3.color = argb(255,255,228,80);
		GameInterface.TABLE_SHIPYARD.(row).td4.str = refShip.rcannon + "\n" + refShip.fcannon + "ðñ:::::::::ð" + refShip.bcannon + "\n" + refShip.lcannon;
		GameInterface.TABLE_SHIPYARD.(row).td5.str = GetCargoMaxSpace(refNPCShipyard);
		GameInterface.TABLE_SHIPYARD.(row).td6.str = sti(refShip.OptCrew);
		GameInterface.TABLE_SHIPYARD.(row).td7.str = sti(refShip.hp);
		GameInterface.TABLE_SHIPYARD.(row).td8.str = FloatToString(stf(refShip.SpeedRate),1);
		GameInterface.TABLE_SHIPYARD.(row).td8.textoffset = "-8,0";
    }

	Table_UpdateWindow("TABLE_SHIPYARD");
}

string GetShipsType(ref refBaseShip)
{
	string spectypes = "";
	if (refBaseShip.Type.War == true) spectypes = GetCOnvertStr("Variable_specType_War_S", "Interface.txt");
	if (refBaseShip.Type.Merchant == true) spectypes = GetCOnvertStr("Variable_specType_Trade_S", "Interface.txt");
	if (refBaseShip.Type.Merchant == true && refBaseShip.Type.War == true) spectypes = GetCOnvertStr("Variable_specType_Universal_S", "Interface.txt");
	return spectypes;
}

void ProcessFilter()
{
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 1))
	{
		FIS_FilterState = FIS_ALL;
		FillShipyardTable();
		return;
	}
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 2))
	{
		FIS_FilterState = FIS_7;
		FillShipyardTable();
		return;
	}
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 3))
	{
		FIS_FilterState = FIS_6;
		FillShipyardTable();
		return;
	}
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 4))
	{
		FIS_FilterState = FIS_5;
		FillShipyardTable();
		return;
	}
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 5))
	{
		FIS_FilterState = FIS_4;
		FillShipyardTable();
		return;
	}
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 6))
	{
		FIS_FilterState = FIS_3;
		FillShipyardTable();
		return;
	}
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 7))
	{
		FIS_FilterState = FIS_2;
		FillShipyardTable();
		return;
	}
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "CLASS_ALL", 3, 8))
	{
		FIS_FilterState = FIS_1;
		FillShipyardTable();
		return;
	}
}

void SetButtionsAccess()
{
    SetSelectable("BUTTON_REPAIR", false);
    SetSelectable("BUTTON_SELL", true);
    SetSelectable("BUTTON_BUY", true);
	//#20180922-01
    SetSelectable("BUTTON_PAINT", true);
	SetSelectable("SHIPSUP_BUTTON", true);
	SetSelectable("BUTTON_CABIN", true);

    if (bShipyardOnTop)
    {
        SetSelectable("BUTTON_REPAIR", false);
    	SetSelectable("BUTTON_SELL", false);
    	//#20180922-01
    	SetSelectable("BUTTON_PAINT", false);
    	SetSelectable("BUTTON_CABIN", false);
		SetSelectable("SHIPSUP_BUTTON", false);
    	if (shipIndex == -1)// проверка на цену
    	{
    	    if (GetShipBuyPrice(sti(refNPCShipyard.Ship.Type), refNPCShipyard) > sti(pchar.Money) )
    	    {
    	        SetSelectable("BUTTON_BUY", false);
    	    }
   			if (bEmptySlot && sti(pchar.ship.type) == SHIP_NOTUSED)
   			{ // нельзя купить корабль компаньону, если у ГГ нету
   			    SetSelectable("BUTTON_BUY", false);
   			}
    	}
    	else
    	{
    	    if (GetShipBuyPrice(sti(refNPCShipyard.Ship.Type), refNPCShipyard) > (GetShipSellPrice(xi_refCharacter, refNPCShipyard) + sti(pchar.Money)) )
    	    {
    	        SetSelectable("BUTTON_BUY", false);
    	    }
    	    if (!GetRemovable(xi_refCharacter) || !GetShipRemovable(xi_refCharacter))
    	    {
    	        SetSelectable("BUTTON_BUY", false);
    	    }
    	}
    }
    else
    {
        SetSelectable("BUTTON_BUY", false);
		if (!checkAttribute(xi_refCharacter, "ship.soiling")) xi_refCharacter.ship.soiling = 0;//фикс ошибок в логах
		if (GetCannonQuantity(xi_refCharacter) > 0 && GetCannonsNum(xi_refCharacter) > 0 && GetCannonsNum(xi_refCharacter) < GetCannonQuantity(xi_refCharacter)) SetSelectable("BUTTON_REPAIR", true);
        if (GetHullPercent(xi_refCharacter) < 100 || GetSailPercent(xi_refCharacter) < 100 || sti(xi_refCharacter.ship.soiling) > 0)
        {
            SetSelectable("BUTTON_REPAIR", true);
        }
        if (shipIndex == -1)
        {
            SetSelectable("BUTTON_SELL", false);
            SetSelectable("BUTTON_REPAIR", false);
            //#20180922-01
            SetSelectable("BUTTON_PAINT", false);
            SetSelectable("BUTTON_CABIN", false);
			SetSelectable("SHIPSUP_BUTTON", false);
        }
        else
        {

            if (!GetRemovable(xi_refCharacter) || !GetShipRemovable(xi_refCharacter))
    	    {
    	        SetSelectable("BUTTON_SELL", false);
                //#20180922-01
                SetSelectable("BUTTON_PAINT", false);
	            SetSelectable("BUTTON_CABIN", false);
				SetSelectable("SHIPSUP_BUTTON", false);
    	    }
    	    else {
                //#20180922-01
                SetSelectable("BUTTON_PAINT", true);
				SetSelectable("SHIPSUP_BUTTON", true);
    	    }
    	    if (xi_refCharacter.id == pchar.id && GetCompanionQuantity(pchar) > 1)
    	    { // нельзя продать корабль ГГ, если есть ещё компаньоны
    	        SetSelectable("BUTTON_SELL", false);
    	    }
		}
    }
	if (S_armor(xi_refCharacter)) SetSelectable("SHIPSUP_BUTTON", false); // СФ
}

//////////////// назначение капитана  //////////////////////////////////////////////////////////////////////
void ShipChangeCaptan()
{
	FillPassengerScroll();
    SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"PASSENGERSLIST",-1);
    SetCurrentNode("PASSENGERSLIST");
	ProcessFrame();
	SetOfficersSkills();

	XI_WindowShow("OFFICERS_WINDOW", true);
	XI_WindowDisable("OFFICERS_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
}

void ExitOfficerMenu()
{
	XI_WindowShow("OFFICERS_WINDOW", false);
	XI_WindowDisable("OFFICERS_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("SHIPS_SCROLL");
}

void AcceptAddOfficer()
{
	string  attributeName2 = "pic"+(nCurScrollOfficerNum+1);
    ref     sld;

    if (checkAttribute(GameInterface, "PASSENGERSLIST."+attributeName2 + ".character"))
    {
		int iChar = sti(GameInterface.PASSENGERSLIST.(attributeName2).character);
        // назначение нового кэпа, возможно, если там уже не наш, те или враг или снят
        sld = GetCharacter(iChar);

		if(!CheckAttribute(sld,"HPminusDaysNeedtoRestore"))
		{
			DeleteAttribute(sld, "ship");
			sld.ship.Type = SHIP_NOTUSED;

			// снимем пассажира -->
			CheckForReleaseOfficer(iChar);
			RemovePassenger(pchar, sld);
			// снимем пассажира <--
			SetCompanionIndex(pchar, -1, iChar);

			xi_refCharacter = sld;
			DoBuyShip();
		}
	}
	ExitOfficerMenu();
}

void SetOfficersSkills()
{
	string sCharacter = "pic"+(sti(GameInterface.PASSENGERSLIST.current)+1);
	if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter))
	{
		if (checkAttribute(GameInterface, "PASSENGERSLIST."+sCharacter + ".character"))
		{
			sCharacter = GameInterface.PASSENGERSLIST.(sCharacter).character;
			ref otherchr = &characters[sti(sCharacter)];
	        SetSPECIALMiniTable("TABLE_SMALLSKILL", otherchr);
	        SetOTHERMiniTable("TABLE_SMALLOTHER", otherchr);
	        SetFormatedText("OFFICER_NAME", GetFullName(otherchr));
	        SetSelectable("ACCEPT_ADD_OFFICER", true);
	        return;
        }
	}
    Table_Clear("TABLE_SMALLSKILL", false, true, true);
    Table_Clear("TABLE_SMALLOTHER", false, true, true);
    SetFormatedText("OFFICER_NAME", "");
    SetSelectable("ACCEPT_ADD_OFFICER", false);
}

void FillPassengerScroll()
{
	int i, howWork;
	string faceName;
	string attributeName;
	int _curCharIdx;
	ref _refCurChar;
	bool  ok;

	DeleteAttribute(&GameInterface, "PASSENGERSLIST");

	nCurScrollOfficerNum = -1;
	GameInterface.PASSENGERSLIST.current = 0;
	GameInterface.PASSENGERSLIST.NotUsed = 6;
	GameInterface.PASSENGERSLIST.ImagesGroup.t0 = "EMPTYFACE";
	FillFaceList("PASSENGERSLIST.ImagesGroup", pchar, 2); // passengers
	GameInterface.PASSENGERSLIST.BadTex1 = 0;
	GameInterface.PASSENGERSLIST.BadPic1 = "emptyface";

	int nListSize = GetPassengersQuantity(pchar);
	int m = 0;
	for(i=0; i<nListSize; i++)
	{
		attributeName = "pic" + (m+1);
		_curCharIdx = GetPassenger(pchar,i);

		if (_curCharIdx!=-1)
		{
			ok = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
			bool ob = CheckAttribute(&characters[_curCharIdx], "CompanionDisable") && sti(characters[_curCharIdx].CompanionDisable) == true;
			if (!ok && !ob && GetRemovable(&characters[_curCharIdx]) && !CheckAttribute(&characters[_curCharIdx], "HPminusDaysNeedtoRestore"))
			{
				GameInterface.PASSENGERSLIST.(attributeName).character = _curCharIdx;
				GameInterface.PASSENGERSLIST.(attributeName).img1 = GetFacePicName(GetCharacter(_curCharIdx));
				GameInterface.PASSENGERSLIST.(attributeName).tex1 = FindFaceGroupNum("PASSENGERSLIST.ImagesGroup", "FACE128_"+Characters[_curCharIdx].FaceID);

				//специализация первой
				sTemp = "#" + GetIconFromFont(GetSpeciality(characters[_curCharIdx]));
				//должности	 - не оптимально, но не должно быть проблемой
				if (isOfficer(characters[_curCharIdx])) sTemp += GetIconFromFont("fighter");
				if (sti(pchar.Fellows.Passengers.navigator) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("navigator");
				if (sti(pchar.Fellows.Passengers.boatswain) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("boatswain");
				if (sti(pchar.Fellows.Passengers.cannoner) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("cannoner");
				if (sti(pchar.Fellows.Passengers.doctor) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("doctor");
				if (sti(pchar.Fellows.Passengers.carpenter) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("carpenter");
				if (sti(pchar.Fellows.Passengers.treasurer) == sti(characters[_curCharIdx].index)) sTemp += GetIconFromFont("treasurer");
				GameInterface.PASSENGERSLIST.(attributeName).str1 = sTemp; 

				m++;
			}
		}
	}
	GameInterface.PASSENGERSLIST.ListSize = m + 2; // не знаю зачем, но для совместимости с 'было'
}

// рефреш скилов офа и ГГ, если офа сняли для компаньона
void DelBakSkill()
{
	DelBakSkillAttr(xi_refCharacter);
    ClearCharacterExpRate(xi_refCharacter);
    RefreshCharacterSkillExpRate(xi_refCharacter);
    SetEnergyToCharacter(xi_refCharacter);

    DelBakSkillAttr(pchar);
    ClearCharacterExpRate(pchar);
    RefreshCharacterSkillExpRate(pchar);
    SetEnergyToCharacter(pchar);
}

void ShowMessageInfo()
{
	bool   bBuy;
	int    iMoney;
	string add = "";

	if (bShipyardOnTop) // на стороне верфи (купить)
    {
        bBuy = true;
		if (shipIndex == -1)// проверка на цену
    	{
    	    iMoney = GetShipBuyPrice(sti(refNPCShipyard.Ship.Type), refNPCShipyard);
    	}
    	else
    	{
            iMoney = GetShipBuyPrice(sti(refNPCShipyard.Ship.Type), refNPCShipyard) - GetShipSellPrice(xi_refCharacter, refNPCShipyard);
    	}
    }
    else  // продать
    {
        // LEO: трофей с эпидемией не купят - верфист откажет и запустит штатный мор
        if (AICapturedTrap_ShipyardEpidemy(xi_refCharacter))
        {
            SetFormatedText("MSG_WINDOW_CAPTION", XI_ConvertString("Ship"));
            SetFormatedText("MSG_WINDOW_TEXT", XI_ConvertString("ShipyardEpidemyRefuse"));
            SetFormatedText("MSG_WINDOW_TEXT2", "");
            SetFormatedText("MSG_WINDOW_TEXT3", "");
            SetFormatedText("MSG_WINDOW_TEXT4", "");
            SetSelectable("MSG_OK", true);
            sMessageMode = "ShipEpidemyRefuse";
            ShowMsgMenu(false);
            return;
        }
        bBuy = false;
        iMoney = GetShipSellPrice(xi_refCharacter, refNPCShipyard);
    }
	SetFormatedText("MSG_WINDOW_CAPTION", XI_ConvertString("Ship"));

	if (bBuy)
	{
		if (iMoney < 0) add = "-";
		SetFormatedText("MSG_WINDOW_TEXT", "");
		SetFormatedText("MSG_WINDOW_TEXT2", "");
		if (add == "") SetFormatedText("MSG_WINDOW_TEXT3", GetConvertStrWithReplace("Variable_shipyard_34", "Interface.txt", "#space#", " ") + add + FindRussianMoneyString(abs(iMoney)) + "?");
		else SetFormatedText("MSG_WINDOW_TEXT3", GetConvertStrWithReplace("Variable_shipyard_35", "Interface.txt", "#space#", " ") + add + FindRussianMoneyString(abs(iMoney)) + "?\n" + GetConvertStrWithReplace("Variable_shipyard_35.1", "Interface.txt", "#space#", " "));
		SetFormatedText("MSG_WINDOW_TEXT4", "");
		SetSelectable("MSG_OK", true);
		sMessageMode = "ShipBuy";
	}
	else
	{
	    SetFormatedText("MSG_WINDOW_TEXT", GetConvertStrWithReplace("Variable_shipyard_36", "Interface.txt", "#space#", " ") + add + FindRussianMoneyString(abs(iMoney)) + "?");
		SetFormatedText("MSG_WINDOW_TEXT2", GetConvertStrWithReplace("Variable_shipyard_37", "Interface.txt", "#space#", " "));
		SetFormatedText("MSG_WINDOW_TEXT3", "");
		string sSellHint = XI_ConvertString("SellShipHint"); // LEO: + предупреждение про раненых
		if (GetWoundedQuantity(xi_refCharacter) > 0) {
			if (LanguageGetLanguage() == "russian") sSellHint += "\nРаненые (" + GetWoundedQuantity(xi_refCharacter) + ") отправятся в лазарет.";
			else sSellHint += "\nThe wounded (" + GetWoundedQuantity(xi_refCharacter) + ") will be sent to the infirmary.";
		}
		SetFormatedText("MSG_WINDOW_TEXT4", sSellHint);
		SetSelectable("MSG_OK", true);
		sMessageMode = "ShipSell";
	}
	ShowMsgMenu(bBuy);
}

//#20180922-01
void ShowMessageInfo2()
{
    ref rShip;
	bool bShipOK = false;
	float iMoney = 0.0;
	string sText;
    int nClass, nMinClass = 1;

	if(bShipyardOnTop) return;

    int iShip = sti(xi_refCharacter.ship.type);
	SetFormatedText("MSG_WINDOW_CAPTION", GetConvertStrWithReplace("Variable_shipyard_38", "Interface.txt", "#space#", " "));
    SetSelectable("MSG_OK", false);
    //Check ship and add msg can/can't do based on class, size, hull avail, etc.  If not, disable MSG_OK
    if (shipIndex != -1 && iShip != SHIP_NOTUSED)
    {
        rShip = GetRealShip(iShip);
        nClass = sti(rShip.Class);
        bShipOK = true;
		iMoney = rShip.price;
		iMoney /= 25;
    }
    if(bShipOK)
    {
        if(iMoney > sti(pchar.money)) {
            sText = GetConvertStrWithReplace("Variable_shipyard_39", "Interface.txt", "#space#", " ");
        }
        else {
			sText = GetConvertStrWithReplace("Variable_shipyard_40", "Interface.txt", "#space#", " ") + makeint(iMoney) + GetConvertStrWithReplace("Variable_shipyard_41", "Interface.txt", "#space#", " ");
            SetSelectable("MSG_OK", true);

        }
    }
    else {
        sText = GetConvertStrWithReplace("Variable_shipyard_42", "Interface.txt", "#space#", " ");
    }
    SetFormatedText("MSG_WINDOW_TEXT", sText);
    sMessageMode = "ShipPaint";
	//ShowMsgMenu();
}

void MessageOk()
{
	switch (sMessageMode)
	{
	    case "ShipSell" :
	        DoSellShip(true);
	        ExitMsgMenu();
	    break;

	    case "ShipEpidemyRefuse" : // LEO: верфь отказала из-за эпидемии (штатный мор уже запущен)
	        ExitMsgMenu();
	    break;

	    case "ShipBuy" :
	        DoBuyShipMain();
	    break;
		//#20180922-01
	    case "ShipPaint" :
	        DoPaintShipMain();
	    break;
	}
}

void DoSellShip(bool _refresh)
{
	ref chref = xi_refCharacter;
	if (shipIndex != -1 && sti(chref.ship.type) != SHIP_NOTUSED)
	{
    	int sellPrice = GetShipSellPrice(chref, refNPCShipyard);

    	AddMoneyToCharacter(pchar, sellPrice);
        AddCharacterExpToSkill(pchar, "Commerce", sellPrice / 1600.0);
        WaitDate("",0,0,0, 0, 10);
        Statistic_AddValue(pchar, "SellShip", 1);

		DeleteAttribute(chref,"ship.sails");
		DeleteAttribute(chref,"ship.blots");
		RepairMasts(chref);

		if (_refresh)
		{
			if (GetCannonsNum(chref) > 0)
			{
				SetCannonsToBort(chref, "fcannon", 0);
				SetCannonsToBort(chref, "bcannon", 0);
				SetCannonsToBort(chref, "rcannon", 0);
				SetCannonsToBort(chref, "lcannon", 0);
			}
			aref refGoods;
			ref Colony = GetColonyByIndex(FindColony(refNPCShipyard.City));
			ref trader = &stores[sti(Colony.StoreNum)];
			int moneyback = 0;
			for (int iGoodIndex = 0; iGoodIndex < GOODS_QUANTITY; iGoodIndex++)
			{
				int ShipQ = GetCargoGoods(chref, iGoodIndex);
				if (ShipQ <= 0) continue;
				string goodname = Goods[iGoodIndex].Name;
				makearef(refGoods,trader.Goods.(goodname));
				if (sti(refGoods.TradeType) == TRADE_TYPE_CONTRABAND) 
				{
					if (sti(chref.index) != GetMainCharacterIndex()) 
					{
						AddCharacterGoods(pchar, iGoodIndex, ShipQ);
						RemoveCharacterGoods(chref, iGoodIndex, ShipQ);
					}
					continue;
				}
				int iUnits = Goods[iGoodIndex].Units;
				int iStoreQty = GetStoreGoodsQuantity(trader, iGoodIndex);
				int iStorePrice = GetStoreGoodsPrice(trader, iGoodIndex, PRICE_TYPE_SELL, pchar, 1);
				RemoveCharacterGoods(chref, iGoodIndex, ShipQ);
				SetStoreGoods(trader, iGoodIndex, iStoreQty + ShipQ);
				moneyback += makeint(iStorePrice * ShipQ / iUnits + 0.5);
			}
			if (moneyback > 0) log_info(GetConvertStrWithReplace("Variable_shipyard_43", "Interface.txt", "#space#", " "));
			AddMoneyToCharacter(pchar, moneyback);
			Statistic_AddValue(Pchar, "Money_get", moneyback);
			AddCharacterExpToSkill(pchar, "Commerce", moneyback / (2600.0 - sti(GetCharacterSPECIALSimple(pchar, SPECIAL_I)) * sti(GetCharacterSPECIALSimple(pchar, SPECIAL_C)) * 10));
		}
		
        AddShip2Shipyard(chref);
        // в списке на покупку. <--
    	chref.ship.type = SHIP_NOTUSED;
    	// снять кэпа -->
    	if (_refresh)
		{
	    	if (sti(chref.index) != GetMainCharacterIndex())
	    	{
				if (SellState == "Max") SendCrewToPchar(chref);
				if (SellState == "Opt") SendOptCrewToPchar(chref);
				if (SellState == "Fire") FireCrew(chref);
				Wounded_ClearToInfirmary(chref); // LEO: раненых - в лазарет (+подсказка), любой режим продажи
	            RemoveCharacterCompanion(pchar, chref);
				AddPassenger(pchar, chref, false);
				ReturnAutoOfficers(); // если ex-капитан занимал должности - вернуть его на них
				DelBakSkill();
				//navy --> Фигвам, а не халява :)
				if (CheckAttribute(chref, "PGGAi") && CheckAttribute(chref, "PGGAi.OwnShip")) //за корабли данные ГГ, деньги ГГ.
				{
					chref.Payment = true;
					DeleteAttribute(chref, "PGGAi.OwnShip")
					Log_Info(GetConvertStrWithReplace("Variable_shipyard_44", "Interface.txt", "#space#", " "));
					AddMoneyToCharacter(pchar, -(sellPrice));
				}
				//navy <--
	    	}
	    	else
	    	{
          		pchar.location.from_sea = ""; // нет корабля в порту
	    	}
    		RefreshShipLists();
    	}
	}
}

void SendCrewToPchar(ref chref)
{
	int i, cn;
	ref chreff;
	if (GetCrewQuantity(pchar) < GetMaxCrewQuantity(pchar)) // LEO: раненых chref обнуляет DoSellShip (для всех режимов) - тут не дублируем
	{
		int crewn = GetFreeCrewQuantity(pchar); // LEO: с учётом раненых (занимают места)
		float fTemp =  stf(GetCrewQuantity(pchar) + crewn); // LEO: делитель опыта = новый актив (актив+добор), НЕ Max — иначе при раненых опыт разбавляется и актив льётся сверх лимита
		float fTemp2 =  stf(GetCrewQuantity(pchar) + GetCrewQuantity(chref));

		if (GetCrewQuantity(chref)>crewn)
		{
			pchar.Ship.Crew.Exp.Sailors   = (stf(pchar.Ship.Crew.Exp.Sailors)*GetCrewQuantity(pchar) +
												stf(chref.Ship.Crew.Exp.Sailors)*crewn) / fTemp;
			pchar.Ship.Crew.Exp.Cannoners   = (stf(pchar.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(pchar) +
													stf(chref.Ship.Crew.Exp.Cannoners)*crewn) / fTemp;
			pchar.Ship.Crew.Exp.Soldiers   = (stf(pchar.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.Exp.Soldiers)*crewn) / fTemp;
			pchar.Ship.Crew.morale   = (stf(pchar.Ship.Crew.morale)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.morale)*crewn) / fTemp;

			SetCrewQuantity(pchar,GetCrewQuantity(pchar) + crewn); // LEO: доливаем по свободным слотам (crewn учитывает раненых), не до Max
			Log_Info(GetConvertStrWithReplace("Variable_shipyard_45", "Interface.txt", "#space#", " "));
			crewn = GetCrewQuantity(chref)-crewn;
			if (crewn > 0)
			{
				for(i=1; i<COMPANION_MAX; i++)
				{
					cn = GetCompanionIndex(PChar, i);
					if(cn > 0 && crewn > 0)
					{
						chreff = GetCharacter(cn);
						if (crewn != 0) crewn = SendCrewToShip(chref, chreff, crewn);
					}
				}
				if (crewn > 0) SendCrewToTavern(chref, crewn);

			}
		}
		else
		{
			if (fTemp2 != 0)//фикс - убираем из лога ошибки деления на ноль
			{
			pchar.Ship.Crew.Exp.Sailors   = (stf(pchar.Ship.Crew.Exp.Sailors)*GetCrewQuantity(pchar) +
												stf(chref.Ship.Crew.Exp.Sailors)*GetCrewQuantity(chref)) / fTemp2;
			pchar.Ship.Crew.Exp.Cannoners   = (stf(pchar.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(pchar) +
													stf(chref.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chref)) / fTemp2;
			pchar.Ship.Crew.Exp.Soldiers   = (stf(pchar.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(chref)) / fTemp2;

			pchar.Ship.Crew.morale   = (stf(pchar.Ship.Crew.morale)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.morale)*GetCrewQuantity(chref)) / fTemp2;
			SetCrewQuantity(pchar,GetCrewQuantity(pchar)+GetCrewQuantity(chref));
			Log_Info(GetConvertStrWithReplace("Variable_shipyard_46", "Interface.txt", "#space#", " "));
			}
		}
	}
	else
	{
		int creww = GetCrewQuantity(chref);
		for(i=1; i<COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(PChar, i);
			if(cn > 0 && creww > 0)
			{
				chreff = GetCharacter(cn);
				creww = SendCrewToShip(chref, chreff, creww);
			}
		}
		if (creww > 0) SendCrewToTavern(chref, creww);
	}
}

int SendCrewToShip(ref chref, ref chreff, int crewnum)
{
	int crewn = GetFreeCrewQuantity(chreff); // LEO: с учётом раненых (занимают места)
	if (crewn == 0) return crewnum;
	float fTemp =  stf(GetCrewQuantity(chreff) + crewn); // LEO: делитель опыта = новый актив (не Max)
	float fTemp2 =  stf(GetCrewQuantity(chreff) + crewnum);

	if (crewnum>crewn)
	{
		chreff.Ship.Crew.Exp.Sailors   = (stf(chreff.Ship.Crew.Exp.Sailors)*GetCrewQuantity(chreff) +
											stf(chref.Ship.Crew.Exp.Sailors)*crewn) / fTemp;
		chreff.Ship.Crew.Exp.Cannoners   = (stf(chreff.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chreff) +
												stf(chref.Ship.Crew.Exp.Cannoners)*crewn) / fTemp;
		chreff.Ship.Crew.Exp.Soldiers   = (stf(chreff.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.Exp.Soldiers)*crewn) / fTemp;
		chreff.Ship.Crew.morale   = (stf(chreff.Ship.Crew.morale)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.morale)*crewn) / fTemp;

		SetCrewQuantity(chreff,GetCrewQuantity(chreff) + crewn); // LEO: по свободным слотам (с учётом раненых)
		Log_Info(GetConvertStrWithReplace("Variable_shipyard_47", "Interface.txt", "#space#", " ")+chreff.name+" "+chreff.lastname+".");
		crewn = crewnum-crewn;
	}
	else
	{
		if (fTemp2 != 0)//фикс - убираем из лога ошибки деления на ноль
		{
		chreff.Ship.Crew.Exp.Sailors   = (stf(chreff.Ship.Crew.Exp.Sailors)*GetCrewQuantity(chreff) +
											stf(chref.Ship.Crew.Exp.Sailors)*crewnum) / fTemp2;
		chreff.Ship.Crew.Exp.Cannoners   = (stf(chreff.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chreff) +
												stf(chref.Ship.Crew.Exp.Cannoners)*crewnum) / fTemp2;
		chreff.Ship.Crew.Exp.Soldiers   = (stf(chreff.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.Exp.Soldiers)*crewnum) / fTemp2;

		chreff.Ship.Crew.morale   = (stf(chreff.Ship.Crew.morale)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.morale)*crewnum) / fTemp2;
		SetCrewQuantity(chreff,GetCrewQuantity(chreff)+crewnum);
		Log_Info(GetConvertStrWithReplace("Variable_shipyard_48", "Interface.txt", "#space#", " ")+chreff.name+" "+chreff.lastname+".");
		crewn = 0;
		}
	}
	return crewn;
}

void SendCrewToTavern(ref chref, int crewn)
{
	ref refTown = GetColonyByIndex(FindColony(loadedLocation.fastreload));
	float fTemp3 =  stf(GetCrewQuantity(refTown) + crewn);
	refTown.Ship.Crew.Exp.Sailors   = (stf(refTown.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Exp.Sailors)*crewn) / fTemp3;
	refTown.Ship.Crew.Exp.Cannoners   = (stf(refTown.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Exp.Cannoners)*crewn) / fTemp3;
	refTown.Ship.Crew.Exp.Soldiers   = (stf(refTown.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Exp.Soldiers)*crewn) / fTemp3;
	refTown.Ship.Crew.Morale   = (stf(refTown.Ship.Crew.Morale)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Morale)*crewn) / fTemp3;
	refTown.Ship.Crew.Quantity = sti(refTown.Ship.Crew.Quantity) + crewn;
	Log_Info(GetConvertStrWithReplace("Variable_shipyard_49", "Interface.txt", "#space#", " "))
}

void SendOptCrewToPchar(ref chref)
{
	int i, cn;
	ref chreff;
	if (GetCrewQuantity(pchar) < GetOptCrewQuantity(pchar))
	{
		int crewn = GetOptCrewQuantity(pchar) - GetCrewQuantity(pchar);
		if(crewn > GetFreeCrewQuantity(pchar)) crewn = GetFreeCrewQuantity(pchar); // LEO: не больше свободных слотов (раненые занимают места)
		float fTemp =  stf(GetCrewQuantity(pchar) + crewn); // LEO: делитель опыта = новый актив (не Opt)
		float fTemp2 =  stf(GetCrewQuantity(pchar) + GetCrewQuantity(chref));

		if (GetCrewQuantity(chref)>crewn)
		{
			pchar.Ship.Crew.Exp.Sailors   = (stf(pchar.Ship.Crew.Exp.Sailors)*GetCrewQuantity(pchar) +
												stf(chref.Ship.Crew.Exp.Sailors)*crewn) / fTemp;
			pchar.Ship.Crew.Exp.Cannoners   = (stf(pchar.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(pchar) +
													stf(chref.Ship.Crew.Exp.Cannoners)*crewn) / fTemp;
			pchar.Ship.Crew.Exp.Soldiers   = (stf(pchar.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.Exp.Soldiers)*crewn) / fTemp;
			pchar.Ship.Crew.morale   = (stf(pchar.Ship.Crew.morale)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.morale)*crewn) / fTemp;

			SetCrewQuantity(pchar,GetCrewQuantity(pchar) + crewn); // LEO: по свободным слотам (с учётом раненых)
			Log_Info(GetConvertStrWithReplace("Variable_shipyard_50", "Interface.txt", "#space#", " "));
			crewn = GetCrewQuantity(chref)-crewn;
			if (crewn > 0)
			{
				for(i=1; i<COMPANION_MAX; i++)
				{
					cn = GetCompanionIndex(PChar, i);
					if(cn > 0 && crewn > 0)
					{
						chreff = GetCharacter(cn);
						if (crewn != 0) crewn = SendOptCrewToShip(chref, chreff, crewn);
					}
				}
				if (crewn > 0) SendCrewToTavern(chref, crewn);

			}
		}
		else
		{
			if (fTemp2 != 0)//фикс - убираем из лога ошибки деления на ноль
			{
			pchar.Ship.Crew.Exp.Sailors   = (stf(pchar.Ship.Crew.Exp.Sailors)*GetCrewQuantity(pchar) +
												stf(chref.Ship.Crew.Exp.Sailors)*GetCrewQuantity(chref)) / fTemp2;
			pchar.Ship.Crew.Exp.Cannoners   = (stf(pchar.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(pchar) +
													stf(chref.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chref)) / fTemp2;
			pchar.Ship.Crew.Exp.Soldiers   = (stf(pchar.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(chref)) / fTemp2;

			pchar.Ship.Crew.morale   = (stf(pchar.Ship.Crew.morale)*GetCrewQuantity(pchar) +
														stf(chref.Ship.Crew.morale)*GetCrewQuantity(chref)) / fTemp2;
			SetCrewQuantity(pchar,GetCrewQuantity(pchar)+GetCrewQuantity(chref));
			Log_Info(GetConvertStrWithReplace("Variable_shipyard_51", "Interface.txt", "#space#", " "));
			}
		}
	}
	else
	{
		int creww = GetCrewQuantity(chref);
		for(i=1; i<COMPANION_MAX; i++)
		{
			cn = GetCompanionIndex(PChar, i);
			if(cn > 0 && creww > 0)
			{
				chreff = GetCharacter(cn);
				creww = SendOptCrewToShip(chref, chreff, creww);
			}
		}
		if (creww > 0) SendCrewToTavern(chref, creww);
	}
}

int SendOptCrewToShip(ref chref, ref chreff, int crewnum)
{
	if (GetCrewQuantity(chreff) >= GetOptCrewQuantity(chreff)) return crewnum;
	int crewn = GetOptCrewQuantity(chreff) - GetCrewQuantity(chreff);
	if(crewn > GetFreeCrewQuantity(chreff)) crewn = GetFreeCrewQuantity(chreff); // LEO: не больше свободных слотов (раненые занимают места)
	if (crewn == 0) return crewnum;
	float fTemp =  stf(GetCrewQuantity(chreff) + crewn); // LEO: делитель опыта = новый актив (не Opt)
	float fTemp2 =  stf(GetCrewQuantity(chreff) + crewnum);

	if (crewnum>crewn)
	{
		chreff.Ship.Crew.Exp.Sailors   = (stf(chreff.Ship.Crew.Exp.Sailors)*GetCrewQuantity(chreff) +
											stf(chref.Ship.Crew.Exp.Sailors)*crewn) / fTemp;
		chreff.Ship.Crew.Exp.Cannoners   = (stf(chreff.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chreff) +
												stf(chref.Ship.Crew.Exp.Cannoners)*crewn) / fTemp;
		chreff.Ship.Crew.Exp.Soldiers   = (stf(chreff.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.Exp.Soldiers)*crewn) / fTemp;
		chreff.Ship.Crew.morale   = (stf(chreff.Ship.Crew.morale)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.morale)*crewn) / fTemp;

		SetCrewQuantity(chreff,GetCrewQuantity(chreff) + crewn); // LEO: по свободным слотам (с учётом раненых)
		Log_Info(GetConvertStrWithReplace("Variable_shipyard_52", "Interface.txt", "#space#", " ")+chreff.name+" "+chreff.lastname+".");
		crewn = crewnum-crewn;
	}
	else
	{
		if (fTemp2 != 0)//фикс - убираем из лога ошибки деления на ноль
		{
		chreff.Ship.Crew.Exp.Sailors   = (stf(chreff.Ship.Crew.Exp.Sailors)*GetCrewQuantity(chreff) +
											stf(chref.Ship.Crew.Exp.Sailors)*crewnum) / fTemp2;
		chreff.Ship.Crew.Exp.Cannoners   = (stf(chreff.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(chreff) +
												stf(chref.Ship.Crew.Exp.Cannoners)*crewnum) / fTemp2;
		chreff.Ship.Crew.Exp.Soldiers   = (stf(chreff.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.Exp.Soldiers)*crewnum) / fTemp2;

		chreff.Ship.Crew.morale   = (stf(chreff.Ship.Crew.morale)*GetCrewQuantity(chreff) +
													stf(chref.Ship.Crew.morale)*crewnum) / fTemp2;
		SetCrewQuantity(chreff,GetCrewQuantity(chreff)+crewnum);
		Log_Info(GetConvertStrWithReplace("Variable_shipyard_53", "Interface.txt", "#space#", " ")+chreff.name+" "+chreff.lastname+".");
		crewn = 0;
		}
	}
	return crewn;
}

void FireCrew(ref chref)
{
	int crewn = GetCrewQuantity(chref);
	ref refTown = GetColonyByIndex(FindColony(loadedLocation.fastreload));
	float fTemp3 =  stf(GetCrewQuantity(refTown) + crewn);
	refTown.Ship.Crew.Exp.Sailors   = (stf(refTown.Ship.Crew.Exp.Sailors)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Exp.Sailors)*crewn) / fTemp3;
	refTown.Ship.Crew.Exp.Cannoners   = (stf(refTown.Ship.Crew.Exp.Cannoners)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Exp.Cannoners)*crewn) / fTemp3;
	refTown.Ship.Crew.Exp.Soldiers   = (stf(refTown.Ship.Crew.Exp.Soldiers)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Exp.Soldiers)*crewn) / fTemp3;
	refTown.Ship.Crew.Morale   = (stf(refTown.Ship.Crew.Morale)*GetCrewQuantity(refTown) +
											stf(chref.Ship.Crew.Morale)*crewn) / fTemp3;
	refTown.Ship.Crew.Quantity = sti(refTown.Ship.Crew.Quantity) + crewn;
	Log_Info(GetConvertStrWithReplace("Variable_shipyard_54", "Interface.txt", "#space#", " "))
}

void RefreshShipLists()
{
    xi_refCharacter = pchar;
	FillShipsScroll();
   	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"SHIPS_SCROLL",-1);
    SetCurrentNode("SHIPS_SCROLL");
	ProcessFrame();

	FillShipyardTable();
	bShipyardOnTop = false;
}

void AddShip2Shipyard(ref _chr)
{
    aref    arTo, arFrom;

	aref   arDest, arImt;
	string sAttr;
	int    iNum, i, iShip;
	ref    refBaseShip;
	string sShip;
	string row;

    iShip = sti(_chr.ship.type);
    RealShips[iShip].StoreShip = true;  // кораль на верфи, трется отдельным методом
    RealShips[iShip].Stolen = 0; // уже не ворованный
	makearef(arDest, refNPCShipyard.shipyard);
	iNum = GetAttributesNum(arDest);
	if (iNum == 0)
	{
	    sAttr = "ship1";
	}
	else
	{
	    arImt = GetAttributeN(arDest, iNum - 1);
	    sAttr = GetAttributeName(arImt) + "2";
	}
	makearef(arTo,   refNPCShipyard.ship);
	makearef(arFrom, _chr.Ship);
	CopyAttributes(arTo, arFrom);
	// нулим экипаж и трюм
	SetCrewQuantity(refNPCShipyard, 0);
	DeleteAttribute(refNPCShipyard, "Ship.Cargo");  //пустой трюм
	SetGoodsInitNull(refNPCShipyard);
    RecalculateCargoLoad(refNPCShipyard);

	FillShipyardShipBack(refNPCShipyard, sAttr);
	// бакапим атрибуты груза и матросов
	DeleteAttribute(refNPCShipyard, "BakCargo");
 	DeleteAttribute(refNPCShipyard, "BakCrew");
 	refNPCShipyard.BakCargo = "";
	makearef(arTo,   refNPCShipyard.BakCargo);
	makearef(arFrom, _chr.Ship.Cargo);
	CopyAttributes(arTo, arFrom);

	refNPCShipyard.BakCrew = "";
	makearef(arTo,   refNPCShipyard.BakCrew);
	makearef(arFrom, _chr.Ship.Crew);
	CopyAttributes(arTo, arFrom);
}

void DoBuyShip()
{
    aref    arTo, arFrom;
	// LEO: нельзя выменять новый корабль, сдав в трейд-ин ЗАРАЖЁННЫЙ (старый авто-продаётся через DoSellShip) - отказ
	if (AICapturedTrap_ShipyardEpidemy(xi_refCharacter))
	{
		SetFormatedText("MSG_WINDOW_CAPTION", XI_ConvertString("Ship"));
		SetFormatedText("MSG_WINDOW_TEXT", XI_ConvertString("ShipyardEpidemyRefuse"));
		SetFormatedText("MSG_WINDOW_TEXT2", "");
		SetFormatedText("MSG_WINDOW_TEXT3", "");
		SetFormatedText("MSG_WINDOW_TEXT4", "");
		SetSelectable("MSG_OK", true);
		sMessageMode = "ShipEpidemyRefuse";
		ShowMsgMenu(false);
		return;
	}
	int     iBuyMoney = GetShipBuyPrice(sti(refNPCShipyard.Ship.Type), refNPCShipyard);

 	DeleteAttribute(refNPCShipyard, "BakCargo");
 	DeleteAttribute(refNPCShipyard, "BakCrew");
	int tempcrew = GetCrewQuantity(xi_refCharacter);
	DoSellShip(false);
	AddMoneyToCharacter(pchar, -iBuyMoney);

    AddCharacterExpToSkill(pchar, "Commerce", iBuyMoney / 700.0);
    WaitDate("",0,0,0, 0, 10);
    Statistic_AddValue(pchar, "BuyShip", 1);

    FillShipyardShip(refNPCShipyard, sShipId); // то, что покупаем

    makearef(arTo,   xi_refCharacter.ship);
	makearef(arFrom, refNPCShipyard.Ship);
	CopyAttributes(arTo, arFrom);

	DeleteAttribute(refNPCShipyard, "shipyard." + sShipId);

    int iShip = sti(xi_refCharacter.ship.type);
    DeleteAttribute(&RealShips[iShip], "StoreShip"); // можно тереть

	if (CheckAttribute(refNPCShipyard, "BakCargo")) // есть бакап корабля
	{
        DeleteAttribute(xi_refCharacter, "Ship.Cargo");
		DeleteAttribute(xi_refCharacter, "Ship.Crew");

		xi_refCharacter.Ship.Cargo = "";
		makearef(arTo,   xi_refCharacter.Ship.Cargo);
		makearef(arFrom, refNPCShipyard.BakCargo);
		CopyAttributes(arTo, arFrom);
		
        xi_refCharacter.Ship.Crew = "";
		makearef(arTo,   xi_refCharacter.Ship.Crew);
		makearef(arFrom, refNPCShipyard.BakCrew);
		CopyAttributes(arTo, arFrom);
		if (tempcrew-GetMaxCrewQuantity(xi_refCharacter)>0) SendCrewToTavern(xi_refCharacter,tempcrew-GetMaxCrewQuantity(xi_refCharacter));
		AddCharacterCrew(xi_refCharacter, 0); // обрезать перегруз
	    RecalculateCargoLoad(xi_refCharacter);
	}
	if(!CheckAttribute(xi_refCharacter, "Ship.soiling"))
	{
		xi_refCharacter.Ship.soiling = 0;
	}
	if (sti(xi_refCharacter.index) == GetMainCharacterIndex())
	{
  		pchar.location.from_sea = sFrom_sea; // корабль в порту
	}
	DelBakSkill();
	RefreshShipLists();
}

void DoBuyShipMain()
{
	if (!bEmptySlot) // был коарбль или ГГ без корабля
	{
	    DoBuyShip();
	    ExitMsgMenu();
	}
	else
	{
	    ExitMsgMenu();
	    ShipChangeCaptan();
	}
}

void DoPaintShip()
{
    ref rRealShip;
    int nHulls = 3;
    int nCurrHull = 1;
    int nRandHull;
    int iShip = sti(xi_refCharacter.ship.type);

    timePaint = 0;
	if (iShip != SHIP_NOTUSED)
	{
	    rRealShip = GetRealShip(iShip);
		int iBuyMoney = rRealShip.price;
		iBuyMoney /= 25;
	    if(CheckAttribute(rRealShip, "BaseType")) {
           if(CheckAttribute(&ShipsTypes[sti(rRealShip.BaseType)], "hullNums"))
             nHulls = ShipsTypes[sti(rRealShip.BaseType)].hullNums;
        }
        if(nHulls < 2)
            return;
	    timePaint = timePaint + (8-GetCharacterShipClass(xi_refCharacter));
	    AddMoneyToCharacter(pchar, makeint(-iBuyMoney));
	    AddCharacterExpToSkill(pchar, "Commerce", iBuyMoney / 700.0);
        if(CheckAttribute(rRealShip, "ship.upgrades.hull"))
            nCurrHull = sti(rRealShip.ship.upgrades.hull);
        nRandHull = nCurrHull;
        while(nCurrHull == nRandHull) {
            nRandHull = rand(nHulls-1) + 1;
        }
        rRealShip.ship.upgrades.hull = nRandHull;
	}
}

void DoPaintShipMain()
{
	if (shipIndex != -1 && sti(xi_refCharacter.ship.type) != SHIP_NOTUSED)
	{
		DoPaintShip();
	}
	ExitMsgMenu();
}

int ScrollGetTexture()
{
	string sName = GetEventData();
	int iPicIndex = GetEventData();
	string sScrollID = GetEventData();

	//Boyer change #20170301-2 to new function as NFGetFile (NF) functions no longer exist in our source
	int iTexture = GetTexture(sName); //NFGetTexture(sScrollID, sName);

	return iTexture;
}

//////////////////// ремонт ///////////////
void ExitRepairMenu()
{
	bSquadronRepair = false;
	XI_WindowShow("REPAIR_WINDOW", false);
	XI_WindowDisable("REPAIR_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);

	SetCurrentNode("SHIPS_SCROLL");
	sMessageMode = "";
}

void ShowRepairMenu()
{
	bSquadronRepair = false;
	XI_WindowShow("REPAIR_WINDOW", true);
	XI_WindowDisable("REPAIR_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

    SetRepairData();
	SetCurrentNode("REPAIR_CANCEL");
}

// LEO: «Общий ремонт» — то же окно REPAIR_WINDOW, но чинит ВСЮ эскадру. Стрелки корпуса/парусов задают целевой %,
// LEO: галочки очистка/орудия — на все корабли, стоимость суммарная. Одиночный путь не затронут (гейт bSquadronRepair).
void ShowSquadronRepairMenu()
{
	bSquadronRepair = true;
	XI_WindowShow("REPAIR_WINDOW", true);
	XI_WindowDisable("REPAIR_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

    SetRepairData();
	SetCurrentNode("REPAIR_CANCEL");
}

// LEO: === «Общий ремонт эскадры» (bSquadronRepair). RepairHull/RepairSail трактуются как ЦЕЛЕВОЙ % (0..100). ===
// LEO: Переиспользуем одиночные хелперы (soiling/cannons читают глобал xi_refCharacter) через временную подмену.
void RepairMoneyShowSquadron()
{
	ref mc = GetMainCharacter();
	ref chr;
	int cn;
	int hullCost = 0;
	int sailCost = 0;
	int soilCost = 0;
	int canCost = 0;
	bool bSoilOn = (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_Soiling_CHECKBOX", 3, 1) > 0);
	bool bCanOn  = (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_CANNONS_CHECKBOX", 3, 1) > 0);
	ref saveRef = xi_refCharacter;
	float fTimeM = 0.0;
	int cc = 0; // LEO: объявление В ШАПКЕ - Storm не принимает его внутри вложенного блока if (COMPILE ERROR)

	for (int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mc, i);
		if (cn <= 0) continue;
		chr = GetCharacter(cn);
		if (sti(chr.ship.type) == SHIP_NOTUSED) continue;

		xi_refCharacter = chr;
		int st = GetCharacterShipType(chr);
		int curHull = MakeInt(GetHullPercent(chr));
		int curSail = MakeInt(GetSailPercent(chr));
		int needHull = RepairHull - curHull;
		int needSail = RepairSail - curSail;
		if (needHull < 0) needHull = 0;
		if (needSail < 0) needSail = 0;

		if (needHull > 0)
		{
			hullCost += GetHullRepairCost(st, needHull, refNPCShipyard);
			fTimeM = retMax(fTimeM, (needHull * (8-GetCharacterShipClass(chr)))/4.0);
		}
		if (needSail > 0)
		{
			sailCost += GetSailRepairCost(st, needSail, refNPCShipyard);
			fTimeM = retMax(fTimeM, (needSail * (8-GetCharacterShipClass(chr)))/6.0);
		}
		if (bSoilOn && sti(chr.ship.soiling) > 0)
		{
			soilCost += GetChrClearSoilingCoast();
			fTimeM = retMax(fTimeM, (8-GetCharacterShipClass(chr)));
		}
		if (bCanOn && GetCannonQuantity(chr) > 0)
		{
			cc = GetCannonsRepairCost();
			if (cc > 0) { canCost += cc; fTimeM = retMax(fTimeM, (10-GetCharacterShipClass(chr))); }
		}
	}
	xi_refCharacter = saveRef;

	int total = hullCost + sailCost + soilCost + canCost;
	fTimeM *= 60;

	string _sTime;
	if (fTimeM < 1) _sTime = "\n";
	else _sTime = GetConvertStrWithReplace("Variable_shipyard_55", "Interface.txt", "#space#", " ") + makeint(fTimeM)/60 + GetConvertStrWithReplace("Variable_shipyard_55.1", "Interface.txt", "#space#", " ") + its(makeint(fTimeM)%60) + "\n";
	SetFormatedText("REPAIR_WINDOW_TEXT", _sTime + its(total));

	if (total > 0 && sti(pchar.Money) >= total) SetSelectable("REPAIR_OK", true);
	else SetSelectable("REPAIR_OK", false);
}

void RepairStatShowSquadron()
{
	SetFormatedText("REPAIR_QTY_H", RepairHull + "%");
	SetFormatedText("REPAIR_QTY_S", RepairSail + "%");

	ref mc = GetMainCharacter();
	ref chr;
	int cn;
	int totSoil = 0;
	int totCan = 0;
	int cc = 0; // LEO: объявление В ШАПКЕ - Storm не принимает его внутри вложенного блока if
	ref saveRef = xi_refCharacter;

	for (int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mc, i);
		if (cn <= 0) continue;
		chr = GetCharacter(cn);
		if (sti(chr.ship.type) == SHIP_NOTUSED) continue;

		xi_refCharacter = chr;
		if (sti(chr.ship.soiling) > 0) totSoil += GetChrClearSoilingCoast();
		if (GetCannonQuantity(chr) > 0)
		{
			cc = GetCannonsRepairCost();
			if (cc > 0) totCan += cc;
		}
	}
	xi_refCharacter = saveRef;

	if (totCan > 0)
	{
		SetNodeUsing("REPAIR_CANNONS_CHECKBOX", true);
		SetNodeUsing("REPAIR_CANNONS_TEXT", true);
		SetFormatedText("REPAIR_CANNONS_TEXT", FindRussianMoneyString(totCan));
	}
	else
	{
		SetNodeUsing("REPAIR_CANNONS_CHECKBOX", false);
		SetNodeUsing("REPAIR_CANNONS_TEXT", false);
	}

	if (totSoil > 0)
	{
		SetNodeUsing("REPAIR_Soiling_CHECKBOX", true);
		SetNodeUsing("REPAIR_Soiling_TEXT", true);
		SetFormatedText("REPAIR_Soiling_TEXT", FindRussianMoneyString(totSoil));
	}
	else
	{
		SetNodeUsing("REPAIR_Soiling_CHECKBOX", false);
		SetNodeUsing("REPAIR_Soiling_TEXT", false);
	}

	RepairMoneyShowSquadron();
}

void RepairOkSquadron()
{
	ref mc = GetMainCharacter();
	ref chr;
	int cn;
	int totalCost = 0;
	int cc = 0; // LEO: объявление В ШАПКЕ - Storm не принимает его внутри вложенного блока if
	bool bSoilOn = (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_Soiling_CHECKBOX", 3, 1) > 0);
	bool bCanOn  = (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_CANNONS_CHECKBOX", 3, 1) > 0);
	ref saveRef = xi_refCharacter;

	for (int i = 0; i < COMPANION_MAX; i++)
	{
		cn = GetCompanionIndex(mc, i);
		if (cn <= 0) continue;
		chr = GetCharacter(cn);
		if (sti(chr.ship.type) == SHIP_NOTUSED) continue;

		xi_refCharacter = chr;
		int st = GetCharacterShipType(chr);
		int curHull = MakeInt(GetHullPercent(chr));
		int curSail = MakeInt(GetSailPercent(chr));
		int needHull = RepairHull - curHull;
		int needSail = RepairSail - curSail;
		if (needHull < 0) needHull = 0;
		if (needSail < 0) needSail = 0;

		if (needHull > 0)
		{
			totalCost += GetHullRepairCost(st, needHull, refNPCShipyard);
			timeHull = timeHull + needHull * (8-GetCharacterShipClass(chr));
			ProcessHullRepair(chr, stf(needHull));
			if ((curHull + needHull) >= 100)
			{
				chr.ship.hp = GetCharacterShipHP(chr);
				DeleteAttribute(chr, "ship.blots");
			}
		}
		if (needSail > 0)
		{
			totalCost += GetSailRepairCost(st, needSail, refNPCShipyard);
			timeRig = timeRig + needSail * (8-GetCharacterShipClass(chr));
			ProcessSailRepair(chr, stf(needSail));
			if ((curSail + needSail) >= 100)
			{
				chr.ship.sp = GetCharacterShipSP(chr);
				DeleteAttribute(chr, "ship.sails");
				RepairMasts(chr);
			}
		}
		if (bSoilOn && sti(chr.ship.soiling) > 0)
		{
			totalCost += GetChrClearSoilingCoast();
			timeSoil = timeSoil + (8-GetCharacterShipClass(chr));
			chr.ship.soiling = 0;
		}
		if (bCanOn && GetCannonQuantity(chr) > 0)
		{
			cc = GetCannonsRepairCost(); // LEO: выставляет глобал CannonsQty для ReSetCannons (xi_refCharacter=chr)
			if (cc > 0)
			{
				totalCost += cc;
				timeCan = timeCan + (10-GetCharacterShipClass(chr));
				ReSetCannons(chr);
			}
		}
	}
	xi_refCharacter = saveRef;

	AddMoneyToCharacter(pchar, -totalCost);

	ExitRepairMenu();
	int scr = sti(GameInterface.SHIPS_SCROLL.current);
	FillShipsScroll();
	GameInterface.SHIPS_SCROLL.current = scr;
	nCurScrollNum = scr;
   	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"SHIPS_SCROLL",-1);
	OnShipScrollChange();
	SetButtionsAccess();
}

void SetRepairData()
{
	if (bSquadronRepair)
	{
		RepairHull = 100; // LEO: цель по умолчанию — полный ремонт всей эскадры (стрелками можно снизить)
		RepairSail = 100;
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_CANNONS_CHECKBOX", 2, 1, 0 ); // старт: галочки сняты
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_Soiling_CHECKBOX", 2, 1, 0 );
	}
	else
	{
		RepairSail = 0;
		RepairHull = 0;
	}
	CannonsQty = 0; // LEO: сброс глобала докупки орудий между кораблями скролла (иначе stale → фантом-пушки в трюме)
 	RepairStatShow();
}

void RepairMoneyShow()
{
	if (bSquadronRepair) { RepairMoneyShowSquadron(); return; }
    int st = GetCharacterShipType(xi_refCharacter);
	int soiltime = 0;
	int cantime = 0;
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_Soiling_CHECKBOX", 3, 1)) soiltime = (8-GetCharacterShipClass(xi_refCharacter));
	else soiltime = 0;
	if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_CANNONS_CHECKBOX", 3, 1)) cantime = (10-GetCharacterShipClass(xi_refCharacter));
	else cantime = 0;
	float fTimeM = soiltime;
	fTimeM = retMax(fTimeM, cantime);
	fTimeM = retMax(fTimeM, (RepairSail * (8-GetCharacterShipClass(xi_refCharacter)))/6.0);
	fTimeM = retMax(fTimeM, (RepairHull * (8-GetCharacterShipClass(xi_refCharacter)))/4.0);
	fTimeM *= 60;

	string _sTime;
	if (fTimeM<1) _sTime = "\n"; else _sTime = GetConvertStrWithReplace("Variable_shipyard_55", "Interface.txt", "#space#", " ") + makeint(fTimeM)/60 + GetConvertStrWithReplace("Variable_shipyard_55.1", "Interface.txt", "#space#", " ") + its(makeint(fTimeM)%60) + "\n";
	int repwt = its(iSoilClearCost + GetSailRepairCost(st, RepairSail, refNPCShipyard) + GetHullRepairCost(st, RepairHull, refNPCShipyard) + iCannonsRepairCost);
	SetFormatedText("REPAIR_WINDOW_TEXT", _sTime + its(iSoilClearCost + GetSailRepairCost(st, RepairSail, refNPCShipyard) + GetHullRepairCost(st, RepairHull, refNPCShipyard)+iCannonsRepairCost));
	if (repwt > 0) SetSelectable("REPAIR_OK", true);
	else SetSelectable("REPAIR_OK", false);
}

void RepairStatShow()
{
	if (bSquadronRepair) { RepairStatShowSquadron(); return; }
    int hp = MakeInt(GetHullPercent(xi_refCharacter));
	int sp = MakeInt(GetSailPercent(xi_refCharacter));

	SetFormatedText("REPAIR_QTY_H", (hp+RepairHull) + "%");
	SetFormatedText("REPAIR_QTY_S", (sp+RepairSail) + "%");
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_CANNONS_CHECKBOX", 2, 1, 0 );
	if (GetCannonQuantity(xi_refCharacter) > 0)
	{
		SetNodeUsing("REPAIR_CANNONS_CHECKBOX",true);
		SetNodeUsing("REPAIR_CANNONS_TEXT",true);
		SetFormatedText("REPAIR_CANNONS_TEXT", FindRussianMoneyString(GetCannonsRepairCost()));
	}
	else
	{
		SetNodeUsing("REPAIR_CANNONS_CHECKBOX",false);
		SetNodeUsing("REPAIR_CANNONS_TEXT",false);
	}
	if(sti(xi_refCharacter.ship.soiling) == 0)
	{
		//SetFormatedText("REPAIR_Soiling_TEXT" , "не требуется");
		SetNodeUsing("REPAIR_Soiling_CHECKBOX",false);
		SetNodeUsing("REPAIR_Soiling_TEXT",false);
	}else{
		SetFormatedText("REPAIR_Soiling_TEXT" , FindRussianMoneyString(GetChrClearSoilingCoast()));
	}

	RepairMoneyShow();
}

int GetCannonsRepairCost()
{
	if (GetCannonQuantity(xi_refCharacter) <= 0) return 0;
	int classcost = (8-GetCharacterShipClass(xi_refCharacter))*250;
	if (GetCannonsNum(xi_refCharacter) == GetCannonQuantity(xi_refCharacter)) { CannonsQty = 0; return classcost; } // LEO: все орудия целы и борта полны — докупать нечего, обнуляем дельту (иначе stale → фантом-пушки в трюме при ReSetCannons)
	int needQ = GetCannonQuantity(xi_refCharacter);
	int idx = GetCannonGoodsIdxByType(sti(xi_refCharacter.Ship.Cannons.Type));
	int qtyC = GetCargoGoods(xi_refCharacter, idx);
	CannonsQty = qtyC + GetBortCannonsQty(xi_refCharacter, "fcannon")+ GetBortCannonsQty(xi_refCharacter, "bcannon")+ GetBortCannonsQty(xi_refCharacter, "rcannon")+ GetBortCannonsQty(xi_refCharacter, "lcannon");
	if (CannonsQty >= needQ)
	{
		CannonsQty = needQ - CannonsQty;
		return classcost;
	}
	CannonsQty = needQ - CannonsQty;
	return CannonsQty * GetCannonPrice(idx);
}

int GetCannonPrice(int idx)
{
	int basePrice = MakeInt(Goods[idx].Cost);
	// Gregg: формула была локальной копией — теперь единый хелпер из StoreUtilite.c
	float skillModify = GetTradeSkillModify(pchar, pchar, PRICE_TYPE_BUY) * TRADE_MOD_CANNONS_BUY;
	return MakeInt(basePrice*skillModify + 0.5);
}

void ClickRepairArror(string _type, int add)
{
	if (bSquadronRepair)
	{
		// LEO: в эскадренном режиме проценты — это ЦЕЛЕВОЙ уровень (0..100) для всех кораблей, без гейта по деньгам (стоимость покажется, «Ок» заблокируется если не хватает)
		if (_type == "hull") { RepairHull = RepairHull + add; if (RepairHull < 0) RepairHull = 0; if (RepairHull > 100) RepairHull = 100; }
		else                 { RepairSail = RepairSail + add; if (RepairSail < 0) RepairSail = 0; if (RepairSail > 100) RepairSail = 100; }
		RepairStatShow();
		return;
	}
    int st = GetCharacterShipType(xi_refCharacter);
    int i;
    int hp = MakeInt(GetHullPercent(xi_refCharacter));
	int sp = MakeInt(GetSailPercent(xi_refCharacter));

	if (_type == "hull")
	{
		if (add > 0)
		{
			if (sti(pchar.Money) >= (GetSailRepairCost(st, RepairSail, refNPCShipyard) + GetHullRepairCost(st, RepairHull + add, refNPCShipyard)))
		    {
		        RepairHull = RepairHull + add;
		    }
		    else
		    {
		        i = sti(pchar.Money) - (GetSailRepairCost(st, RepairSail, refNPCShipyard) + GetHullRepairCost(st, RepairHull, refNPCShipyard));
		        i = i / GetHullRepairCost(st, 1, refNPCShipyard); // на сколько хватит
		        RepairHull = RepairHull + i;
		    }
		    if ((RepairHull + hp) > 100)  RepairHull = 100 - hp;
	    }
	    else
	    {
            RepairHull = RepairHull + add;
			if (RepairHull < 0)  RepairHull = 0;
	    }
	}
	else
	{
	    if (add > 0)
		{
			if (sti(pchar.Money) >= (GetSailRepairCost(st, RepairSail+ add, refNPCShipyard) + GetHullRepairCost(st, RepairHull, refNPCShipyard)))
		    {
		        RepairSail = RepairSail + add;
		    }
		    else
		    {
		        i = sti(pchar.Money) - (GetSailRepairCost(st, RepairSail, refNPCShipyard) + GetHullRepairCost(st, RepairHull, refNPCShipyard));
		        i = i / GetSailRepairCost(st, 1, refNPCShipyard); // на сколько хватит
		        RepairSail = RepairSail + i;
		    }
		    if ((RepairSail+sp) > 100)  RepairSail = 100 - sp;
	    }
	    else
	    {
            RepairSail = RepairSail + add;
			if (RepairSail < 0)  RepairSail = 0;
	    }
	}
	RepairStatShow();
}

void RepairOk()
{
	if (bSquadronRepair) { RepairOkSquadron(); return; }
    int st = GetCharacterShipType(xi_refCharacter);
    int hp = MakeInt(GetHullPercent(xi_refCharacter));
	int sp = MakeInt(GetSailPercent(xi_refCharacter));
	float ret;

	if ((GetChrClearSoilingCoast() > 0) && (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_Soiling_CHECKBOX", 3, 1) > 0) && (xi_refCharacter.ship.soiling > 0))
	{
		xi_refCharacter.ship.soiling = 0;
		timeSoil = timeSoil + (8-GetCharacterShipClass(xi_refCharacter));
		// LEO: опыт за ремонт на верфи убран — опыт даётся только за быструю починку (repair.c), см. правило плотник/ГГ
		AddMoneyToCharacter(pchar, -GetChrClearSoilingCoast());
		iSoilClearCost = 0;
	}

	if (RepairHull > 0)
	{
		timeHull = timeHull + RepairHull * (8-GetCharacterShipClass(xi_refCharacter));
		AddMoneyToCharacter(pchar, -GetHullRepairCost(st, RepairHull, refNPCShipyard));

		ret = ProcessHullRepair(xi_refCharacter, stf(RepairHull));
	}
	if (RepairSail > 0)
	{
	  	timeRig = timeRig + RepairSail * (8-GetCharacterShipClass(xi_refCharacter));
		AddMoneyToCharacter(pchar,-GetSailRepairCost(st, RepairSail, refNPCShipyard));

		ret = ProcessSailRepair(xi_refCharacter, stf(RepairSail));
	}
	if ((hp + RepairHull) >= 100)
	{
        xi_refCharacter.ship.hp = GetCharacterShipHP(xi_refCharacter);
		DeleteAttribute(xi_refCharacter, "ship.blots");
	}
	if ((sp+RepairSail) >= 100)
	{
		xi_refCharacter.ship.sp = GetCharacterShipSP(xi_refCharacter);
		DeleteAttribute(xi_refCharacter, "ship.sails");
		RepairMasts(xi_refCharacter);
	}
	if ((GetCannonsRepairCost() > 0) && (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_CANNONS_CHECKBOX", 3, 1) > 0)) 
	{
		timeCan = timeCan + (10-GetCharacterShipClass(xi_refCharacter));
		AddMoneyToCharacter(pchar, -GetCannonsRepairCost());
		ReSetCannons(xi_refCharacter);
		iCannonsRepairCost = 0;
	}
	///
	ExitRepairMenu();
	st = sti(GameInterface.SHIPS_SCROLL.current);
	FillShipsScroll();
	GameInterface.SHIPS_SCROLL.current = st;
	nCurScrollNum = st;
   	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"SHIPS_SCROLL",-1);
	OnShipScrollChange();
	SetButtionsAccess();
}

void ReSetCannons(ref chr)
{
    if (GetCannonQuantity(chr) <= 0) return;
    //if (GetCannonQuantity(chr) <= GetCannonsNum(chr)) return;
	// сначала все убрать
	int CannonsType = sti(chr.Ship.Cannons.Type);
    if (GetCannonsNum(chr) > 0)
    {
		SetCannonsToBort(chr, "fcannon", 0);
	    SetCannonsToBort(chr, "bcannon", 0);
	    SetCannonsToBort(chr, "rcannon", 0);
	    SetCannonsToBort(chr, "lcannon", 0);
    }
    // потом все выставить раскидав по бортам
    int idx = GetCannonGoodsIdxByType(CannonsType);
    int fb, lb, rb, bb;
    int qty;

    if (idx != -1)
    {
    	qty = GetCargoGoods(chr, idx);
		string good = Goods[idx].name;
		if (CannonsQty < 0) CannonsQty = 0; // LEO: при избытке запасных пушек дельта отрицательна — не урезаем трюм (иначе молча уничтожаются запасные орудия игрока)
		chr.Ship.Cargo.Goods.(good) = qty+CannonsQty;
		qty = makeint(chr.Ship.Cargo.Goods.(good));

    	rb = GetBortCannonsMaxQty(chr, "rcannon");

    	if (rb  > (qty / 2)) rb = qty / 2;
    	qty = qty - rb;
    	if (qty < 0) qty = 0;

    	lb = GetBortCannonsMaxQty(chr, "lcannon");
    	if (lb > qty) lb = qty;
    	qty = qty - lb;
    	if (qty < 0) qty = 0;

    	bb = GetBortCannonsMaxQty(chr, "bcannon");
    	if (bb > qty) bb = qty;
    	qty = qty - bb;
    	if (qty < 0) qty = 0;

    	fb = GetBortCannonsMaxQty(chr, "fcannon");
    	if (fb > qty) fb = qty;

    	SetCannonsToBort(chr, "fcannon", fb);
	    SetCannonsToBort(chr, "bcannon", bb);
	    SetCannonsToBort(chr, "rcannon", rb);
	    SetCannonsToBort(chr, "lcannon", lb);
    }
}

/// установить орудия по борту (сперва расчитать дельту было стало - лишнее в запасы)
void SetCannonsToBort(ref chr, string sBort, int iQty)
{
	int     curQty = GetBortCannonsQty(chr, sBort);
	int     maxQty = GetBortCannonsMaxQty(chr, sBort);
	int     i, delta;
	string  attr;
	int     center, left, right; // счетчики орудий для распределения
	bool    bLeft; // направление хода
	string  sBort_real;

	if(sBort == "rcannon") sBort_real = "cannonr";
	if(sBort == "lcannon") sBort_real = "cannonl";
	if(sBort == "fcannon") sBort_real = "cannonf";
	if(sBort == "bcannon") sBort_real = "cannonb";

	if (iQty > maxQty) iQty = maxQty;
	if (iQty < 0) iQty = 0;

	int idx = GetCannonGoodsIdxByType(sti(chr.Ship.Cannons.Type));
    delta = iQty - curQty;
    if (delta > 0)
    {
    	if (GetCargoGoods(chr, idx) < delta) iQty = curQty + GetCargoGoods(chr, idx);
    }
	if (iQty > curQty)
	{ // списать со склада
		RemoveCharacterGoodsSelf(chr, idx, (iQty - curQty));
	}
	else
	{
		if (iQty < curQty)
		{// лишние на склад
			SetCharacterGoods(chr, idx, GetCargoGoods(chr, idx) + (curQty - iQty)); // этот метод, тк перегруз может быть, а  AddCharacterGoodsSimple режет перегруз
		}
	}
	// нулим колво пушек на борту и распределяем заново от центра (как они на модели по номерам не знаю, допуск, что подряд)
	for (i = 0; i < maxQty; i++)
	{
		attr = "c" + i;
		chr.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0; // поломана на 100 процентов, не палит, те нет её
		chr.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0; // поломана на 100 процентов, не палит, те нет её
	}
	// распределяем
	if (iQty > 0)
	{
		center = makeint(maxQty / 2); // целочисленное деление
		left   = center - 1;
		right  = center;
		i = 0; // сколько распределили уже
		bLeft = true;
		while (i < iQty)
		{
			if (bLeft)
			{
				if (left >= 0)
				{
					attr = "c" + left;
					left--;
				}
				else
				{
					attr = "c" + right;
					right++;
				}
				if (right < maxQty) bLeft = false;
			}
			else
			{
				if (right < maxQty)
				{
					attr = "c" + right;
					right++;
				}
				else
				{
					attr = "c" + left;
					left--;
				}
				if (left >= 0) bLeft = true;
			}
			chr.Ship.Cannons.borts.(sBort).damages.(attr) = 0.0; // новая, не битая
			chr.Ship.Cannons.borts.(sBort_real).damages.(attr) = 0.0; // новая, не битая
			i++;
		}
	}
	RecalculateCargoLoad(chr);  // пересчет, тк пушки снялись
}

void RepairAll()
{
	if (bSquadronRepair)
	{
		RepairHull = 100;
		RepairSail = 100;
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_Soiling_CHECKBOX", 2, 1, 1);
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_CANNONS_CHECKBOX", 2, 1, 1);
		RepairStatShow();
		return;
	}
    ClickRepairArror("sail", 100);
    ClickRepairArror("hull", 100);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_Soiling_CHECKBOX", 2, 1, 1);
	SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_CANNONS_CHECKBOX", 2, 1, 1);
	ClickREPAIR_Soiling();
	ClickREPAIR_CANNONS();
}

void BuyShipEvent()
{
	if (GetSelectable("BUTTON_BUY"))
	{
	    ShowMessageInfo();
	}
}

void ClickREPAIR_CANNONS()
{
	if (bSquadronRepair) { RepairStatShow(); return; }
	if(sti(pchar.money) >= GetCannonsRepairCost())
	{
		if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_CANNONS_CHECKBOX", 3, 1))
		{
			iCannonsRepairCost = GetCannonsRepairCost();
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_CANNONS_TEXT", 8,0,argb(255,255,255,200));
		}else{
			iCannonsRepairCost = 0;
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_CANNONS_TEXT", 8,0,argb(255,196,196,196));
		}
	}else{
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_CANNONS_CHECKBOX", 2, 1, 0);
		iCannonsRepairCost = 0;
	}

	RepairMoneyShow();

//	iFullPrice = iHullRepairCost + iSailsRepairCost + iCannonsRepairCost + iSoilClearCost;
//	UpdateRepairWindow();
}

void ClickREPAIR_Soiling()
{
	if (bSquadronRepair) { RepairStatShow(); return; }
	if((sti(xi_refCharacter.ship.soiling) != 0)&&(sti(pchar.money) >= GetChrClearSoilingCoast()))
	{
		if(SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "REPAIR_Soiling_CHECKBOX", 3, 1))
		{
			iSoilClearCost = GetChrClearSoilingCoast();
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_Soiling_TEXT", 8,0,argb(255,255,255,200));
		}else{
			iSoilClearCost = 0;
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_Soiling_TEXT", 8,0,argb(255,196,196,196));
		}
	}else{
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE,"REPAIR_Soiling_CHECKBOX", 2, 1, 0);
		iSoilClearCost = 0;
	}

	RepairMoneyShow();

//	iFullPrice = iHullRepairCost + iSailsRepairCost + iCannonsRepairCost + iSoilClearCost;
//	UpdateRepairWindow();
}

int GetChrClearSoilingCoast()
{
	int Soiling = xi_refCharacter.ship.soiling;
	int iClass = sti(RealShips[sti(xi_refCharacter.ship.type)].Class);
	int sum = 50;

	switch(iClass)
	{
		case 7:
			sum= 100;
			if(Soiling > 25){sum= sum + ((Soiling-25) * 5)}
		break;
		case 6:
			sum= 350;
			if(Soiling > 25){sum= sum + ((Soiling-25) * 10)}
		break;
		case 5:
			sum= 600;
			if(Soiling > 25){sum= sum + ((Soiling-25) * 12)}
		break;
		case 4:
			sum= 1000;
			if(Soiling > 25){sum= sum + ((Soiling-25) * 15)}
		break;
		case 3:
			sum= 1500;
			if(Soiling > 25){sum= sum + ((Soiling-25) * 25)}
		break;
		case 2:
			sum= 1700;
			if(Soiling > 25){sum= sum + ((Soiling-25) * 30)}
		break;
		case 1:
			sum= 2500;
			if(Soiling > 25){sum= sum + ((Soiling-25) * 35)}
		break;
	}

	return sum;
}

void CloseShipUp()
{
	XI_WindowShow("MAIN_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);
	XI_WindowShow("SHIPSUP_WINDOW", false);
	XI_WindowDisable("SHIPSUP_WINDOW", true);
	SetNodeUsing("MAIN_WINDOW", true);
	SetNodeUsing("SHIPSUP_WINDOW", false);
	SetShipOTHERTable("TABLE_OTHER",xi_refCharacter);

	string attributeName = "pic" + (nCurScrollNum+1);
	GameInterface.SHIPS_SCROLL.(attributeName).str3 = "#" + MakeMoneyShow(GetShipSellPrice(&characters[sti(GameInterface.SHIPS_SCROLL.(attributeName).character)], refNPCShipyard), MONEY_SIGN,MONEY_DELIVER);
   	SendMessage(&GameInterface,"lsl",MSG_INTERFACE_SCROLL_CHANGE,"SHIPS_SCROLL",-1);
}

/* void OpenShipUp()
{
	int upsq = 0;
	XI_WindowShow("SHIPSUP_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", true);
	XI_WindowDisable("SHIPSUP_WINDOW", false);
	SetNodeUsing("MAIN_WINDOW", false);
	SetNodeUsing("SHIPSUP_WINDOW", true);
	upgradevalue = 0;
	int iShip = sti(xi_refCharacter.Ship.Type);
	for (int i = 0; i < 6; i++)
	{
        string row = "tr" + (i+1);
        GameInterface.SHIPSUP_TABLE.(row).td1.icon.group = "SHIP_UPGRADES";
		GameInterface.SHIPSUP_TABLE.(row).td1.icon.width = 128;
    	GameInterface.SHIPSUP_TABLE.(row).td1.icon.height = 128;
    	GameInterface.SHIPSUP_TABLE.(row).td1.icon.offset = "-3, 0";
    	GameInterface.SHIPSUP_TABLE.(row).td1.textoffset = "65,0";
		GameInterface.SHIPSUP_TABLE.(row).td1.str = "";
		GameInterface.SHIPSUP_TABLE.(row).td1.align = "left";
		GameInterface.SHIPSUP_TABLE.(row).td1.scale = 0.8;
		GameInterface.SHIPSUP_TABLE.(row).td2.scale = 1.0;
		GameInterface.SHIPSUP_TABLE.(row).td3.scale = 1.0;
		switch (i)
		{
			case 0:
				GameInterface.SHIPSUP_TABLE.(row).td2.str = GetConvertStr("SUP_HULL_descr","ShipsDescribe.txt");
				if (CheckAttribute(RealShips[iShip],"Tuning.HullSpecial"))
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRAHULLON";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_56", "Interface.txt", "#space#", " ");
					upgradevalue = 0;
					upsq++;
				}
				else
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRAHULLOFF";
					if (RealShips[iShip].Type.War == true && RealShips[iShip].Type.Merchant == false) GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_57", "Interface.txt", "#space#", " ");
						else GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_58", "Interface.txt", "#space#", " ");
				}
				if (RealShips[iShip].Type.War == false && RealShips[iShip].Type.Merchant == true) GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_59", "Interface.txt", "#space#", " ");
			break;
			case 1:
				GameInterface.SHIPSUP_TABLE.(row).td2.str = GetConvertStr("SUP_SAIL_descr","ShipsDescribe.txt");
				if (CheckAttribute(RealShips[iShip],"Tuning.SailsSpecial"))
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRASAILON";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_60", "Interface.txt", "#space#", " ");
					upgradevalue = 0;
					upsq++;
				}
				else
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRASAILOFF";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_61", "Interface.txt", "#space#", " ");
				}
			break;
			case 2:
				GameInterface.SHIPSUP_TABLE.(row).td2.str = GetConvertStr("SUP_CANNONS_descr","ShipsDescribe.txt");
				if (CheckAttribute(RealShips[iShip],"Tuning.CannonsSpecial"))
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRAGUNSON";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_62", "Interface.txt", "#space#", " ");
					upgradevalue = 0;
					upsq++;
				}
				else
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRAGUNSOFF";
					if (RealShips[iShip].Type.War == true && RealShips[iShip].Type.Merchant == false) GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_63", "Interface.txt", "#space#", " ");
						else GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_64", "Interface.txt", "#space#", " ");
				}
				if (RealShips[iShip].Type.War == false && RealShips[iShip].Type.Merchant == true) GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_65", "Interface.txt", "#space#", " ");
			break;
			case 3:
				GameInterface.SHIPSUP_TABLE.(row).td2.str = GetConvertStr("SUP_HULLCOPPER_descr","ShipsDescribe.txt");
				if (CheckAttribute(RealShips[iShip],"Tuning.CuBot"))
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRAHULLKRENGON";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_66", "Interface.txt", "#space#", " ");
					upgradevalue = 0;
					upsq++;
				}
				else
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "EXTRAHULLKRENGOFF";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_67", "Interface.txt", "#space#", " ");
				}
			break;
			case 4:
				GameInterface.SHIPSUP_TABLE.(row).td2.str = GetConvertStr("SUP_CAPACITY_descr","ShipsDescribe.txt");
				if (CheckAttribute(RealShips[iShip],"Tuning.BotPack"))
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "ExtraCapacityOn";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_68", "Interface.txt", "#space#", " ");
					upgradevalue = 0;
					upsq++;
				}
				else
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "ExtraCapacityOff";
					if (RealShips[iShip].Type.War == false && RealShips[iShip].Type.Merchant == true) GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_69", "Interface.txt", "#space#", " ");
						else GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_70", "Interface.txt", "#space#", " ");
				}
				if (RealShips[iShip].Type.War == true && RealShips[iShip].Type.Merchant == false) GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_71", "Interface.txt", "#space#", " ");
			break;
			case 5:
				GameInterface.SHIPSUP_TABLE.(row).td2.str = GetConvertStr("SUP_BIGSIDES_descr","ShipsDescribe.txt");
				if (CheckAttribute(RealShips[iShip],"Tuning.HighBort"))
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "ExtraBigSidesOn";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_72", "Interface.txt", "#space#", " ");
					upgradevalue = 0;
					upsq++;
				}
				else
				{
					GameInterface.SHIPSUP_TABLE.(row).td1.icon.image = "ExtraBigSidesOff";
					GameInterface.SHIPSUP_TABLE.(row).td3.str = GetConvertStrWithReplace("Variable_shipyard_73", "Interface.txt", "#space#", " ");
				}
			break;
		}
		if (upsq >= 3) SetFormatedText("SHIPSUP_VALUE",GetConvertStrWithReplace("Variable_shipyard_74", "Interface.txt", "#space#", " "));
		else SetFormatedText("SHIPSUP_VALUE",FindRussianMoneyString(upgradevalue));
		SetFormatedText("SHIPSUP_CAPTION", GetConvertStrWithReplace("Variable_shipyard_75", "Interface.txt", "#space#", " "));
		SetFormatedText("SHIPSUP_Q", GetConvertStrWithReplace("Variable_shipyard_76", "Interface.txt", "#space#", " ")+upsq+"/3");
		SetSelectable("SHIPSUP_DOUP", false);
    }
	GameInterface.SHIPSUP_TABLE.select = 1;
	Table_UpdateWindow("SHIPSUP_TABLE");
}

void DoUpgrade()
{
	if (upgradevalue > 0) AddMoneyToCharacter(pchar, -upgradevalue);
	ref shTo = &RealShips[sti(xi_refCharacter.Ship.Type)];
	bool up = true;
	switch (uptype)
	{
		case 1:
			if (!CheckAttribute(shTo,"Tuning.HullSpecial"))
			{
				shTo.Tuning.HullSpecial = 1;
				shTo.price = makeint(sti(shTo.price)*1.5);
			}
			else
			{
				DeleteAttribute(shTo,"Tuning.HullSpecial");
				shTo.price = makeint(sti(shTo.price)/1.5);
				up = false;
			}
		break;
		case 2:
			if (!CheckAttribute(shTo,"Tuning.SailsSpecial"))
			{
				shTo.Tuning.SailsSpecial = 1;
				shTo.price = makeint(sti(shTo.price)*1.35);
			}
			else
			{
				DeleteAttribute(shTo,"Tuning.SailsSpecial");
				shTo.price = makeint(sti(shTo.price)/1.35);
				up = false;
			}
		break;
		case 3:
			if (!CheckAttribute(shTo,"Tuning.CannonsSpecial"))
			{
				shTo.Tuning.CannonsSpecial = 1;
				shTo.price = makeint(sti(shTo.price)*1.25);
			}
			else
			{
				DeleteAttribute(shTo,"Tuning.CannonsSpecial");
				shTo.price = makeint(sti(shTo.price)/1.25);
				up = false;
			}
		break;
		case 4:
			if (!CheckAttribute(shTo,"Tuning.CuBot"))
			{
				shTo.Tuning.CuBot = 1;
				shTo.price = makeint(sti(shTo.price)*1.4);
			}
			else
			{
				DeleteAttribute(shTo,"Tuning.CuBot");
				shTo.price = makeint(sti(shTo.price)/1.4);
				up = false;
			}
		break;
		case 5:
			if (!CheckAttribute(shTo,"Tuning.BotPack"))
			{
				shTo.Tuning.BotPack = 1;
				shTo.price = makeint(sti(shTo.price)*1.5);
				shTo.Capacity = sti(shTo.Capacity) + makeint(sti(shTo.Capacity)/2);
				shTo.HP = sti(shTo.HP) - makeint(sti(shTo.HP)/2);
				if (xi_refCharacter.ship.hp > sti(shTo.HP)) xi_refCharacter.ship.hp = sti(shTo.HP);
			}
			else
			{
				DeleteAttribute(shTo,"Tuning.BotPack");
				shTo.price = makeint(sti(shTo.price)/1.5);
				shTo.Capacity = makeint(sti(shTo.Capacity)/1.5);
				shTo.HP = sti(shTo.HP)*2;
				up = false;
			}
		break;
		case 6:
			if (!CheckAttribute(shTo,"Tuning.HighBort"))
			{
				shTo.Tuning.HighBort = 1;
				shTo.price = makeint(sti(shTo.price)*1.4);
			}
			else
			{
				DeleteAttribute(shTo,"Tuning.HighBort");
				shTo.price = makeint(sti(shTo.price)/1.4);
				up = false;
			}
		break;
	}
	timeUpgrade = timePreUpgrade;
	OpenShipUp();
} */

// Lugger: Смена разцветки корпуса -->
void ExitChangeHullMenu()
{
	HideChangeHullMenu();
	OnShipScrollChange();
}

void HideChangeHullMenu()
{
	XI_WindowShow("CHANGE_HULL_WINDOW", false);
	XI_WindowDisable("CHANGE_HULL_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);
	SetCurrentNode("TABLE_OTHER");
	sMessageMode = "";
}

void ShowChangeHullMenu()
{
	ref rShip = GetRealShip(sti(xi_refCharacter.Ship.Type));

	XI_WindowShow("CHANGE_HULL_WINDOW", true);
	XI_WindowDisable("CHANGE_HULL_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	SetCurrentNode("CHANGE_Hull_CANCEL");

	ref refShip = GetRealShip(sti(xi_refCharacter.Ship.Type));
	int iCurrentType = sti(refShip.Ship.Upgrades.Hull);
	PChar.ChangeHull.Type = iCurrentType;

	SetChangeHullInfo();
}

int GetShipHullCount(ref rShip)
{
	int iMax = 3;
	if (CheckAttribute(rShip,"hullNums")) iMax = sti(rShip.hullNums);
	return iMax;
}

void SetChangeHullInfo()
{
	ref rShip = GetRealShip(sti(xi_refCharacter.Ship.Type));
	string sShipName = rShip.name;

	SetFormatedText("CHANGE_HULL_CAPTION", GetConvertStrWithReplace("Variable_shipyard_77", "Interface.txt", "#space#", " ") + sti(PChar.ChangeHull.Type) + "/" + GetShipHullCount(rShip) + ")");

	string sTypeName = "Hull" + sti(PChar.ChangeHull.Type);

	string sTexture = GetShipTexturesForChangeHull();
	SetNewPicture("CHANGE_HULL_TYPE", "ships\" + sShipName + "\" + sTypeName + "\" + sTexture);

	int iMoney = CalculateHullChangePrice(sti(rShip.Class));
	SetFormatedText("CHANGE_HULL_MONEY", GetConvertStrWithReplace("Variable_shipyard_78", "Interface.txt", "#space#", " ") + iMoney);

	if(sti(PChar.money) > iMoney)
	{
		SetSelectable("CHANGE_HULL_OK", true);
	}
	else
	{
		SetSelectable("CHANGE_HULL_OK", false);
	}
}

string GetShipTexturesForChangeHull()
{
	ref rShip = GetRealShip(sti(xi_refCharacter.Ship.Type));
	string sShipName = rShip.name;

	int Hulls = LanguageOpenFile("Hulls.txt");
	string sTex = LanguageConvertString(Hulls, sShipName);
	LanguageCloseFile(Hulls);

	return sTex;
}

void SelectChangeHull(bool bLeft)
{
	ref rShip = GetRealShip(sti(xi_refCharacter.Ship.Type));
	string sShipName = rShip.name;

	int iMax = GetShipHullCount(rShip);

	if(bLeft)
	{
		PChar.ChangeHull.Type = sti(PChar.ChangeHull.Type) - 1;

		if(sti(PChar.ChangeHull.Type) < 1)
		{
			PChar.ChangeHull.Type = iMax;
		}
	}
	else
	{
		PChar.ChangeHull.Type = sti(PChar.ChangeHull.Type) + 1;

		if(sti(PChar.ChangeHull.Type) > iMax)
		{
			PChar.ChangeHull.Type = 1;
		}
	}

	SetChangeHullInfo();
}

void SetNewHullToCharacterShip()
{
	ref refShip = GetRealShip(sti(xi_refCharacter.Ship.Type));

	int iMoney = CalculateHullChangePrice(sti(refShip.Class));
	AddMoneyToCharacter(PChar, -iMoney);

	refShip.Ship.Upgrades.Hull = sti(PChar.ChangeHull.Type);

	ExitChangeHullMenu();
}

int CalculateHullChangePrice(int value)
{
	switch (value)
	{
		case 7: return 1000+drand(500);
		break;
		case 6: return 5000+drand(2500);
		break;
		case 5: return 10000+drand(5000);
		break;
		case 4: return 20000+drand(10000);
		break;
		case 3: return 35000+drand(17500);
		break;
		case 2: return 50000+drand(25000);
		break;
		case 1: return 100000+drand(50000);
		break;
	}
}
// Lugger: Смена разцветки корпуса <--

//Смена каюты -->
void ExitChangeCABINMenu()
{
	HideChangeCABINMenu();
	OnShipScrollChange();
}

void HideChangeCABINMenu()
{
	XI_WindowShow("CHANGE_CABIN_WINDOW", false);
	XI_WindowDisable("CHANGE_CABIN_WINDOW", true);
	XI_WindowDisable("MAIN_WINDOW", false);
	SetCurrentNode("TABLE_OTHER");
	sMessageMode = "";
}

void ShowChangeCABINMenu()
{
	XI_WindowShow("CHANGE_CABIN_WINDOW", true);
	XI_WindowDisable("CHANGE_CABIN_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);
	SetCurrentNode("CHANGE_CABIN_CANCEL");

	ref refShip = GetRealShip(sti(xi_refCharacter.Ship.Type));
	for (int i=0;i<iCabinsNum;i++)
	{
		if (sti(refShip.Class)==7) break;
		if (refShip.CabinType != sCabins[i]) continue;
		iCurCabin = i; 
		break;
	}
	SetChangeCABINInfo();
}

void SetChangeCABINInfo()
{
	string sTexture = GetShipTexturesForChangeCABIN();
	SetNewPicture("CHANGE_CABIN_TYPE", sTexture);
	int iMoney = CalculateCABINChangePrice();
	SetFormatedText("CHANGE_CABIN_MONEY", GetConvertStrWithReplace("Variable_shipyard_79", "Interface.txt", "#space#", " ") + iMoney);

	bool isNewCabin = true;
	ref refShip = GetRealShip(sti(xi_refCharacter.Ship.Type));
	if (!isCabinforClass(sti(refShip.Class))) 
	{
		isNewCabin = false;
		SetFormatedText("CHANGE_CABIN_MONEY", GetConvertStrWithReplace("Variable_shipyard_80", "Interface.txt", "#space#", " "));
	}
	if (sti(refShip.Class)<7 && refShip.CabinType == sCabins[iCurCabin]) 
	{
		isNewCabin = false; 
		SetFormatedText("CHANGE_CABIN_MONEY", GetConvertStrWithReplace("Variable_shipyard_81", "Interface.txt", "#space#", " "));
	}

	if (isNewCabin && sti(PChar.money) > iMoney) SetSelectable("CHANGE_CABIN_OK", true);
											else SetSelectable("CHANGE_CABIN_OK", false);
}

bool isCabinforClass(int iClass)
{
	switch (iClass)
	{
	case 7: return false; break;
	case 6: if (iCurCabin>-1 && iCurCabin<3) return true; break; 
	case 5: if (iCurCabin>0 && iCurCabin<4) return true; break;//№0-1
	case 4: if (iCurCabin>2 && iCurCabin<6) return true; break;	//№2-4
	case 3: if (iCurCabin>3 && iCurCabin<7) return true; break; 
	case 2: if (iCurCabin>5 && iCurCabin<9) return true; break;	//№5-7
	case 1: if (iCurCabin>6 && iCurCabin<10) return true; break;
	}
	return false;
}

string GetShipTexturesForChangeCABIN()
{
	int nLoc = FindLocation(sCabins[iCurCabin]);
	if (nLoc >= 0 && CheckAttribute(&Locations[nLoc],"image")) 
	return Locations[nLoc].image;
}

void SelectChangeCABIN(bool bLeft)
{
	if (bLeft) iCurCabin--; else iCurCabin++;
	if (iCurCabin < 0) iCurCabin = iCabinsNum-1;
	if (iCurCabin == iCabinsNum) iCurCabin = 0;
	SetChangeCABINInfo();
}

void SetNewCABINToCharacterShip()
{
	ref refShip = GetRealShip(sti(xi_refCharacter.Ship.Type));
	refShip.CabinType = sCabins[iCurCabin];

	int iMoney = CalculateCABINChangePrice();
	AddMoneyToCharacter(PChar, -iMoney);
	ExitChangeCABINMenu();
}

int CalculateCABINChangePrice()
{	//10ая часть цены корабля с учётом наценки конкретного верфиста
	return makeint(GetShipBuyPrice(sti(xi_refCharacter.Ship.Type), refNPCShipyard)/10);
}
//Смена каюты <--

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();

	//string sRow = "tr" + (iRow + 1);
	if (sControl == "TABLE_SHIPYARD")
	{
		if (!SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sControl, 1 )) SortTable(sControl, iColumn);
		Table_UpdateWindow(sControl);
	}
}
