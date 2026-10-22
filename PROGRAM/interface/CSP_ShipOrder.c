//Смена каюты -->
string sCabins[10] = {"Cabin_Small","Cabin_Small2","New_Cabin1","Cabin_Medium1","Cabin_Medium","Cabin_Medium2","New_Cabin2","Cabin_Quest","Cabin","Cabin_Huge"};//берём из SetCabinTypeEx
int iCabinsNum = 10;//число видов кают
int iCurCabin = 0;
//Смена каюты <--

ref refNPCShipyard;
ref refStore;

int iYarderSkill = 100;
int iTunPoints = 2;
int iTimeMake, iShipPoints, iQBorders, iPriceOrder;
int iQMAX, iQMIN, iFreeSP, iFreeTP;
bool Tune_Sheme[10] = {0,0,0,0,0,0,0,0,0,0};//элементы 0(заголовок таблицы) и 2(паруса) пропускаем. просто для удобства
float Ship_Sheme[11] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,-10.0};//тип флоат, чтоб не преобразовывать лишний раз при умножении на коэффициент, по факту - целые
// Caribbean Legend 1.1.2 cannon IDs: none, twelve cannon calibers, then
// three culverines. The former CSP numeric ordering referred to another game.
int CannonTypes[20] = {1000,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,1000,1000,1000,1000};
int CannonTypesCount = 16;

string CurTable, CurRow, sNation;
int iSelected; // курсор в таблице
int FIS_FilterState = 7;
bool FState_Material = 1;
bool bRankRequirement = false;
string sMessageMode, index;

int sundukSum;//на все апгрейды одинаковое колво сундуков	//chest
int Tun_Mater1[10];//кол-во товар
int Tun_Mater2[10];//кол-во предмет
int Tun_Mater3[10];//кол-во деньги	//gold
string sAdd[10];

// Local copies keep this Workshop mod independent from CSP's global arrays.
string CSP_TuningGoods[10] = {"","Mahogany","","Planks","Silk","Linen","Cotton","Sandal","Leather","Ebony"};
string CSP_TuningItems[10] = {"","jewelry17","","jewelry11","jewelry2","jewelry3","jewelry4","jewelry5","jewelry1","icollection"};

int saves = 0;
int savem = 0;

void InitInterface_R(string iniName, ref _shipyarder)
{
	GameInterface.title = "titleShipyard";
	SendMessage(&GameInterface,"ls",MSG_INTERFACE_INIT,iniName);

	refNPCShipyard  = _shipyarder;
	sNation = GetNationNameByType(sti(refNPCShipyard.nation));
	trace("[CSP_SHIPYARD_DIAG] OPEN npc=" + refNPCShipyard.id + " city=" + refNPCShipyard.City + " face=" + refNPCShipyard.FaceId + " ini=" + iniName);

	InitSAddMassive();

	SetNodeUsing("Check_Material", false);
	FState_Material = true;
	if (refNPCShipyard.id != "Pirates_shipyarder") iYarderSkill = sti(refNPCShipyard.reputation)/2+50;
	// CSP custom orders always provide two improvement slots.  The original
	// reputation formula can produce one slot with Caribbean Legend NPC data.
	iTunPoints = 2;
	//берём за навык кораблестроения репутацию верфиста и приводим к отрезку (56:100)

	iShipPoints = 6 + iYarderSkill/6 - (MOD_SKILL_ENEMY_RATE*3)/3;//целые переменные делятся с округлением вниз
	iQBorders = (iShipPoints+10)/3;
	if (iQBorders > 10) iQBorders = 10;
	iQMAX = iQBorders;
	iQMIN = -iQBorders;
	iFreeSP = iShipPoints;
	iFreeTP = iTunPoints;

	int iTest = FindColony(refNPCShipyard.City); // город магазина
	ref rColony;
	if (iTest != -1) {rColony = GetColonyByIndex(iTest);}
	refStore = &stores[sti(rColony.StoreNum)];

	if (iTunPoints < 1) SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, "TunSheme", -1, 1, 0);//если тюнинга нет из-за низкого навыка, отключаем клики по кнопкам

	SetEventHandler("InterfaceBreak","ProcessExitCancel",0);
	SetEventHandler("exitCancel","ProcessExitCancel",0);
	SetEventHandler("ievnt_command","ProcessCommandExecute",0);
	SetEventHandler("ShowInfoWindow","ShowInfoWindow",0);
	SetEventHandler("MouseRClickUp","HideInfoWindow",0);
	SetEventHandler("CheckButtonChange", "ProcessFilter", 0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ExitMsgMenu", "ExitMsgMenu", 0);
	SetEventHandler("OnTableClick", "OnTableClick", 0);
	SetEventHandler("ExitChangeCabinMenu", "ExitChangeCabinMenu",0);

	EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",156,40,366,275); // tak from SHIP_BIG_PICTURE
	EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 161,246,361,1, 4);
	EI_CreateFrame("SHIP_BIG_PICTURE_BORDER",20,46,134,161);
	EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,187,147,1, 4);
	EI_CreateHLine("SHIP_BIG_PICTURE_BORDER", 8,166,147,1, 4);

	SetNewGroupPicture("Money_PIC", "ICONS_CHAR", "Money");

	int portraitFace = sti(refNPCShipyard.FaceId);
	SetNewPicture("MAIN_CHARACTER_PICTURE", "interfaces\portraits\128\face_" + portraitFace + ".tga");

	SetFormatedText("HERO_TEXT", GetConvertStr("Variable_shipyard2_30", "Interface.txt"));

	SetFormatedText("HERO_NAME", GetFullName(refNPCShipyard));
	SetFormatedText("HERO_RANK",refNPCShipyard.Rank);

	SetFormatedText("HERO_SKILL","" + iYarderSkill);
	SetFormatedText("HERO_QBORDERS", iQBorders*10 + "%");
	SetFormatedText("HERO_SHIPPOINTS",iFreeSP + "/" + iShipPoints);
	SetFormatedText("HERO_TUNPOINTS",iFreeTP + "/" + iTunPoints);

	SetFormatedText("Money_TEXT", MakeMoneyShow(sti(pchar.Money), MONEY_SIGN,MONEY_DELIVER));

	FillShipyardTable();
	FillOrderShip(3);//шлюп на старте
	FillShipParam();
	SetCurrentNode("TABLE_OTHER");
	sMessageMode = "";
	SetButtionsAccess();//доступность кнопки GetConvertStrWithReplace("Variable_shipyard1_1", "Interface.txt", "#space#", " ")
	SetSelectable("BUTTON_CABIN", true);
}

void ProcessExitCancel()
{
	IDoExit(RC_INTERFACE_ANY_EXIT);
}

