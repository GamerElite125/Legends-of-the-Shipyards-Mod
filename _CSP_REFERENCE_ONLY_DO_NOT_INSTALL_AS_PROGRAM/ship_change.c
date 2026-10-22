// Gregg: Debug — смена корабля с ручным набором команды, её качеством и выбором орудий.
// Слева грид всех классов кораблей (ShipsTypes[]), справа превью (картинка + статы),
// сверху — селектор своего корабля (свой + компаньоны), который будет заменён.
// Клик по классу открывает окно: команда + качество + орудия + «Применить».
// Запускается из меню отладчика (F11) кнопкой «Сменить корабль».

int  nCurScrollNumS = 0;   // индекс в скролле своих кораблей
int  iCurCompanion  = 0;   // индекс персонажа-владельца заменяемого корабля
int  iCurShipType   = 0;   // выбранный базовый тип из таблицы (индекс ShipsTypes[])
string sCurShipRow;
int  gQualityTier   = 1;   // 0 новобранцы, 1 опытные, 2 ветераны
int  gCannIdx       = 0;   // 0 = орудия как сгенерировано; 1..gCannCount = товар-орудие
int  gCannCount     = 0;

void InitInterface(string iniName)
{
	FillShipsScroll();
	BuildCannonList();
	SendMessage(&GameInterface, "ls", MSG_INTERFACE_INIT, iniName);
	FillShipTypesTable();

	GameInterface.title = "titleShipChange";

	SetEventHandler("exitCancel", "ProcessCancelExit", 0);
	SetEventHandler("evntDoPostExit", "DoPostExit", 0);
	SetEventHandler("ievnt_command", "ProcCommand", 0);
	SetEventHandler("TableSelectChange", "TableSelectChange", 0);
	SetEventHandler("ShowItemInfo", "ShowItemInfo", 0);
	SetEventHandler("frame", "ProcessFrame", 1);
	SetEventHandler("UnShowWindow", "UnShowWindow", 0);
}

void IDoExit(int exitCode)
{
	DelEventHandler("exitCancel", "ProcessCancelExit");
	DelEventHandler("evntDoPostExit", "DoPostExit");
	DelEventHandler("ievnt_command", "ProcCommand");
	DelEventHandler("TableSelectChange", "TableSelectChange");
	DelEventHandler("ShowItemInfo", "ShowItemInfo");
	DelEventHandler("frame", "ProcessFrame");
	DelEventHandler("UnShowWindow", "UnShowWindow");

	interfaceResultCommand = exitCode;
	EndCancelInterface(true);
}

void ProcessCancelExit()
{
	IDoExit(RC_INTERFACE_QUICK_SAVE);   // назад в игру
}

void DoPostExit()
{
	int exitCode = GetEventData();
	IDoExit(exitCode);
}

// -------- служебное --------

void FillShipsScroll()
{
	FillScrollImageWithCompanionShips("SHIPS_SCROLL", COMPANION_MAX);
	if(!CheckAttribute(&GameInterface, "SHIPS_SCROLL.current")) GameInterface.SHIPS_SCROLL.current = 0;
}

// список товаров-орудий (у них есть атрибут CannonIdx) для селектора в окне применения
void BuildCannonList()
{
	int i, n = 0;
	string key;
	for (i = 0; i < GOODS_QUANTITY; i++)
	{
		if (!CheckAttribute(Goods[i], "CannonIdx")) continue;
		n = n + 1;
		key = "c" + n;
		GameInterface.cannlist.(key) = i;
	}
	gCannCount = n;
}

int CannGoodAt(int idx)
{
	if (idx <= 0) return -1;
	string key = "c" + idx;
	return sti(GameInterface.cannlist.(key));
}

