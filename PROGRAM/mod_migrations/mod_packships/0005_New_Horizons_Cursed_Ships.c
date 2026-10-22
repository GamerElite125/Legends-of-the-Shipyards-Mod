void ApplyMigration(ref migrationState)
{
	ref refShip;
	int i;

	// Upgrade the already-registered Cursed Pearl for existing and new saves.
	for (i = 0; i < GetArraySize(&ShipsTypes); i++)
	{
		if (!CheckAttribute(&ShipsTypes[i], "Name")) continue;
		if (ShipsTypes[i].Name != "CursedPearl" && ShipsTypes[i].Name != "CursedPearl1") continue;
		ShipsTypes[i].HP = 6500;
		ShipsTypes[i].SP = 100;
		ShipsTypes[i].fog_trail = true;
		ShipsTypes[i].night_stealth = true;
		ShipsTypes[i].cursed_ship = true;
		ShipsTypes[i].CanEncounter = false;
		ShipsTypes[i].Type.Merchant = false;
		ShipsTypes[i].Type.War = true;
	}

	Log_Info("New Horizons Cursed Pearl attributes registered");
}
