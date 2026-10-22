void ApplyMigration(ref migrationState)
{
	int i;
	for (i = 0; i < GetArraySize(&ShipsTypes); i++)
	{
		if (!CheckAttribute(&ShipsTypes[i], "Name")) continue;
		if (ShipsTypes[i].Name != "CursedDutchman" &&
			ShipsTypes[i].Name != "FlyingDutchman" &&
			ShipsTypes[i].Name != "Flyingdutchman" &&
			ShipsTypes[i].Name != "Flyingdutchman_o") continue;

		ShipsTypes[i].CanBuy = false;
		ShipsTypes[i].CanEncounter = false;
		ShipsTypes[i].CanGenerate = false;
		ShipsTypes[i].CanBeQuestShip = false;
	}

	// Old saves can retain generated real-ship entries after their registering
	// migration is removed.  Detach those entries before the engine tries to
	// load models which are intentionally no longer in this Workshop item.
	for (i = 0; i < MAX_CHARACTERS; i++)
	{
		if (!CheckAttribute(&Characters[i], "Ship.Type")) continue;
		int realType = sti(Characters[i].Ship.Type);
		if (realType < 0 || realType == SHIP_NOTUSED) continue;
		if (!CheckAttribute(&RealShips[realType], "BaseType")) continue;
		int baseType = sti(RealShips[realType].BaseType);
		if (!CheckAttribute(&ShipsTypes[baseType], "Name")) continue;
		if (ShipsTypes[baseType].Name != "CursedDutchman" &&
			ShipsTypes[baseType].Name != "FlyingDutchman" &&
			ShipsTypes[baseType].Name != "Flyingdutchman" &&
			ShipsTypes[baseType].Name != "Flyingdutchman_o") continue;

		DeleteAttribute(&Characters[i], "Ship");
		Characters[i].Ship.Type = SHIP_NOTUSED;
	}

	DeleteAttribute(pchar, "SubmergeDutchman");
	DeleteAttribute(pchar, "KrakenAttack");
	DeleteAttribute(pchar, "questTemp.Dutchman");
	Log_Info("Flying Dutchman ships and legacy command state removed");
}