void IDoExit(int exitCode)
{
	DelEventHandler("InterfaceBreak","ProcessExitCancel");
	DelEventHandler("exitCancel","ProcessExitCancel");
	DelEventHandler("ievnt_command","ProcessCommandExecute");
	DelEventHandler("ShowInfoWindow","ShowInfoWindow");
	DelEventHandler("MouseRClickUp","HideInfoWindow");
	DelEventHandler("CheckButtonChange", "ProcessFilter");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ExitMsgMenu", "ExitMsgMenu");
	DelEventHandler("OnTableClick", "OnTableClick");
	DelEventHandler("ExitChangeCabinMenu", "ExitChangeCabinMenu");

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

void ProcessCommandExecute()
{
	string comName = GetEventData();
	string nodName = GetEventData();
	trace("[CSP_SHIPYARD_DIAG] COMMAND node=" + nodName + " command=" + comName);

	switch(nodName)
	{
		case "CANNON_NEXT":
			if (comName=="click" || comName=="activate")
			{
				CurTable = "TABLE_OTHER";
				iSelected = 10;
				trace("[CSP_SHIPYARD_DIAG] CANNON next requested from fixed control");
				IncShipQ(1);
			}
		break;

		case "CANNON_PREV":
			if (comName=="click" || comName=="activate")
			{
				CurTable = "TABLE_OTHER";
				iSelected = 10;
				trace("[CSP_SHIPYARD_DIAG] CANNON previous requested from fixed control");
				DecShipQ(1);
			}
		break;

		case "TABLE_OTHER":
			if (iSelected == 10)
			{
				if (comName=="click" || comName=="activate")
				{
					trace("[CSP_SHIPYARD_DIAG] CANNON next requested from table row");
					IncShipQ(1);
				}
				if (comName=="rclick")
				{
					trace("[CSP_SHIPYARD_DIAG] CANNON previous requested from table row");
					DecShipQ(1);
				}
			}
		break;

		case "BUTTON_CABIN":
			if (comName=="click" || comName=="activate")
			{
			    ShowChangeCABINMenu();
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

		case "B_SKILLUP":
			if (comName=="click")
			{
				IncShipQ(1);
			}
			if (comName=="rclick")
			{
				IncShipQ(8);
			}
		break;

		case "B_SKILLDOWN":
			if (comName=="click")
			{
				DecShipQ(1);
			}
			if (comName=="rclick")
			{
				DecShipQ(8);
			}
		break;

		case "BUTTON_BUY":
			if (comName=="click" || comName=="activate")
			{
				ShowMessageInfo();
			}
		break;

		case "MSG_OK":
			if (comName=="click" || comName=="activate")
			{
				DoBuyShip();
			}
		break;
	}
}

void FillOrderShip(int _iShipBaseType)
{
	int iShip = CreateBaseShip(_iShipBaseType);
	refNPCShipyard.Ship.Type = iShip;
	aref refShip;
	makearef(refShip, refNPCShipyard.Ship);
	if (!CheckAttribute(refShip,"soiling")) refShip.soiling = 0;//чтоб не срало в лог об отсутствии

	if(!CheckAttribute(refNPCShipyard, "ship.upgrades"))
	{
		refNPCShipyard.ship.upgrades.hull = 1;
		refNPCShipyard.ship.upgrades.sails = 1;
		refNPCShipyard.ship.upgrades.cannons = 1;
	}

	refShip.Pos.x = 0;
	refShip.Pos.y = 0;
	refShip.Pos.z = 0;
	refShip.Ang = "";
	refShip.Cannons.Charge.Type = GOOD_BALLS;

	if (!CheckAttribute(refShip,"Cannons.Type")) refShip.Cannons.Type = CANNON_TYPE_NONECANNON;
	if (!CheckAttribute(refShip,"Crew.Morale"))	refShip.Crew.Morale = 20 + rand(79);
	if (!CheckAttribute(refShip,"Crew.Quantity")) refShip.Crew.Quantity = 0;

	if(!CheckAttribute(refNPCShipyard, "ship.crew.Exp"))
	{
		refNPCShipyard.Ship.Crew.Exp.Sailors   = 1 + rand(80);
		refNPCShipyard.Ship.Crew.Exp.Cannoners = 1 + rand(80);
		refNPCShipyard.Ship.Crew.Exp.Soldiers  = 1 + rand(80);
	}
	refNPCShipyard.ship.crew.disease = 0;
	ResetShipCannonsDamages(refNPCShipyard);
	DeleteAttribute(refNPCShipyard, "Ship.Cargo");
	NullCharacterGoods(refNPCShipyard);
	SetRandomNameToShip(refNPCShipyard);
	string canonicalName = CSP_GetSpecialShipCanonicalName(RealShips[iShip].BaseName);
	if (canonicalName != "") refNPCShipyard.ship.name = canonicalName;

	if (isCabinforClass(sti(RealShips[iShip].Class))) SetNewCABINToCharacterShip();//ставим выбранную у прошлого корабля каюту, если она подходит классу
		else iCurCabin = 0; // Current game assigns its native cabin when the ship is delivered.
}

void FillShipParam()
{
	int iShip = sti(refNPCShipyard.ship.type);
	if (iShip != SHIP_NOTUSED)
	{
		ref rRealShip = &RealShips[iShip];
		ref rBaseShip = GetShipByType(sti(rRealShip.BaseType));
		DeleteAttribute(rRealShip, "Tuning");//просто затираем записи об апгрейдах
		DeleteAttribute(rRealShip, "Untuned");
		//снимаем чекбокс тюнинга на 42фт	//получается рекурсия какая-то - после снятия чекбокса заного вызывается эта же функция
		if (Tune_Sheme[9] && sti(rBaseShip.MaxCaliber) == 42) SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, "TunSheme", 2, 9, 0);

		rRealShip.HP = stf(rBaseShip.HP) * (1 + SelectShipsBonuses(rBaseShip, "bonusHP", saves, savem));
		// Current Caribbean Legend tracks damage for each mast and has no CSP MastMultiplier field.
		rRealShip.SpeedRate = stf(rBaseShip.SpeedRate) * (1 + SelectShipsBonuses(rBaseShip, "bonusSpeed", saves, savem));
		rRealShip.TurnRate = stf(rBaseShip.TurnRate) * (1 + SelectShipsBonuses(rBaseShip, "bonusTurnRate", saves, savem));
		rRealShip.WindAgainstSpeed = stf(rBaseShip.WindAgainstSpeed) * (1 + SelectShipsBonuses(rBaseShip, "bonusWindAgainstSpeed", saves, savem));
		rRealShip.Capacity = makeint(stf(rBaseShip.Capacity) * (1 + SelectShipsBonuses(rBaseShip, "bonusCapacity", saves, savem)));
		rRealShip.OptCrew = stf(rBaseShip.OptCrew) * (1 + SelectShipsBonuses(rBaseShip, "bonusOptCrew", saves, savem));
		rRealShip.MinCrew = stf(rBaseShip.MinCrew) * (1 + SelectShipsBonuses(rBaseShip, "bonusOptCrew", saves, savem));
		rRealShip.MaxCrew = stf(rBaseShip.MaxCrew) * (1 + SelectShipsBonuses(rBaseShip, "bonusOptCrew", saves, savem));
		if (CSP_IsPearlOrQueenAnne(rRealShip.BaseName))
		{
			rRealShip.HP = 6700;
			rRealShip.SpeedRate = 16.28;
			rRealShip.TurnRate = 28.79;
			rRealShip.WindAgainstSpeed = 4.90;
			rRealShip.MinCrew = 98;
			rRealShip.OptCrew = 416;
			rRealShip.MaxCrew = 416;
		}
		/* rRealShip.MaxCrew = stf(rBaseShip.MaxCrew) * (1 + Ship_Sheme[8]/10.0*SHIP_STAT_RANGE_REQUEST);
		rRealShip.MinCrew = stf(rBaseShip.MinCrew) * (1 + Ship_Sheme[8]/10.0*SHIP_STAT_RANGE_REQUEST); */
		rRealShip.MaxCaliber = rBaseShip.MaxCaliber;
		rRealShip.CannonsQuantityMax = sti(rRealShip.CannonsQuantity);

		refNPCShipyard.Ship.Cannons.Type = CANNON_TYPE_NONECANNON;
		// Always restart from the immutable ship definition. Writing the retail
		// result back here makes every interface refresh compound price and time.
		rRealShip.Price = rBaseShip.Price;
		CalcTuningPrice();//стоимость апгрейда до применения апгрейдов считаем

		CSP_SetTuningStates(refNPCShipyard, Tune_Sheme[9], Tune_Sheme[7], Tune_Sheme[4], Tune_Sheme[8], Tune_Sheme[5], Tune_Sheme[1], Tune_Sheme[6]);
		//void SetBermudeTuningStates2Ship(ref chr, bool MaxCaliber, bool Capacity, bool SpeedRate, bool MaxCrew, bool TurnRate, bool HP, bool MastMulti, bool WAS)

		int selectedCannonIndex = makeint(Ship_Sheme[10]) + 10;
		int effectiveMaxCaliber = CSP_GetEffectiveMaxCaliber();
		if (selectedCannonIndex < 0 || selectedCannonIndex >= CannonTypesCount) selectedCannonIndex = 0;
		if (selectedCannonIndex > 0 && GetCannonCaliber(CannonTypes[selectedCannonIndex]) > effectiveMaxCaliber) selectedCannonIndex = 0;
		Ship_Sheme[10] = selectedCannonIndex - 10;
		refNPCShipyard.Ship.Cannons.Type = CannonTypes[selectedCannonIndex];
		trace("[CSP_SHIPYARD_DIAG] RECALC shipType=" + iShip + " buildStyle=" + saves + " material=" + savem + " cannonScheme=" + Ship_Sheme[10] + " cannonType=" + refNPCShipyard.Ship.Cannons.Type);

		refNPCShipyard.ship.hp = GetCharacterShipHP(refNPCShipyard);//прочность корпуса на максимум
		refNPCShipyard.ship.sp = 100;
		DeleteAttribute(refNPCShipyard,"ship.blots");//и пробоины убираем //а они могут тут быть???
		DeleteAttribute(refNPCShipyard,"ship.sails");
		DeleteAttribute(refNPCShipyard,"ship.masts");

		// Construction time is derived once from the immutable base definition.
		// Cannon browsing and interface refreshes must never add time repeatedly.
		iTimeMake = 5 + sti(rBaseShip.Price)/15000 + (iTunPoints - iFreeTP)*0.27;
		if (iTimeMake < 5) iTimeMake = 5;
		SetFormatedText("Time_TEXT", GetConvertStr("Variable_shipyard1_2", "Interface.txt") + " " + iTimeMake + " " + GetConvertStr("Variable_shipyard1_3", "Interface.txt"));

		FillPrice();

		string sShip = rRealShip.BaseName;
		SetNewPicture("SHIP_BIG_PICTURE", "interfaces\ships\" + sShip + ".tga");
		GameInterface.edit_box.str = refNPCShipyard.ship.name;
		SetFormatedText("SHIP_RANK", rRealShip.Class);
		SetShipOTHERTable("TABLE_OTHER", refNPCShipyard);
		CSP_SetShipQualityBars("BAR_HP", "BAR_Mast", "BAR_Speed", "BAR_TurnRate", "BAR_WAS", "BAR_Capacity", "BAR_Crew");
		SetFormatedText("FRAME_INFO_CAPTION", CSP_GetShipDisplayName(sShip));
		SetFormatedText("INFO_TEXT", GetConvertStr(sShip, "ShipsDescribe.txt"));
	}
	else
	{
		SetNewPicture("SHIP_BIG_PICTURE", "interfaces\ships\Liuto.tga");
		GameInterface.edit_box.str = XI_Convertstring("NoneBoat");
		SetFormatedText("FRAME_INFO_CAPTION","");
		SetFormatedText("INFO_TEXT","");
	}
	Table_UpdateWindow("TABLE_OTHER");

	HideSkillChanger();
	SetSkillArrows();
	SetButtionsAccess();
}

void ShowInfoWindow()
{
	string sCurrentNode = GetCurrentNode();
	string sHeader, sText1, sText2, sText3, sText4, sPicture;
	string sGroup, sGroupPicture;
	int iItem;
	int xx = 64;
	int yy = 64;

	sPicture = "";
	string sAttributeName;
	int nChooseNum = -1;
	int iShip;
	ref refBaseShip;

	bool  bShowHint = true;

	ref rChr = refNPCShipyard;

	switch (sCurrentNode)
	{
		case "SHIP_BIG_PICTURE":
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
			if (GameInterface.(CurTable).(CurRow).UserData.ID == "CannonType" && sti(rChr.Ship.Cannons.Type) != CANNON_TYPE_NONECANNON)
			{
				sGroup = "GOODS";
				sGroupPicture = GetCannonType(sti(rChr.Ship.Cannons.Type)) + "_" + GetCannonCaliber(sti(rChr.Ship.Cannons.Type));
			}
		break;
		case "TABLE_SHIPYARD" :
			sHeader = XI_Convertstring("Shipyard");
			sText1  = GetConvertStr("Shipyard_hint", "ShipsDescribe.txt");
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
}

void TableSelectChange()
{
	string sControl = GetEventData();
	iSelected = GetEventData();
	CurTable = sControl;
	CurRow   =  "tr" + (iSelected);
	NullSelectTable("TABLE_OTHER");

	if (CurTable == "TABLE_SHIPYARD")
	{

		int iCurShip = RealShips[sti(refNPCShipyard.ship.type)].basetype;
		int iNewShip = sti(GameInterface.(CurTable).(CurRow).index);
		if (iNewShip == iCurShip) return;

		DeleteAttribute(&RealShips[sti(refNPCShipyard.Ship.Type)], "name");//при смене типа корабля помечаем предыдущий на удаление из массива реальных
		FillOrderShip(iNewShip);
	}
	FillShipParam();
	if (CurTable == "TABLE_SHIPYARD") SetCurrentNode("TABLE_SHIPYARD");
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

	SetCurrentNode("TABLE_OTHER");
	sMessageMode = "";
}

void FillShipyardTable()
{
	Table_Clear("TABLE_SHIPYARD", false, true, false);

	GameInterface.TABLE_SHIPYARD.hr.td1.str = GetConvertStr("Variable_shipyard_25", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td1.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.hr.td2.str = GetConvertStr("Variable_shipyard_27", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td2.scale = 0.9;
	GameInterface.TABLE_SHIPYARD.hr.td3.str = GetConvertStr("Variable_shipyard1_15", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td3.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td4.str = GetConvertStr("Variable_shipyard_30", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td4.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td5.str = GetConvertStr("Variable_shipyard1_17", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td5.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td6.str = GetConvertStr("Variable_shipyard1_18", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td6.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td7.str = GetConvertStr("Variable_ColonyGuardiang_18", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td7.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.hr.td8.str = GetConvertStr("Variable_badwind_short", "Interface.txt");
	GameInterface.TABLE_SHIPYARD.hr.td8.scale = 0.8;
	GameInterface.TABLE_SHIPYARD.select = 0;
	GameInterface.TABLE_SHIPYARD.top = 0;
//--> mod tablesort
	GameInterface.TABLE_SHIPYARD.hr.td1.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td2.sorttype = "string";
	GameInterface.TABLE_SHIPYARD.hr.td3.sorttype = "string";//нарушение сортировки 1 - 10 - 2, нужно проверять длину строки и дописывать в начале 0 коротким
	GameInterface.TABLE_SHIPYARD.hr.td4.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td5.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td6.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td7.sorttype = "";
	GameInterface.TABLE_SHIPYARD.hr.td8.sorttype = "";
//<-- mod tablesort

	// Enumerate the live ship array so Workshop migrations can append CSP/Pack
	// Ships vessels without requiring this interface to replace ships.h.
	int iStart = SHIP_LIUTO;
	int iEnd = GetArraySize(&ShipsTypes);
	int i;
	int k = 0;
	string sShip, sTemp;
	string row;
	aref aTemp;
	bool isCapital;
	if (refNPCShipyard.city == "Tortuga" ||
		refNPCShipyard.city == "Villemstad" ||
		refNPCShipyard.city == "PortRoyal" ||
		refNPCShipyard.city == "Havana" ||
		refNPCShipyard.city == "Charles" ||
		refNPCShipyard.city == "PortoBello")
		isCapital = 1;
	else
		isCapital = 0;

	for (i = iStart; i < iEnd; i++)
	{
		if (i == SHIP_FORT) continue;
		if (!CheckAttribute(&ShipsTypes[i], "Name")) continue;
		if (!CheckAttribute(&ShipsTypes[i], "Class")) continue;
		bool isCSPPackShip = (i >= SHIP_TYPES_QUANTITY_WITH_FORT);
		row = "tr" + (k+1);

		if (FIS_FilterState >= 1 && FIS_FilterState <= 6 && sti(ShipsTypes[i].Class) != FIS_FilterState) continue;

		if (isCSPPackShip)
		{
			// Appended CSP ships deliberately have no national stock flags.
			// Reaching this shipwright already means CL accepted the player's
			// flag, reputation and stealth checks for the port, so custom
			// commissions are available here without a fictional pirate yard.
		}
		else if (CheckAttribute(&ShipsTypes[i], "nation." + sNation) && sti(ShipsTypes[i].nation.(sNation)))
		{
			if (sti(ShipsTypes[i].Class) == 1 && !isCapital) continue;
		}
		else continue;//пропускаем корабли без нации верфиста

		sShip = shipstypes[i].Name;
		GameInterface.TABLE_SHIPYARD.(row).index = i;

		if (isCSPPackShip)
			GameInterface.TABLE_SHIPYARD.(row).td1.icon.texture = "interfaces\LE\ships\" + sShip + ".tga";
		else
			GameInterface.TABLE_SHIPYARD.(row).td1.icon.texture = "interfaces\ships\" + sShip + ".tga";
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.uv = "0,0,1,1";
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.width = 46;
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.height = 46;
		GameInterface.TABLE_SHIPYARD.(row).td1.icon.offset = "0, 1";
		GameInterface.TABLE_SHIPYARD.(row).td1.str = shipstypes[i].Class;
		GameInterface.TABLE_SHIPYARD.(row).td1.textoffset = "40,-16";

		if (sti(shipstypes[i].Class) == 1)
		{
			switch (sNation)
			{
			case "England": sTemp = GetConvertStr("Variable_shipyard1_19", "Interface.txt"); break;
			case "Spain": sTemp = GetConvertStr("Variable_shipyard1_20", "Interface.txt"); break;
			case "France": sTemp = GetConvertStr("Variable_shipyard1_21", "Interface.txt"); break;
			case "Holland": sTemp = GetConvertStr("Variable_shipyard1_22", "Interface.txt"); break;
			}
		}
		else sTemp = "";
		GameInterface.TABLE_SHIPYARD.(row).td2.str = CSP_GetShipDisplayName(sShip) + sTemp;
		GameInterface.TABLE_SHIPYARD.(row).td2.align = "left";
		GameInterface.TABLE_SHIPYARD.(row).td2.scale = 0.82;

		GameInterface.TABLE_SHIPYARD.(row).td2.textoffset = "0,6";//сдвинул текст чуть ниже, для более красивого расположения плашки, по умолчанию пустая строка IG Baron. 
		// CSP's old trader/war badges use atlas entries that are not present in
		// Caribbean Legend. Leaving them assigned draws corrupted diagonal
		// fragments over every ship name, so the CL port uses the ship portrait
		// in column one and keeps the name column clean.

		GameInterface.TABLE_SHIPYARD.(row).td3.str = shipstypes[i].CannonsQuantity;
		GameInterface.TABLE_SHIPYARD.(row).td4.str = shipstypes[i].Capacity;
		GameInterface.TABLE_SHIPYARD.(row).td5.str = FloatToString(stf(shipstypes[i].Speedrate),1);
		GameInterface.TABLE_SHIPYARD.(row).td6.str = shipstypes[i].OptCrew;
		GameInterface.TABLE_SHIPYARD.(row).td7.str = shipstypes[i].hp;
		GameInterface.TABLE_SHIPYARD.(row).td8.str = FloatToString(stf(shipstypes[i].WindAgainstSpeed),2);
		GameInterface.TABLE_SHIPYARD.(row).td8.textoffset = "-8,0";
		k++;
	}
	Table_UpdateWindow("TABLE_SHIPYARD");
}

void ProcessFilter()
{
	string sControl = GetEventData();
	int iSelectedCB = GetEventData();
	int iNewState = GetEventData();
	trace("[CSP_SHIPYARD_DIAG] SELECT control=" + sControl + " index=" + iSelectedCB + " state=" + iNewState);
//========================================//
	if (sControl == "CLASS_ALL")
	{
		FIS_FilterState = 8 - iSelectedCB;
		FillShipyardTable();
		return;
	}
	if (sControl == "SHIPYARD")
	{
		saves = iSelectedCB-1;
		FillShipParam();
		return;
	}
	if (sControl == "MATERIAL")
	{
		savem = iSelectedCB-1;
		FillShipParam();
		return;
	}
//========================================//
	if (sControl == "Check_Material")
	{
	FState_Material = 1;
	FillPrice();
	return;
	}
//========================================//
	if (sControl == "TunSheme")
	{
		bool bneedunlock = false;
		int i;
		if (!iFreeTP) bneedunlock = true;//было залочено

		Tune_Sheme[iSelectedCB] = iNewState;
		CSP_RecountFreeImprovementPoints();

		if (iFreeTP < 1 || bneedunlock)
		{
		for(i=1;i<10;i++)
			{
				if (i != 3 || i != 10) continue;
				if (SendMessage(&GameInterface,"lsll",MSG_INTERFACE_MSG_TO_NODE, "TunSheme", 3, i)) continue;//нажатые кнопки не трогаем
				SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, "TunSheme", 5, i, !bneedunlock); //ненажатые блочим, если апгрейды кончились
			}
		SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, "TunSheme", 5, 2, 1);//паруса всегда заблочены
		if (CheckAttribute(&RealShips[refNPCShipyard.ship.type], "MaxCaliber") &&
			sti(RealShips[refNPCShipyard.ship.type].MaxCaliber) == 42)
		{
			SendMessage(&GameInterface,"lslll",MSG_INTERFACE_MSG_TO_NODE, "TunSheme", 5, 9, 1);
		}
		}
		SetFormatedText("HERO_TUNPOINTS",iFreeTP + "/" + iTunPoints);
		FillShipParam();
		return;
	}
//========================================//
}

void SetButtionsAccess()
{
	int iShip = sti(refNPCShipyard.Ship.Type);
	int iClass = RealShips[iShip].Class;
	int iRank = sti(pchar.rank);
	string sText = GetConvertStr("Variable_shipyard1_23", "Interface.txt");
	if (iPriceOrder > sti(pchar.Money)) SetSelectable("BUTTON_BUY", false); else SetSelectable("BUTTON_BUY", true);
	if (bRankRequirement)
	{
//		if (iRank<-4 && iClass<6) {SetSelectable("BUTTON_BUY", false); sText = "c -4 ранга";}
//		if (iRank<1 && iClass<5) {SetSelectable("BUTTON_BUY", false); sText = "c 1 ранга";}
		if (iRank<6 && iClass<4) {SetSelectable("BUTTON_BUY", false); sText = "c 6 ранга";}
		if (iRank<12 && iClass<3) {SetSelectable("BUTTON_BUY", false); sText = "c 12 ранга";}
		if (iRank<20 && iClass<2) {SetSelectable("BUTTON_BUY", false); sText = "c 20 ранга";}
	}
	else
	{
//		if (iRank<-8 && iClass<6) {SetSelectable("BUTTON_BUY", false); sText = "c -8 ранга";}
//		if (iRank<-3 && iClass<5) {SetSelectable("BUTTON_BUY", false); sText = "c -3 ранга";}
		if (iRank<2 && iClass<4) {SetSelectable("BUTTON_BUY", false); sText = "c 2 ранга";}
		if (iRank<8 && iClass<3) {SetSelectable("BUTTON_BUY", false); sText = "c 8 ранга";}
		if (iRank<16 && iClass<2) {SetSelectable("BUTTON_BUY", false); sText = "c 16 ранга";}//снижаем требования ранга ГГ
	}
	if (refNPCShipyard.id != "Pirates_shipyarder" && sti(RealShips[iShip].Class) == 1 && !CheckCharacterItem(Pchar, "patent_" + NationShortName(sti(refNPCShipyard.nation))) && sText == GetConvertStr("Variable_shipyard1_24", "Interface.txt"))
	{SetSelectable("BUTTON_BUY", false); sText = GetConvertStr("Variable_shipyard1_25", "Interface.txt");}
	SendMessage(&GameInterface,"lsls",MSG_INTERFACE_MSG_TO_NODE,"BUTTON_BUY", 0, "#" + sText);
}

void ShowMessageInfo()
{
	SetFormatedText("MSG_WINDOW_CAPTION", XI_ConvertString("Ship"));
	SetFormatedText("MSG_WINDOW_TEXT", GetConvertStr("Variable_shipyard1_26", "Interface.txt") + " " + FindMoneyString(iPriceOrder) + "?");
	SetSelectable("MSG_OK", true);
	XI_WindowShow("MSG_WINDOW", true);
	XI_WindowDisable("MSG_WINDOW", false);
	XI_WindowDisable("MAIN_WINDOW", true);

	SetCurrentNode("MSG_CANCEL");
}

void GetBortName(string _sBort, string _sBort_real, int BortNumb)
{
	if (BortNumb == 1) {_sBort = "rcannon"; _sBort_real = "cannonr"; return;}
	if (BortNumb == 2) {_sBort = "lcannon"; _sBort_real = "cannonl"; return;}
	if (BortNumb == 3) {_sBort = "fcannon"; _sBort_real = "cannonf"; return;}
	if (BortNumb == 4) {_sBort = "bcannon"; _sBort_real = "cannonb"; return;}
}

bool CSP_IsPearlOrQueenAnne(string shipName)
{
	return shipName == "BlackPearl" ||
		shipName == "CursedPearl" ||
		shipName == "CursedPearl1" ||
		shipName == "QueenAnnesRevenge";
}

void CSP_RecountFreeImprovementPoints()
{
	int i;
	int selectedCount = 0;
	for (i = 1; i < 10; i++)
	{
		if (Tune_Sheme[i]) selectedCount++;
	}
	if (selectedCount < 0) selectedCount = 0;
	if (selectedCount > iTunPoints) selectedCount = iTunPoints;
	iFreeTP = iTunPoints - selectedCount;
}

void CSP_SetCommissionedBortState(ref chr, string sBort, string sBortReal, float damageState)
{
	int maxQty = GetBortCannonsMaxQty(chr, sBort);
	for (int cannonIndex = 0; cannonIndex < maxQty; cannonIndex++)
	{
		string cannonAttr = "c" + cannonIndex;
		chr.Ship.Cannons.borts.(sBort).damages.(cannonAttr) = damageState;
		chr.Ship.Cannons.borts.(sBortReal).damages.(cannonAttr) = damageState;
	}
}

void DoBuyShip()
{
	trace("[CSP_SHIPYARD_DIAG] COMMISSION begin shipType=" + refNPCShipyard.Ship.Type + " buildStyle=" + saves + " material=" + savem + " cannonScheme=" + Ship_Sheme[10] + " price=" + iPriceOrder + " days=" + iTimeMake);
	if (!CheckAttribute(pchar, "ShipInOrder")) pchar.ShipInOrder = 0;
	AddMoneyToCharacter(pchar, -iPriceOrder);
	AddCharacterExpToSkill(pchar, "Commerce", GetShipBuyPrice(sti(refNPCShipyard.Ship.Type), refNPCShipyard) / 700.0);//опыт торговли надо начислять только за корабль? За апгрейды и пушки не начисляется?

	int iClass = RealShips[sti(refNPCShipyard.Ship.Type)].Class;//рандом брони при нажатии кнопки заказа, а не в диалоге забирания готового корабля	//в рамках антисейвлоада
	float armorSkill = stf(refNPCShipyard.reputation)/100.0;//половину брони от навыка корабела, половина остаётся рандомной
	switch (iClass)
	{
		case 6: RealShips[sti(refNPCShipyard.Ship.Type)].HullArmor = makeint(7+(frnd()+armorSkill)*1.0); break;	//от 0.12 до 1.00 от навыка корабела + от 0.00 до 1.00 от рандома
		case 5: RealShips[sti(refNPCShipyard.Ship.Type)].HullArmor = makeint(11+(frnd()+armorSkill)*1.0); break;
		case 4: RealShips[sti(refNPCShipyard.Ship.Type)].HullArmor = makeint(14+(frnd()+armorSkill)*2.0); break;
		case 3: RealShips[sti(refNPCShipyard.Ship.Type)].HullArmor = makeint(20+(frnd()+armorSkill)*2.0); break;
		case 2: RealShips[sti(refNPCShipyard.Ship.Type)].HullArmor = makeint(26+(frnd()+armorSkill)*2.0); break;
		case 1: RealShips[sti(refNPCShipyard.Ship.Type)].HullArmor = makeint(32+(frnd()+armorSkill)*2.0); break;
	}

	WaitDate("",0,0,0, 0, 35);
	Statistic_AddValue(pchar, "BuyShip", 1);
	Statistic_AddValue(Pchar, "Money_spend", iPriceOrder);

	pchar.ShipInOrder = sti(pchar.ShipInOrder) + 1;//TODO - Проверить, может ли верфист исчезнуть-умереть и добавить там проверок закрытия стройки корабля
	SaveCurrentNpcQuestDateParam(refNPCShipyard,"questTemp.ShipOrderDate");
	refNPCShipyard.questTemp.ShipOrderTime = iTimeMake;
	refNPCShipyard.questTemp.NPCid = NPC_GenerateCharacter("ShipOrder", "citiz_"+(rand(10)+1), "man", "man", rand(5)+1, refNPCShipyard.nation, -1, false);// hidden storage character
	ref chref = GetCharacter(sti(refNPCShipyard.questTemp.NPCid));
	chref.id = chref.id + "_" + chref.index; //меняем ID на оригинальный
	DeleteAttribute(chref,"ship");
	chref.ship = "";

	aref	arTo, arFrom;
	makearef(arTo, chref.ship);
	makearef(arFrom, refNPCShipyard.Ship);
	CopyAttributes(arTo, arFrom);
	RealShips[sti(chref.ship.type)].Ordered = true;
	RealShips[sti(chref.ship.type)].savesBonus = saves;
	RealShips[sti(chref.ship.type)].savemBonus = savem;
	chref.location = "";
	chref.location.group = "";
	chref.location.locator = "";

	if (Ship_Sheme[10] == -10) //если пушки не выбраны, ставим 8фт и обнуляем их кол-во
	{
		int maxQty;
		string sBort, sBort_real, attr;

		chref.Ship.Cannons.Type = CANNON_TYPE_CANNON_LBS8;//внимание - вот тут надо ставить макс калибр!!! и на обычной верфи тоже

		sBort = "rcannon";
		sBort_real = "cannonr";
		maxQty = GetBortCannonsMaxQty(chref, sBort);
		for (i = 0; i < maxQty; i++)
		{
			attr = "c" + i;
			chref.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0;//нет пушки
			chref.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0;//нет пушки
		}
		sBort = "lcannon";
		sBort_real = "cannonl";
		maxQty = GetBortCannonsMaxQty(chref, sBort);
		for (i = 0; i < maxQty; i++)
		{
			attr = "c" + i;
			chref.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0;//нет пушки
			chref.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0;//нет пушки
		}
		sBort = "fcannon";
		sBort_real = "cannonf";
		maxQty = GetBortCannonsMaxQty(chref, sBort);
		for (i = 0; i < maxQty; i++)
		{
			attr = "c" + i;
			chref.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0;//нет пушки
			chref.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0;//нет пушки
		}
		sBort = "bcannon";
		sBort_real = "cannonb";
		maxQty = GetBortCannonsMaxQty(chref, sBort);
		for (i = 0; i < maxQty; i++)
		{
			attr = "c" + i;
			chref.Ship.Cannons.borts.(sBort).damages.(attr) = 1.0;//нет пушки
			chref.Ship.Cannons.borts.(sBort_real).damages.(attr) = 1.0;//нет пушки
		}
	}
	else
	{
		int commissionedCannonIndex = makeint(Ship_Sheme[10]) + 10;
		chref.Ship.Cannons.Type = CannonTypes[commissionedCannonIndex];
		CSP_SetCommissionedBortState(chref, "rcannon", "cannonr", 0.0);
		CSP_SetCommissionedBortState(chref, "lcannon", "cannonl", 0.0);
		CSP_SetCommissionedBortState(chref, "fcannon", "cannonf", 0.0);
		CSP_SetCommissionedBortState(chref, "bcannon", "cannonb", 0.0);
		trace("[CSP_SHIPYARD_DIAG] COMMISSION installed cannonType=" + chref.Ship.Cannons.Type + " quantity=" + RealShips[sti(chref.ship.type)].CannonsQuantity);
	}

	string sTemp, sTemp2;
	if (!FState_Material && iFreeTP<iTunPoints)//запоминаем число нужных материалов и записываем их судовой журнал
	{
		int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
		sTemp = GetConvertStr("Variable_shipyard1_27", "Interface.txt");
		for (int i=1; i<10; i++)
		{
			if (i==2) continue;
			if (Tune_Sheme[i])
			{
				sTemp2 = CSP_TuningGoods[i];
				refNPCShipyard.questTemp.(sTemp2) = Tun_Mater1[i];
				sTemp += NewStr() + XI_ConvertString(sTemp2) + " - " + Tun_Mater1[i] + " " + GetConvertStr("Variable_shipyard1_28", "Interface.txt");
				sTemp2 = CSP_TuningItems[i];
				refNPCShipyard.questTemp.(sTemp2) = Tun_Mater2[i];
				sTemp += NewStr() + LanguageConvertString(idLngFile, Items[FindItem(CSP_TuningItems[i])].name) + " - " + Tun_Mater2[i] + " " + GetConvertStr("Variable_shipyard1_29", "Interface.txt");
			}
			else
			{
				sTemp2 = CSP_TuningGoods[i];
				refNPCShipyard.questTemp.(sTemp2) = 0;
				sTemp2 = CSP_TuningItems[i];
				refNPCShipyard.questTemp.(sTemp2) = 0;
			}
		}
		refNPCShipyard.questTemp.chest = sundukSum*(iTunPoints-iFreeTP);
		sTemp += NewStr() + GetConvertStr("Variable_shipyard1_30", "Interface.txt") + " " + sundukSum*(iTunPoints-iFreeTP) + " " + GetConvertStr("Variable_shipyard1_31", "Interface.txt");
	}
	else sTemp = "";

	string sTitle = "ShipOrder";
	ReOpenQuestHeader(sTitle);
	AddQuestRecordEx(sTitle, "ShipOrder", "t1");
	AddQuestUserData(sTitle, "sName", GetFullName(refNPCShipyard));
	AddQuestUserData(sTitle, "cityName", XI_ConvertString("Colony" + refNPCShipyard.city + "Gen"));
	AddQuestUserData(sTitle, "sShipType", XI_ConvertString(GetBaseShipParamFromType(sti(RealShips[sti(refNPCShipyard.ship.type)].basetype), "Name") + "Acc"));
	AddQuestUserData(sTitle, "money", iPriceOrder);
	AddQuestUserData(sTitle, "time", iTimeMake);
	AddQuestUserData(sTitle, "TuningMaterial", sTemp);

	DeleteAttribute(refNPCShipyard,"ship");
	refNPCShipyard.ship = "";
	ProcessExitCancel();
}

int CSP_GetEffectiveMaxCaliber()
{
	ref selectedShip = &RealShips[sti(refNPCShipyard.Ship.Type)];
	if (sti(selectedShip.CannonsQuantity) <= 0) return 0;
	int effectiveMax = sti(selectedShip.MaxCaliber);
	int shipClass = sti(selectedShip.Class);
	int classCap = 42;
	switch (shipClass)
	{
		case 7: classCap = 8; break;
		case 6: classCap = 12; break;
		case 5: classCap = 16; break;
		case 4: classCap = 20; break;
		case 3: classCap = 24; break;
		// Tier-2 heavy raiders and special warships can mount 36-pounders.
		case 2: classCap = 36; break;
		case 1: classCap = 36; break;
		case 0: classCap = 42; break;
	}
	if (effectiveMax > classCap) effectiveMax = classCap;
	return effectiveMax;
}

void IncShipQ(int _add)
{
	if (CurTable != "TABLE_OTHER") return;
	if (iSelected == 10)
	{
		int cannonIndex = makeint(Ship_Sheme[10]) + 10;
		int effectiveMaxCaliber = CSP_GetEffectiveMaxCaliber();
		for (int cannonStep = 0; cannonStep < CannonTypesCount; cannonStep++)
		{
			cannonIndex++;
			if (cannonIndex >= CannonTypesCount) cannonIndex = 0;
			if (cannonIndex == 0 || GetCannonCaliber(CannonTypes[cannonIndex]) <= effectiveMaxCaliber)
			{
				Ship_Sheme[10] = cannonIndex - 10;
				trace("[CSP_SHIPYARD_DIAG] CANNON validated next index=" + cannonIndex + " caliberLimit=" + effectiveMaxCaliber);
				FillShipParam();
				return;
			}
		}
		return;
	}

	int iQMAXTemp = iQMAX;
	int iFreeSPTemp = iFreeSP;
	if (iSelected == 10) //если выбран тип пушек
	{
		iFreeSPTemp = 10;//выбор типа орудий не тратит очки апгрейда
		int curCaliber = sti(RealShips[sti(refNPCShipyard.ship.type)].MaxCaliber);
		iQMAXTemp = 9;//42 фт
		switch (curCaliber)
		{
		case 8: iQMAXTemp = -8; break;
		case 10: iQMAXTemp = -6; break;
		case 12: iQMAXTemp = -4; break;
		case 16: iQMAXTemp = -2; break;
		case 20: iQMAXTemp = 0; break;
		case 24: iQMAXTemp = 2; break;
		case 28: iQMAXTemp = 4; break;
		case 32: iQMAXTemp = 6; break;
		case 36: iQMAXTemp = 8; break;
		}
	}

	if (iFreeSPTemp < _add) _add = iFreeSPTemp;
	if ((Ship_Sheme[iSelected] + _add) > iQMAXTemp)
	{
		_add = iQMAXTemp - Ship_Sheme[iSelected];
	}
	if (_add > 0)
	{
		Ship_Sheme[iSelected] += _add;
		trace("[CSP_SHIPYARD_DIAG] INCREASE row=" + iSelected + " amount=" + _add + " value=" + Ship_Sheme[iSelected]);
		if (iSelected-10) iFreeSP -= _add;//если строка 10, то свободные баллы не меняем
	}
	else return;
	FillShipParam();
	SetFormatedText("HERO_SHIPPOINTS",iFreeSP + "/" + iShipPoints);
}

void DecShipQ(int _add)
{
	if (CurTable != "TABLE_OTHER") return;
	if (iSelected == 10)
	{
		int cannonIndex = makeint(Ship_Sheme[10]) + 10;
		int effectiveMaxCaliber = CSP_GetEffectiveMaxCaliber();
		for (int cannonStep = 0; cannonStep < CannonTypesCount; cannonStep++)
		{
			cannonIndex--;
			if (cannonIndex < 0) cannonIndex = CannonTypesCount - 1;
			if (cannonIndex == 0 || GetCannonCaliber(CannonTypes[cannonIndex]) <= effectiveMaxCaliber)
			{
				Ship_Sheme[10] = cannonIndex - 10;
				trace("[CSP_SHIPYARD_DIAG] CANNON validated previous index=" + cannonIndex + " caliberLimit=" + effectiveMaxCaliber);
				FillShipParam();
				return;
			}
		}
		return;
	}
	int iQMINTemp = iQMIN;

	if (iSelected == 10) iQMINTemp = -10;//если строка 10, то нижний предел всегда максимальный

	if ((Ship_Sheme[iSelected] - _add) < iQMINTemp)
	{
		_add = Ship_Sheme[iSelected] - iQMINTemp;
	}
	if (_add > 0)
	{
		Ship_Sheme[iSelected] -= _add;
		trace("[CSP_SHIPYARD_DIAG] DECREASE row=" + iSelected + " amount=" + _add + " value=" + Ship_Sheme[iSelected]);
		if (iSelected-10) iFreeSP += _add;//если строка 10, то свободные баллы не меняем
	}
	else return;
	FillShipParam();
	SetFormatedText("HERO_SHIPPOINTS",iFreeSP + "/" + iShipPoints);
}

void HideSkillChanger()
{
	SetNodeUsing("B_SKILLUP",  false);
	SetNodeUsing("B_SKILLDown",  false);
}

void SetSkillArrows()
{
	HideSkillChanger();
	return;
	if (CurTable == "TABLE_OTHER")
	{
		if (iSelected != 10) return;//на парусах не включаем
		if (iSelected == 2) return;//на парусах не включаем
		if (iSelected == 9) return;//на калибре тоже

		int iQMINTemp = iQMIN;
		int iQMAXTemp = iQMAX;
		int iFreeSPTemp = iFreeSP;
		if (iSelected == 10) //тип пушек
		{
			iFreeSPTemp = 1;//выбор типа орудий не тратит очки апгрейда
			iQMINTemp = -10;//минимум - без пушек

			int curCaliber = sti(RealShips[sti(refNPCShipyard.ship.type)].MaxCaliber);
			iQMAXTemp = 9;//42 фт
			switch (curCaliber)
			{
			case 8: iQMAXTemp = -8; break;
			case 10: iQMAXTemp = -6; break;
			case 12: iQMAXTemp = -4; break;
			case 16: iQMAXTemp = -2; break;
			case 20: iQMAXTemp = 0; break;
			case 24: iQMAXTemp = 2; break;
			case 28: iQMAXTemp = 4; break;
			case 32: iQMAXTemp = 6; break;
			case 36: iQMAXTemp = 8; break;
			}
		}

		if (Ship_Sheme[iSelected] > iQMINTemp)
		{
			SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLDown",0, 237,298 + 26*(iSelected-1), 258, 319 + 26*(iSelected-1), 0);
			SetNodeUsing("B_SKILLDown", true);
		}

		if (iFreeSPTemp > 0 && Ship_Sheme[iSelected] < iQMAXTemp)
		{
			SendMessage(&GameInterface,"lsllllll", MSG_INTERFACE_MSG_TO_NODE,"B_SKILLUP",0, 258,298 + 26*(iSelected-1), 279, 319 + 26*(iSelected-1), 0);
			SetNodeUsing("B_SKILLUP",  true);
		}
	}
}

void FillPrice()
{
	int iShip = sti(refNPCShipyard.ship.type);
	ref rRealShip = &RealShips[iShip];
	int iTemp, i, k, m;
	string row, sGood;
	iPriceOrder = GetShipBuyPrice(iShip, refNPCShipyard);
	string stextPRICE = GetConvertStr("Variable_shipyard1_32", "Interface.txt") + " " + MakeMoneyShow(iPriceOrder, MONEY_SIGN, MONEY_DELIVER);

	if (Ship_Sheme[10] != -10.0)
	{
	iTemp = GetStoreGoodsPrice(refStore, GetCannonGoodsIdxByType(sti(refNPCShipyard.Ship.Cannons.Type)), PRICE_TYPE_BUY, pchar, sti(rRealShip.CannonsQuantity));
	stextPRICE += newStr() + GetConvertStr("Variable_cannons_price", "Interface.txt") + " " + MakeMoneyShow(iTemp, MONEY_SIGN, MONEY_DELIVER);
	iPriceOrder += iTemp;
	}
	else stextPRICE += NewStr();

	if (iFreeTP < iTunPoints)
	{
	SetNodeUsing("TABLE_TUN", !FState_Material);//показать/скрыть таблицу с материалами
	SetNodeUsing("SCROLL_TUN", !FState_Material);
	SetNodeUsing("Money_TEXT4", !FState_Material);
		if (FState_Material)
		{
			stextPRICE += NewStr() + NewStr() + NewStr();
			for (k=1;k<10;k++)
			{
				if(Tune_Sheme[k])
				{
					iTemp = Tun_Mater3[k];
					iTemp += GetStoreGoodsPrice(refStore, FindGood(CSP_TuningGoods[k]), PRICE_TYPE_BUY, pchar, Tun_Mater1[k]);
					m = FindItem(CSP_TuningItems[k]);
					if (m >= 0) iTemp += Tun_Mater2[k] * sti(Items[m].price);
					m = FindItem("chest");
					if (m >= 0) iTemp += sundukSum * sti(Items[m].price);
					stextPRICE += sAdd[k] + MakeMoneyShow(iTemp, MONEY_SIGN, MONEY_DELIVER);
					iPriceOrder += iTemp;
				}
			}
		}
		else
		{
			Table_Clear("TABLE_TUN", false, true, false);
			GameInterface.TABLE_TUN.hr.td1.str = GetConvertStr("Variable_shipyard1_33", "Interface.txt");//заголовок скрываю, но если его не объявлять первая строчка не будет показываться
			GameInterface.TABLE_TUN.hr.td2.str = GetConvertStr("Variable_shipyard1_34", "Interface.txt");

			int Tun_Mater3_sum = 0;
			int idLngFile = LanguageOpenFile("ItemsDescribe.txt");
			i = 1;
			for (k=1;k<10;k++)
			{
				if (Tune_Sheme[k])
				{
					row = "tr" + i;
					sGood = CSP_TuningGoods[k];
					GameInterface.TABLE_TUN.(row).td1.icon.group = "GOODS";
					GameInterface.TABLE_TUN.(row).td1.icon.image = sGood;
					GameInterface.TABLE_TUN.(row).td1.icon.offset = "-3, -1";
					GameInterface.TABLE_TUN.(row).td1.icon.width = 20;
					GameInterface.TABLE_TUN.(row).td1.icon.height = 20;
					GameInterface.TABLE_TUN.(row).td1.textoffset = "17,0";
					GameInterface.TABLE_TUN.(row).td1.str = XI_ConvertString(sGood);
					GameInterface.TABLE_TUN.(row).td1.scale = 0.85;
					GameInterface.TABLE_TUN.(row).td1.align = "left";
					GameInterface.TABLE_TUN.(row).td2.str = Tun_Mater1[k];
					GameInterface.TABLE_TUN.(row).td2.scale = 0.85;
					i++;
					row = "tr" + i;
					m = FindItem(CSP_TuningItems[k]);
					GameInterface.TABLE_TUN.(row).td1.icon.group = Items[m].picTexture;
					GameInterface.TABLE_TUN.(row).td1.icon.image = "itm" + Items[m].picIndex;
					GameInterface.TABLE_TUN.(row).td1.icon.offset = "-2,0";
					GameInterface.TABLE_TUN.(row).td1.icon.width = 18;
					GameInterface.TABLE_TUN.(row).td1.icon.height = 18;
					GameInterface.TABLE_TUN.(row).td1.textoffset = "17,0";
					GameInterface.TABLE_TUN.(row).td1.scale = 0.65;
					GameInterface.TABLE_TUN.(row).td1.align = "left";
					GameInterface.TABLE_TUN.(row).td1.str = LanguageConvertString(idLngFile, Items[m].name);
					GameInterface.TABLE_TUN.(row).td2.str = Tun_Mater2[k];
					GameInterface.TABLE_TUN.(row).td2.scale = 0.85;
					i++;
					Tun_Mater3_sum += Tun_Mater3[k];
				}
			}
			row = "tr" + i;
			m = FindItem("Chest");
			GameInterface.TABLE_TUN.(row).td1.icon.group = Items[m].picTexture;
			GameInterface.TABLE_TUN.(row).td1.icon.image = "itm" + Items[m].picIndex;
			GameInterface.TABLE_TUN.(row).td1.icon.offset = "-2, 0";
			GameInterface.TABLE_TUN.(row).td1.icon.width = 18;
			GameInterface.TABLE_TUN.(row).td1.icon.height = 18;
			GameInterface.TABLE_TUN.(row).td1.textoffset = "17,0";
			GameInterface.TABLE_TUN.(row).td1.scale = 0.85;
			GameInterface.TABLE_TUN.(row).td1.align = "left";
			GameInterface.TABLE_TUN.(row).td1.str = LanguageConvertString(idLngFile, Items[m].name);
			GameInterface.TABLE_TUN.(row).td2.str = sundukSum*(iTunPoints - iFreeTP);
			GameInterface.TABLE_TUN.(row).td2.scale = 0.85;

			Table_UpdateWindow("TABLE_TUN");
			SetFormatedText("Money_TEXT4", GetConvertStr("Variable_shipyard1_35", "Interface.txt") + " " + MakeMoneyShow(Tun_Mater3_sum, MONEY_SIGN, MONEY_DELIVER));
			iPriceOrder += Tun_Mater3_sum;
		}
	}
	else
	{
	SetNodeUsing("TABLE_TUN", false);//скрыть таблицу с материалами
	SetNodeUsing("SCROLL_TUN", false);
	SetNodeUsing("Money_TEXT4", false);
	}

	SetFormatedText("Money_TEXT3", stextPRICE);
	SetFormatedText("Money_TEXT2", GetConvertStr("Variable_shipyard1_36", "Interface.txt") + " " + MakeMoneyShow(iPriceOrder, MONEY_SIGN, MONEY_DELIVER));
}

void CalcTuningPrice()
{
	int iShipT = refNPCShipyard.ship.type;
	int cannonQ = sti(RealShips[iShipT].CannonsQuantity);
	int shipClass = sti(RealShips[iShipT].Class);
	int shipCapacity = sti(RealShips[iShipT].Capacity);
	float shipSpeedRate = stf(RealShips[iShipT].SpeedRate);
	float MastMulti = 1.0;
	int shipMaxCrew = sti(RealShips[iShipT].MaxCrew);
	int shipHP		= sti(RealShips[iShipT].HP);
	float shipTurnRate = stf(RealShips[iShipT].TurnRate);
	float shipWindAgainst = stf(RealShips[iShipT].WindAgainstSpeed);

	float fBasePrice = 0.0;
	if(shipClass <= 4){ fBasePrice = stf(RealShips[iShipT].Price) / 20 * MOD_SKILL_ENEMY_RATE*3;}


	sundukSum = 7-shipClass;

	int i = 1;//HP
	Tun_Mater1[i] = makeint(35 * (7-shipClass) + shipHP * 7/100);
	Tun_Mater2[i] = makeint(5 * ((7-shipClass)*3));
	Tun_Mater3[i] = makeint(shipHP * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
	i = i + 2;//MAST
	Tun_Mater1[i] = makeint((shipHP * 25/1000 + 70 * (7-shipClass)));
	/* Tun_Mater2[i] = makeint(10 * (7-shipClass));
	Tun_Mater3[i] = makeint(100 * MastMulti * MOD_SKILL_ENEMY_RATE*3 + fBasePrice); */
	i++;//Speed
	Tun_Mater1[i] = makeint((shipHP * 25/1000 + 70 * (7-shipClass)));
	Tun_Mater2[i] = makeint(5 * (7-shipClass));
	Tun_Mater3[i] = makeint(100 * shipSpeedRate * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
	i++;//Turn
	Tun_Mater1[i] = makeint((shipHP * 25/1000 + 375/shipClass+20));
	Tun_Mater2[i] = makeint((5 * (7-shipClass)+2));
	Tun_Mater3[i] = makeint(30 * shipTurnRate * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
	i++;//Wind
	Tun_Mater1[i] = makeint((shipHP * 25/1000 + 750/shipClass+20));
	Tun_Mater2[i] = makeint((6 * (7-shipClass)+1));
	Tun_Mater3[i] = makeint(300 * shipWindAgainst * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
	i++;//capacity
	Tun_Mater1[i] = makeint((35 * (7-shipClass) + shipCapacity * 7/100));
	Tun_Mater2[i] = makeint((10 * (7-shipClass)+10));
	Tun_Mater3[i] = makeint(2 * shipCapacity * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
	i++;//MaxCrew
	Tun_Mater1[i] = makeint((shipMaxCrew * 6/10 + 50 * (7-shipClass)));
	Tun_Mater2[i] = makeint((6 * (7-shipClass)+3));
	Tun_Mater3[i] =  makeint(15 * shipMaxCrew * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
	i++;//cannon
	Tun_Mater1[i] = makeint((cannonQ * 2 + 35 * (7-shipClass) - 20));
	/* Tun_Mater2[i] = makeint((7-shipClass)/2);
	if (Tun_Mater2[i] < 1) Tun_Mater2[i] = 1;
	Tun_Mater3[i] = makeint(100 * cannonQ * MOD_SKILL_ENEMY_RATE*3 + fBasePrice); */
}

void OnTableClick()
{
	string sControl = GetEventData();
	int iRow = GetEventData();
	int iColumn = GetEventData();

	//string sRow = "tr" + (iRow + 1);
	if (sControl == "TABLE_SHIPYARD")
	{
		if (!SendMessage(&GameInterface,"lsl",MSG_INTERFACE_MSG_TO_NODE, sControl, 1 )) QoLSortTable(sControl, iColumn, "string", true, 0);
		Table_UpdateWindow(sControl);
	}
}

void InitSAddMassive()
{
	string sAddString = GetConvertStr("Variable_sAdd_1", "Interface.txt");
	sAdd[1] = sAddString + " ";
	sAddString = GetConvertStr("Variable_sAdd_2", "Interface.txt");
	sAdd[3] = sAddString + " ";
	sAddString = GetConvertStr("Variable_sAdd_3", "Interface.txt");
	sAdd[4] = sAddString + " ";
	sAddString = GetConvertStr("Variable_sAdd_4", "Interface.txt");
	sAdd[5] = sAddString + " ";
	sAddString = GetConvertStr("Variable_sAdd_5", "Interface.txt");
	sAdd[6] = sAddString + " ";
	sAddString = GetConvertStr("Variable_sAdd_6", "Interface.txt");
	sAdd[7] = sAddString + " ";
	sAddString = GetConvertStr("Variable_sAdd_7", "Interface.txt");
	sAdd[8] = sAddString + " ";
	sAddString = GetConvertStr("Variable_sAdd_8", "Interface.txt");
	sAdd[9] = sAddString + " ";
}

//Смена каюты -->
void ExitChangeCABINMenu()
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

	ref refShip = GetRealShip(sti(refNPCShipyard.Ship.Type));
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
	if (sTexture != "") SetNewPicture("CHANGE_CABIN_TYPE", sTexture);

	bool isNewCabin = true;
	ref refShip = GetRealShip(sti(refNPCShipyard.Ship.Type));
	if (!isCabinforClass(sti(refShip.Class))) isNewCabin = false;

	if (isNewCabin) 
	{
		SetSelectable("CHANGE_CABIN_OK", true);
		SetFormatedText("CHANGE_CABIN_TEXT", "");
	}
	else 
	{
		SetSelectable("CHANGE_CABIN_OK", false);
		SetFormatedText("CHANGE_CABIN_TEXT", GetConvertStr("Variable_shipyard_80", "Interface.txt"));
	}
}
//Каюты, доступные разным классам при смене
bool isCabinforClass(int iClass)
{
	switch (iClass)
	{
	case 7: return false; break;									//тут только один тип, так что всегда отказ в смене
	case 6: if (iCurCabin>-1 && iCurCabin<3) return true; break; 	//0-2
	case 5: if (iCurCabin>0 && iCurCabin<4) return true; break;		//1-3
	case 4: if (iCurCabin>2 && iCurCabin<6) return true; break;		//3-5
	case 3: if (iCurCabin>3 && iCurCabin<7) return true; break; 	//4-6
	case 2: if (iCurCabin>5 && iCurCabin<9) return true; break;		//6-8
	case 1: if (iCurCabin>6 && iCurCabin<10) return true; break;	//7-9
	}
	return false;
}
string GetShipTexturesForChangeCABIN()
{
	int nLoc = FindLocation(sCabins[iCurCabin]);
	if (nLoc >= 0 && CheckAttribute(&Locations[nLoc],"image"))
	return Locations[nLoc].image;
	return "";
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
	ref refShip = GetRealShip(sti(refNPCShipyard.Ship.Type));
	refShip.CabinType = sCabins[iCurCabin];
	ExitChangeCABINMenu();
}
//Смена каюты <--

// Compatibility helpers for the CSP construction screen. They use the
// current game's native Tuning attributes so achievements and ship panels
// continue to recognize commissioned upgrades.
float CSP_SelectStatBonus(string statType, float hp, float speed, float turn, float wind, float capacity, float crew)
{
	if (statType == "bonusHP") return hp;
	if (statType == "bonusSpeed") return speed;
	if (statType == "bonusTurnRate") return turn;
	if (statType == "bonusWindAgainstSpeed") return wind;
	if (statType == "bonusCapacity") return capacity;
	if (statType == "bonusOptCrew") return crew;
	return 0.0;
}

float SelectShipsBonuses(ref rShip, string statType, int buildStyle, int materialStyle)
{
	float result = 0.0;
	switch (buildStyle)
	{
		case 0: result += CSP_SelectStatBonus(statType, 0.00, 0.05, 0.00, 0.12, -0.08, -0.04); break;
		case 1: result += CSP_SelectStatBonus(statType, 0.06, 0.00, 0.06, -0.07, 0.05, -0.05); break;
		case 2: result += CSP_SelectStatBonus(statType, -0.06, 0.07, 0.00, 0.00, -0.08, 0.07); break;
		case 3: result += CSP_SelectStatBonus(statType, 0.00, 0.00, -0.10, 0.00, 0.08, 0.08); break;
		case 4: result += CSP_SelectStatBonus(statType, 0.05, -0.03, -0.04, 0.00, 0.10, 0.06); break;
		case 5: result += CSP_SelectStatBonus(statType, 0.02, 0.02, -0.09, 0.05, 0.05, 0.02); break;
		case 6: result += CSP_SelectStatBonus(statType, 0.10, -0.08, 0.08, 0.00, -0.07, -0.05); break;
		case 7: result += CSP_SelectStatBonus(statType, 0.03, 0.00, 0.03, 0.00, 0.10, -0.10); break;
		case 8: result += CSP_SelectStatBonus(statType, -0.06, 0.04, 0.08, 0.00, -0.05, 0.04); break;
		case 9: result += CSP_SelectStatBonus(statType, 0.04, 0.00, -0.08, 0.00, 0.07, 0.00); break;
		case 10: result += CSP_SelectStatBonus(statType, -0.04, 0.07, 0.00, 0.00, 0.00, 0.05); break;
		case 11: result += CSP_SelectStatBonus(statType, 0.03, -0.05, 0.10, 0.00, 0.00, 0.00); break;
		case 12: result += CSP_SelectStatBonus(statType, 0.05, 0.00, 0.00, 0.00, 0.06, 0.03); break;
	}
	switch (materialStyle)
	{
		case 0: result += CSP_SelectStatBonus(statType, 0.00, -0.04, 0.06, 0.06, 0.00, 0.06); break;
		case 1: result += CSP_SelectStatBonus(statType, 0.00, 0.02, 0.02, -0.10, 0.06, 0.00); break;
		case 2: result += CSP_SelectStatBonus(statType, -0.10, 0.07, 0.04, 0.15, -0.08, -0.04); break;
		case 3: result += CSP_SelectStatBonus(statType, -0.03, -0.03, 0.08, 0.06, 0.05, -0.05); break;
		case 4: result += CSP_SelectStatBonus(statType, 0.04, 0.00, 0.06, 0.05, 0.00, 0.00); break;
		case 5: result += CSP_SelectStatBonus(statType, -0.07, 0.05, 0.04, -0.04, -0.04, 0.00); break;
		case 6: result += CSP_SelectStatBonus(statType, 0.05, -0.03, -0.08, 0.05, 0.05, 0.06); break;
		case 7: result += CSP_SelectStatBonus(statType, 0.10, -0.07, 0.06, -0.07, -0.08, 0.07); break;
	}
	return result;
}

void CSP_SetTuningStates(ref chr, bool cannon, bool capacity, bool speed, bool crew, bool turn, bool hull, bool wind)
{
	ref sh = GetRealShip(sti(chr.Ship.Type));
	if (hull)
	{
		sh.Tuning.HP = true;
		sh.HP = makeint(sti(sh.HP) * 1.20);
	}
	if (speed)
	{
		sh.Tuning.SpeedRate = true;
		sh.SpeedRate = stf(sh.SpeedRate) * 1.20;
	}
	if (turn)
	{
		sh.Tuning.TurnRate = true;
		sh.TurnRate = stf(sh.TurnRate) * 1.20;
	}
	if (capacity)
	{
		sh.Tuning.Capacity = true;
		sh.Capacity = makeint(sti(sh.Capacity) * 1.20);
	}
	if (crew)
	{
		sh.Tuning.MaxCrew = true;
		sh.MaxCrew = makeint(sti(sh.MaxCrew) * 1.20);
	}
	if (wind)
	{
		sh.Tuning.WindAgainst = true;
		sh.WindAgainstSpeed = stf(sh.WindAgainstSpeed) * 1.10;
	}
	if (cannon)
	{
		sh.Tuning.Cannon = true;
		if (sti(sh.MaxCaliber) < 36) sh.MaxCaliber = sti(sh.MaxCaliber) + 4;
	}
}

void CSP_SetBar(string node, float value)
{
	GameInterface.StatusLine.(node).Max = 100;
	GameInterface.StatusLine.(node).Min = 0;
	GameInterface.StatusLine.(node).Value = value;
	SendMessage(&GameInterface, "lsl", MSG_INTERFACE_MSG_TO_NODE, node, 0);
}

void CSP_SetShipQualityBars(string hp, string mast, string speed, string turn, string wind, string capacity, string crew)
{
	CSP_SetBar(hp, 50 + Ship_Sheme[1] * 5);
	CSP_SetBar(mast, 50);
	CSP_SetBar(speed, 50 + Ship_Sheme[4] * 5);
	CSP_SetBar(turn, 50 + Ship_Sheme[5] * 5);
	CSP_SetBar(wind, 50 + Ship_Sheme[6] * 5);
	CSP_SetBar(capacity, 50 + Ship_Sheme[7] * 5);
	CSP_SetBar(crew, 50 + Ship_Sheme[8] * 5);
}

string CSP_GetShipDisplayName(string baseName)
{
	string specialName = "";
	switch (baseName)
	{
		case "BlackPearl": specialName = "Black Pearl"; break;
		case "CursedPearl": specialName = "Cursed Black Pearl"; break;
		case "CursedPearl1": specialName = "Cursed Black Pearl"; break;
		case "QueenAnnesRevenge": specialName = "Queen Anne's Revenge"; break;
	}
	if (specialName != "") return specialName;
	string converted = XI_ConvertString(baseName);
	if (converted == "" || converted == baseName) return baseName;
	return converted;
}

string CSP_GetSpecialShipCanonicalName(string baseName)
{
	string canonicalName = "";
	switch (baseName)
	{
		case "BlackPearl": canonicalName = "Black Pearl"; break;
		case "CursedPearl": canonicalName = "Cursed Black Pearl"; break;
		case "CursedPearl1": canonicalName = "Cursed Black Pearl"; break;
		case "QueenAnnesRevenge": canonicalName = "Queen Anne's Revenge"; break;
	}
	return canonicalName;
}

string AddTextAboutHull(ref chr)
{
	ref sh = GetRealShip(sti(chr.Ship.Type));
	string text = "";
	if (CheckAttribute(sh, "savesBonus")) text = "Custom shipyard construction";
	if (CheckAttribute(sh, "savemBonus")) text = text + NewStr() + "Selected hull material";
	return text;
}
