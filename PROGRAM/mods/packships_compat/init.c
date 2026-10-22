// Pack Ships compatibility layer.
//
// This file is optional at runtime: it is loaded only when Caribbean Legend
// Hook API (CLHA) 0.3.0 or newer is installed.  The ships themselves do not
// depend on CLHA.  Keeping the fog effect here avoids replacing AIShip.c,
// which used to overwrite the game's naval fixes whenever the game updated.

string PACKSHIPS_COMPAT_MOD = "packships_compat";
int iPackShipsCompatFrameCount = 0;

void CL_Mod_packships_compat_Init()
{
	CLHA_Manifest_Begin(PACKSHIPS_COMPAT_MOD);
	CLHA_Manifest_SetName(PACKSHIPS_COMPAT_MOD, "Pack Ships Compatibility Layer");
	CLHA_Manifest_SetVersion(PACKSHIPS_COMPAT_MOD, "1.8.2");
	CLHA_Manifest_SetAuthor(PACKSHIPS_COMPAT_MOD, "William Turturici");
	CLHA_Manifest_RequiresCLHA(PACKSHIPS_COMPAT_MOD, "0.3.0");
	CLHA_Manifest_DeclaresFile(PACKSHIPS_COMPAT_MOD, "Program\\mods\\packships_compat\\init.c");
	CLHA_Manifest_DeclaresCallback(PACKSHIPS_COMPAT_MOD, "CL_Mod_packships_compat_Init");
	CLHA_Manifest_DeclaresCallback(PACKSHIPS_COMPAT_MOD, "CL_Mod_packships_compat_Frame");
	CLHA_Manifest_End(PACKSHIPS_COMPAT_MOD);

	CLHA_Frame_Register(PACKSHIPS_COMPAT_MOD);
	CLHA_Log(PACKSHIPS_COMPAT_MOD, "Cursed Pearl fog compatibility layer enabled");
}

void CL_Mod_packships_compat_Frame()
{
	if (!bSeaActive)
	{
		iPackShipsCompatFrameCount = 0;
		DeleteAttribute(&CLHA_State, "fogSeen");
		return;
	}

	// The original code emitted one six-puff burst when a Cursed Pearl was
	// created.  Scan at a modest interval and do the same for ships first seen
	// during this sea encounter, rather than changing the game's AIShip.c.
	iPackShipsCompatFrameCount++;
	if (iPackShipsCompatFrameCount < 120) return;
	iPackShipsCompatFrameCount = 0;

	for (int i = 0; i < iNumShips; i++)
	{
		int characterIndex = Ships[i];
		if (characterIndex < 0 || characterIndex >= MAX_CHARACTERS) continue;

		ref rCharacter = GetCharacter(characterIndex);
		if (!PackShipsCompat_IsCursedPearl(rCharacter)) continue;

		// Refresh the bank periodically so it continues to envelop a moving ship.
		PackShipsCompat_CreateCursedPearlFog(rCharacter);
	}
}

int PackShipsCompat_IsCursedPearl(ref rCharacter)
{
	int shipType = GetCharacterShipType(rCharacter);
	if (shipType == SHIP_NOTUSED) return 0;
	if (!CheckAttribute(&RealShips[shipType], "basetype")) return 0;

	int baseType = sti(RealShips[shipType].basetype);
	if (!CheckAttribute(&ShipsTypes[baseType], "Name")) return 0;

	if (ShipsTypes[baseType].Name == "CursedPearl" || ShipsTypes[baseType].Name == "CursedPearl1") return 1;
	return 0;
}

void PackShipsCompat_CreateCursedPearlFog(ref rCharacter)
{
	// Dense low fog bank around the complete hull and surrounding water.
	PackShipsCompat_CreateFogPuff(rCharacter, 0.0, 1.0, -32.0);
	PackShipsCompat_CreateFogPuff(rCharacter, -12.0, 1.0, -24.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 12.0, 1.0, -24.0);
	PackShipsCompat_CreateFogPuff(rCharacter, -18.0, 0.0, -10.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 0.0, 2.0, -10.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 18.0, 0.0, -10.0);
	PackShipsCompat_CreateFogPuff(rCharacter, -22.0, -1.0, 5.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 0.0, 3.0, 5.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 22.0, -1.0, 5.0);
	PackShipsCompat_CreateFogPuff(rCharacter, -18.0, 0.0, 20.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 0.0, 2.0, 20.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 18.0, 0.0, 20.0);
	PackShipsCompat_CreateFogPuff(rCharacter, -10.0, 1.0, 34.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 10.0, 1.0, 34.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 0.0, 8.0, -4.0);
	PackShipsCompat_CreateFogPuff(rCharacter, 0.0, 11.0, 13.0);
}

void PackShipsCompat_CreateFogPuff(ref rCharacter, float offsetX, float offsetY, float offsetZ)
{
	if (!CheckAttribute(rCharacter, "Ship.Pos.x")) return;
	if (!CheckAttribute(rCharacter, "Ship.Pos.y")) return;
	if (!CheckAttribute(rCharacter, "Ship.Pos.z")) return;
	if (!CheckAttribute(rCharacter, "Ship.Ang.y")) return;

	float shipX = stf(rCharacter.Ship.Pos.x);
	float shipY = stf(rCharacter.Ship.Pos.y);
	float shipZ = stf(rCharacter.Ship.Pos.z);
	float angle = stf(rCharacter.Ship.Ang.y);

	CreateParticleSystemX(
		"pearlfog",
		shipX - offsetX * cos(angle) + offsetZ * sin(angle),
		shipY + offsetY,
		shipZ + offsetX * sin(angle) + offsetZ * cos(angle),
		0.0,
		0.0,
		0.0,
		2
	);
}
