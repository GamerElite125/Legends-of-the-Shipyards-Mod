void ApplyMigration(ref migrationState)
{
	int i;
	for (i = 0; i < GetArraySize(&ShipsTypes); i++)
	{
		if (!CheckAttribute(&ShipsTypes[i], "Name")) continue;
		string shipName = ShipsTypes[i].Name;

		if (shipName == "BlackPearl")
		{
			CSP_Beta14Pearl(&ShipsTypes[i], false);
			ShipsTypes[i].Walk = "BlackPearl";
		}
		if (shipName == "CursedPearl" || shipName == "CursedPearl1")
		{
			CSP_Beta14Pearl(&ShipsTypes[i], true);
			ShipsTypes[i].Walk = "BlackPearl";
		}
		if (shipName == "QueenAnnesRevenge")
		{
			ShipsTypes[i].Walk = "QueenAnnesRevenge";
			ShipsTypes[i].CannonsQuantity = 44;
			ShipsTypes[i].CannonsQuantityMin = 44;
			ShipsTypes[i].fcannon = 2;
			ShipsTypes[i].bcannon = 2;
			ShipsTypes[i].lcannon = 20;
			ShipsTypes[i].rcannon = 20;
			ShipsTypes[i].cannonf = 2;
			ShipsTypes[i].cannonb = 2;
			ShipsTypes[i].cannonl = 20;
			ShipsTypes[i].cannonr = 20;
			ShipsTypes[i].WaterLine = -0.7;
			ShipsTypes[i].lowpolycrew = 24;
			ShipsTypes[i].EmblemedSails.normalTex = "Ships\Sails\sail_torn_red_qar.tga";
		}
	}

	Log_Info("Beta 14 special ship models, cannon layouts, walk paths and abilities restored");
}

void CSP_Beta14Pearl(ref refShip, bool cursed)
{
	refShip.CannonsQuantity = 36;
	refShip.CannonsQuantityMin = 36;
	refShip.fcannon = 2;
	refShip.bcannon = 2;
	refShip.lcannon = 16;
	refShip.rcannon = 16;
	refShip.cannonf = 2;
	refShip.cannonb = 2;
	refShip.cannonl = 16;
	refShip.cannonr = 16;
	refShip.WaterLine = -0.7;
	refShip.lowpolycrew = 24;
	refShip.DeckTexturesDir = "BlackPearl";
	if (cursed)
	{
		refShip.EmblemedSails.normalTex = "Ships\Sails\sail_BlackPearl_torn.tga";
		refShip.fog_trail = true;
	}
	else
	{
		refShip.EmblemedSails.normalTex = "Ships\Sails\sail_BlackPearl_whole.tga";
	}
}
