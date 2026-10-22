// boal 08/04/06 общий диалог верфи
string sProf;
#include "DIALOGS\english\Rumours\Common_rumours.c"  //homo 25/06/06
void ProcessDialogEvent()
{
	ref NPChar, sld, compref;
	aref Link, NextDiag;
	string sTemp;

	int iShipClass = GetCharacterShipClass(PChar); // Если корабля нет, вернёт 7 (лодка)
	int iRank = sti(PChar.rank);
	int iSumm;
	int comp;

	ref rRealShip;
	
	string attr;
	int amount;
	ref shTo;
	string s1;
	
	string sGood = "";
	aref aroot, arcur;
	int num, gooda, iGood, SG;

	DeleteAttribute(&Dialog, "Links");

	makeref(NPChar,CharacterRef);
	makearef(Link, Dialog.Links);
	makearef(NextDiag, NPChar.Dialog);

	// вызов диалога по городам -->
	NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Shipyard\" + NPChar.City + "_Shipyard.c";
	if (!HasCityDialogFile("Shipyard", NPChar.City, "_Shipyard.c")) NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Shipyard\DefaultCity_Shipyard.c";
	if (LoadSegment(NPChar.FileDialog2))
	{
		ProcessCommonDialog(NPChar, Link, NextDiag);
		UnloadSegment(NPChar.FileDialog2);
	}
	else
	{
		NPChar.FileDialog2 = "DIALOGS\" + LanguageGetLanguage() + "\Shipyard\DefaultCity_Shipyard.c";
	    if (LoadSegment(NPChar.FileDialog2))
		{
	        ProcessCommonDialog(NPChar, Link, NextDiag);
			UnloadSegment(NPChar.FileDialog2);
		}
	}
	// вызов диалога по городам <--
	if (NPChar.id != "Pirates_shipyarder") ProcessCommonDialogRumors(NPChar, Link, NextDiag);//homo 16/06/06
	bool ok, ok1, ok2;
	int iTest = FindColony(NPChar.City); // город магазина
	ref rColony;
	string sFrom_sea = "";
	if (iTest != -1)
	{
		rColony = GetColonyByIndex(iTest);
		sFrom_sea = rColony.from_sea;
	}

	// генератор парусов по кейсу -->
	string attrLoc   = Dialog.CurrentNode;
	int i;
  	if (findsubstr(attrLoc, "SailsColorIdx_" , 0) != -1)
 	{
		i = findsubstr(attrLoc, "_" , 0);
	 	NPChar.SailsColorIdx = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 		Dialog.CurrentNode = "SailsColorDone";
 	}
 	if (findsubstr(attrLoc, "SailsTypeChooseIDX_" , 0) != -1)
 	{
		i = findsubstr(attrLoc, "_" , 0);
	 	NPChar.SailsTypeChooseIDX = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 		Dialog.CurrentNode = "SailsTypeChoose2";
 	}
	if (findsubstr(attrLoc, "SailsTypeChoose_" , 0) != -1)
 	{
		i = findsubstr(attrLoc, "_" , 0);
	 	comp = strcut(attrLoc, i+1, strlen(attrLoc)-1); // индех в конце
 		Dialog.CurrentNode = "SailsTypeChoosen_selected";
 	}
	if(HasSubStr(attrLoc, "shiporderend2_"))
	{
		i = findsubstr(attrLoc, "_" , 0);

		sld = GetCharacter(sti(npchar.questTemp.NPCid));
		int iChar = GetPassenger(PChar, sti(strcut(attrLoc, i+1, strlen(attrLoc)-1))); //выбранный пассажир
		if (iChar == -1) compref = GetMainCharacter();	//пришёл нулевой пассажир, а это сам ГГ, но GetPassenger вернёт -1 в этом случае.
			else compref = GetCharacter(iChar);

		DeleteAttribute(compref, "ship");//зачем-то стираем корабль офицера, хотя его там и не должно быть
		compref.ship = "";

		aref	arTo, arFrom;
		makearef(arTo, compref.ship);
		makearef(arFrom, sld.Ship);
		CopyAttributes(arTo, arFrom);

		if (iChar != -1) 
		{
			CheckForReleaseOfficer(iChar);//увольнение офицера с должностей, если он не просто пассажир
			RemovePassenger(pchar, compref);
			SetCompanionIndex(pchar, -1, iChar);
			DelBakSkill(compref);
		}
		else pchar.location.from_sea = rColony.from_sea; 	//корабль берет ГГ и его в порт ставим

		int iClass = RealShips[sti(compref.Ship.Type)].Class;

		NPChar.reputation = sti(NPChar.reputation) + (8 - iClass);//минимальная репа ~12. две единицы репы ~ одна единица навыка корабела
		if (sti(NPChar.reputation)>100) NPChar.reputation = 100;
		WaitDate("",0,0,0, 0, 20);

		DeleteAttribute(npchar, "questTemp.ShipOrderDate");
		DeleteAttribute(npchar, "questTemp.ShipOrderTime");
		DeleteAttribute(npchar, "questTemp.NPCid");

		AddQuestRecordEx("ShipOrder", "ShipOrder", "End");
		pchar.ShipInOrder = sti(pchar.ShipInOrder) - 1;
		IncAndUnlock("AchShipOrder", 1);
		if (sti(pchar.ShipInOrder) < 1) CloseQuestHeader("ShipOrder");

		sld.id = "ShipOrder";//сбрасываем индекс к стандартному
		DeleteAttribute(sld, "ship");//затираем данные корабля
		sld.ship = "";
		LAi_SetCurHP(sld, 0.0);//ещё и убивать непися, чтоб точно очистился из массива?

		Dialog.CurrentNode = "exit";//закрываем диалог, ещё одно подтверждение уже не справшиваем
	}
 	// генератор парусов по кейсу <--
	switch(Dialog.CurrentNode)
	{
		case "Exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "ship_tunning_not_now":  // аналог выхода, со старых времён, много переделывать.
			LockControl("DlgDown3", false);
			LockControl("DlgUp3", false);
			DialogExit();
			NextDiag.CurrentNode = "Master_Ship";  // выход для тюнинга, нужно тут из-за LoadSegment
		break;

		case "fight":
			DialogExit();
			NextDiag.CurrentNode = NextDiag.TempNode;
			LAi_group_Attack(NPChar, Pchar);
			AddDialogExitQuest("MainHeroFightModeOn");
		break;

		case "First time":
			if (LAi_grp_playeralarm > 0)
			{
	   			dialog.text = NPCharRepPhrase(pchar,
					LinkRandPhrase("The alarm has been raised in the city, they are looking for you everywhere! If I were you, I wouldn't linger here.", "The entire city guard is scouring the city looking for you. I'm not an idiot and I'm not going to talk to you!", "Run, "+ GetSexPhrase("buddy", "girlfriend") +"before the soldiers made a sieve out of you..."),
					LinkRandPhrase("What do you need, scoundrel?! The city guards have already are on your trail, you can't get far, dirty pirate!", "Dirty murderer, get out of my house! Guards!!", "I'm not afraid of you, bastard! You'll be hanged in our fort soon, you won't get far..."));
				link.l1 = NPCharRepPhrase(pchar,
					RandPhraseSimple("Heh, anxiety is not a problem for me...", "They'll never catch me."),
					RandPhraseSimple("Shut your mouth, " + GetWorkTypeOfMan(npchar, "") + "Or I'll rip out your filthy tongue!", "Heh, " + GetWorkTypeOfMan(npchar, "") + "and still there - to catch pirates! I'll tell you what, buddy: stay quiet and you'll live..."));
				link.l1.go = "fight";
				break;
			}
			NextDiag.TempNode = "First time";

			//homo Линейка Блада
			if (CheckAttribute(Pchar, "questTemp.CapBloodLine") && Pchar.questTemp.CapBloodLine == true)
			{
				dialog.Text = LinkRandPhrase("Hey, Dr. Blood!" + TimeGreeting() + ".",
									"Glad to see you, Peter Blood.",
									"It's good that you came to see me, " + GetFullName(pchar) + ". How is Colonel Bishop?");
				Link.l1 = "Alas, I'm already leaving, " + npchar.name + ". See you later.";
				Link.l1.go = "exit";
				if(CheckAttribute(Pchar, "questTemp.CapBloodLine.ShipForJack") && Pchar.questTemp.CapBloodLine.ShipForJack == true)
				{
					Link.l2 = "I need to help one guy... my friend, I mean, to get to Porto Bello.";
					Link.l2.go = "Blood_Shipyard1";
				}
				break;
			}

			if(NPChar.quest.meeting == "0")
			{
				dialog.Text = TimeGreeting() + "! I "+ GetFullName(NPChar) + ", the owner of this shipyard.";
				Link.l1 = "I'm glad to meet you. I'm captain " + GetFullName(Pchar) + ".";
				Link.l1.go = "meeting";
				NPChar.quest.meeting = "1";
			}
			else
			{
				dialog.Text = pcharrepphrase("Oh, is that you again? Oh well, money doesn't smell.",
										TimeGreeting() + ". How can I help you, " + GetAddress_Form(NPChar) + "?");
				Link.l1 = pcharrepphrase("That's it, I'm crying - you're doing it.",
										"I need to use the shipyard's services.");
				Link.l1.go = "Shipyard";

				if (!CheckAttribute(npchar, "questTemp.ShipOrderTime"))
				{
				link.l14 = "Can I order a ship from your shipyard that suits my personal preferences?";
				link.l14.go = "Shipyard1";
				}
				else
				{
				link.l14 = "Is the ship I ordered ready?";
				link.l14.go = "shiporder1";
				}
				if (sti(RealShips[sti(Pchar.Ship.Type)].class) < 7)
				{
					if ( NPChar.city == "Tortuga" || NPChar.city == "Villemstad" || NPChar.city == "PortRoyal" || NPChar.city == "Havana")
					{
						Link.l22 = "What about upgrading my ship?";
						Link.l22.go = "ship_upgrades_start";
					}
				}
				
				if (GetNationRelation2MainCharacter(sti(NPChar.nation)) != RELATION_ENEMY || NPChar.city == "Caiman")
				{
					link.l11 = "I need guns for the ship.";
					link.l11.go = "Cannons";
				}
				link.l12 = "I want to change the appearance of the ship.";
				link.l12.go = "SailsGerald";
				//Jason --> генератор Призонер
				if (CheckAttribute(pchar, "GenQuest.Findship.Shipyarder") && NPChar.location == pchar.GenQuest.Findship.Shipyarder.City + "_shipyard")
				{
					ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea);
					if(sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok)
					{
						for(i = 1; i < COMPANION_MAX; i++)
						{
							int iTemp = GetCompanionIndex(PChar, i);
							if(iTemp > 0)
							{
								sld = GetCharacter(iTemp);
								NPchar.ShipCheck1 = GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(RealShips[sti(sld.ship.type)].basetype), "Name")));
								NPchar.ShipCheck2 = GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.Findship.Shipyarder.ShipType), "Name")));
								ok1 = (NPChar.ShipCheck1 == NPChar.ShipCheck2);
								ok2 = (sti(RealShips[sti(sld.ship.type)].basetype) == sti(pchar.GenQuest.Findship.Shipyarder.ShipType));
								if(GetRemovable(sld))
								{
									if (ok1 || ok2)
									{
										pchar.GenQuest.Findship.Shipyarder.CompanionIndex = sld.Index;
										pchar.GenQuest.Findship.Shipyarder.OK = 1;
										pchar.GenQuest.Findship.Shipyarder.ShipName = sld.Ship.Name;
										pchar.GenQuest.Findship.Shipyarder.Money = makeint(GetShipSellPrice(sld, CharacterFromID(NPChar.city + "_shipyarder"))*1.5);
									}
								}
							}
						}
					}
					if (CheckAttribute(pchar, "GenQuest.Findship.Shipyarder.OK") && sti(pchar.GenQuest.Findship.Shipyarder.OK) == 1)
					{
						link.l13 = "I delivered you a "+GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.Findship.Shipyarder.ShipType), "Name")))+", as you requested.";
						link.l13.go = "Findship_check";
					}// <-- генератор Призонер
				}
				Link.l2 = "I just want to talk.";
				Link.l2.go = "quests"; //(перессылка в файл города)

				link.l15 = "I just wanted to find out if you have a job for me?";
				link.l15.go = "AskQuest";

				// -->
				if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
				{
					link.l3 = "I wanted to talk to you about financial matters.";
					link.l3.go = "LoanForAll";
				}
  				if (CheckAttribute(pchar, "GenQuest.Intelligence") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "") //квест мэра - на связь с нашим шпионом
				{
					link.l7 = RandPhraseSimple("I'm here on behalf of a man. His name is the governor " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".",
						GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + "he sent me to you. I must pick up something...");
					link.l7.go = "IntelligenceForAll";
				}
				//--> квест Аззи.
				if (CheckCharacterItem(pchar, "Azzy_bottle"))
				{
					link.l5 = "Listen, I have one thing -a tin can. I need to open it carefully. Can you do it? I'll pay.";
					if (npchar.id == pchar.questTemp.Ascold.ShipyarderId)
					{
						link.l5.go = "AggryHelp";
						pchar.questTemp.Azzy = "Azzy_IHaveLamp";
					}
					else
					{
						link.l5.go = "AzzyTryTalk";
					}
				}
				//<-- на квест Аззи.
				//проверка выполнения квеста украсть чертёж на верфи
				if (CheckCharacterItem(pchar, "ShipyardsMap") && CheckAttribute(pchar, "questTemp.different.ShipyardsMap.Id") && pchar.questTemp.different.ShipyardsMap.Id == npchar.id)
				{
					link.l6 = "I did your assignment. I have blueprint from the " + XI_ConvertString("Colony" + pchar.questTemp.different.ShipyardsMap.city + "Gen") + "'s shipyard.";
					link.l6.go = "ShipyardsMapOk_1";
				}

				if(CheckAttribute(pchar, "GenQuest.EncGirl"))
				{
					if(pchar.GenQuest.EncGirl == "toLoverFather" && pchar.GenQuest.EncGirl.LoverFather == "shipyard_keeper" && pchar.GenQuest.EncGirl.LoverCity == npchar.city)
					{
						link.l7 = "Hi, I'm here at your son's invitation.";
						link.l7.go = "EncGirl_4";
						pchar.quest.EncGirl_GetLoverFather.over = "yes";
					}
					if(pchar.GenQuest.EncGirl == "toParents" || pchar.GenQuest.EncGirl == "FindLover")
					{
						if(pchar.GenQuest.EncGirl.Father == "shipyard_keeper" && npchar.city == pchar.GenQuest.EncGirl.city)
						{
							link.l8 = "It's about your daughter...";
							link.l8.go = "EncGirl_1";
						}
					}
				}
				Link.l9 = "I have to go.";
				Link.l9.go = "exit";
			}
		break;
		
		case "ship_upgrades_start":
			if (CheckAttribute(&RealShips[sti(Pchar.Ship.Type)],"Shipyarder"))
			{
				shTo = &RealShips[sti(pchar.Ship.Type)];
				if (sti(shTo.Shipyarder) != sti(NPChar.index))
				{
					dialog.Text = "I cannot help you with your ship.";
					Link.l1 = "Well, no means no. Goodbye!";
					Link.l1.go = "exit";
					break;
				}
			}
			if (CheckAttribute(&RealShips[sti(Pchar.Ship.Type)],"GoodsForUpgrade"))
			{
				dialog.Text = "Ah, you again. Let's see if you got anything needed...";
				Link.l1 = "So, what is it?";
				Link.l1.go = "ship_upgrades_2";
				if (bBettaTestMode)
				{
					Link.l2 = "DO UPGRADE NOW";
					Link.l2.go = "ship_upgrades_do";
				}
			}
			else
			{
				NextDiag.CurrentNode = NextDiag.TempNode;
				DialogExit();
				LaunchShipUpgrades(npchar);
			}
		break;
		
		case "ship_upgrades_2":
			shTo = &RealShips[sti(pchar.Ship.Type)];
			sGood = "";
			makearef(aroot,shTo.GoodsForUpgrade);
			num = GetAttributesNum(aroot);
			gooda = 0;
			iGood = 0;
			SG = 0;
			for (i = 0; i < num; i++)
			{
				arcur = GetAttributeN(aroot,i);
				sGood = GetAttributeName(arcur);
				iGood = FindGood(sGood);
				gooda = sti(shTo.GoodsForUpgrade.(sGood));
				SG = GetSquadronGoods(pchar, iGood);
				if (gooda > 0 && SG > 0)
				{
					if (SG >= gooda)
					{
						RemoveCharacterGoods(pchar, iGood, gooda);
						shTo.GoodsForUpgrade.(sGood) = 0;
						log_info(XI_ConvertString(sGood)+" was delivered in quantity "+gooda+" out of "+gooda);
					}
					else 
					{
						RemoveCharacterGoods(pchar, iGood, SG);
						shTo.GoodsForUpgrade.(sGood) = sti(shTo.GoodsForUpgrade.(sGood))-SG;
						log_info(XI_ConvertString(sGood)+" was delivered in quantity "+SG+" out of "+gooda);
					}
				}
				else continue;
			}
			
			bool enough = true;				
			sGood = "";
			makearef(aroot,shTo.GoodsForUpgrade);
			num = GetAttributesNum(aroot);
			gooda = 0;
			for (i = 0; i < num; i++)
			{
				arcur = GetAttributeN(aroot,i);
				sGood = GetAttributeName(arcur);
				gooda = sti(shTo.GoodsForUpgrade.(sGood));
				if (gooda > 0)
				{
					enough = false;
				}
			}
			if (enough)
			{
				dialog.Text = "All goods are here, let's do these upgrades now.";
				Link.l1 = "Perfect, I hope it wouldn't take long.";
				Link.l1.go = "ship_upgrades_do";
			}
			else 
			{
				dialog.Text = "According to my records, you still have some goods to get. Can't wait to start.";
				Link.l1 = "Working as hard as I can.";
				Link.l1.go = "ship_upgrades_exit";
			}
		break;
		
		case "ship_upgrades_do":
			shTo = &RealShips[sti(pchar.Ship.Type)];
			DeleteAttribute(shTo,"GoodsForUpgrade");
			makearef(aroot,shTo.UpgradePrepared);
			num = GetAttributesNum(aroot);
			for (i = 0; i < num; i++)
			{
				arcur = GetAttributeN(aroot,i);
				sGood = GetAttributeName(arcur);
				shTo.Upgrade.(sGood) = true;
				CheckForShipStatsChange(shTo,sGood); //чек на метод в шипсутилитах для обновления статов, пока пустышка
			}
			DeleteAttribute(shTo,"UpgradePrepared");
			DeleteAttribute(shTo,"Shipyarder");
			int uptime = num * (7-sti(shTo.class)+4);
			AddTimeToCurrent(uptime, 0);
			log_info("Ship upgrade was completed.");
			log_info("Took hours: "+uptime+".");
			dialog.Text = "...Done, go and try. Better than new!";
			Link.l1 = "Thanks for your hard work. Goodbye!";
			Link.l1.go = "ship_upgrades_exit";
		break;
		
		case "ship_upgrades_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;


		case "AskQuest":
				//--->> квест украсть чертёж на верфи
				if (rand(1) && pchar.questTemp.different == "free" && GetNpcQuestPastDayWOInit(npchar, "questShipyardsMap") > 7 && !CheckAttribute(pchar, "questTemp.different.ShipyardsMap") && GetSummonSkillFromName(pchar, SKILL_SNEAK) > 25)
				{
					dialog.text = "Listen, I have an important matter for you. I hope you can help me...";
					link.l1 = "Let's hear what the deal is.";
					link.l1.go = "ShipyardsMap_1";
					SaveCurrentNpcQuestDateParam(npchar, "questShipyardsMap");
					break;
				}

				/* автор - Jason (BlackMark Studio); перенос в CSP - Nathaniel ---------- */
				if (drand(4) == 2 && !CheckAttribute(pchar, "GenQuest.Findship.Shipyarder"))
				{
					if (!CheckAttribute(npchar, "Findship") || GetNpcQuestPastDayParam(npchar, "Findship") >= 20)
					{
						SelectFindship_ShipType(sti(npchar.nation)); //выбор типа корабля
						pchar.GenQuest.Findship.Shipyarder.ShipBaseName = GetStrSmallRegister(XI_ConvertString(GetBaseShipParamFromType(sti(pchar.GenQuest.Findship.Shipyarder.ShipType), "Name") + ""));
						pchar.GenQuest.Findship.Shipyarder.City = npchar.city; //город квестодателя
						dialog.text = "Yes, I have a problem that needs to be solved. I received an order. My client needs it as soon as possible " + pchar.GenQuest.Findship.Shipyarder.ShipBaseName + ". However, I do not have such a ship at the shipyard now, to make it in two months I also have no possibility. \nIf you can deliver me such a ship, I would be very grateful and will pay you an amount of one and a half times its selling price.";
						link.l1 = "An interesting suggestion!";
						link.l1.go = "Findship";
						link.l2 = "I'm not interested in this.";
						link.l2.go = "Findship_exit";
						SaveCurrentNpcQuestDateParam(npchar, "Findship");
						break;
					}
				}

				dialog.Text = "Well, we were surprised! Everyone comes to me for help here... Are you sure your ship doesn't need repairs?";
				link.l1 = pcharrepphrase("That's right, I'm crying-you're doing it.",
										"I really need to use the shipyard's services.");
				link.l1.go = "Shipyard";
				link.l2	= "Not this time, goodbye.";
				link.l2.go = "exit";
		break;

		case "Meeting":
				dialog.Text = "Well, I'm glad to meet a new client. My shipyard is at your service.";
				Link.l1 = "Great, " + GetFullName(NPChar) + ". Let's see what you can offer me.";
				Link.l1.go = "Shipyard";

				link.l14 = "Can I order a ship at your shipyard that suits my personal preferences?";
				link.l14.go = "Shipyard1";

				if (GetNationRelation2MainCharacter(sti(NPChar.nation)) != RELATION_ENEMY)
				{
					link.l13 = "I need guns for the ship.";
					link.l13.go = "Cannons";
				}
				link.l12 = "I want to change the appearance of the ship.";
				link.l12.go = "SailsGerald";
				Link.l2 = "I just want to talk.";
				link.l2.go = "quests";

				link.l15 = "I just wanted to find out if you have a job for me? ";
				link.l15.go = "AskQuest";
				// -->
				if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
				{
					link.l3 = "I wanted to talk to you about financial matters.";
					link.l3.go = "LoanForAll";//(перессылка в файл города)
				}
  				if (CheckAttribute(pchar, "GenQuest.Intelligence") && pchar.GenQuest.Intelligence.SpyId == npchar.id && pchar.GenQuest.Intelligence == "") //квест мэра - на связь с нашим шпионом
				{
					link.l7 = RandPhraseSimple("I'm here on behalf of a man. His name is the governor " + GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + ".",
						GetFullName(characterFromId(pchar.GenQuest.Intelligence.MayorId)) + "he sent me to you. I should pick up something...");
					link.l7.go = "IntelligenceForAll";
				}
				//--> квест Аззи.
				if (CheckCharacterItem(pchar, "Azzy_bottle"))
				{
					link.l5 = "Listen, I have one thing -a tin can. I need to open it carefully. Can you do it? I'll pay.";
					if (npchar.id == pchar.questTemp.Ascold.ShipyarderId)
					{
						link.l5.go = "AggryHelp";
						pchar.questTemp.Azzy = "Azzy_IHaveLamp";
					}
					else
					{
						link.l5.go = "AzzyTryTalk";
					}
				}
				//<-- на квест Аззи.
				if(CheckAttribute(pchar, "GenQuest.EncGirl"))
				{
					if(pchar.GenQuest.EncGirl == "toLoverFather" && pchar.GenQuest.EncGirl.LoverFather == "shipyard_keeper" && pchar.GenQuest.EncGirl.LoverCity == npchar.city)
					{
						link.l8 = "Hello, I'm here at your son's invitation.";
						link.l8.go = "EncGirl_4";
						pchar.quest.EncGirl_GetLoverFather.over = "yes";
					}
					if(pchar.GenQuest.EncGirl == "toParents" || pchar.GenQuest.EncGirl == "FindLover")
					{
						if(pchar.GenQuest.EncGirl.Father == "shipyard_keeper" && npchar.city == pchar.GenQuest.EncGirl.city)
						{
							link.l9 = "It's about your daughter...";
							link.l9.go = "EncGirl_1";
						}
					}
				}
				if (CheckCharacterItem(pchar, "CaptainBook") && !CheckAttribute(pchar, "questTemp.different.GiveShipLetters.speakShipyard"))
				{
					if(CheckAttribute(pchar, "questTemp.different.GiveShipLetters") && (pchar.questTemp.different.GiveShipLetters.city == npchar.city))
					{
						link.l10 = "What can you say about the owner of these documents?";
						link.l10.go = "ShipLetters_6";// генератор  "Found documents"
					}
				}
				Link.l11 = "I should go. Thank you.";
				Link.l11.go = "exit";
		break;

		case "ShipLetters_6":
			pchar.questTemp.different.GiveShipLetters.speakShipyard = true;
			if(sti(pchar.questTemp.different.GiveShipLetters.variant) == 0)
			{
				dialog.text = "Let's take a look! No, I didn't have one. I think you should go to the portmaster.";
				link.l1 = "Undoubtedly....";
				link.l1.go = "exit";
			}
			else
			{
				sTemp = "Let's take a look, Captain! Yeah! These are the ship's documents of my good friend, one might say, my most beloved client. I am sure he will be incredibly satisfied with the find, and will even pay bonuses.";
				sTemp = sTemp + "I think I can offer you something " + sti(pchar.questTemp.different.GiveShipLetters.price2) + "gold in his name";
				dialog.text = sTemp;
				link.l1	= "Perhaps it's not worth it...";
				link.l1.go = "exit";
				link.l2 = "Thank you, " + GetFullName(NPChar) + "! Give my regards to your friend.";
				link.l2.go = "ShipLetters_7";
			}
		break;

		case "ShipLetters_7":
			TakeItemFromCharacter(pchar, "CaptainBook");
			addMoneyToCharacter(pchar, sti(pchar.questTemp.different.GiveShipLetters.price2));
			pchar.questTemp.different = "free";
			pchar.quest.GiveShipLetters_null.over = "yes"; //снимаем таймер
			AddQuestRecord("GiveShipLetters", "4");
			AddQuestUserData("GiveShipLetters", "sSex", GetSexPhrase("", "a"));
			CloseQuestHeader("GiveShipLetters");
			DeleteAttribute(pchar, "questTemp.different.GiveShipLetters");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "EncGirl_1":
			dialog.text = "I'm listening to you carefully.";
			link.l1 = "I brought your fugitive.";
			link.l1.go = "EncGirl_2";
		break;

		case "EncGirl_2":
			dialog.text = "Captain, thank you very much! How is she? healthy, not hurt? Soon a day as she ran, wretch. Do I wish her ill? I was going to marry her to rich, young groom... And that he's not handsome, so do not drink water from his face. No! She's got her own way of doing things, just like her mother, so that she... \nOh, she wouldn't have been born if her mother hadn't run off with some enterprising schmuck in her day... but what can I say... Youth is naive, stupid... and cruel.";
			link.l1 = "Of course, you're the father and it's up to you, but I wouldn't rush the wedding...";
			link.l1.go = "EncGirl_3";
		break;

		case "EncGirl_3":
			dialog.text = "What do you understand? Do you have children of your own? No? Now, when you do, come in, and then we'll talk... \nYeah... I promised a reward to whoever will return her - please take it.";
			link.l1 = "Thank you. And hold her tight. Something tells me she won't stop there.";
			link.l1.go = "exit";
			AddDialogExitQuestFunction("EncGirl_ToLoverParentsExit");
		break;

		case "EncGirl_4":
			if(sti(pchar.GenQuest.EncGirl.LoverFatherAngry) == 0)
			{
				dialog.text = "Ah, so you are that "+ GetSexPhrase("captain who brought", "girl who brought") +" my prodigal son with a young bride?";
				link.l1 = "Yes, I helped them escape.";
				link.l1.go = "EncGirl_5";
			}
			else
			{
				dialog.text = "Ah, I did."+ GetSexPhrase("Xia, the benefactor", "Ah, the benefactress") +". Are you waiting for a reward?";
				link.l1 = "Well, not exactly awards, but it wouldn't hurt to hear gratitude.";
				link.l1.go = "EncGirl_6";
			}
		break;

		case "EncGirl_5":
			dialog.text = "I am very grateful to you for not leaving my child in a difficult situation and helping him to get out of such a delicate situation with honor. Let me thank you as much as I can. Accept this modest sum and a gift from me personally.";
			link.l1 = "Thank you, it was a pleasure to help these young people.";
			link.l1.go = "EncGirl_5_1";
		break;

		case "EncGirl_6":
			dialog.text = "What kind of gratitude?! What is the gratitude for?! Not only has this blockhead been hanging around without work for six months, he's also found time to be nice! Yes, I was already running my own business at his age. And look, he's gone hunting! The governor has a marriageable daughter, and this one has brought some kind of rootless slut, God forgive me, and is asking for parental blessing!";
			link.l1 = "Hmm, so you don't believe in sincere feelings?";
			link.l1.go = "EncGirl_6_1";
		break;

		case "EncGirl_5_1":
			AddMoneyToCharacter(pchar, sti(pchar.GenQuest.EncGirl.sum));
			GiveItem2Character(pchar, pchar.GenQuest.EncGirl.item);
			AddQuestRecord("JungleGirl", "18");
			CloseQuestHeader("JungleGirl");
			DeleteAttribute(pchar, "GenQuest.EncGirl");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "EncGirl_6_1":
			dialog.text = "What kind of feelings are these?! What kind of feelings are you talking about? An adult, like a person, but there too... Feelings! It should be a shame for you young people to indulge in such matters, and to contract as a pimp. The girl was torn out of her parents' nest and my sucker's life was ruined. There will be no gratitude to you. Goodbye.";
			link.l1 = "Well, you don't get sick either...";
			link.l1.go = "EncGirl_6_2";
		break;

		case "EncGirl_6_2":
			ChangeCharacterReputation(pchar, -1);
			AddQuestRecord("JungleGirl", "19");
			AddQuestUserData("JungleGirl", "sSex", GetSexPhrase("ate", "la"));
			AddQuestUserData("JungleGirl", "sSex1", GetSexPhrase("", "a"));
			CloseQuestHeader("JungleGirl");
			DeleteAttribute(pchar, "GenQuest.EncGirl");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		/* Nathaniel (12.03.21): генераторный квест "Ship Search" */
		case "Findship_exit":
			DialogExit();
			DeleteAttribute(pchar, "GenQuest.Findship.Shipyarder");
		break;
		/* -------------------------------------------------------- */

		case "Findship":
			pchar.GenQuest.Findship.Shipyarder = "begin";
			pchar.GenQuest.Findship.Shipyarder.Name = GetFullName(npchar);
			pchar.GenQuest.Findship.Shipyarder.City = npchar.city;
			ReOpenQuestHeader("Findship");
			AddQuestRecord("Findship", "1");
			AddQuestUserData("Findship", "sCity", XI_ConvertString("Colony"+pchar.GenQuest.Findship.Shipyarder.City+"Gen"));
			AddQuestUserData("Findship", "sName", pchar.GenQuest.Findship.Shipyarder.Name);
			AddQuestUserData("Findship", "sShip", pchar.GenQuest.Findship.Shipyarder.ShipBaseName);
			SetFunctionTimerCondition("Findship_Over", 0, 0, 60, false);
			DialogExit();
		break;

		case "Findship_check":
			dialog.text = "Great! I am glad that you completed this task so quickly. Where is this ship?";
			link.l1 = "At the moment" + pchar.GenQuest.Findship.Shipyarder.ShipBaseName + " ''" + pchar.GenQuest.Findship.Shipyarder.ShipName + " it is on the raid. You can start the examination...";
			link.l1.go = "Findship_complete";
		break;

		case "Findship_complete":
			pchar.quest.Findship_Over.over = "yes";//снять прерывание
			sld = GetCharacter(sti(pchar.GenQuest.Findship.Shipyarder.CompanionIndex));

			SetCrewQuantityOverMax(Pchar, GetCrewQuantity(Pchar) + GetCrewQuantity(sld));//добавляем число матросов с отдаваемого корабя на корабль ГГ сверх лимита

			RemoveCharacterCompanion(PChar, sld);//удаляем корабль вместе с матросами
			AddPassenger(pchar, sld, false);
			AddTroopersCrewToOther(Pchar);//делим сверхлимитных матросов с корабля ГГ по эскадре
			dialog.text = "Here's your money, " + GetAddress_Form(npchar) + ". Your fee is " + FindRussianMoneyString(sti(pchar.GenQuest.Findship.Shipyarder.Money)) + ". I sincerely thank you for the work you have done. Don't forget to look me up. Goodbye!";
			link.l1 = "Have a nice day, " + GetAddress_FormToNPC(npchar) + " " + npchar.lastname + ".";
			link.l1.go = "exit";
			AddMoneyToCharacter(pchar, sti(pchar.GenQuest.Findship.Shipyarder.Money));
			AddQuestRecord("Findship", "3");
			AddQuestUserData("Findship", "sMoney", FindRussianMoneyString(sti(pchar.GenQuest.Findship.Shipyarder.Money)));
			CloseQuestHeader("Findship");
			DeleteAttribute(pchar, "GenQuest.Findship.Shipyarder");
		break;
		//<-- генератор Поиск корабля

		case "shipyard":
			ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea);
			if (sti(Pchar.Ship.Type) == SHIP_NOTUSED || ok)
			{
				NextDiag.CurrentNode = NextDiag.TempNode;
				DialogExit();
				LaunchShipyard(npchar);
			}
			else
			{
				dialog.text = NPCharRepPhrase(npchar, pcharrepphrase("Are you kidding me? And where is your ship? Pierce doesn't have it!",
																	 "By the devil, you can't fool me! The pier doesn't have your ship!"),
													  pcharrepphrase("I don't see your ship in port, Captain " +GetFullName(pchar)+ ". I hope it's not the Flying Dutchman?",
																	 "Captain, it's more convenient to repair the ship in the dock. Dock the ship and come."));
				link.l1 = NPCharRepPhrase(npchar, pcharrepphrase("" +RandSwear()+"What a nuisance!!! Okay, old weasel, I'll see you later!",
																 "I didn't mean to deceive you, " +GetFullName(npchar)+ ", my ship is on the other side of the island."),
												  pcharrepphrase("No, my ship is called Black Pearl... You scared! Ha, I'm joking!",
																 "Thanks for the advice, I will definitely use it."));
				link.l1.go = "exit";
			}
		break;

		case "shiporderend":
			bool ok_ship = (sFrom_sea == "") || (Pchar.location.from_sea == sFrom_sea);
			if (sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok_ship)
			{
				if (GetCompanionQuantity(pchar) < COMPANION_MAX)
				{
					dialog.Text = "Which of your officers should I give it to?";
					int _curCharIdx;
					int q = 0;
					int nListSize = GetPassengersQuantity(pchar);
					for(i=0; i<nListSize; i++)
						{
						_curCharIdx = GetPassenger(pchar,i);
						sld = GetCharacter(_curCharIdx);
							if (_curCharIdx!=-1)
							{
								ok = CheckAttribute(&characters[_curCharIdx], "prisoned") && sti(characters[_curCharIdx].prisoned) == true;
								if (!CheckAttribute(sld,"HPminusDaysNeedtoRestore"))
								{
									if (!ok && GetRemovable(&characters[_curCharIdx]))
									{
										attrLoc = "l"+i;
										sProf = "";
										if (IsOfficer(sld)) sProf += "(boarder)";
										if (sti(pchar.Fellows.Passengers.navigator) == sti(sld.index)) sProf += "(navigator)";
										if (sti(pchar.Fellows.Passengers.boatswain) == sti(sld.index)) sProf += "(boatswain)";
										if (sti(pchar.Fellows.Passengers.cannoner) == sti(sld.index)) sProf += "(gunner)";
										if (sti(pchar.Fellows.Passengers.doctor) == sti(sld.index)) sProf += "(doctor)";
										if (sti(pchar.Fellows.Passengers.carpenter) == sti(sld.index)) sProf += "(carpenter)";
									    if (sti(pchar.Fellows.Passengers.treasurer) == sti(sld.index)) sProf += " (treasurer)";
										Link.(attrLoc)	= GetFullName(&characters[_curCharIdx]) + sProf;
										Link.(attrLoc).go = "shiporderend2_" + i;
										q++;
									}
								}
							}
						}
					attrLoc = "l"+nListSize;
					if (q == 0)
						{
						Link.(attrLoc) = RandSwear() + "I forgot to bring captain for this ship with me.";
						Link.(attrLoc).go = "exit";
						}
					else
						{
						Link.(attrLoc) = "No, I've changed my mind.";
						Link.(attrLoc).go = "exit";
						}
				}
				else
				{
					dialog.text = "You have too big squadron. There is no longer enough space in the water area for your ships.";
					Link.l99 = "Yes, that's right. I'll pick it up another time.";
					Link.l99.go = "exit";
				}
			}
			else			
			{
				dialog.text = "Hmm... I don't see your flagship in port. And you can only pick up your ship here.";
				if (sti(Pchar.Ship.Type) == SHIP_NOTUSED)	//фикс - забираем себе, если нет корабля
				{
					Link.l99	= "Yes, I'm just without a ship, so I'll pick it up myself.";
					Link.l99.go = "shiporderend2_-1";
				}
				else
				{
					Link.l99 = "Yeah, you right. I fogot about it.";
					Link.l99.go = "exit";
				}
			}
		break;

		case "shiporder1":
			if (CheckAttribute(npchar, "questTemp.chest"))
			{
				dialog.Text = "The work is underway. Did you bring the materials I asked for?";
				if (checkOrderMatherial(NPChar))
				{
					Link.l1 = "Yes. I managed to get something.";
					Link.l1.go = "shiporder2";
				}
				link.l2 = "Circumstances have changed, I want to cancel the order.";
				link.l2.go = "shipordercancel";
				link.l3 = "No. I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
				int timeleft = sti(npchar.questTemp.ShipOrderTime) - GetNpcQuestPastDayParam(npchar, "questTemp.ShipOrderDate");
				string sAdd = "";
				switch (timeleft)
				{
				case 1: sAdd = " It will be ready tomorrow!"; break;
				case 2: sAdd = " It will be ready the day after tomorrow!"; break;
				}
				if (timeleft < 1)
				{//готов
				dialog.text = "Captain" +GetFullName(pchar)+ "! The ship is ready and, by the devil, you will not be disappointed. Everything strictly corresponds to your wishes.";
				link.l1 = "It's nice to hear that. Can I take it back?";
				link.l1.go = "shiporderend";
				}
				else
				{//не готов
				dialog.text = "Don't worry! The work is going strictly according to plan " + sAdd;
				link.l1 = "I see. I'll come back later.";
				link.l1.go = "exit";
				link.l2 = "Circumstances have changed, I want to cancel the order.";
				link.l2.go = "shipordercancel";
				}
			}
		break;

		case "shiporder2":
			string sTemp2 = GiveOrderMatherial(NPChar);
			if (sTemp2 == "")
			{
				DeleteAttribute(npchar,"questTemp.chest");//Всё доставили
				AddQuestRecordEx("ShipOrder", "ShipOrder", "t3");
				AddQuestUserData("ShipOrder", "cityName3", XI_ConvertString("Colony" + npchar.city + "Gen"));
				dialog.Text = "That's fine. This is a complete set. Nothing else is required.";
				Link.l1 = "Great, when will the ship be ready?";
				Link.l1.go = "shiporder1";
				link.l2 = "Circumstances have changed, I want to cancel the order.";
				link.l2.go = "shipordercancel";
			}
			else
			{
				dialog.Text = "You also need to deliver... um... I'll check my notes now... So: " + sTemp2;
				AddQuestRecordEx("ShipOrder", "ShipOrder", "t2");
				AddQuestUserData("ShipOrder", "cityName2", XI_ConvertString("Colony" + npchar.city + "Gen"));
				AddQuestUserData("ShipOrder", "TuningMaterial2", sTemp2);
				Link.l1 = "Everything is clear. I'll be back when I get the rest.";
				Link.l1.go = "exit";
				link.l2 = "Circumstances have changed, I want to cancel the order.";
				link.l2.go = "shipordercancel";
			}
		break;

		case "shipordercancel2":
			AddQuestRecordEx("ShipOrder", "ShipOrder", "Lose");
			AddQuestUserData("ShipOrder", "sSex", GetSexPhrase("xia", "as"));
			pchar.ShipInOrder = sti(pchar.ShipInOrder) - 1;
			if (sti(pchar.ShipInOrder) < 1) CloseQuestHeader("ShipOrder");

			sld = GetCharacter(sti(npchar.questTemp.NPCid));
			DeleteAttribute(npchar, "questTemp.ShipOrderDate");
			DeleteAttribute(npchar, "questTemp.ShipOrderTime");
			DeleteAttribute(npchar, "questTemp.NPCid");
			sld.id = "ShipOrder";//сбрасываем индекс к стандартному, чтобы этот номер массива в следующий раз можно было занять
			DeleteAttribute(npchar, "questTemp.chest"));//фикс - стирание необходимости доставки материала
			DeleteAttribute(sld, "ship");//затираем данные корабля
			DeleteAttribute(npchar, "questTemp.chest"));//убираем необходимость доставки материалов
			sld.ship = "";
			LAi_SetCurHP(sld, 0.0);//ещё и убивать непися, чтоб точно очистился из массива?

			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "shipordercancel":
			dialog.text = "Wait, Captain! We discussed the terms. We can break this agreement, but I won't be able to return the money. Everything has already been spent on materials and payment for the work. ";
			link.l1 = "I remember it, but that's how the circumstances turned out. I can't wait for this ship to be completed.";
			link.l1.go = "shipordercancel2";
			link.l2 = "You're right, I'd rather wait until the ordered ship is ready.";
			link.l2.go = "exit";
		break;

		case "shipyard1":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchShipyard1(npchar);
		break;

		case "Cannons":
			ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea);
			if (sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok)
			{
				NextDiag.CurrentNode = NextDiag.TempNode;
				DialogExit();
				LaunchCannons(sti(rColony.StoreNum));
			}
			else
			{
				dialog.text = NPCharRepPhrase(npchar, pcharrepphrase("Are you kidding me? And where is your ship? Pierce doesn't have it!",
																	 "By the devil, you can't fool me! The pier doesn't have your ship!"),
													  pcharrepphrase("I don't see your ship in port, Captain " +GetFullName(pchar)+ ". I hope it's not the Flying Dutchman?",
																	 "Captain, it's more convenient to repair the ship in the dock. Dock the ship and come."));
				link.l1 = NPCharRepPhrase(npchar, pcharrepphrase("" +RandSwear()+"What a nuisance!!! Okay, old weasel, I'll see you later!",
																 "I didn't mean to deceive you, " +GetFullName(npchar)+ ", my ship is on the other side of the island."),
												  pcharrepphrase("No, my ship is called Black Pearl... You scared! Ha, I'm joking!",
																 "Thanks for the advice, I will definitely use it."));
				link.l1.go = "exit";
			}
		break;

		case "SailsGerald":
			ok = (rColony.from_sea == "") || (Pchar.location.from_sea == rColony.from_sea);
			if (sti(Pchar.Ship.Type) != SHIP_NOTUSED && ok)
			{
				/*NextDiag.CurrentNode = NextDiag.TempNode;
				DialogExit();
				npchar.sailsgerald = "gerald";
				LaunchSailsGeraldScreen(npchar);*/
				dialog.text = "On which ship?";
				Link.l1 = "The flagship.";
				Link.l1.go = "SailsTypeChoose_"+1;
				for(int z = 1; z < COMPANION_MAX; z++)
				{
					int cnn = GetCompanionIndex(pchar, z);
					if(cnn != -1)
					{
						string ln = "l"+(z+1);
						Link.(ln) = "The ship ''"+characters[cnn].ship.name+"''";
						Link.(ln).go = "SailsTypeChoose_"+(z+1);
					}
				}
				Link.l9 = "No, I've changed my mind.";
				Link.l9.go = "exit";
				break;
				dialog.text = "It's worth changing the color of the sails " +
							  FindRussianMoneyString(GetSailsTuningPrice(Pchar, npchar, SAILSCOLOR_PRICE_RATE))+
							  "It's worth putting up a new coat of arms " +
							  FindRussianMoneyString(GetSailsTuningPrice(Pchar, npchar, SAILSGERALD_PRICE_RATE)) +
							  ", the price of changing the type of canvas depends on the material.";

				link.l1 = "Choose a new color.";
				link.l1.go = "SailsColorChoose";
				link.l2 = "To depict a new coat of arms.";
				link.l2.go = "SailsGeraldChoose";

				// Паруса ЛГ
				if(!CheckForFlyingDuchmanSails(PChar))
				{
					link.l3 = "Change the material of the sails."; // Материал изменить нельзя, если стоят паруса ЛГ
					link.l3.go = "SailsTypeChoose";
					link.l4 = "I want to set awesome sails like on a legendary ghost ship!";
					link.l4.go = "FlyingDutchmanSails";
				}
				else
				{
					link.l4 = "I want to remove the intimidating sails.";
					link.l4.go = "FlyingDutchmanSails_Clear";
				}

				Link.l9 = "I've changed my mind.";
				Link.l9.go = "exit";
			}
			else
			{
				dialog.text = NPCharRepPhrase(npchar, pcharrepphrase("Are you kidding me? And where is your ship? Pierce doesn't have it!",
																	 "By the devil, you can't fool me! The pier doesn't have your ship!"),
													  pcharrepphrase("I don't see your ship in port, Captain " +GetFullName(pchar)+ ". I hope it's not the Flying Dutchman?",
																	 "Captain, it is more convenient to load and repair the ship in the dock. Dock the ship and come."));
				link.l1 = NPCharRepPhrase(npchar, pcharrepphrase("" +RandSwear()+"What a nuisance!!! Okay, old weasel, I'll see you later!",
																 "I didn't mean to deceive you, " +GetFullName(npchar)+ ", my ship is on the other side of the island."),
												  pcharrepphrase("No, my ship is called Black Pearl... You scared! Ha, I'm joking!",
																 "Thanks for the advice, I will definitely use it."));
				link.l1.go = "exit";
			}
		break;

		// Warship 16.06.09 Паруса ЛГ
		case "FlyingDutchmanSails":
			if(iShipClass < 4)
			{
				iSumm = (10 / iShipClass) * iRank * MOD_SKILL_ENEMY_RATE*3 * 10; // Цена за установку парусов
				dialog.text = "I just came up with an interesting sail geometry: it resembles a ragged one, so that the knippels and balls cause less damage. By the way, it can be sewn from non-woven cloth. I think such a set will be worth it for your ship " + FindRussianMoneyString(iSumm) + ".";

				if(sti(PChar.money) >= iSumm)
				{
					Link.l1 = "Great! It suits me fine.";
					Link.l1.go = "FlyingDutchmanSails_SetDuchman";
				}

				Link.l2 = "What an absurd price! Forget about it.";
				Link.l2.go = "exit";
			}
			else
			{
				dialog.text = "Such sails will look funny on your ship. Find a more impressive ship...";
				Link.l1 = "I'll be looking for somewhere to go...";
				Link.l1.go = "exit";
			}
		break;

		case "FlyingDutchmanSails_SetDuchman":
			iSumm = (10 / iShipClass) * iRank * MOD_SKILL_ENEMY_RATE*3 * 10; // Цена за установку парусов

			AddMoneyToCharacter(PChar, -iSumm);

			dialog.text = "That's great. We will do everything in the best possible way.";
			Link.l1 = "Thank you.";
			Link.l1.go = "exit";

			SetShipSailsFromFile(PChar, "ships/parus_common_torn.dds"); // Паруса ЛГ

			WaitDate("", 0, 0, 0, 0, 10);
		break;

		// Убрать паруса ЛГ
		case "FlyingDutchmanSails_Clear":
			iSumm = (10 / iShipClass) * iRank * MOD_SKILL_ENEMY_RATE*3 * 10; // Цена за установку парусов

			dialog.text = "Well, clean it up so clean it up. It will cost you in " + FindRussianMoneyString(iSumm) + ".";

			if(sti(PChar.money) >= iSumm)
			{
				Link.l1 = "Great! It suits me fine.";
				Link.l1.go = "FlyingDutchmanSails_SetNormal";
			}

			Link.l2 = "What an absurd price! Forget about it.";
			Link.l2.go = "exit";
		break;

		case "FlyingDutchmanSails_SetNormal":
			rRealShip = GetRealShip(sti(PChar.Ship.Type));

			iSumm = (10 / iShipClass) * iRank * MOD_SKILL_ENEMY_RATE*3 * 10; // Цена за установку парусов

			AddMoneyToCharacter(PChar, -iSumm);

			dialog.text = "That's great. We will do everything in the best possible way.";
			Link.l1 = "Thank you.";
			Link.l1.go = "exit";

			DeleteAttribute(rRealShip, "EmblemedSails.normalTex")

			WaitDate("", 0, 0, 0, 0, 10);
		break;

		case "SailsTypeChoosen_selected":
		npchar.sailsgerald = "sails";
		DialogExit();
		LaunchSailsGeraldScreen(npchar,&characters[GetCompanionIndex(pchar, comp-1)]);
		break;

		case "SailsTypeChoose2":
			NPChar.SailsTypeMoney = GetSailsTypePrice(sti(RealShips[sti(Pchar.Ship.Type)].ship.upgrades.sails),
													  sti(NPChar.SailsTypeChooseIDX),
													  stf(NPChar.ShipCostRate),
													  sti(RealShips[sti(Pchar.Ship.Type)].Price));

			dialog.text = "Right now you have sails made of '" + GetSailsType(sti(RealShips[sti(Pchar.Ship.Type)].ship.upgrades.sails)) +
						  "', you chose '"+ GetSailsType(sti(NPChar.SailsTypeChooseIDX))+
						  "'. Replacement costs " + FindRussianMoneyString(sti(NPChar.SailsTypeMoney)) +". Are we changing it?";

			if (sti(NPChar.SailsTypeMoney) <= sti(Pchar.Money))
			{
				Link.l1 = "Yes.";
				Link.l1.go = "SailsTypeChooseDone";
			}
			Link.l99 = "No.";
			Link.l99.go = "exit";
		break;

		case "SailsTypeChooseDone":
			AddMoneyToCharacter(Pchar, -sti(NPChar.SailsTypeMoney));
			dialog.text = "Wonderful! We will do everything in the best possible way.";
			Link.l9 = "Thank you.";
			Link.l9.go = "exit";

			RealShips[sti(Pchar.Ship.Type)].ship.upgrades.sails = sti(NPChar.SailsTypeChooseIDX);
			WaitDate("",0,0,0, 0, 10);
		break;

		case "SailsColorChoose":
			if (GetSailsTuningPrice(Pchar, npchar, SAILSCOLOR_PRICE_RATE) <= sti(Pchar.Money))
			{
				dialog.text = "What color are we putting on? Price " + FindRussianMoneyString(GetSailsTuningPrice(Pchar, npchar, SAILSCOLOR_PRICE_RATE)) + ".";
				for (i = 0; i < SAILS_COLOR_QUANTITY; i++)
				{
					if (CheckSailsColor(Pchar, i))
					{
						attrLoc = "l" + i;
						Link.(attrLoc) = SailsColors[i].name;
						Link.(attrLoc).go = "SailsColorIdx_" + i;
					}
				}
				Link.l99 = "I've changed my mind.";
				Link.l99.go = "exit";
			}
			else
			{
				dialog.text = "Come back when you have the money.";
				Link.l9 = "Good.";
				Link.l9.go = "exit";
			}
		break;

		case "SailsColorDone":
			AddMoneyToCharacter(Pchar, -GetSailsTuningPrice(Pchar, npchar, SAILSCOLOR_PRICE_RATE));
			dialog.text = "Agreed. Painting the sails in "+ GetStrSmallRegister(SailsColors[sti(NPChar.SailsColorIdx)].name) +"the color.";
			Link.l9 = "Thank you.";
			Link.l9.go = "exit";

			SetSailsColor(Pchar, sti(NPChar.SailsColorIdx));
			WaitDate("",0,0,0, 0, 10);
		break;

		// квест украсть чертёж на верфи
		case "ShipyardsMap_1":
			dialog.text = "I need to get hold of an important blueprint. A very important blueprint...";
			link.l1 = "Hmm. What is the blueprint and where can I get it?";
			link.l1.go = "ShipyardsMap_2";
		break;
		case "ShipyardsMap_2":
			pchar.questTemp.different = "ShipyardsMap";
			pchar.questTemp.different.ShipyardsMap = "toTarget";
			pchar.questTemp.different.ShipyardsMap.Id = npchar.id; //квестодатель
			pchar.questTemp.different.ShipyardsMap.city = findShipyardCity(npchar);
			if (pchar.questTemp.different.ShipyardsMap.city == "none")
			{
				dialog.text = "I'm sorry, I made a mistake. I can't offer you this job.";
				link.l1 = "Well, don't!";
				DeleteAttribute(pchar,"questTemp.different.ShipyardsMap");
				break;
			}
			pchar.questTemp.different.ShipyardsMap.what = LinkRandPhrase("corvette", "fluyt", "frigate");
			pchar.questTemp.different.ShipyardsMap.chance = rand(6); //ценность чертежа
			if (rand(12) < GetCharacterSPECIAL(pchar, "Charisma"))
			{
				pchar.questTemp.different.ShipyardsMap.skladFight = false; //не будет драки на складе
			}
			else
			{
				pchar.questTemp.different.ShipyardsMap.skladFight = true; //будет драка на складе
			}
			pchar.questTemp.different.ShipyardsMap.sklad = rand(3)+1; //сколько денег попросят на складе за открытие двери
			pchar.questTemp.different.ShipyardsMap.fightQty = 2 + rand(3); //количество бойцов на верфи
			dialog.text = "It became known to me that at the shipyard " + XI_ConvertString("Colony" + pchar.questTemp.different.ShipyardsMap.city + "Gen") + " the construction of the newest one is underway " + pchar.questTemp.different.ShipyardsMap.what + ". I need the blueprint that the work is being done on.";
			link.l1 = "And what I should do?";
			link.l1.go = "ShipyardsMap_3";
		break;
		case "ShipyardsMap_3":
			dialog.text = "Bring him to me. The problem is that the blueprint, due to its value, is constantly in full view of the owner of the shipyard. Or, to be precise, on his desk.";
			link.l1 = "And what do you suggest?";
			link.l1.go = "ShipyardsMap_4";
		break;
		case "ShipyardsMap_4":
			dialog.text = "Hmm, I suggest you deliver the blueprint to me. How you do it is your problem. I am ready to pay handsomely for this work, although much will depend on the actual value of the information contained in the blueprint.";
			link.l1 = "How do you understand that?";
			link.l1.go = "ShipyardsMap_5";
		break;
		case "ShipyardsMap_5":
			dialog.text = "If the project has new technological solutions that I am not aware of, then the amount will be quite large. Otherwise, the fee will be low... Well, do you agree to take up this case?";
			link.l1 = "No, I'm not interested. Delivering a 'pig in a poke' seems like a thankless job to me.";
			link.l1.go = "ShipyardsMap_disagree";
			link.l2 = "I think I should do it. I'll take it!";
			link.l2.go = "ShipyardsMap_agree";
		break;

		case "ShipyardsMap_disagree":
			dialog.text = "Hmm, well, it's a pity.";
			link.l1 = "Me too...";
			link.l1.go = "exit";
			pchar.questTemp.different = "free";
			DeleteAttribute(pchar, "questTemp.different.ShipyardsMap");
		break;
		case "ShipyardsMap_agree":
			dialog.text = "Well, good luck to you then. I will be waiting for you with the results. You have no time limits.";
			link.l1 = "I see...";
			link.l1.go = "exit";
			sld = ItemsFromID("ShipyardsMap");
			sld.shown = true;
			sld.startLocation = pchar.questTemp.different.ShipyardsMap.city + "_Shipyard";
			sld.startLocator = "item1";
			sld.ShipName = pchar.questTemp.different.ShipyardsMap.what;
			SetTimerFunction("SmallQuests_free", 0, 0, 0); //освобождаем разрешалку на миниквесты
			ReOpenQuestHeader("ShipyardsMap");
			AddQuestRecord("ShipyardsMap", "1");
			AddQuestUserData("ShipyardsMap", "sSex", GetSexPhrase("", "a"));
			AddQuestUserData("ShipyardsMap", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData("ShipyardsMap", "sTarget", XI_ConvertString("Colony" + pchar.questTemp.different.ShipyardsMap.city + "Voc"));
			AddQuestUserData("ShipyardsMap", "sShip", pchar.questTemp.different.ShipyardsMap.what);
		break;

		case "Allarm":
			dialog.text = "You're bastard! What are you doing?! Well, hang on, I'll teach you a lesson. Come on, guys, grab the axes!";
			link.l1 = "Axes?";
			link.l1.go = "fight";
			LAi_SetPlayerType(pchar);
			AddCharacterExpToSkill(pchar, "FencingLight", 10);
			AddCharacterExpToSkill(pchar, "Fencing", 10);
			AddCharacterExpToSkill(pchar, "FencingHeavy", 10);
			ChangeCharacterReputation(pchar, -1);
			for (i=1; i<=sti(pchar.questTemp.different.ShipyardsMap.fightQty); i++)
			{
				iTemp = 10 + rand(10);
				sld = GetCharacter(NPC_GenerateCharacter("shipowner_"+i, "shipowner_"+(rand(15)+1), "man", "man", iTemp, sti(pchar.nation), 0, true));
				FantomMakeCoolFighter(sld, iTemp, 70, 70, "topor"+(rand(2)+1), "pistol2", 10);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, npchar.chr_ai.group);
				ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
			}
		break;

		case "Allarm_1":
			dialog.text = "Are you threatening me?! Come on, guys, grab the axes!";
			link.l1 = "Axes?";
			link.l1.go = "fight";
			ChangeCharacterReputation(pchar, -1);
			for (i=1; i<=5; i++)
			{
				iTemp = 10 + rand(10);
				sld = GetCharacter(NPC_GenerateCharacter("shipowner_"+i, "shipowner_"+(rand(15)+1), "man", "man", iTemp, sti(pchar.nation), 0, true));
				FantomMakeCoolFighter(sld, iTemp, 70, 70, "topor"+(rand(2)+1), "pistol2", 10);
				LAi_SetWarriorType(sld);
				LAi_group_MoveCharacter(sld, npchar.chr_ai.group);
				ChangeCharacterAddressGroup(sld, pchar.location, "reload", "reload1");
			}
		break;

		case "ShipyardsMapOk_1":
			sTemp = pchar.questTemp.different.ShipyardsMap.what;
			dialog.text = "Great! Well, let's see what kind of vaunted one it is " + strcut(sTemp, 0, strlen(sTemp)-2) + "...";
			link.l1 = "Look.";
			link.l1.go = "ShipyardsMapOk_2";
		break;

		case "ShipyardsMapOk_2":
			TakeItemFromCharacter(pchar, "ShipyardsMap");
			if (rand(9)<3)
			{
				GiveItem2Character(pchar, "Ship_Print_5");
				Log_info("You have received an expanded blueprint.");
				dialog.text = "Well, well... hmm, I have to disappoint you. I don't see anything useful in it. You can keep it for yourself.";
				link.l1 = "Are you refusing to pay me to deliver it?";
				link.l1.go = "ShipyardsMapOk_3";
			}
			else
			{
				switch (sti(pchar.questTemp.different.ShipyardsMap.chance))
				{
					case 0:
					dialog.text = "Well, well... hmm, I have to disappoint you. I don't see anything useful for myself. You seem to have picked up the wrong thing.";
					link.l1 = "You ordered what was on the table. That's exactly what I'm going to tell you."+ GetSexPhrase("collective", "if") +". I don't understand these blueprints!";
					link.l1.go = "ShipyardsMapOk_3";
					break;
					case 1:
						dialog.text = "Yes, this blueprint has a certain value. I'm willing to pay you for it " + FindRussianMoneyString(1500 * GetCharacterSPECIALSimple(PChar, SPECIAL_L)) + ". I will make the payment in silver bars. Do you mind?";
						link.l1 = "No, of course not! Great!";
						link.l1.go = "ShipyardsMapOk_5";
						NPChar.reputation = sti(NPChar.reputation) + 2;
						TakeNItems(pchar, "jewelry17", makeint(1500 * GetCharacterSPECIALSimple(PChar, SPECIAL_L)/100));
					break;
					case 2:
						dialog.text = "Yes, this blueprint has a certain value. I'm willing to pay you for it " + FindRussianMoneyString(3000 * GetCharacterSPECIALSimple(PChar, SPECIAL_L)) + ". I will make the payment in gold bars. Do you mind?";
						link.l1 = "No, of course not! Great!";
						link.l1.go = "ShipyardsMapOk_5";
						NPChar.reputation = sti(NPChar.reputation) + 2;
						TakeNItems(pchar, "jewelry5", makeint(3000 * GetCharacterSPECIALSimple(PChar, SPECIAL_L)/200));
					break;
					case 3:
						dialog.text = "Yes, this blueprint has a certain value. I'm willing to pay you for it " + FindRussianMoneyString(4500 * GetCharacterSPECIALSimple(PChar, SPECIAL_L)) + ".";
						link.l1 = "Great!";
						link.l1.go = "ShipyardsMapOk_5";
						NPChar.reputation = sti(NPChar.reputation) + 2;
						AddMoneyToCharacter(pchar, 4500 * GetCharacterSPECIALSimple(PChar, SPECIAL_L));
					break;
					case 4:
						dialog.text = "Yes, this blueprint has considerable value. I'm willing to pay you for it " + FindRussianMoneyString(6000 * GetCharacterSPECIALSimple(PChar, SPECIAL_L)) + ".";
						link.l1 = "Great!";
						link.l1.go = "ShipyardsMapOk_5";
						NPChar.reputation = sti(NPChar.reputation) + 2;
						AddMoneyToCharacter(pchar, 6000 * GetCharacterSPECIALSimple(PChar, SPECIAL_L));
					break;
					case 5:
						dialog.text = "Oh! This is a very, very valuable blueprint! I am ready to pay you 60,000 gold for it. I will make the payment with credit chests. Do you mind?";
						link.l1 = "No, of course not! Great!";
						link.l1.go = "ShipyardsMapOk_5";
						NPChar.reputation = sti(NPChar.reputation) + 5;
						TakeNItems(pchar, "chest", (GetCharacterSPECIALSimple(PChar, SPECIAL_L)+5)/3);//У3 - 2 сундука, У4-6 - 3, У7-9 - 4, У10 - 5
					break;
					case 6:
						dialog.text = "Yes, this blueprint has a certain value. However, I don't have any money available right now, so I can give you one of the chests with craft materials.";
						link.l1 = "Well, if there's nothing else, then let's do it.";
						link.l1.go = "ShipyardsMapOk_5";
						NPChar.reputation = sti(NPChar.reputation) + 2;
						TakeNItems(pchar, "Chest_Craftsmans", 1);
						Log_info("You have received the artisan's chest.")
					break;
				}
				if (sti(NPChar.reputation)>100) NPChar.reputation = 100;
			}
		break;

		case "ShipyardsMapOk_3":
			dialog.text = "Alas, I can't pay anything at all for this. I'm sorry, but I don't need this paper.";
			link.l1 = "Hmm, I don't think it's an honest thing to do, but I won't make a row.";
			link.l1.go = "ShipyardsMapOk_4";
			link.l2 = "Give me my money! Otherwise I'll bleed!";
			link.l2.go = "Allarm_1";
		break;
		case "ShipyardsMapOk_4":
			dialog.text = "Don't get me wrong, you could have safely taken it somewhere in a landfill. This blueprint is worthless.";
			link.l1 = "I understand you...";
			link.l1.go = "exit";
			DeleteAttribute(pchar, "questTemp.different.ShipyardsMap");
			AddQuestRecord("ShipyardsMap", "3");
			AddQuestUserData("ShipyardsMap", "sSex", GetSexPhrase("", "a"));
			AddQuestUserData("ShipyardsMap", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			CloseQuestHeader("ShipyardsMap");

			AchievementsCounter_genquests(1);
		break;

		case "ShipyardsMapOk_5":
			dialog.text = "Please receive it. Thank you for the work you've done.";
			link.l1 = "You're welcome...";
			link.l1.go = "exit";
			AddCharacterExpToSkill(pchar, "Commerce", 20);
			ChangeCharacterReputation(pchar, 5);
			AddMoneyToCharacter(pchar, sti(pchar.questTemp.different.ShipyardsMap.chance)*1000);
			AddQuestRecord("ShipyardsMap", "4");
			AddQuestUserData("ShipyardsMap", "sSex", GetSexPhrase("", "a"));
			AddQuestUserData("ShipyardsMap", "sCity", XI_ConvertString("Colony" + npchar.city + "Gen"));
			AddQuestUserData("ShipyardsMap", "iMoney", sti(pchar.questTemp.different.ShipyardsMap.chance)*1000);
			CloseQuestHeader("ShipyardsMap");

			AchievementsCounter_genquests(1);

			DeleteAttribute(pchar, "questTemp.different.ShipyardsMap");
		break;

		////////////////////////////////////////////////homo линейка Блада/////////////////////////////////////////
		case "Blood_Shipyard1":
			dialog.text = "Ahem, well, it depends on who your friend is and why he needs to be in Porto Bello, Doctor.";
			link.l1 = "This one... My friend, he is... She is a beautiful young lady... Hmm, I forgot her name - it's so forgettable...";
			link.l1.go = "Blood_Shipyard2";
			link.l2 = "My dear friend is a filibuster who recently lost his ship in an attack on a military frigate.";
			link.l2.go = "Blood_Shipyard3";
			link.l3 = "You've heard of him. He's a privateer... He calls himself Rolando Pizarro - the Terror of the Seven Seas and so on, so on...";
			link.l3.go = "Blood_Shipyard4";
		break;

		case "Blood_Shipyard2":
			dialog.text = "Young lady? Your friend, who is actually a young lady with an expressionless name, wants to get to Porto Bello? Are you kidding me, Doctor?";
			link.l1 = "I had no idea, sir! I'm just, uh, trying to help. Well, him... By her, I mean. A friend. Yes.";
			link.l1.go = "Blood_Shipyard5";

		break;
		case "Blood_Shipyard3":
			dialog.text = "And where did this happen?";
			link.l1 = "It was... Ehh, anchor me in the..! I forgot it.";
			link.l1.go = "exit";
			CloseQuestHeader("PirateQuest");
			Pchar.questTemp.CapBloodLine.ShipForJack = false;

		break;
		case "Blood_Shipyard4":
			dialog.text = "You mean a former privateer... Isn't that right?";
			link.l1 = "Uh... well, yes... ex. Do you know him?";
			link.l1.go = "Blood_Shipyard6";
		break;

		case "Blood_Shipyard5":
			dialog.text = "Well, I'll believe you this time. Somewhere in the city there is a man named John Miner. Talk to him, he just bought a corvette from me.";
			link.l1 = "Thank you very much.";
			link.l1.go = "Exit";
			LoginMainer();
			AddQuestRecord("PirateQuest", "2");
		break;

		case "Blood_Shipyard6":
			dialog.text = "As you correctly noted, I have heard about him. I don't know who Rolando Pizarro was before he showed up at the Brethren. But I remember his ship Dancing with Wolves well. It's a beautiful name, and a beautiful ship... It was.";
			link.l1 = "What?! He didn't say a word about it... in our conversation, I mean.";
			link.l1.go = "Blood_Shipyard7";
		break;

		case "Blood_Shipyard7":
			dialog.text = "Now he doesn't loosen his tongue - life has taught him a lesson. After grabbing a couple of rich prizes, he began to rush to the top of the Brethren, trying to sideline Morgan himself. Well, you know - this idea of Sharp's Libertania and all the rest. He talked a lot and beautifully, and, of course, there were people who believed in these nonsense and followed him... Who knew that everything would turn out this way...";
			link.l1 = "Was he betrayed? Was there a riot?";
			link.l1.go = "Blood_Shipyard8";
		break;

		case "Blood_Shipyard8":
			dialog.text = "No, everything happened much more banal... Just like in real life. Pizarro was not going to create any Libertania - he just wanted to sit in a beautiful chair in his own mansion in Port Royal and watch young privateers tremble and fawn before him. Well, you know - this is the eternal desire to punish and pardon at your discretion, to give out ranks or a lead thaler in your heart... And, of course, fame... That's what we all want, isn't it? Only now the essence of it has been revealed. Morgan actually wanted to hang him right there - you probably heard - he hates the rebels. So Pizarro was captured warm right in the cabin, as soon as Morgan received all the evidence.";
			link.l1 = "It's a very informative story. I'll have to offer it to some novelist. But, what about the ship for that, um... truth-lover?";
			link.l1.go = "Blood_Shipyard9";
			link.l2 = "And how did Rolando manage to escape?";
			link.l2.go = "Blood_Shipyard10";
		break;

		case "Blood_Shipyard9":
			dialog.text = "Find a man named John Miner in the city, he just bought a corvette. I think a passenger to Porto Bello will not be superfluous to him.";
			link.l1 = "Thank you very much.";
			link.l1.go = "Exit";
			LoginMainer();
			AddQuestRecord("PirateQuest", "2");
		break;

		case "Blood_Shipyard10":
			dialog.text = "Morgan usually has a short trial-no longer than the powder burns on the shelf of his pistol. But, he held out something with this 'people's favorite'... so Pizarro escaped to the Spaniards. He slipped right out from under the admiral's nose, confirming that the Fox is not only a cunning creature, but also a nimble one.";
			link.l1 = "Hmm, but how did he trick the admiral of the Brethren? Well, maybe it will be useful for the future.";
			link.l1.go = "Blood_Shipyard11";
		break;

		case "Blood_Shipyard11":
			dialog.text = "I don't think this is the way to go about it, but if you like, he's been in Port Royal for three days and three nights, and then one of his associates manages to hand him a woman's dress. And a certain Lady Taylor safely left the fort, reporting to the commandant that her beloved Pizarro was being treated with dignity. Next, this lady boarded a ship in the port... and the fox escaped from the cage. He escaped, and then reappeared. Except the Brethren already knew the value of it, and even the Admiral isn't looking for it anymore... probably doesn't want to get his hands dirty.";
			link.l1 = "So, as I understand it, there is no point in handing him over to the authorities, since no one is looking for him. Very instructive. But how do I get him to Porto Bello?";
			link.l1.go = "Blood_Shipyard12";
		break;

		case "Blood_Shipyard12":
			dialog.text = "You need Captain Miner, John Miner-he is engaged in equipping the newly acquired corvette. Look for him in the city.";
			link.l1 = "Thank you very much.";
			link.l1.go = "Exit";
			LoginMainer();
			AddQuestRecord("PirateQuest", "3");
		break;

	}
}

