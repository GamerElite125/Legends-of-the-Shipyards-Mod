void ApplyMigration(ref migrationState)
{
	int i;
	for (i = 0; i < GetArraySize(&RealShips); i++)
	{
		if (!CheckAttribute(&RealShips[i], "BaseType")) continue;
		int baseType = sti(RealShips[i].BaseType);
		if (baseType < 0 || baseType >= GetArraySize(&ShipsTypes)) continue;
		if (!CheckAttribute(&ShipsTypes[baseType], "Name")) continue;
		string specialName = ShipsTypes[baseType].Name;
		if (specialName == "BlackPearl" ||
			specialName == "CursedPearl" ||
			specialName == "CursedPearl1" ||
			specialName == "QueenAnnesRevenge")
		{
			RealShips[i].BaseName = specialName;
		}
	}
	Trace("[CSP_SPECIAL_DIAG] Migration 0018 applied: fleet portraits and special-command state repaired.");
	Log_SetStringToLog("CSP special-ship interface and command compatibility updated.");
}
