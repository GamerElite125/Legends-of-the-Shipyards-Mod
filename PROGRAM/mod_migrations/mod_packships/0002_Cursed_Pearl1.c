

void ApplyMigration(ref migrationState) {
	Log_Info("migration 2");
	trace("migration 2");
	
	
	
	ref refShip;


	// Важно, вставляем новый корабль.
	refShip = InsertNewShip();

    refShip.Name            			= "CursedPearl";
	refship.Soundtype					= "frigate";
	refShip.Class						= 2;
	refShip.Cannon          			= CANNON_TYPE_CANNON_LBS24;
	refShip.MaxCaliber      			= 24;
	refShip.Weight						= Tonnes2CWT(1200);
	refShip.Capacity        			= 5430;
	refShip.CannonsQuantity				= 36;	// 4,4,4,2	4,4,4,2
	refShip.CannonsQuantityMin			= 36;
	refShip.rcannon 					= 16;
	refShip.lcannon 					= 16;
	refShip.fcannon 					= 2;
	refShip.bcannon 					= 2;	
	refShip.MaxCrew         			= 240;
	refShip.OptCrew         			= 90;
	refShip.MinCrew         			= 50;
	
	refShip.SpeedRate       			= 8.20;
	refShip.TurnRate        			= 35.0;
	refShip.Price           			= 73700;
	refShip.HP              			= 5800;
	refShip.SP              			= 100;
	
	refship.Type.Merchant				= false;
	refship.Type.War					= true;
	refship.Spec                        = SHIP_SPEC_RAIDER;
	
	refShip.lowpolycrew 				= 20;
	
	refship.Rocking.y 					= 0.4;
	refship.Rocking.az 					= 0.035;	
	refship.WaterLine					= -0.75;

	refship.SpeedDependWeight			= 0.3;
	refship.SubSeaDependWeight			= 1.0;
	refship.TurnDependWeight			= 0.3;
	refship.WindAgainstSpeed   			= 0.61;
	refship.CabinType          			= "Cabin"; // boal 28.03.05
	refship.DeckType           			= "Big";
		
	refship.InertiaAccelerationX	= 5.0;	refship.InertiaBrakingX		= 5.0;
	refship.InertiaAccelerationY	= 4;	refship.InertiaBrakingY		= 4;
	refship.InertiaAccelerationZ	= 5.0;	refship.InertiaBrakingZ		= 5.0;
	
	refShip.Height.Bombs.Y				= 2.7;		refShip.Height.Bombs.DY		= 1.35;
	refShip.Height.Grapes.Y				= 5.325;	refShip.Height.Grapes.DY	= 1.575;
	refShip.Height.Knippels.Y			= 19.5;		refShip.Height.Knippels.DY	= 13.5;
	refShip.Height.Balls.Y				= 2.7;		refShip.Height.Balls.DY		= 1.35;
	
	refShip.GeraldSails.rey_b1		= 1;
	refShip.GeraldSails.rey_b2		= 1;
	refShip.GeraldSails.rey_b3		= 1;
	refShip.GeraldSails.rey_b1.vscale		= 0.9;
	refShip.GeraldSails.rey_b2.vscale		= 0.7;
	refShip.GeraldSails.rey_b3.vscale		= 0.7;

	refship.Track.Enable	= true;
	refship.Track1.ZStart	= 0.20;
	refship.Track1.LifeTime = 14.5;
	refship.Track1.Width	= "3.7, 3.9";
	refship.Track1.Speed	= "7.0, 8.0";

	refship.Track2.ZStart	= -0.15;
	refship.Track2.LifeTime = 8.5;
	refship.Track2.Width	= "8.0, 9.0";
	refship.Track2.Speed	= "0.15, 0.25";
	
	refship.nation.england	= false;
	refship.nation.france	= false;
	refship.nation.spain	= false;
	refship.nation.holland	= false;
	refship.nation.pirate	= false;
    refship.EmblemedSails.normalTex = "Ships/CursedPearl1/Hull1/sail_BlackPearl_torn.tga";

	//refship.icons.FirstIconPos = 0+1*16;
	//refship.icons.SecondIconPos = 0+1*16 + 1;
	
	refship.icons.FirstIconPos = 4+0*16;
	refship.icons.SecondIconPos = 4+0*16 + 1;
	refship.modname = "my_mod_ships";

	refship.InertiaAccelerationY = stf(refship.InertiaAccelerationY) / 6.0;

	refShip.cannonr  = refShip.rcannon;
	refShip.cannonl  = refShip.lcannon;
	refShip.cannonf  = refShip.fcannon;
	refShip.cannonb  = refShip.bcannon;



	// обновляем Hull'ы для всех кораблей
	InitShipHulls();
	Log_Info("Migration done 2");
}


