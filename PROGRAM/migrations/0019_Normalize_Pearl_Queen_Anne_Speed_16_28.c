void ApplyMigration(ref migrationState)
{
	int blackPearl = CSP19_FindShipType("BlackPearl");
	int cursedPearl = CSP19_FindShipType("CursedPearl");
	int queenAnne = CSP19_FindShipType("QueenAnnesRevenge");

	if (blackPearl < 0) blackPearl = CSP19_AddSpecialShip("BlackPearl", 36, 480, 400, 100, "Ships/BlackPearl1/Hull1/sail_BlackPearl_whole.tga");
	if (cursedPearl < 0) cursedPearl = CSP19_AddSpecialShip("CursedPearl", 36, 240, 90, 50, "Ships/CursedPearl1/Hull1/sail_BlackPearl_torn.tga");
	if (queenAnne < 0) queenAnne = CSP19_AddSpecialShip("QueenAnnesRevenge", 24, 398, 98, 100, "Ships/QueenAnnesRevenge1/Hull1/sail_Black_Red_torn.tga");

	CSP19_RestoreRealShips(blackPearl, cursedPearl, queenAnne);
	CSP19_NormalizeShipType(blackPearl);
	CSP19_NormalizeShipType(cursedPearl);
	CSP19_NormalizeShipType(queenAnne);
	CSP19_NormalizeOwnedSpecialShips();

	Trace("[CSP_SPECIAL_DIAG] Black Pearl, Cursed Black Pearl and Queen Anne's Revenge ship types revived.");
	Log_SetStringToLog("Black Pearl, Cursed Black Pearl and Queen Anne's Revenge restored.");
}

void CSP19_NormalizeShipType(int shipType)
{
	if (shipType < 0) return;
	ShipsTypes[shipType].HP = 6700;
	ShipsTypes[shipType].SpeedRate = 16.28;
	ShipsTypes[shipType].TurnRate = 28.79;
	ShipsTypes[shipType].WindAgainstSpeed = 4.90;
	ShipsTypes[shipType].MinCrew = 98;
	ShipsTypes[shipType].OptCrew = 416;
	ShipsTypes[shipType].MaxCrew = 416;
}

void CSP19_NormalizeOwnedSpecialShips()
{
	int i;
	for (i = 0; i < GetArraySize(&RealShips); i++)
	{
		string shipName = "";
		if (CheckAttribute(&RealShips[i], "BaseName")) shipName = RealShips[i].BaseName;
		if (!CSP19_IsSpecialName(shipName)) continue;
		RealShips[i].HP = 6700;
		RealShips[i].SpeedRate = 16.28;
		RealShips[i].TurnRate = 28.79;
		RealShips[i].WindAgainstSpeed = 4.90;
		RealShips[i].MinCrew = 98;
		RealShips[i].OptCrew = 416;
		RealShips[i].MaxCrew = 416;
	}
}

bool CSP19_IsSpecialName(string shipName)
{
	return shipName == "BlackPearl" ||
		shipName == "CursedPearl" ||
		shipName == "CursedPearl1" ||
		shipName == "QueenAnnesRevenge";
}

int CSP19_FindShipType(string shipName)
{
	int i;
	for (i = 0; i < GetArraySize(&ShipsTypes); i++)
	{
		if (CheckAttribute(&ShipsTypes[i], "Name") && ShipsTypes[i].Name == shipName) return i;
	}
	return -1;
}