void FillShipTypesTable()
{
	string row;
	int t, n;
	string attributeName = "pic" + (nCurScrollNumS + 1);
	iCurCompanion = GameInterface.SHIPS_SCROLL.(attributeName).companionIndex;
	String companionId = Characters[iCurCompanion].Id;
	ref refCharacter = characterFromID(companionId);

	SetNewPicture("CHARACTER_PICTURE", "interfaces\portraits\256\face_" + refCharacter.FaceId + ".tga");
	SetFormatedText("SHIP_NAME", XI_ConvertString(RealShips[sti(refCharacter.Ship.Type)].BaseName) + " '" + refCharacter.Ship.Name + "'");
	SetFormatedText("COMP_NAME", GetFullName(refCharacter));

	Table_Clear("SHIPS_TABLE_LIST", false, true, false);
	GameInterface.SHIPS_TABLE_LIST.hr.td1.str = GetConvertStrWithReplace("ShipChange_ColShip", "Interface.txt", "#space#", " ");
	GameInterface.SHIPS_TABLE_LIST.hr.td2.str = GetConvertStrWithReplace("ShipChange_ColClass", "Interface.txt", "#space#", " ");
	GameInterface.SHIPS_TABLE_LIST.hr.td3.str = GetConvertStrWithReplace("ShipChange_ColCrew", "Interface.txt", "#enter#", "\n");
	GameInterface.SHIPS_TABLE_LIST.hr.td4.str = GetConvertStrWithReplace("ShipChange_ColCargo", "Interface.txt", "#space#", " ");

	for (t = 0, n = 1; t < SHIP_TYPES_QUANTITY; t++)
	{
		if (!CheckAttribute(ShipsTypes[t], "Name")) continue;
		if (ShipsTypes[t].Name == "") continue;

		row = "tr" + n;
		GameInterface.SHIPS_TABLE_LIST.(row).index = t;
		GameInterface.SHIPS_TABLE_LIST.(row).td1.str = XI_ConvertString(ShipsTypes[t].Name);
		GameInterface.SHIPS_TABLE_LIST.(row).td1.align = "left";
		GameInterface.SHIPS_TABLE_LIST.(row).td1.textoffset = "4,0";
		GameInterface.SHIPS_TABLE_LIST.(row).td2.str = ShipsTypes[t].Class;
		GameInterface.SHIPS_TABLE_LIST.(row).td3.str = ShipsTypes[t].OptCrew + " / " + ShipsTypes[t].MaxCrew;
		GameInterface.SHIPS_TABLE_LIST.(row).td4.str = ShipsTypes[t].Capacity;
		n++;
	}
	GameInterface.SHIPS_TABLE_LIST.select = 0;
	Table_UpdateWindow("SHIPS_TABLE_LIST");

	RefreshPreview();
}

// правая панель: картинка + подробные статы выбранного класса
void RefreshPreview()
{
	int t = iCurShipType;
	if (!CheckAttribute(ShipsTypes[t], "Name")) return;
	string sShip = ShipsTypes[t].Name;
	SetNewPicture("SHIP_PREVIEW_PIC", InterfaceStates.InterfaceShipString + sShip + ".dds");

	string s = XI_ConvertString(sShip) + NewStr() + NewStr();
	s = s + GetConvertStrWithReplace("ShipStat_Class", "Interface.txt", "#space#", " ") + ShipsTypes[t].Class + NewStr();
	s = s + GetConvertStrWithReplace("ShipStat_Crew", "Interface.txt", "#space#", " ") + ShipsTypes[t].OptCrew + " / " + ShipsTypes[t].MaxCrew + " / " + ShipsTypes[t].MinCrew + NewStr();
	s = s + GetConvertStrWithReplace("ShipStat_Cargo", "Interface.txt", "#space#", " ") + ShipsTypes[t].Capacity + NewStr();
	if (CheckAttribute(ShipsTypes[t], "HP"))        s = s + GetConvertStrWithReplace("ShipStat_HP", "Interface.txt", "#space#", " ") + ShipsTypes[t].HP + NewStr();
	if (CheckAttribute(ShipsTypes[t], "SP"))        s = s + GetConvertStrWithReplace("ShipStat_Sail", "Interface.txt", "#space#", " ") + ShipsTypes[t].SP + NewStr();
	if (CheckAttribute(ShipsTypes[t], "CannonsQuantity") && CheckAttribute(ShipsTypes[t], "MaxCaliber")) s = s + GetConvertStrWithReplace("ShipStat_Guns", "Interface.txt", "#space#", " ") + XI_ConvertString("caliber" + ShipsTypes[t].MaxCaliber) + " / " + ShipsTypes[t].CannonsQuantity + NewStr();
	if (CheckAttribute(ShipsTypes[t], "SpeedRate")) s = s + GetConvertStrWithReplace("ShipStat_Speed", "Interface.txt", "#space#", " ") + ShipsTypes[t].SpeedRate + " / " + ShipsTypes[t].TurnRate + NewStr();
	if (CheckAttribute(ShipsTypes[t], "Price"))     s = s + GetConvertStrWithReplace("ShipStat_Price", "Interface.txt", "#space#", " ") + ShipsTypes[t].Price + NewStr();
	s = s + "\n\n" + GetConvertStr(sShip, "ShipsDescribe.txt");
	SetFormatedText("SHIP_PREVIEW_INFO", s);
}

// -------- окно применения корабля --------

