void ApplyMigration(ref migrationState)
{
	int i;

	for (i = 0; i < SHIP_TYPES_QUANTITY; i++)
	{
		if (ShipsTypes[i].Name == "CursedPearl1")
		{
			ShipsTypes[i].Name = "CursedPearl";
		}
	}

	Log_Info("Cursed Black Pearl model-family name repaired for Steam");
}