ref InsertNewShip()
{
	int idx = GetArraySize(&ShipsTypes);
	SetArraySize(&ShipsTypes, idx + 1);
	ref refShip = &ShipsTypes[idx];
	aref cannonl, cannonr, cannonb, cannonf;
	refship.index = idx;

	refship.InertiaAccelerationX = 0.2;		refship.InertiaBrakingX = 0.7;
	refship.InertiaAccelerationY = 0.4;		refship.InertiaBrakingY = 0.4;
	refship.InertiaAccelerationZ = 1.0;		refship.InertiaBrakingZ = 1.0;

	refship.Rocking.y 	= 0.5;
	refship.Rocking.az 	= 0.05;

	refship.NetGeraldSails = "";
	refship.Soundtype = "";

	refship.WaterLine 			= 0.0;
	refship.SpeedDependWeight 	= 0.0;
	refship.SubSeaDependWeight 	= 0.0;
	refship.TurnDependWeight 	= 0.0;
	refship.TurnDependSpeed 	= 0.0;

	refShip.Height.Bombs.Y		= 3.0;		refShip.Height.Bombs.DY		= 1.0;
	refShip.Height.Grapes.Y		= 4.0;		refShip.Height.Grapes.DY	= 1.0;
	refShip.Height.Knippels.Y	= 25.0;		refShip.Height.Knippels.DY	= 15.0;
	refShip.Height.Balls.Y		= 3.0;		refShip.Height.Balls.DY		= 1.0;

	// WindAgainstSpeed (WAS) от 0 до 2 это параметр курсового угла (КУ)
	// (1 - WAS)     от -1 до 1 это cos КУ
	// acos(1 - WAS) от  0 до π это КУ

	refship.WindAgainstSpeed   	= 1.0;
	refship.CabinType          	= "Cabin_Small"; // потом припишем слово My_ и будет каютой для ГГ
	refShip.AbordageLocation	= "BOARDING_SMALL_DECK";
	refship.DeckType          	= "Low";

	refShip.MaxCaliber			= 6;
	refShip.Cannon				= CANNON_TYPE_CANNON_LBS6;
	refShip.CannonsQuantity		= 0;
	refShip.CannonsQuantityMin	= 0;

	refship.CanEncounter 	= true;
	refship.Type.Merchant 	= true;
	refship.Type.War 		= true;
	refship.Spec            = SHIP_SPEC_WAR;
	refship.Track.Enable 	= false;
	refShip.FireHeight 		= 3.0;

	refShip.Massa = 1500000.0;
	refShip.Volume = 900.0;
	refShip.lowpolycrew = 5;

	refShip.rcannon = 0;
	refShip.lcannon = 0;
	refShip.fcannon = 0;
	refShip.bcannon = 0;

	ShipsTypes[idx].Cannons.Borts.cannonl = "";
	ShipsTypes[idx].Cannons.Borts.cannonr = "";
	ShipsTypes[idx].Cannons.Borts.cannonf = "";
	ShipsTypes[idx].Cannons.Borts.cannonb = "";

	makearef(cannonl,ShipsTypes[idx].Cannons.Borts.cannonl);
	makearef(cannonr,ShipsTypes[idx].Cannons.Borts.cannonr);
	makearef(cannonf,ShipsTypes[idx].Cannons.Borts.cannonf);
	makearef(cannonb,ShipsTypes[idx].Cannons.Borts.cannonb);

	cannonf.FireZone	= Degree2Radian(30.0);
	cannonf.FireDir		= Degree2Radian(0.0);
	cannonf.FireAngMin	= -0.35;
	cannonf.FireAngMax	= 0.60;

	cannonr.FireZone	= Degree2Radian(80.0);
	cannonr.FireDir		= Degree2Radian(90.0);
	cannonr.FireAngMin	= -0.35;
	cannonr.FireAngMax	= 0.60;

	cannonb.FireZone	= Degree2Radian(30.0);
	cannonb.FireDir		= Degree2Radian(180.0);
	cannonb.FireAngMin	= -0.35;
	cannonb.FireAngMax	= 0.60;

	cannonl.FireZone	= Degree2Radian(80.0);
	cannonl.FireDir		= Degree2Radian(270.0);
	cannonl.FireAngMin	= -0.35;
	cannonl.FireAngMax	= 0.60;

	return refShip;
}