void ShowWindow(int WindowNum)
{
	switch (WindowNum)
	{
	case 0:
		XI_WindowDisable("MAIN_WINDOW", false);
		XI_WindowDisable("APPLY_WINDOW", true);
		XI_WindowShow("APPLY_WINDOW", false);
		SetCurrentNode("MAIN_BACK");
		break;
	case 1:
		XI_WindowDisable("MAIN_WINDOW", true);
		XI_WindowDisable("APPLY_WINDOW", false);
		XI_WindowShow("APPLY_WINDOW", true);
		SetCurrentNode("TG_CANCEL_BUTTON");
		break;
	}
}

void UnShowWindow()
{
	ShowWindow(0);
}

string QualityName(int tier)
{
	if (tier == 0) return XI_ConvertString("ShipQ_Green");
	if (tier == 2) return XI_ConvertString("ShipQ_Vet");
	return XI_ConvertString("ShipQ_Seasoned");
}

void RefreshShipDesc()
{
	string sShip = ShipsTypes[iCurShipType].Name;
	// качество — первой строкой (всегда видно), описание ниже (длинное может обрезаться снизу)
	SetFormatedText("TG_GOODS_INFO", XI_ConvertString("Sailors") + ": " + QualityName(gQualityTier));
}

void RefreshCannLabel()
{
	string s = GetConvertStrWithReplace("ShipCann_Label", "Interface.txt", "#space#", " ");
	if (gCannIdx <= 0) s = s + GetConvertStrWithReplace("ShipCann_AsIs", "Interface.txt", "#space#", " ");
	else s = s + XI_ConvertString(Goods[CannGoodAt(gCannIdx)].name);
	SetFormatedText("CANN_LABEL", s);
}

void ShowShipInfo()
{
	string sShip = ShipsTypes[iCurShipType].Name;
	gCannIdx = 0;   // орудия по умолчанию — как сгенерировано
	SetNewPicture("TG_GOODS_PICTURE", InterfaceStates.InterfaceShipString + sShip + ".dds");
	SetFormatedText("TG_GOODS_CAPTION", XI_ConvertString(sShip));
	SetFormatedText("SC_CREW_LABEL", GetConvertStrWithReplace("ShipChange_CrewLabel", "Interface.txt", "#space#", " "));
	RefreshShipDesc();
	RefreshCannLabel();
	GameInterface.TG_EDIT.str = sti(ShipsTypes[iCurShipType].OptCrew);
}

void ShowItemInfo()
{
	ShowWindow(1);
	ShowShipInfo();
}

void OnAddBtnClick(int _add)
{
	int iNum = MakeInt(GameInterface.TG_EDIT.str) + _add;
	if (iNum < 0) iNum = 0;
	int cap = sti(ShipsTypes[iCurShipType].MaxCrew);
	if (iNum > cap) iNum = cap;
	GameInterface.TG_EDIT.str = iNum;
}

// установка выбранных орудий на корабль в максимум (бесплатно); при «как есть» — no-op
void DbgApplyCannons(ref targetChar)
{
	if (gCannIdx <= 0) return;                     // «как сгенерировано» — не трогаем
	if (sti(targetChar.Ship.Type) == SHIP_NOTUSED) return;

	int good = CannGoodAt(gCannIdx);
	int cannonType = CANNON_TYPE_NONECANNON;
	if (CheckAttribute(Goods[good], "CannonIdx")) cannonType = sti(Goods[good].CannonIdx);
	if (cannonType == CANNON_TYPE_NONECANNON) return;

	// вернуть инстанс к номинальному классовому максимуму (снять случайный дефицит GenerateShipExt)
	ref rReal = GetRealShip(sti(targetChar.Ship.Type));
	ref rBase = GetShipByType(sti(rReal.BaseType));
	rReal.rcannon = sti(rBase.rcannon);  rReal.cannonr = sti(rBase.rcannon);
	rReal.lcannon = sti(rBase.lcannon);  rReal.cannonl = sti(rBase.lcannon);
	rReal.fcannon = sti(rBase.fcannon);  rReal.cannonf = sti(rBase.fcannon);
	rReal.bcannon = sti(rBase.bcannon);  rReal.cannonb = sti(rBase.bcannon);
	rReal.CannonsQuantity = sti(rReal.CannonsQuantityMax);

	targetChar.Ship.Cannons.Type = cannonType;         // тип орудий на весь корабль
	targetChar.Ship.Cannons.Charge.Type = GOOD_BALLS;  // заряд — ядра
	ResetShipCannonsDamages(targetChar);               // все борта — в полный комплект
	RecalculateCargoLoad(targetChar);                  // орудия — вес трюма
}

