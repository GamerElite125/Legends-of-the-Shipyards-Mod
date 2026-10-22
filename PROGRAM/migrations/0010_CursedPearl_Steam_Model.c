void ApplyMigration(ref migrationState)
{
	int i;
	int realType;
	int baseType;

	for (i = 0; i < SHIP_TYPES_QUANTITY; i++)
	{
		if (ShipsTypes[i].Name == "CursedPearl")
		{
			ShipsTypes[i].Name = "CursedPearl1";
		}
	}

	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (!CheckAttribute(&Characters[i], "Ship.Type")) continue;
		realType = sti(Characters[i].Ship.Type);
		if (realType < 0 || realType == SHIP_NOTUSED) continue;
		if (!CheckAttribute(&RealShips[realType], "BaseType")) continue;
		baseType = sti(RealShips[realType].BaseType);

		if (ShipsTypes[baseType].Name == "CursedPearl1")
		{
			Characters[i].Ship.Name = "Cursed Black Pearl";
		}
	}
	Log_Info("Cursed Black Pearl switched to the Steam-compatible CursedPearl1 sea model");
}