string findShipyardCity(ref NPChar)
{
	int n, nation;
	int storeArray[MAX_COLONIES];
	int howStore = 0;

	for(n=0; n<MAX_COLONIES; n++)
	{
		nation = GetNationRelation(sti(npchar.nation), sti(colonies[n].nation));
		if (sti(colonies[n].nation) != PIRATE && colonies[n].id != "Panama" && colonies[n].id != "DesMoines" && colonies[n].nation != "none" && GetIslandByCityName(npchar.city) != colonies[n].islandLable) //здесь прописываем города, где нет верфи
		{
			if (GetCharacterIndex(colonies[n].id + "_shipyarder") != -1)
			{
				storeArray[howStore] = n;
				howStore++;
			}
		}
	}
	if (howStore == 0) return "none";
	nation = storeArray[rand(howStore-1)];
	return colonies[nation].id;
}

// проверка какой уже цвет есть
bool CheckSailsColor(ref chr, int col)
{
	int	st = GetCharacterShipType(chr);
	ref	shref;

	if (st != SHIP_NOTUSED)
	{
		shref = GetRealShip(st);
		st = -1;
		if (!CheckAttribute(shref, "ShipSails.SailsColor"))  st = 0; // нету - это белый
		if (CheckAttribute(shref, "SailsColorIdx"))  st = sti(shref.SailsColorIdx);
		if (st == col) return false;
	}
	return true;
}

