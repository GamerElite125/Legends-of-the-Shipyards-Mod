void ApplyMigration(ref migrationState)
{
	int i;
	for (i = 0; i < GetArraySize(&ShipsTypes); i++)
	{
		if (!CheckAttribute(&ShipsTypes[i], "Name")) continue;
		string shipName = ShipsTypes[i].Name;
		if (!CSP_IsSpecialSeaShip(shipName)) continue;
		CSP_SetCLWakeTracks(&ShipsTypes[i]);
		CSP_SetCLCannonBorts(&ShipsTypes[i]);
	}

	for (i = 0; i < GetArraySize(&RealShips); i++)
	{
		if (!CheckAttribute(&RealShips[i], "BaseType")) continue;
		int baseType = sti(RealShips[i].BaseType);
		if (baseType < 0 || baseType >= GetArraySize(&ShipsTypes)) continue;
		if (!CheckAttribute(&ShipsTypes[baseType], "Name")) continue;
		if (!CSP_IsSpecialSeaShip(ShipsTypes[baseType].Name)) continue;
		CSP_SetCLWakeTracks(&RealShips[i]);
		CSP_SetCLCannonBorts(&RealShips[i]);
	}

	Log_Info("Special ship wake tracks and immersion safety applied");
}

bool CSP_IsSpecialSeaShip(string shipName)
{
	return shipName == "BlackPearl" || shipName == "CursedPearl" ||
		shipName == "CursedPearl1" || shipName == "QueenAnnesRevenge";
}

void CSP_SetCLWakeTracks(ref ship)
{
	ship.Track.Enable = true;
	ship.Track1.ZStart = 0.25;
	ship.Track1.LifeTime = 9.5;
	ship.Track1.Width = "7.0, 7.5";
	ship.Track1.Speed = "8.0, 9.0";
	ship.Track2.ZStart = -0.15;
	ship.Track2.LifeTime = 7.5;
	ship.Track2.Width = "8.0, 10.0";
	ship.Track2.Speed = "1.0, 2.0";
}

void CSP_SetCLCannonBorts(ref ship)
{
	aref cannonl, cannonr, cannonf, cannonb;
	ship.Cannons.Borts.cannonl = "";
	ship.Cannons.Borts.cannonr = "";
	ship.Cannons.Borts.cannonf = "";
	ship.Cannons.Borts.cannonb = "";
	makearef(cannonl, ship.Cannons.Borts.cannonl);
	makearef(cannonr, ship.Cannons.Borts.cannonr);
	makearef(cannonf, ship.Cannons.Borts.cannonf);
	makearef(cannonb, ship.Cannons.Borts.cannonb);

	cannonf.qty = sti(ship.cannonf);
	cannonf.FireZone = Degree2Radian(30.0);
	cannonf.FireDir = Degree2Radian(0.0);
	cannonf.FireAngMin = -0.35;
	cannonf.FireAngMax = 0.60;

	cannonr.qty = sti(ship.cannonr);
	cannonr.FireZone = Degree2Radian(80.0);
	cannonr.FireDir = Degree2Radian(90.0);
	cannonr.FireAngMin = -0.35;
	cannonr.FireAngMax = 0.60;

	cannonb.qty = sti(ship.cannonb);
	cannonb.FireZone = Degree2Radian(30.0);
	cannonb.FireDir = Degree2Radian(180.0);
	cannonb.FireAngMin = -0.35;
	cannonb.FireAngMax = 0.60;

	cannonl.qty = sti(ship.cannonl);
	cannonl.FireZone = Degree2Radian(80.0);
	cannonl.FireDir = Degree2Radian(270.0);
	cannonl.FireAngMin = -0.35;
	cannonl.FireAngMax = 0.60;
}
