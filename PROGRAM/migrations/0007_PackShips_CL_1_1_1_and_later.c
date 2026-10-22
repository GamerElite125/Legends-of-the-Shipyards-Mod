// Pack Ships compatibility migration for Caribbean Legend 1.1.1 and later.
//
// The filename is intentionally retained as migration 0004 compatibility
// with already-published Workshop builds. The implementation itself is not
// tied to Caribbean Legend 1.8.2.
//
// Older Pack Ships builds could assign these vessels to a custom battle-icon
// atlas that was not always present. Removing only that optional override
// lets supported Caribbean Legend versions use their standard/fallback icons.

void ApplyMigration(ref migrationState)
{
	int shipCount = GetArraySize(&ShipsTypes);

	for (int i = 0; i < shipCount; i++)
	{
		ref refShip = &ShipsTypes[i];
		if (!CheckAttribute(refShip, "Name")) continue;

		string shipName = refShip.Name;
		if (shipName == "BlackPearl" ||
			shipName == "CursedPearl" ||
			shipName == "QueenAnnesRevenge")
		{
			if (CheckAttribute(refShip, "modname"))
			{
				DeleteAttribute(refShip, "modname");
			}
		}
	}

	Log_Info("Pack Ships: applied Caribbean Legend 1.1.1+ compatibility migration");
}