bool CheckSailsGerald(ref chr)
{
	int	st = GetCharacterShipType(chr);
	ref	shref;
	if (st != SHIP_NOTUSED)
	{
		shref = GetRealShip(st);
		if (CheckAttribute(shref, "GeraldSails"))  return true;
	}
	return false;
}

string GetSailsType(int _type)
{
	switch (_type)
	{
		case 1 : return "Hemp";  break;
		case 2 : return "Flax";	 break;
		case 3 : return "Cotton";  break;
		case 4 : return "Black canvas";  break;
	}
	return "Hemp";
}

int GetSailsTypePrice(int _asis, int _tobe, float _shipCostRate, int _price)
{
	int ret;
	ret = _tobe - _asis;

	if (ret < 0) ret = 0;

	return makeint((ret*_price*0.05 + _price*0.05)*_shipCostRate / 10) * 10;
}

// Warship Ghdjthrf на паруса ЛГ
bool CheckForFlyingDuchmanSails(ref _char)
{
	ref realShip;
	int shipType = sti(_char.Ship.Type);

	if(shipType == SHIP_NOTUSED) return false;

	realShip = GetRealShip(shipType);

	if(CheckAttribute(realShip, "EmblemedSails.normalTex") && realShip.EmblemedSails.normalTex == "ships/parus_common_torn.dds")
	{
		return true;
	}

	return false;
}

