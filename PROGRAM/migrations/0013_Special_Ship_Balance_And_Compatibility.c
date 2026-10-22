void ApplyMigration(ref migrationState)
{
	int i;
	for (i = 0; i < GetArraySize(&ShipsTypes); i++)
	{
		if (!CheckAttribute(&ShipsTypes[i], "Name")) continue;
		string shipName = ShipsTypes[i].Name;

		if (shipName == "BlackPearl" || shipName == "CursedPearl" || shipName == "CursedPearl1")
		{
			ShipsTypes[i].Class = 2;
			ShipsTypes[i].Cannon = CANNON_TYPE_CANNON_LBS36;
			ShipsTypes[i].MaxCaliber = 36;
			ShipsTypes[i].HP = 6500;
			ShipsTypes[i].SP = 100;
			ShipsTypes[i].Capacity = 4200;
			ShipsTypes[i].MaxCrew = 360;
			ShipsTypes[i].OptCrew = 280;
			ShipsTypes[i].MinCrew = 70;
			ShipsTypes[i].SpeedRate = 11.5;
			ShipsTypes[i].TurnRate = 38.0;
			ShipsTypes[i].WindAgainstSpeed = 0.65;
			ShipsTypes[i].Price = 145000;
			ShipsTypes[i].lowpolycrew = 8;
		}

		if (shipName == "QueenAnnesRevenge")
		{
			ShipsTypes[i].Class = 2;
			ShipsTypes[i].Cannon = CANNON_TYPE_CANNON_LBS32;
			ShipsTypes[i].MaxCaliber = 32;
			ShipsTypes[i].HP = 7200;
			ShipsTypes[i].SP = 100;
			ShipsTypes[i].Capacity = 4500;
			ShipsTypes[i].MaxCrew = 400;
			ShipsTypes[i].OptCrew = 300;
			ShipsTypes[i].MinCrew = 80;
			ShipsTypes[i].SpeedRate = 10.5;
			ShipsTypes[i].TurnRate = 32.0;
			ShipsTypes[i].WindAgainstSpeed = 0.58;
			ShipsTypes[i].Price = 175000;
			ShipsTypes[i].lowpolycrew = 8;
		}

	}

	// Repair already-generated real ships and ships already owned in this save.
	for (i = 0; i < GetArraySize(&RealShips); i++)
	{
		if (!CheckAttribute(&RealShips[i], "basetype")) continue;
		int baseType = sti(RealShips[i].basetype);
		if (baseType < 0 || baseType >= GetArraySize(&ShipsTypes)) continue;
		if (!CheckAttribute(&ShipsTypes[baseType], "Name")) continue;
		string realName = ShipsTypes[baseType].Name;
		if (realName != "BlackPearl" && realName != "CursedPearl" && realName != "CursedPearl1" &&
			realName != "QueenAnnesRevenge") continue;

		RealShips[i].Class = ShipsTypes[baseType].Class;
		RealShips[i].Cannon = ShipsTypes[baseType].Cannon;
		RealShips[i].MaxCaliber = ShipsTypes[baseType].MaxCaliber;
		RealShips[i].HP = ShipsTypes[baseType].HP;
		RealShips[i].SP = ShipsTypes[baseType].SP;
		RealShips[i].Capacity = ShipsTypes[baseType].Capacity;
		RealShips[i].MaxCrew = ShipsTypes[baseType].MaxCrew;
		RealShips[i].OptCrew = ShipsTypes[baseType].OptCrew;
		RealShips[i].MinCrew = ShipsTypes[baseType].MinCrew;
		RealShips[i].SpeedRate = ShipsTypes[baseType].SpeedRate;
		RealShips[i].TurnRate = ShipsTypes[baseType].TurnRate;
		RealShips[i].WindAgainstSpeed = ShipsTypes[baseType].WindAgainstSpeed;
	}

	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		ref chr = GetCharacter(i);
		if (!CheckAttribute(chr, "Ship.Type")) continue;
		int realType = sti(chr.Ship.Type);
		if (realType < 0 || realType >= GetArraySize(&RealShips)) continue;
		if (!CheckAttribute(&RealShips[realType], "basetype")) continue;
		int characterBaseType = sti(RealShips[realType].basetype);
		if (characterBaseType < 0 || characterBaseType >= GetArraySize(&ShipsTypes)) continue;
		if (!CheckAttribute(&ShipsTypes[characterBaseType], "Name")) continue;
		string characterShipName = ShipsTypes[characterBaseType].Name;
		if (characterShipName != "BlackPearl" && characterShipName != "CursedPearl" && characterShipName != "CursedPearl1" &&
			characterShipName != "QueenAnnesRevenge") continue;

		chr.Ship.HP = RealShips[realType].HP;
		chr.Ship.SP = RealShips[realType].SP;
	}

	Log_Info("Special ship balance, cannon caliber and crew compatibility applied");
}