int CSP19_AddSpecialShip(string shipName, int cannonCount, int maxCrew, int optCrew, int minCrew, string sailTexture)
{
	int idx = GetArraySize(&ShipsTypes);
	SetArraySize(&ShipsTypes, idx + 1);
	ref refShip = &ShipsTypes[idx];
	aref cannonl, cannonr, cannonb, cannonf;

	refShip.index = idx;
	refShip.Name = shipName;
	refShip.Soundtype = "frigate";
	refShip.Class = 2;
	refShip.Cannon = CANNON_TYPE_CANNON_LBS24;
	refShip.MaxCaliber = 24;
	refShip.Weight = Tonnes2CWT(1200);
	refShip.Capacity = 5430;
	refShip.CannonsQuantity = cannonCount;
	refShip.CannonsQuantityMin = cannonCount;
	refShip.rcannon = 16;
	refShip.lcannon = 16;
	refShip.fcannon = 2;
	refShip.bcannon = 2;
	if (cannonCount == 24)
	{
		refShip.rcannon = 10;
		refShip.lcannon = 10;
	}
	refShip.cannonr = refShip.rcannon;
	refShip.cannonl = refShip.lcannon;
	refShip.cannonf = refShip.fcannon;
	refShip.cannonb = refShip.bcannon;
	refShip.MaxCrew = maxCrew;
	refShip.OptCrew = optCrew;
	refShip.MinCrew = minCrew;
	refShip.SpeedRate = 16.28;
	refShip.TurnRate = 35.0;
	refShip.Price = 73700;
	refShip.HP = 5800;
	refShip.SP = 100;
	refShip.Type.Merchant = false;
	refShip.Type.War = true;
	refShip.CanEncounter = false;
	refShip.lowpolycrew = 20;
	refShip.Rocking.y = 0.4;
	refShip.Rocking.az = 0.035;
	refShip.WaterLine = -0.75;
	refShip.SpeedDependWeight = 0.3;
	refShip.SubSeaDependWeight = 1.0;
	refShip.TurnDependWeight = 0.3;
	refShip.TurnDependSpeed = 0.0;
	refShip.WindAgainstSpeed = 0.61;
	refShip.CabinType = "Cabin";
	refShip.AbordageLocation = "BOARDING_BIG_DECK";
	refShip.DeckType = "Big";
	refShip.InertiaAccelerationX = 5.0;
	refShip.InertiaBrakingX = 5.0;
	refShip.InertiaAccelerationY = 0.6667;
	refShip.InertiaBrakingY = 4.0;
	refShip.InertiaAccelerationZ = 5.0;
	refShip.InertiaBrakingZ = 5.0;
	refShip.Height.Bombs.Y = 2.7;
	refShip.Height.Bombs.DY = 1.35;
	refShip.Height.Grapes.Y = 5.325;
	refShip.Height.Grapes.DY = 1.575;
	refShip.Height.Knippels.Y = 19.5;
	refShip.Height.Knippels.DY = 13.5;
	refShip.Height.Balls.Y = 2.7;
	refShip.Height.Balls.DY = 1.35;
	refShip.GeraldSails.rey_b1 = 1;
	refShip.GeraldSails.rey_b2 = 1;
	refShip.GeraldSails.rey_b3 = 1;
	refShip.GeraldSails.rey_b1.vscale = 0.9;
	refShip.GeraldSails.rey_b2.vscale = 0.7;
	refShip.GeraldSails.rey_b3.vscale = 0.7;
	refShip.Track.Enable = true;
	refShip.Track1.ZStart = 0.20;
	refShip.Track1.LifeTime = 14.5;
	refShip.Track1.Width = "3.7, 3.9";
	refShip.Track1.Speed = "7.0, 8.0";
	refShip.Track2.ZStart = -0.15;
	refShip.Track2.LifeTime = 8.5;
	refShip.Track2.Width = "8.0, 9.0";
	refShip.Track2.Speed = "0.15, 0.25";
	refShip.nation.england = false;
	refShip.nation.france = false;
	refShip.nation.spain = false;
	refShip.nation.holland = false;
	refShip.nation.pirate = false;
	refShip.EmblemedSails.normalTex = sailTexture;
	refShip.icons.FirstIconPos = 4;
	refShip.icons.SecondIconPos = 5;
	refShip.modname = "my_mod_ships";
	refShip.NetGeraldSails = "";
	refShip.FireHeight = 3.0;
	refShip.Massa = 1500000.0;
	refShip.Volume = 900.0;

	ShipsTypes[idx].Cannons.Borts.cannonl = "";
	ShipsTypes[idx].Cannons.Borts.cannonr = "";
	ShipsTypes[idx].Cannons.Borts.cannonf = "";
	ShipsTypes[idx].Cannons.Borts.cannonb = "";
	makearef(cannonl, ShipsTypes[idx].Cannons.Borts.cannonl);
	makearef(cannonr, ShipsTypes[idx].Cannons.Borts.cannonr);
	makearef(cannonf, ShipsTypes[idx].Cannons.Borts.cannonf);
	makearef(cannonb, ShipsTypes[idx].Cannons.Borts.cannonb);
	CSP19_SetCannonArc(cannonf, 30.0, 0.0);
	CSP19_SetCannonArc(cannonr, 80.0, 90.0);
	CSP19_SetCannonArc(cannonb, 30.0, 180.0);
	CSP19_SetCannonArc(cannonl, 80.0, 270.0);

	return idx;
}

void CSP19_SetCannonArc(aref cannon, float zone, float direction)
{
	cannon.FireZone = Degree2Radian(zone);
	cannon.FireDir = Degree2Radian(direction);
	cannon.FireAngMin = -0.35;
	cannon.FireAngMax = 0.60;
}

void CSP19_RestoreRealShips(int blackPearl, int cursedPearl, int queenAnne)
{
	int i;
	for (i = 0; i < GetArraySize(&RealShips); i++)
	{
		string savedName = "";
		if (CheckAttribute(&RealShips[i], "BaseName")) savedName = RealShips[i].BaseName;
		if (savedName == "BlackPearl") CSP19_SetRealShipBase(i, blackPearl, "BlackPearl");
		if (savedName == "CursedPearl" || savedName == "CursedPearl1") CSP19_SetRealShipBase(i, cursedPearl, "CursedPearl");
		if (savedName == "QueenAnnesRevenge") CSP19_SetRealShipBase(i, queenAnne, "QueenAnnesRevenge");
	}
}

void CSP19_SetRealShipBase(int realShipIndex, int baseType, string baseName)
{
	if (baseType < 0) return;
	RealShips[realShipIndex].basetype = baseType;
	RealShips[realShipIndex].BaseType = baseType;
	RealShips[realShipIndex].BaseName = baseName;
	if (!CheckAttribute(&RealShips[realShipIndex], "ship.upgrades.hull")) RealShips[realShipIndex].ship.upgrades.hull = 1;
	if (!CheckAttribute(&RealShips[realShipIndex], "ship.upgrades.sails")) RealShips[realShipIndex].ship.upgrades.sails = 1;
	if (!CheckAttribute(&RealShips[realShipIndex], "ship.upgrades.cannons")) RealShips[realShipIndex].ship.upgrades.cannons = 1;
}
