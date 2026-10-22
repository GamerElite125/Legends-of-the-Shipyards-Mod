// Compatibility migration for Caribbean Legend 1.8.2.
//
// Earlier versions pointed these ships at a custom battle-icon atlas that was
// not included in the Workshop package.  Removing that optional override lets
// the game use its standard icon atlas, avoiding missing-texture errors for
// both existing saves and fresh installations.

void ApplyMigration(ref migrationState)
{
	for (int i = 0; i < GetArraySize(&ShipsTypes); i++)
	{
		ref refShip = &ShipsTypes[i];
		if (!CheckAttribute(refShip, "Name")) continue;

		if (refShip.Name == "BlackPearl" || refShip.Name == "CursedPearl" || refShip.Name == "QueenAnnesRevenge")
		{
			DeleteAttribute(refShip, "modname");
		}
	}

	InitShipHulls();
	Log_Info("Pack Ships: applied 1.8.2 compatibility migration");
}
