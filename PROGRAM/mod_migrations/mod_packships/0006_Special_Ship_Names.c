void ApplyMigration(ref migrationState)
{
	for (int i = 0; i < MAX_CHARACTERS; i++)
	{
		if (!CheckAttribute(&Characters[i], "Ship.Type")) continue;
		int realType = sti(Characters[i].Ship.Type);
		if (realType < 0 || realType == SHIP_NOTUSED) continue;
		if (!CheckAttribute(&RealShips[realType], "BaseType")) continue;
		int baseType = sti(RealShips[realType].BaseType);
		if (!CheckAttribute(&ShipsTypes[baseType], "Name")) continue;

		switch (ShipsTypes[baseType].Name)
		{
			case "BlackPearl":
				Characters[i].Ship.Name = "Black Pearl";
			break;
			case "CursedPearl":
				Characters[i].Ship.Name = "Cursed Black Pearl";
			break;
			case "CursedPearl1":
				Characters[i].Ship.Name = "Cursed Black Pearl";
			break;
			case "QueenAnnesRevenge":
				Characters[i].Ship.Name = "Queen Anne's Revenge";
			break;
		}
	}
	Log_Info("Special ship canonical names applied");
}