void SelectFindship_ShipType(int iNation)
{
	int iRank, pcharRank;
	pcharRank = sti(pchar.rank);
	iRank = 6;
	if (pcharRank > 4) iRank = 5;
	if (pcharRank > 9) iRank = 4;
	if (pcharRank > 14) iRank = 3;
	if (pcharRank > 19) iRank = 2;
	if (pcharRank > 25) iRank = 1;
	pchar.GenQuest.Findship.Shipyarder.ShipType = GetShipTypeExtNotNation(iRank, iRank, "", iNation);
}


void DelBakSkill(ref _compref)
{
	DelBakSkillAttr(pchar);
	ClearCharacterExpRate(pchar);
	RefreshCharacterSkillExpRate(pchar);
	SetEnergyToCharacter(pchar);

	DelBakSkillAttr(_compref);
	ClearCharacterExpRate(_compref);
	RefreshCharacterSkillExpRate(_compref);
	SetEnergyToCharacter(_compref);
}

string GiveOrderMatherial(ref NPChar)
{
	int amount;
	string sGood1, sItem2;
	string sLeft = "";
	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	for (int k=1;k<10;k++)
	{
		sGood1 = g_ShipBermudeTuningGoods[k];
		amount = GetSquadronGoods(Pchar, FindGood(sGood1)) - sti(NPChar.questtemp.(sGood1));
		if (amount < 0) amount = amount + sti(NPChar.questtemp.(sGood1)); else amount = sti(NPChar.questtemp.(sGood1));
		RemoveCharacterGoods(Pchar, FindGood(sGood1), amount);
		NPChar.questtemp.(sGood1) = sti(NPChar.questtemp.(sGood1)) - amount;
		amount = sti(NPChar.questtemp.(sGood1));
		if (amount > 0) sLeft += NewStr() + XI_ConvertString(sGood1) + " - " + amount + "pcs., ";

		sItem2 = g_ShipBermudeTuningItems[k];
		amount = GetCharacterItemWithCabin(pchar, sItem2, true) - sti(NPChar.questtemp.(sItem2));
		if (amount < 0) amount = amount + sti(NPChar.questtemp.(sItem2)); else amount = sti(NPChar.questtemp.(sItem2));
		TakeNItemsWithCabin(pchar, sItem2, -amount, false);
		NPChar.questtemp.(sItem2) = sti(NPChar.questtemp.(sItem2)) - amount;
		amount = sti(NPChar.questtemp.(sItem2));
		if (amount > 0) sLeft += NewStr() + LanguageConvertString(idLngFile, Items[FindItem(sItem2)].name) + " - " + amount + "pcs., ";
	}
		sItem2 = "chest";
		amount = GetCharacterItemWithCabin(pchar, sItem2, true) - sti(NPChar.questtemp.(sItem2));
		if (amount < 0) amount = amount + sti(NPChar.questtemp.(sItem2)); else amount = sti(NPChar.questtemp.(sItem2));
		TakeNItemsWithCabin(pchar, sItem2, -amount, false);
		NPChar.questtemp.(sItem2) = sti(NPChar.questtemp.(sItem2)) - amount;
		amount = sti(NPChar.questtemp.(sItem2));
		if (amount > 0) sLeft += NewStr() + "Chests of coins-" + amount + "pcs.";

	return sLeft; //возвращаем строку со списком оставшегося, или строка пустая
}

bool CheckOrderMatherial(ref NPChar)
{
	int amount;
	string sGood1, sItem2;
	string sLeft = "";
	int idLngFile = LanguageOpenFile("ItemsDescribe.txt");

	for (int k=1;k<10;k++)
	{
		sGood1 = g_ShipBermudeTuningGoods[k];
		if (sti(NPChar.questtemp.(sGood1)) > 0 && GetSquadronGoods(pchar, FindGood(sGood1)) > 0) return true;

		sItem2 = g_ShipBermudeTuningItems[k];
		if (sti(NPChar.questtemp.(sItem2)) > 0 && GetCharacterItem(pchar, sItem2) > 0) return true;
	}
		sItem2 = "chest";
		if (sti(NPChar.questtemp.(sItem2)) > 0 && GetCharacterItem(pchar, sItem2) > 0) return true;

	return false; //ничего не привезли
}