// ставит выбранный корабль персонажу, набирает команду и её качество
void DbgChangeShip(ref targetChar, int shipType, int crewCount, int qualityTier)
{
	int qExp, qMorale;
	if (qualityTier == 0)      { qExp = 15; qMorale = 40;  }   // green
	else if (qualityTier == 2) { qExp = 95; qMorale = 100; }   // veteran
	else                       { qExp = 55; qMorale = 70;  }   // seasoned

	targetChar.Ship.Type = GenerateShipTop(shipType, true, targetChar);   // квест/топ-генератор: полный комплект пушек, номинальные статы (совпадают с превью); ставит QuestShip
	if (sti(targetChar.Ship.Type) == SHIP_NOTUSED) { Trace("DbgChangeShip: нет свободного слота корабля"); return; }
	SetRandomNameToShip(targetChar);
	SetBaseShipData(targetChar);                 // HP/SP/пушки/товары; команду/опыт/мораль сеет только если их нет

	SetCrewQuantityFull(targetChar);             // сброс раненых + команда в максимум
	SetCrewQuantity(targetChar, crewCount);      // затем точная численность (сама зажимается в максимум)

	targetChar.Ship.Crew.Exp.Sailors   = qExp;   // качество: опыт 1..100
	targetChar.Ship.Crew.Exp.Cannoners = qExp;
	targetChar.Ship.Crew.Exp.Soldiers  = qExp;
	targetChar.Ship.Crew.Morale = qMorale;       // мораль 0..100

	DbgApplyCannons(targetChar);                 // орудия по выбору (если задан)
	// Примечание: видимая 3D-модель игрока в море обновится при следующем перезаходе (reload).
}

// -------- команды --------

void ProcCommand()
{
	string comName = GetEventData();
	string nodName = GetEventData();

	String companionId = Characters[iCurCompanion].Id;
	ref refCharacter = characterFromID(companionId);
	int iNum;

	switch(nodName)
	{
		case "TG_ADD_ALL_BUTTON":    OnAddBtnClick(50);   break;
		case "TG_ADD_BUTTON":        OnAddBtnClick(10);   break;
		case "TG_REMOVE_ALL_BUTTON": OnAddBtnClick(-50);  break;
		case "TG_REMOVE_BUTTON":     OnAddBtnClick(-10);  break;

		case "Q_GREEN":
			if(comName == "click" || comName == "activate") { gQualityTier = 0; RefreshShipDesc(); }
			break;
		case "Q_SEASONED":
			if(comName == "click" || comName == "activate") { gQualityTier = 1; RefreshShipDesc(); }
			break;
		case "Q_VET":
			if(comName == "click" || comName == "activate") { gQualityTier = 2; RefreshShipDesc(); }
			break;

		case "CANN_LEFT":
			if(comName == "click" || comName == "activate") { gCannIdx = gCannIdx - 1; if(gCannIdx < 0) gCannIdx = gCannCount; RefreshCannLabel(); }
			break;
		case "CANN_RIGHT":
			if(comName == "click" || comName == "activate") { gCannIdx = gCannIdx + 1; if(gCannIdx > gCannCount) gCannIdx = 0; RefreshCannLabel(); }
			break;

		case "TG_CANCEL_BUTTON":
			if(comName == "click" || comName == "activate") UnShowWindow();
			break;

		case "TG_OK_BUTTON":
			if(comName == "click" || comName == "activate")
			{
				iNum = sti(GameInterface.TG_EDIT.str);
				if (iNum < 0) iNum = 0;
				DbgChangeShip(refCharacter, iCurShipType, iNum, gQualityTier);
				UnShowWindow();
				FillShipsScroll();
				FillShipTypesTable();
			}
			break;
	}
}

void TableSelectChange()
{
	string sControl = GetEventData();
	int iSelected = GetEventData();
	string sRow = "tr" + (iSelected);

	if (sControl == "SHIPS_TABLE_LIST")
	{
		iCurShipType = sti(GameInterface.SHIPS_TABLE_LIST.(sRow).index);
		sCurShipRow = sRow;
		RefreshPreview();
	}
}

void ProcessFrame()
{
	if (GetCurrentNode() == "SHIPS_SCROLL")
	{
		if (sti(GameInterface.SHIPS_SCROLL.current) != nCurScrollNumS)
		{
			nCurScrollNumS = sti(GameInterface.SHIPS_SCROLL.current);
			FillShipTypesTable();
		}
	}
}
