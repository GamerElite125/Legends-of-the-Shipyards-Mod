// диалог по городам
// Это мастер тюнинга
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    if ( sti(Pchar.Ship.Type) != SHIP_NOTUSED && !S_armor(Pchar))
    {
	    int   	cannonQ       = sti(RealShips[sti(Pchar.Ship.Type)].CannonsQuantity);
		int   	cannonMax     = sti(RealShips[sti(Pchar.Ship.Type)].MaxCaliber);
		int		cannonMaxQ    = 0;
		if (CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "CannonsQuantityMax")) cannonMaxQ = sti(RealShips[sti(Pchar.Ship.Type)].CannonsQuantityMax);
		int   	shipClass     = sti(RealShips[sti(Pchar.Ship.Type)].Class);
		//int   shipPrice     = sti(RealShips[sti(Pchar.Ship.Type)].Price);
		int   	shipCapacity  = sti(RealShips[sti(Pchar.Ship.Type)].Capacity);
		float 	shipSpeedRate = stf(RealShips[sti(Pchar.Ship.Type)].SpeedRate);
		float 	MastMulti     = stf(RealShips[sti(Pchar.Ship.Type)].MastMultiplier);
		int   	shipMaxCrew   = sti(RealShips[sti(Pchar.Ship.Type)].MaxCrew);
		int   	shipHP        = sti(RealShips[sti(Pchar.Ship.Type)].HP);
		float 	shipTurnRate  = stf(RealShips[sti(Pchar.Ship.Type)].TurnRate);
		float 	shipWindAgainst = stf(RealShips[sti(Pchar.Ship.Type)].WindAgainstSpeed);
		int		cannonDiff		= cannonMaxQ - cannonQ;
		int		hullArmor		= sti(RealShips[sti(Pchar.Ship.Type)].HullArmor);
		int 	iCaliber;

/*
		if(cannonDiff >= 2)
		{
			cannonDiff = 2;
			cannonMaxQ = cannonQ + cannonDiff;
		}
		else
		{
			cannonDiff = 0;
		}
*/
		float fBasePrice = 0.0;
		if(shipClass <= 4){ fBasePrice = stf(RealShips[sti(Pchar.Ship.Type)].Price) / 20 * MOD_SKILL_ENEMY_RATE*3;}

		//int  zWorkPrice  = makeint(8000 * ((7-shipClass) * MOD_SKILL_ENEMY_RATE*3) * fQuestShip);
		int sundukSum = makeint((7-shipClass)*1.0);

	    int  cannonMatherial1 = makeint(cannonQ * 2 + 35 * (7-shipClass) - 20);
	    int	 cannonMatherial2 = makeint((7-shipClass)/2);
		if (cannonMatherial2 < 1) cannonMatherial2 = 1;
	    int	 cannonMatherial3 = sundukSum;
		int  cannonWorkPrice  = makeint(100 * cannonQ * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

	    int  capacityMatherial1 = makeint(35 * (7-shipClass) + shipCapacity * 7/100);
	    int	 capacityMatherial2 = makeint(10 * (7-shipClass)+10);
	    int  capacityMatherial3 = sundukSum;
		int  capacityWorkPrice  = makeint(2 * shipCapacity * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

	    int  SpeedRateMatherial1 = makeint(shipHP * 25/1000 + 70 * (7-shipClass));
	    int	 SpeedRateMatherial2 = makeint(5 * (7-shipClass));
	    int	 SpeedRateMatherial3 = sundukSum;
		int  SpeedRateWorkPrice  = makeint(100 * shipSpeedRate * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

		int  MastMultiplierMatherial1 = makeint(shipHP * 25/1000 + 70 * (7-shipClass));
	    int	 MastMultiplierMatherial2 = makeint(10 * (7-shipClass));
	    int	 MastMultiplierMatherial3 = sundukSum;
		int  MastMultiplierMatherialWorkPrice  = makeint(100 * MastMulti * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

	    int  MaxCrewMatherial1 = makeint(shipMaxCrew * 6/10 + 50 * (7-shipClass));
	    int	 MaxCrewMatherial2 = makeint(6 * (7-shipClass)+3);
	    int	 MaxCrewMatherial3 = sundukSum;
		int  MaxCrewWorkPrice  =  makeint(15 * shipMaxCrew * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

	    int  TurnRateMatherial1 = makeint(shipHP * 25/1000 + 375/shipClass+20);
	    int	 TurnRateMatherial2 = makeint(5 * (7-shipClass)+2);
	    int	 TurnRateMatherial3 = sundukSum;
		int  TurnRateWorkPrice  = makeint(30 * shipTurnRate * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

	    int  HPMatherial1 = makeint(35 * (7-shipClass) + shipHP * 7/100);
	    int	 HPMatherial2 = makeint(5 * (7-shipClass)*3);
	    int	 HPMatherial3 = sundukSum;
		int  HPWorkPrice  = makeint(shipHP * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

	    int  WindAgainstMatherial1 = makeint(shipHP * 25/1000 + 750/shipClass+20);
	    int	 WindAgainstMatherial2 = makeint(6 * (7-shipClass)+1);
	    int	 WindAgainstMatherial3 = sundukSum;
		int  WindAgainstWorkPrice  = makeint(300 * shipWindAgainst * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
		
		int  HullArmorMaterial1 = makeint(shipHP * 25/1000 + 750/shipClass+20);
	    int	 HullArmorMaterial2 = makeint(20 * (7-shipClass)+1);
	    int	 HullArmorMaterial3 = sundukSum;
		int  HullArmorPrice  = makeint(300 * shipSpeedRate * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);

		int quantityMatherial1 = makeint(cannonQ * 2 + 35 * (7-shipClass) - 20);
		int quantityMatherial2 = makeint((7-shipClass)/2);
			if (quantityMatherial2 < 1) quantityMatherial2 = 1;
		int quantityMatherial3 = sundukSum;
		int cannonQWorkPrice = makeint(100 * cannonMaxQ * MOD_SKILL_ENEMY_RATE*3 + fBasePrice);
		int AllMatherial1 = cannonMatherial1 + quantityMatherial1;
		int AllMatherial2 = cannonMatherial2 + quantityMatherial2;
		int AllMatherial3 = cannonMatherial3 + quantityMatherial3;
		int cannonAllWorkPrice = cannonWorkPrice + cannonQWorkPrice;
		//int  zWorkPriceAll  = zWorkPrice * 2;


		float discount = 1.0;
  		if (CheckAttribute(NPChar, "Discount"))
		{
		    discount = 0.75;
		}

	    cannonMatherial1 = cannonMatherial1 * discount;
	    cannonMatherial2 = cannonMatherial2 * discount;
		cannonMatherial3 = cannonMatherial3 * discount;
		if (cannonMatherial3 < 1) cannonMatherial3 = 1;
	    cannonWorkPrice  = cannonWorkPrice * discount;

		quantityMatherial1 = quantityMatherial1 * discount;
		quantityMatherial2 = quantityMatherial2 * discount;
		quantityMatherial3 = quantityMatherial3 * discount;
		if (quantityMatherial3 < 1) quantityMatherial3 = 1;
		cannonQWorkPrice = cannonQWorkPrice * discount;

		AllMatherial1 = AllMatherial1 * discount;
		AllMatherial2 = AllMatherial2 * discount;
		AllMatherial3 = AllMatherial3 * discount;
		if (AllMatherial3 < 1) AllMatherial3 = 1;
		cannonAllWorkPrice = cannonAllWorkPrice * discount;

	    capacityMatherial1 = capacityMatherial1 * discount;
	    capacityMatherial2 = capacityMatherial2 * discount;
		capacityMatherial3 = capacityMatherial3 * discount;
		if (capacityMatherial3 < 1) capacityMatherial3 = 1;
	    capacityWorkPrice  = capacityWorkPrice * discount;

	    SpeedRateMatherial1 = SpeedRateMatherial1 * discount;
	    SpeedRateMatherial2 = SpeedRateMatherial2 * discount;
		SpeedRateMatherial3 = SpeedRateMatherial3 * discount;
		if (SpeedRateMatherial3 < 1) SpeedRateMatherial3 = 1;
	    SpeedRateWorkPrice  = SpeedRateWorkPrice * discount;

		MastMultiplierMatherial1 = MastMultiplierMatherial1 * discount;
	    MastMultiplierMatherial2 = MastMultiplierMatherial2 * discount;
		MastMultiplierMatherial3 = MastMultiplierMatherial3 * discount;
		if (MastMultiplierMatherial3 < 1) MastMultiplierMatherial3 = 1;
	    MastMultiplierMatherialWorkPrice  = MastMultiplierMatherialWorkPrice * discount;

	    MaxCrewMatherial1 = MaxCrewMatherial1 * discount;
	    MaxCrewMatherial2 = MaxCrewMatherial2 * discount;
		MaxCrewMatherial3 = MaxCrewMatherial3 * discount;
		if (MaxCrewMatherial3 < 1) MaxCrewMatherial3 = 1;
	    MaxCrewWorkPrice  = MaxCrewWorkPrice * discount;

	    TurnRateMatherial1 = TurnRateMatherial1 * discount;
	    TurnRateMatherial2 = TurnRateMatherial2 * discount;
		TurnRateMatherial3 = TurnRateMatherial3 * discount;
		if (TurnRateMatherial3 < 1) TurnRateMatherial3 = 1;
	    TurnRateWorkPrice  = TurnRateWorkPrice * discount;

	    HPMatherial1 = HPMatherial1 * discount;
	    HPMatherial2 = HPMatherial2 * discount;
		HPMatherial3 = HPMatherial3 * discount;
		if (HPMatherial3 < 1) HPMatherial3 = 1;
	    HPWorkPrice  = HPWorkPrice * discount;

	    WindAgainstMatherial1 = WindAgainstMatherial1 * discount;
	    WindAgainstMatherial2 = WindAgainstMatherial2 * discount;
		WindAgainstMatherial3 = WindAgainstMatherial3 * discount;
		if (WindAgainstMatherial3 < 1) WindAgainstMatherial3 = 1;
	    WindAgainstWorkPrice  = WindAgainstWorkPrice * discount;
		
		HullArmorMaterial1 = HullArmorMaterial1 * discount;
	    HullArmorMaterial2 = HullArmorMaterial2 * discount;
		HullArmorMaterial3 = HullArmorMaterial3 * discount;
		if (HullArmorMaterial3 < 1) HullArmorMaterial3 = 1;
	    HullArmorPrice  = HullArmorPrice * discount;

		if (cannonMatherial2 < 1) cannonMatherial2 = 1;
		if (quantityMatherial2 < 1) quantityMatherial2 = 1;
	}

	int i;
	int iTemp;
	string attr, attrLoc;
	int amount;
	ref shTo;
	string sTemp;
	string s1;
	
	string sGood = "";
	aref aroot, arcur;
	int num, gooda, iGood, SG;

	switch (Dialog.CurrentNode)
	{

		// -----------------------------------Диалог первый - первая встреча
		case "First time":  // перехват основного диалога, только один раз
			if(NPChar.quest.meeting == "0")
			{
				Dialog.Text = "Huh... What a bad climate... My name is " + GetFullName(npchar) + "and, damn me, I'm the best shipbuilder on these godforsaken islands. But this dampness just knocks me down...";
				Link.l1 = pcharrepphrase("A thousand sharks, only senile grumbling was not enough for me. My name is " + GetFullName(pchar) + ", I think you've heard of me.",
					"It's not damp, it's old age, dear. My name is " + GetFullName(pchar) + ", I think you've heard of me.");
				Link.l1.go = "node_1";
				npchar.quest.meeting = "1";
			}
			else // нужно, если по квестам будет сброс на  "First time"
			{
				dialog.text = "Ah, it's you again, again with your problems... kha... kha.";
				link.l1 = pcharrepphrase("You're right, it's me again. But let's talk about business before you give your soul to God.",
					"You have a good memory, and now, if I may, let's move on to more specific questions.");
				link.l1.go = "node_3";
				link.l2 = pcharrepphrase("You've tired me out with your grumbling. Goodbye.",
					"I have to go, sorry to bother you..");
				link.l2.go = "exit";
			}
			NextDiag.TempNode = "Master_Ship";
			Dialog.CurrentNode = "Master_Ship";
		break;

		case "Master_Ship":
			dialog.text = "Ah, it's you again, again with your problems... kha... kha.";
			link.l1 = pcharrepphrase("You're right, it's me again. But let's talk about business before you give your soul to God.",
	            "You have a good memory, and now, if I may, let's move on to more specific questions.");
			link.l1.go = "node_3";
			link.l2 = pcharrepphrase("You've tired me out with your grumbling. Goodbye.",
				"I have to go, sorry to bother you..");
			link.l2.go = "exit";
			NextDiag.TempNode = "Master_Ship";
		break;

		case "node_1":
			dialog.text = "Young people, young people, you will not get any respect from you. Hotheads, strong hands, what more do you need?";
			link.l1 = pcharrepphrase("That's right. But sooner or later everything comes to an end, and it looks like your time is near...",
				"Something tells me that you didn't have much respect for age in your younger years either. Isn't that right?");
			link.l1.go = "node_2";
		break;

		case "node_2":
			dialog.text = "Don't contradict the elders, "+ GetSexPhrase("the puppy", "brat") +"!";
			link.l1 = pcharrepphrase("Okay, okay, don't get puffed up, or you'll have a stroke. I don't want to be blamed for the death of the best shipbuilder in the Caribbean. Let's get down to business.",
				"I forgive you for this harshness, only out of respect for your age. Now, if you're done, let's get down to business.");
			link.l1.go = "node_3";
			link.l2 = pcharrepphrase("That's it, I've had enough of moralizing! Goodbye!",
				"I see that we will not find a common language. Goodbye.");
			link.l2.go = "exit";
		break;

		case "node_3":
			dialog.text = "Kha, kha... They don't give the old man any peace. What do you want from me again, " + GetAddress_Form(NPChar) + "? ";
			link.l1 = pcharrepphrase("I need to use the shipyard's services.", "I want to use the services of your shipyard.");
			link.l1.go = "shipyard_dlg";
            Link.l2 = "I have a question.";
			Link.l2.go = "quests"; //(пересылка в файл города)
			// -->
			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
			{
				link.l3 = "I wanted to talk to you about financial matters.";
				link.l3.go = "LoanForAll";//(перессылка в файл города)
			}
			link.l9 = pcharrepphrase("Damn, I have a couple of urgent things to do, goodbye.", "I have to go now, I'm sorry.");
			link.l9.go = "exit";
			NextDiag.TempNode = "Master_Ship";
		break;

		case "shipyard_dlg":
			dialog.Text = "I can offer work with piece orders - I improve the characteristics of local tubs. They don't know how to build normal ships here at all.";
			link.l1 = "No, I don't need improvements, I just want to use the shipyard's services.";
			link.l1.go = "shipyard";

			if (!CheckAttribute(npchar, "questTemp.ShipOrderTime"))
			{
				link.l5 = "Can I order a ship from your shipyard that suits my personal preferences?";
				link.l5.go = "Shipyard1";
			}
			else
			{
				link.l5 = "Is the ship I ordered ready?";
				link.l5.go = "shiporder1";
			}
			Link.l2 = "Wonderful! I just need to get my boat ready. Can you take a look at what can be done?";
			Link.l2.go = "ship_tunning_start";
			if (sti(RealShips[sti(Pchar.Ship.Type)].class) < 7)
			{
				Link.l22 = "What about upgrading my ship?";
				Link.l22.go = "ship_upgrades_start";
			}
			if ((RealShips[sti(Pchar.Ship.Type)].name == "Flyingdutchman1") && (pchar.location.from_sea == "Pirates_town"))
			{
				link.l7 = "Master, I have a special boat here...";
				link.l7.go = "DutchmanRepair1";
			}

			link.l3 = "I need guns for the ship.";
			link.l3.go = "Cannons";
			link.l4 = "I want to change the appearance of the ship.";
			link.l4.go = "SailsGerald";
			Link.l9 = "I can't agree with you. There are sometimes very good ships in these waters. Goodbye.";
			Link.l9.go = "ship_tunning_not_now";
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
				Link.l1 = "So, what is it, Master?";
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
				dialog.Text = "All goods are here, let's do these upgrades now...";
				Link.l1 = "Perfect, I hope it wouldn't take long.";
				Link.l1.go = "ship_upgrades_do";
			}
			else 
			{
				dialog.Text = "According to my records, you still have some goods to get. Can't wait to start, so go get them faster...";
				Link.l1 = "Working as hard as I can, Master!";
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
			Link.l1 = "Thanks for your hard work, Master. Bye!";
			Link.l1.go = "ship_upgrades_exit";
		break;
		
		case "ship_upgrades_exit":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

		case "Discount_1":
			dialog.Text = "Cheaper? Hmm, I think I'm willing to give a 20 persents discount on one refinement, but only to a professional who knows the ship business.";
			Link.l1 = "Dear sir, you have a professional captain in front of you, and I know my ship from keel to hood. Let's get a discount!";
			Link.l1.go = "Discount_2";
			Link.l9 = "Okay... perhaps we'll have to do without the discount.";
			Link.l9.go = "ship_tunning_start";
		break;

		case "Discount_2":
			dialog.Text = "A professional captain, you say? Well, we'll check it out now!";
			Link.l1 = "As much as you like!";
			Link.l1.go = "Discount_3";
			LockControl("DlgDown3", true);
			LockControl("DlgUp3", true);
		break;

  		case "Discount_3":
            NPChar.SeaQuestions = rand(sti(NullCharacter.SeaQuestions));
            sTemp = sTemp = "q" + NPChar.SeaQuestions;
			dialog.Text = NullCharacter.SeaQuestions.(sTemp);

   			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Discount_4";
			Link.l9 = "I guess I'll have to do without the discount.";
			Link.l9.go = "ship_tunning_not_now";
		break;

		case "Discount_4":
   			sTemp = "a" + NPChar.SeaQuestions;
			if (NullCharacter.SeaQuestions.(sTemp) == GetStrSmallRegister(dialogEditStrings[1]))
			{
				dialog.text = "That's right!! A really knowledgeable captain"+ GetSexPhrase(".", ", even though a woman") +". You deserve a discount!";
				link.l1 = "I told you that I know. To the point.";
				link.l1.go = "ship_tunning_start";
				NPChar.Discount = true;
				LockControl("DlgDown3", false);
				LockControl("DlgUp3", false);
			}
			else
			{
				dialog.text = "No, it's not true. Maybe it's worth trying again?";
				link.l1 = "Yes, I'll try again.";
				link.l1.go = "Discount_5";
				link.l2 = "No, this question is too complicated for me.";
				link.l2.go = "ship_tunning_not_now";
			}
		break;

		case "Discount_5":
            sTemp = sTemp = "q" + NPChar.SeaQuestions;
			dialog.Text = NullCharacter.SeaQuestions.(sTemp);
   			Link.l1.edit = 1;
			Link.l1 = "";
			Link.l1.go = "Discount_4";
			Link.l9 = "No, I don't think I can do it.";
			Link.l9.go = "ship_tunning_not_now";
		break;

		case "ship_tunning_again":
			dialog.Text = RandPhraseSimple("A... It's you again... Have you decided to fix your boat again?",
			RandPhraseSimple("Greetings, Captain. Are you not satisfied with the boat again?",
                                           "Eh... You don't have a minute's peace, you don't let the old man concentrate.. Did you need improvements again?"));
			Link.l1 = "yeah. There is such a thing. Can we see what else we can do?";
			Link.l1.go = "ship_tunning_start";
			link.l2 = "No, I don't need improvements, I just want to use the shipyard's services.";
			link.l2.go = "shipyard";

			if (!CheckAttribute(npchar, "questTemp.ShipOrderTime"))
			{
				link.l6 = "Can I order a ship from your shipyard that suits my personal preferences?";
				link.l6.go = "Shipyard1";
			}
			else
			{
				link.l6 = "Is the ship I ordered ready?";
				link.l6.go = "shiporder1";
			}

			if ((RealShips[sti(Pchar.Ship.Type)].name == "Flyingdutchman1") && (pchar.location.from_sea == "Pirates_town"))
			{
				link.l7 = "Master, I have a special boat here...";
				link.l7.go = "DutchmanRepair1";
			}

			link.l3 = "I need guns for the ship.";
			link.l3.go = "Cannons";
			link.l31 = "I want to change the appearance of the ship.";
			link.l31.go = "SailsGerald";
            Link.l4 = "I have a question.";
			Link.l4.go = "quests"; //(пересылка в файл города)
			// -->
			if (CheckAttribute(pchar, "GenQuest.LoanChest.TakeChest") && sti(pchar.GenQuest.LoanChest.TargetIdx) == sti(NPChar.index))
			{
				link.l5 = "I wanted to talk to you about financial matters.";
				link.l5.go = "LoanForAll";//(пересылка в файл города)
			}
			Link.l9 = "no. I'm satisfied with what I have.";
			Link.l9.go = "ship_tunning_not_now_2";
		break;

        case "ship_tunning_not_now_2":
			dialog.Text = "Yes, I see you're also thinking! You'll go far! Come when you decide and don't distract me for nothing.";
			Link.l1 = RandPhraseSimple("Thank you! Have a nice day.", "Bye, master. You can do it without sarcasm.");
			Link.l1.go = "ship_tunning_not_now";
		break;

        case "ship_tunning_start":
			if ( sti(Pchar.Ship.Type) == SHIP_NOTUSED || Pchar.location.from_sea != "Pirates_town")
            {
                dialog.Text = "Where is the ship? What are you messing with my head for?!";
			    Link.l1 = "And that's true.. That it's me... I'm sorry.";
			    Link.l1.go = "ship_tunning_not_now";
            }
            else
            {
			    if (GetHour() == 13)
			    {
					dialog.Text = "I'm a workaholic, of course, but I also have lunch time. Come back later!";
			        Link.l1 = "I see...";
			        Link.l1.go = "ship_tunning_not_now";
			    }
			    else
			    {
                    if (shipClass == 7)
                    {
						dialog.Text = "Uh.. I don't do boats. The trough will remain a trough, no matter how much you conjure over it.";
    			        Link.l1 = "I see...";
    			        Link.l1.go = "ship_tunning_not_now";
    			        break;
                    }
					if (S_armor(pchar)) // СФ
					{
						dialog.text = NPCStringReactionRepeat("Let's see what we got here. No... for this. I'm not gonna do that.", "I don't understand the technology on your ship at all. How can I improve it?", "I've told you everything. I'm not taking this boat.", 
                          				"That's it! I'm out of patience! Don't you have any other questions?!", "block", 2, npchar, Dialog.CurrentNode);
						link.l1 = HeroStringReactionRepeat("Hmm. Ok...", "Indeed...",
                      					"Hmmm, however...", "Wen pach?..", npchar, Dialog.CurrentNode);
						Link.l1.go = "ship_tunning_not_now";
						break;
    					}
                    s1 = "Well, well... Let's see what we have here... Yeah " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) + ".";
    			    if (sti(RealShips[sti(Pchar.Ship.Type)].Stolen) == true && !CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning"))
    			    {
    			        s1 = s1 + "Bah! Yes, the ship has been in more than one hand. That's why it's so worn. Do you have a corsair patent? Okay, I'm kidding... to the point.";
    			    }
    			    s1 = s1 + " What would you like to do with him?";
                    dialog.Text = s1;

					if (RealShips[sti(Pchar.Ship.Type)].BaseName == "Wh_corvette_quest")
					{//Капремонт пса
						dialog.text = "So, the 'War Dog'. What are we going to do with him?";
						if(!CheckAttribute(npchar, "WarDogUpgrade"))
						{
							npchar.WarDogUpgrade = "start";
							dialog.text = "Let's see... So! Is this a joke? The 'War Dog'? Have you come to open my old wound?";
							link.l1 = "what? What are you talking about?";
							link.l1.go = "WarDogUpgrade_1";
							break;
						}
						if(CheckAttribute(pchar, "TitanicIsDown"))
						{
							link.lWarDogUpgrade = "Have you heard yet? The unsinkable sank. Your student's reputation has been trampled into the mud forever. ";
							link.lWarDogUpgrade.go = "WarDogUpgrade_8";
						}
						if(CheckAttribute(npchar, "WarDogUpgradePayment") && sti(pchar.money) >= 1000000)
						{
							link.lWarDogUpgrade = "I brought your million. I hope this renovation is really worth it.";
							link.lWarDogUpgrade.go = "WarDogUpgrade_pay";
						}

					}
					
					if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.HullArmor"))
                    {
    			        Link.l0 = "Increase the hull armor.";
    			        Link.l0.go = "ship_tunning_HullArmor";
    			    }
					if (cannonMax < 36 && cannonDiff > 0 && !CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.Cannon"))
					{
						Link.l1 = "Increase the caliber and maximum number of guns.";
						Link.l1.go = "ship_tunning_cannon_all";
					}
					else
					{
						if (cannonMax < 36 && !CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.Cannon"))
						{
							Link.l1 = "Increase the caliber of the guns.";
							Link.l1.go = "ship_tunning_cannon";
						}
						else
						{
							if (cannonDiff > 0)
							{
								Link.l1 = "Increase the maximum number of guns.";
								Link.l1.go = "ship_c_quantity";
							}
						}
					}

    			   /*  if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.Capacity"))
                    {
    			        Link.l2 = "Increase the deadweight.";
    			        Link.l2.go = "ship_tunning_Capacity";
    			    } */
					if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.MastMultiplier"))
                    {
    			        Link.l3 = "Increase the strength of the masts.";
    			        Link.l3.go = "ship_tunning_MastMultiplier";
    			    }
					/* if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.SpeedRate"))
                    {
    			        Link.l4 = "Increase the speed.";
    			        Link.l4.go = "ship_tunning_SpeedRate";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.TurnRate"))
                    {
    			        Link.l5 = "Increase maneuverability.";
    			        Link.l5.go = "ship_tunning_TurnRate";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.MaxCrew"))
                    {
    			        Link.l6 = "Add room for the crew.";
    			        Link.l6.go = "ship_tunning_MaxCrew";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.HP"))
                    {
    			        Link.l7 = "Increase the strength of the hull.";
    			        Link.l7.go = "ship_tunning_HP";
    			    }
    			    if (!CheckAttribute(&RealShips[sti(Pchar.Ship.Type)], "Tuning.WindAgainstSpeed"))
                    {
    			        Link.l8 = "Increase the turn to the backwind.";
    			        Link.l8.go = "ship_tunning_WindAgainst";
    			    } */
    			    Link.l10 = "Wait a minute! I've changed my mind.";
    			    Link.l10.go = "ship_tunning_not_now_2";
			    }
			}
		break;

		////////////////////////////////////////// калибр и количество орудий  ////////////////////////////////////////////////////
		case "ship_tunning_cannon_all":
			s1 = "Let's see what can be done. There are guns on your ship now. " + cannonQ + ", the maximum possible number - " + cannonMaxQ + ", ";
			s1 = s1 + " maximum caliber - " + cannonMax + ". To strengthen the porticos and strengthen the carriages, I will need: ebony - "+AllMatherial1 + ".";
			/* s1 = s1 + "I'm old, and I'm not going to rot in this hole forever, so I'll get to work: treasures-"+AllMatherial2 + ", chests of gold - "+AllMatherial3 + ", "; */
			s1 = s1 + " Price - " + cannonAllWorkPrice + " piastres for work expenses. You know it's not an easy matter... Oh yes - and the money is in advance.";

			dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_cannon_all_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			/* if (!CheckAttribute(NPChar, "Discount"))
			{
				Link.l3 = "Why is it so expensive? Is there any way to reduce the prices?";
				Link.l3.go = "Discount_1";
			} */
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_cannon_all_complite";
			}
		break;

		case "ship_tunning_cannon_all_start":
			amount = cannonAllWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
				NPChar.Tuning.Cannon = true;

				NPChar.Tuning.Matherial1 = AllMatherial1; // чёрное дерево
				NPChar.Tuning.Matherial2 = AllMatherial2; // кожа
				NPChar.Tuning.Matherial3 = AllMatherial3;
				NPChar.Tuning.ShipType       = Pchar.Ship.Type;
				NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
				DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_cannon_all_again";
				dialog.text = "That's nice. I'm waiting for the materials.";
				link.l1 = "Already running for materials...";
				link.l1.go = "Exit";

				ReOpenQuestHeader("ShipTuning");
				AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "For his work to increase the calibre and number of guns on the ship " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
					" the master shipbuilder requested:" + NewStr() + "ebony -" + NPChar.Tuning.Matherial1 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
				dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_cannon_all_again":
			if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
			{
				NextDiag.TempNode = "ship_tunning_cannon_all_again";
				dialog.Text = "The guns are waiting. Did you bring what I asked for?";
				Link.l1 = "Yes. I managed to get something.";
				Link.l1.go = "ship_tunning_cannon_all_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
				DeleteAttribute(NPChar, "Tuning");
				NextDiag.TempNode = "ship_tunning_again";
				dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again..";
				Link.l1 = "There was a case. It's a shame that the deposit is gone...";
				Link.l1.go = "Exit";
				AddQuestRecord("ShipTuning", "Lose");
				AddQuestUserData("ShipTuning", "sSex", GetSexPhrase("", "a"));
				CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_cannon_all_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_EBONY, "icollection", "chest");

			if(sti(NPChar.Tuning.Matherial1) < 1 )
			{
				DeleteAttribute(NPChar, "Tuning");
				NextDiag.TempNode = "ship_tunning_again";
				dialog.text = "Everything is fine! Well done! I'm starting work...";
				link.l1 = "I'm waiting.";
				link.l1.go = "ship_tunning_cannon_all_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_cannon_all_again";
				dialog.Text = "You have to bring: ebony-"+ sti(NPChar.Tuning.Matherial1) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

				AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: ebony - "+ sti(NPChar.Tuning.Matherial1) + ".");
			}
		break;

		case "ship_tunning_cannon_all_complite":
			AddTimeToCurrent(8, 30);
			shTo = &RealShips[sti(Pchar.Ship.Type)];
			DeleteAttribute(NPChar, "Tuning");

			// изменим калибр орудий
			SetShipBermudeTuningMaxCaliber(Pchar);

			// изменим количество орудий
			shTo.rcannon = sti(shTo.rcannon) + cannonDiff/2;
			shTo.lcannon = sti(shTo.rcannon);
			shTo.CannonsQuantity = sti(shTo.rcannon) + sti(shTo.lcannon) + sti(shTo.fcannon) + sti(shTo.bcannon);

			Pchar.Ship.Cannons.Borts.cannonl = sti(shTo.lcannon);
			Pchar.Ship.Cannons.Borts.cannonr = sti(shTo.rcannon);
			Pchar.Ship.Cannons.Borts.cannonf = sti(shTo.fcannon);
			Pchar.Ship.Cannons.Borts.cannonb = sti(shTo.bcannon);
			Pchar.Ship.Cannons = sti(shTo.CannonsQuantity);

			// finish <--
			NextDiag.TempNode = "ship_tunning_again";
			s1 = "... Well, that seems to be it... You can increase the caliber and buy more guns.";
			s1 = s1 + "This, by the way, can be done at my shipyard.";
			dialog.Text = s1;
			Link.l1 = "Thank you! I will definitely check everything.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;


		////////////////////////////////////////// только количество орудий  ////////////////////////////////////////////////////
		case "ship_c_quantity":
			s1 = "Let's see what can be done. There are guns on your ship now. " + cannonQ + ", the maximum possible number - " + cannonMaxQ + ".";
			s1 = s1 + "This case will not be cheap for you, for new gun carriages I will need: ebony-"+quantityMatherial1;
			/* s1 = s1 + "Besides, I'll take on the job: treasures-"+quantityMatherial2+ ", chests of gold - "+quantityMatherial3+ ", "; */
			s1 = s1 + " plus - " + cannonQWorkPrice + " piastres for work expenses. You know it's not an easy matter... Oh yes - and the money is in advance.";
			dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_c_quantity_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			/* if (!CheckAttribute(NPChar, "Discount"))
			{
				Link.l3 = "Why is it so expensive? Is there any way to reduce the prices?";
				Link.l3.go = "Discount_1";
			} */
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_c_quantity_complite";
			}

		break;

		case "ship_c_quantity_start":
			amount = cannonQWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
				NPChar.Tuning.Cannon = true;

				NPChar.Tuning.Matherial1 = quantityMatherial1; // чёрное дерево
				NPChar.Tuning.Matherial2 = quantityMatherial2; // кожа
			        NPChar.Tuning.Matherial3 = quantityMatherial3;
				NPChar.Tuning.ShipType       = Pchar.Ship.Type;
				NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
				DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_c_quantity_again";
				dialog.text = "That's nice. I'm waiting for the materials.";
				link.l1 = "Already running for materials...";
				link.l1.go = "Exit";

				ReOpenQuestHeader("ShipTuning");
				AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "For his work to increase the number of guns on the ship " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				"the master shipbuilder demands:" + NewStr() + " ebony -" + NPChar.Tuning.Matherial1 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
				dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_c_quantity_again":
			if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
			{
				NextDiag.TempNode = "ship_c_quantity_again";
				dialog.Text = "The guns are waiting. Did you bring what I asked for?";
				Link.l1 = "Yes. I managed to get something.";
				Link.l1.go = "ship_c_quantity_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
				DeleteAttribute(NPChar, "Tuning");
				NextDiag.TempNode = "ship_tunning_again";
				dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
				Link.l1 = "There was a case. It's a shame that the deposit is gone...";
				Link.l1.go = "Exit";
				AddQuestRecord("ShipTuning", "Lose");
				CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_c_quantity_again_2":
			checkMatherial(Pchar, NPChar, GOOD_EBONY, "icollection", "chest");

			if(sti(NPChar.Tuning.Matherial1) < 1 )
			{
				DeleteAttribute(NPChar, "Tuning");
				NextDiag.TempNode = "ship_tunning_again";
				dialog.text = "Everything is fine! Well done! I'm starting work...";
				link.l1 = "I'm waiting.";
				link.l1.go = "ship_c_quantity_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_c_quantity_again";
				dialog.Text = "You have to bring: ebony-"+ sti(NPChar.Tuning.Matherial1) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

				AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: ebony - "+ sti(NPChar.Tuning.Matherial1) + ".");
			}
		break;

		case "ship_c_quantity_complite":
			AddTimeToCurrent(6, 30);
			shTo = &RealShips[sti(Pchar.Ship.Type)];
			DeleteAttribute(NPChar, "Tuning");

			// изменим количество орудий
			shTo.rcannon = sti(shTo.rcannon) + cannonDiff/2;
			shTo.lcannon = sti(shTo.rcannon);
			shTo.CannonsQuantity = sti(shTo.rcannon) + sti(shTo.lcannon) + sti(shTo.fcannon) + sti(shTo.bcannon);

			Pchar.Ship.Cannons.Borts.cannonl = sti(shTo.lcannon);
			Pchar.Ship.Cannons.Borts.cannonr = sti(shTo.rcannon);
			Pchar.Ship.Cannons.Borts.cannonf = sti(shTo.fcannon);
			Pchar.Ship.Cannons.Borts.cannonb = sti(shTo.bcannon);
			Pchar.Ship.Cannons = sti(shTo.CannonsQuantity);
			shTo.Tuning.Cannon = true;
			// finish <--
			NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... You can buy additional guns.";
			Link.l1 = "Thank you!";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;

		////////////////////////////////////////// только калибр орудий ////////////////////////////////////////////////////
		case "ship_tunning_cannon":
			s1 = "Let's see what we can do. There are guns on your ship now. " + cannonQ + ", the maximum caliber is " + cannonMax + ".";

			s1 = s1 + " To strengthen the porticos and carriages of heavy guns, I will need: ebony -"+ cannonMatherial1 + ".";
			/* s1 = s1 + " And for the job I'll take: treasures-"+ cannonMatherial2 + ", chests of gold - "+ cannonMatherial3 + ", "; */
			s1 = s1 + " Price - " + cannonWorkPrice + " piastres for work expenses. You know it's not an easy matter... Oh yes - and the money is in advance.";
            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_cannon_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			/* if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is there any way to reduce the prices?";
				Link.l3.go = "Discount_1";
			} */
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_cannon_complite";
			}
		break;

		case "ship_tunning_cannon_start":
		    amount = cannonWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
				NPChar.Tuning.Cannon = true;
			    NPChar.Tuning.Matherial1 = cannonMatherial1; // чёрное дерево
			    NPChar.Tuning.Matherial2 = cannonMatherial2; // кожа
			    NPChar.Tuning.Matherial3 = cannonMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_cannon_again";
                dialog.text = "That's nice. I'm waiting for the materials.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "For his work to increase the calibre of guns on the ship " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				"the master shipbuilder demands:" + NewStr() + " ebony -" + NPChar.Tuning.Matherial1 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_cannon_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_cannon_again";
				dialog.Text = "The guns are waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. I managed to get something.";
			    Link.l1.go = "ship_tunning_cannon_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again..";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";
			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_cannon_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_EBONY, "icollection", "chest");

		    if(sti(NPChar.Tuning.Matherial1) < 1 )
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_cannon_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_cannon_again";
				dialog.Text = "You have to bring: ebony-"+ sti(NPChar.Tuning.Matherial1) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: ebony - "+ sti(NPChar.Tuning.Matherial1) + ".");
			}
		break;

		case "ship_tunning_cannon_complite":
		    AddTimeToCurrent(6, 30);
		    DeleteAttribute(NPChar, "Tuning");
			SetShipBermudeTuningMaxCaliber(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... You can change the caliber - I guarantee the quality.";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;

		////////////////////////////////////////// трюм ////////////////////////////////////////////////////
		case "ship_tunning_Capacity":
			s1 = "Let's see what can be done. The deadweight of your ship is now-"+ shipCapacity;

			s1 = s1 + ". For lightweight bulkheads, I will need: sandal - "+ capacityMatherial1 + ".";
			s1 = s1 + " In payment for my work, I will take: gold bars-"+ capacityMatherial2 + ", chests of gold - "+ capacityMatherial3 + ", ";
			s1 = s1 + " plus - " + capacityWorkPrice + " piastres for work expenses. That seems to be it. Oh yes - money in advance.";
            dialog.Text = s1;
			Link.l1 = "It's okay. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_Capacity_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			}
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_Capacity_complite";
			}

		break;

		case "ship_tunning_Capacity_start":
		    amount = capacityWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1 = capacityMatherial1; // красное дерево GOOD_MAHOGANY
			    NPChar.Tuning.Matherial2 = capacityMatherial2; //  GOOD_OIL
			    NPChar.Tuning.Matherial3 = capacityMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_Capacity_again";
                dialog.text = "That's nice. I'm waiting for the material and my jewelry.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "For his work increasing deadweight on the " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				"the master shipbuilder demands:" + NewStr() + " sandal -" + NPChar.Tuning.Matherial1 + "," + NewStr() + " gold bars - "+ NPChar.Tuning.Matherial2+ "," + NewStr() + " chests of gold - " + NPChar.Tuning.Matherial3 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold. Apparently, the old man is crazy about jewelry. Well, everyone has their own weaknesses...");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_Capacity_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_Capacity_again";
				dialog.Text = "Deadweight is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. I managed to get something.";
			    Link.l1.go = "ship_tunning_Capacity_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_Capacity_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_SANDAL, "jewelry5", "chest");

		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Matherial3) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_Capacity_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_Capacity_again";
				dialog.Text = "You have to bring: sandal-"+ sti(NPChar.Tuning.Matherial1) + ", gold bars - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: sandal - "+ sti(NPChar.Tuning.Matherial1) + ", gold bars - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".");
			}
		break;

		case "ship_tunning_Capacity_complite":
		    AddTimeToCurrent(6, 30);
		    DeleteAttribute(NPChar, "Tuning");
			SetShipBermudeTuningCapacity(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... You can load up to the fullest - I guarantee the quality of the work.";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;

		////////////////////////////////////////// SpeedRate ////////////////////////////////////////////////////
		case "ship_tunning_SpeedRate":
			s1 = "Let's see what can be done. The speed of your ship is now " + shipSpeedRate;

			s1 = s1 + " knots. For the new sails, I will need: silks-"+ SpeedRateMatherial1 + ".";
			s1 = s1 + "I'm old, and I'm not going to rot in this hole forever, so I'll get to work: chests of gold-"+ SpeedRateMatherial3 + ", good faceted diamonds - "+ SpeedRateMatherial2 + ", plus - " + SpeedRateWorkPrice + " piastres for work expenses. That seems to be it. Oh yes - and the money is in advance.";

            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_SpeedRate_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			}
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_SpeedRate_complite";
			}

		break;

		case "ship_tunning_SpeedRate_start":
		    amount = SpeedRateWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1 = SpeedRateMatherial1; // GOOD_SAILCLOTH
			    NPChar.Tuning.Matherial2 = SpeedRateMatherial2; //  GOOD_SILK
			    NPChar.Tuning.Matherial3 = SpeedRateMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_SpeedRate_again";
                dialog.text = "That's nice. I'm waiting for the material and my jewelry.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "For his work on boosting speed for " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				"the master shipbuilder demands:" + NewStr() + " silk-" + NPChar.Tuning.Matherial1 + "," + NewStr() + " diamonds - "+ NPChar.Tuning.Matherial2+ "," + NewStr() + " chests of gold - " + NPChar.Tuning.Matherial3 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold. Apparently, the old man is crazy about jewelry. Well, everyone has their own weaknesses...");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_SpeedRate_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_SpeedRate_again";
				dialog.Text = "Work is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. I managed to get something.";
			    Link.l1.go = "ship_tunning_SpeedRate_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_SpeedRate_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_SILK, "jewelry2", "chest");

		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Matherial3) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_SpeedRate_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_SpeedRate_again";
				dialog.Text = "You have to bring: silks-"+ sti(NPChar.Tuning.Matherial1) + ", diamonds - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "I need to bring: silk - "+ sti(NPChar.Tuning.Matherial1) + ", diamonds - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".");
			}
		break;

		case "ship_tunning_SpeedRate_complite":
		    AddTimeToCurrent(6, 30);
		    DeleteAttribute(NPChar, "Tuning");
			SetShipBermudeTuningSpeedRate(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... You can catch the wind with full sails. Check it out!";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// MastMultiplier ////////////////////////////////////////////////////
		case "ship_tunning_MastMultiplier":
			s1 = "Let's see what can be done. Now the strength of the masts of your ship " + MastMulti;

			s1 = s1 + ". To strengthen the masts, I will need: planks - "+ MastMultiplierMatherial1 + ".";
			s1 = s1 + " Price - " + SpeedRateWorkPrice + " piastres for work expenses. That seems to be it. Oh yes - and the money is in advance.";

            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_MastMultiplier_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			/* if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			} */
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_MastMultiplier_complite";
			}

		break;

		case "ship_tunning_MastMultiplier_start":
		    amount = SpeedRateWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1 = MastMultiplierMatherial1; // GOOD_SAILCLOTH
			    NPChar.Tuning.Matherial2 = MastMultiplierMatherial2; //  GOOD_SILK
			    NPChar.Tuning.Matherial3 = MastMultiplierMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_MastMultiplier_again";
                dialog.text = "That's nice. I'm waiting for the materials.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "For his work on strengthening the mast on " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				" the master shipbuilder requires:" + NewStr() + " planks -" + NPChar.Tuning.Matherial1 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_MastMultiplier_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_MastMultiplier_again";
				dialog.Text = "Work is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. I managed to get something.";
			    Link.l1.go = "ship_tunning_MastMultiplier_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_MastMultiplier_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_PLANKS, "jewelry11", "chest");

		    if(sti(NPChar.Tuning.Matherial1) < 1 )
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_MastMultiplier_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_MastMultiplier_again";
				dialog.Text = "You have to bring: planks-"+ sti(NPChar.Tuning.Matherial1) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "I need to bring: planks - "+ sti(NPChar.Tuning.Matherial1) + ".");
			}
		break;

		case "ship_tunning_MastMultiplier_complite":
		    AddTimeToCurrent(6, 30);
		    DeleteAttribute(NPChar, "Tuning");
		    SetShipBermudeTuningMastMultiplier(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... You can catch knippels with your masts without any fear. Check it out!";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// MaxCrew ////////////////////////////////////////////////////
		case "ship_tunning_MaxCrew":
			s1 = "Let's see what we can do. Now the maximum crew of your vessel, taking into account the overload " + shipMaxCrew + "the man.";

			s1 = s1 + " For additional hammocks and locker upholstery, I will need: leathers-"+ MaxCrewMatherial1 + ".";
			s1 = s1 + " I will take as payment: sapphires - "+ MaxCrewMatherial2 + ", chests of gold - "+ MaxCrewMatherial3 + ", plus " + MaxCrewWorkPrice + " piastres for work expenses. That's about it. Oh yes - and the money is in advance.";
            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_MaxCrew_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			}
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_MaxCrew_complite";
			}
		break;

		case "ship_tunning_MaxCrew_start":
		    amount = MaxCrewWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1 = MaxCrewMatherial1; // GOOD_FRUITS
			    NPChar.Tuning.Matherial2 = MaxCrewMatherial2; //  GOOD_CLOTHES
			    NPChar.Tuning.Matherial3 = MaxCrewMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_MaxCrew_again";
                dialog.text = "That's nice. I'm waiting for the material and my jewelry.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "For his work in increasing the crew on the " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				"the master shipbuilder requires:" + NewStr() + " leather -" + NPChar.Tuning.Matherial1 + "," + NewStr() + " sapphires - "+ NPChar.Tuning.Matherial2+ "," + NewStr() + " chests of gold - " + NPChar.Tuning.Matherial3 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold. Apparently, the old man is crazy about jewelry. Well, everyone has their own weaknesses...");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_MaxCrew_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_MaxCrew_again";
				dialog.Text = "The case is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. I managed to get something.";
			    Link.l1.go = "ship_tunning_MaxCrew_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_MaxCrew_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_LEATHER, "jewelry1", "chest");

		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Matherial3) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_MaxCrew_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_MaxCrew_again";
				dialog.Text = "You have to bring: skins-"+ sti(NPChar.Tuning.Matherial1) + ", sapphires - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: leather - "+ sti(NPChar.Tuning.Matherial1) + ", sapphires - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".");
			}
		break;

		case "ship_tunning_MaxCrew_complite":
		    AddTimeToCurrent(6, 30);
		    DeleteAttribute(NPChar, "Tuning");
		    SetShipBermudeTuningMaxCrew(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... Recruit sailors, there's plenty of room for everyone.";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;

		////////////////////////////////////////// TurnRate ////////////////////////////////////////////////////
		case "ship_tunning_TurnRate":
			s1 = "Let's see what can be done. Now the maneuverability of this ship is " + shipTurnRate;

			s1 = s1 + " For additional sails to stir it up, I will need: canvases-"+ TurnRateMatherial1 + ".";
			s1 = s1 + " And I'll take the job: chests of gold-"+ TurnRateMatherial3 + ", rubinov - "+ TurnRateMatherial2 + ", plus - " + TurnRateWorkPrice + " piastres for work expenses. You understand that I don't plan to die in this dump... That's probably all. Oh yes - money in advance.";
            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_TurnRate_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			}
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_TurnRate_complite";
			}

		break;

		case "ship_tunning_TurnRate_start":
		    amount = TurnRateWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1 = TurnRateMatherial1; // GOOD_PLANKS
			    NPChar.Tuning.Matherial2 = TurnRateMatherial2; //  GOOD_LINEN
				NPChar.Tuning.Matherial3 = TurnRateMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_TurnRate_again";
                dialog.text = "That's nice. I'm waiting for the material and my jewelry.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "For his work on increasing maneuverability on the " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				" the master shipbuilder demands:" + NewStr() + " canvases -" + NPChar.Tuning.Matherial1 + "," + NewStr() + " rubinov - "+ NPChar.Tuning.Matherial2+ "," + NewStr() + " chests of gold - " + NPChar.Tuning.Matherial3 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold. Apparently, the old man is crazy about jewelry. Well, everyone has their own weaknesses...");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_TurnRate_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_TurnRate_again";
				dialog.Text = "Work is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. Something was delivered.";
			    Link.l1.go = "ship_tunning_TurnRate_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_TurnRate_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_LINEN, "jewelry3", "chest");

		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Matherial3) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_TurnRate_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_TurnRate_again";
				dialog.Text = "You have to bring: canvases-"+ sti(NPChar.Tuning.Matherial1) + ", rubinov - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: linen - "+ sti(NPChar.Tuning.Matherial1) + ", rubins - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".");
			}
		break;

		case "ship_tunning_TurnRate_complite":
		    AddTimeToCurrent(6, 30);
		    DeleteAttribute(NPChar, "Tuning");
			SetShipBermudeTuningTurnRate(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... You can turn the wheel.";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// HP ////////////////////////////////////////////////////
		case "ship_tunning_HP":
			s1 = "Let's see what we can do. The hull of the ship is now " + shipHP;

			s1 = s1 + ". To strengthen the paneling, I will need: mahogany - "+ HPMatherial1 + ".";
			s1 = s1 + " I'll take the job: silver bars-"+ HPMatherial2 + ", chests of gold - "+ HPMatherial3 + ", plus - " + HPWorkPrice + " piastres for work expenses... My grandchildren have been calling me back to Europe for a long time, I don't want to come back empty-handed, you know... That's probably all. Oh yes - money in advance.";
            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_HP_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			}
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_HP_complite";
			}

		break;

		case "ship_tunning_HP_start":
		    amount = HPWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1 = HPMatherial1; // GOOD_EBONY
			    NPChar.Tuning.Matherial2 = HPMatherial2; //  GOOD_SANDAL
				NPChar.Tuning.Matherial3 = HPMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
			    DeleteAttribute(NPChar, "Discount");

				NextDiag.TempNode = "ship_tunning_HP_again";
                dialog.text = "That's nice. I'm waiting for the material and my jewelry.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "For his work to increase the hull strength of the " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				"the master shipbuilder demands:" + NewStr() + " mahogany -" + NPChar.Tuning.Matherial1 + "," + NewStr() + " silver bars - "+ NPChar.Tuning.Matherial2+ "," + NewStr() + " chests of gold - " + NPChar.Tuning.Matherial3 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold... He says he's going to Europe- the old devil is probably lying.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_HP_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_HP_again";
				dialog.Text = "Work is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. I managed to get something.";
			    Link.l1.go = "ship_tunning_HP_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_HP_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_MAHOGANY, "jewelry17", "chest");

		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Matherial3) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_HP_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_HP_again";
				dialog.Text = "You have to bring: mahogany-"+ sti(NPChar.Tuning.Matherial1) + ", silver bars - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: mahogany - "+ sti(NPChar.Tuning.Matherial1) + ", silver bars - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".");
			}
		break;

		case "ship_tunning_HP_complite":
		    AddTimeToCurrent(6, 30);
		    shTo = &RealShips[sti(Pchar.Ship.Type)];
		    DeleteAttribute(NPChar, "Tuning");
			SetShipBermudeTuningHP(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			if(shTo.BaseName == "Flyingdutchman") // ЛГ
			{
				dialog.text = "Yeah... I've seen a lot of ships in my time, but I've never seen such filth and mustiness in captain's cabin. You should clean up there more often! However, we rebuilt the cabin and strengthened the hull - it will withstand a couple of direct hits for sure.";
				Link.l1 = "Thank you! I will definitely check it out.";
				Link.l1.go = "Exit";
				shTo.CabinType = "Cabin_Quest";
			}
			else
			{
				dialog.Text = "... That seems to be it... I guarantee that it will be very difficult to wreck this ship now!";
				Link.l1 = "Thank you! But it's better not to experiment with it.";
				Link.l1.go = "Exit";
			}

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// WindAgainstSpeed ////////////////////////////////////////////////////
		case "ship_tunning_WindAgainst":
			s1 = "Let's see what can be done. Beidewind now " + FloatToString(shipWindAgainst, 2);

			s1 = s1 + ". To disperse it against the wind, I will need: cotton - "+ WindAgainstMatherial1 + ", ";
			s1 = s1 + " and as payment I will take: emeralds-"+ WindAgainstMatherial2 + ", chests of gold - "+ WindAgainstMatherial3 + ", plus - " + WindAgainstWorkPrice + " piastres for work expenses. It's not an easy matter... That seems to be it. Oh yes - money in advance.";
            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_WindAgainst_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			}
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_WindAgainst_complite";
			}
		break;

		case "ship_tunning_WindAgainst_start":
		    amount = WindAgainstWorkPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1     = WindAgainstMatherial1;
			    NPChar.Tuning.Matherial2     = WindAgainstMatherial2;
			    NPChar.Tuning.Matherial3     = WindAgainstMatherial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
                DeleteAttribute(NPChar, "Discount");
				NextDiag.TempNode = "ship_tunning_WindAgainst_again";
                dialog.text = "That's nice. I'm waiting for the material and my jewelry.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "For his work on boosting the backwind on the " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				" the master shipbuilder demands:" + NewStr() + " cotton-" + NPChar.Tuning.Matherial1 + "," + NewStr() + " emeralds - "+ NPChar.Tuning.Matherial2+ "," + NewStr() + " chests of gold - " + NPChar.Tuning.Matherial3 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold. The old man went crazy with these trinkets.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_WindAgainst_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_WindAgainst_again";
			    dialog.Text = "Beidewind is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. Something was delivered.";
			    Link.l1.go = "ship_tunning_WindAgainst_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_WindAgainst_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_COTTON, "jewelry4", "chest");

		    if(sti(NPChar.Tuning.Matherial2) < 1 && sti(NPChar.Tuning.Matherial1) < 1 && sti(NPChar.Tuning.Matherial3) < 1)
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_WindAgainst_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_WindAgainst_again";
                dialog.Text = "You have to bring: cotton-"+ sti(NPChar.Tuning.Matherial1) + ", emeralds - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: silk - "+ sti(NPChar.Tuning.Matherial1) + ", emeralds - "+ sti(NPChar.Tuning.Matherial2) + ", chests - "+ sti(NPChar.Tuning.Matherial3) + ".");
			}
		break;

		case "ship_tunning_WindAgainst_complite":
		    AddTimeToCurrent(6, 30);
		    DeleteAttribute(NPChar, "Tuning");
			SetShipBermudeTuningWindAgainstSpeed(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... The speed against the wind will be higher now.";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		////////////////////////////////////////// HullArmor ////////////////////////////////////////////////////
		case "ship_tunning_HullArmor":
			s1 = "Let's see what can be done. Hull Armor now " + hullArmor;

			s1 = s1 + ". To strengthen the case, you will need: iron - "+ HullArmorMaterial1;
			s1 = s1 + " plus - " + HullArmorPrice + " piastres for work expenses. It's not an easy matter... That seems to be it. Oh yes - money in advance.";
            dialog.Text = s1;
			Link.l1 = "It's fine. I accept the terms. Everything agreed upon will be delivered.";
			Link.l1.go = "ship_tunning_HullArmor_start";
			Link.l2 = "no. It doesn't suit me.";
			Link.l2.go = "ship_tunning_not_now";
			/* if (!CheckAttribute(NPChar, "Discount"))
			{
			    Link.l3 = "Why is it so expensive? Is it possible to reduce the prices?";
				Link.l3.go = "Discount_1";
			} */
			if (bBettaTestMode)
			{
				Link.l4 = "DOWNLOAD IT IMMEDIATELY!";
				Link.l4.go = "ship_tunning_HullArmor_complite";
			}
		break;

		case "ship_tunning_HullArmor_start":
		    amount = HullArmorPrice;
		    if(makeint(Pchar.money) >= amount)
			{
				AddMoneyToCharacter(Pchar, -amount);
			    NPChar.Tuning.Money  = amount;
			    NPChar.Tuning.Matherial1     = HullArmorMaterial1;
			    NPChar.Tuning.Matherial2     = HullArmorMaterial2;
			    NPChar.Tuning.Matherial3     = HullArmorMaterial3;
			    NPChar.Tuning.ShipType       = Pchar.Ship.Type;
			    NPChar.Tuning.ShipName       = RealShips[sti(Pchar.Ship.Type)].BaseName;
                DeleteAttribute(NPChar, "Discount");
				NextDiag.TempNode = "ship_tunning_HullArmor_again";
                dialog.text = "That's nice. I'm waiting for the materials.";
			    link.l1 = "Already running for materials...";
			    link.l1.go = "Exit";

			    ReOpenQuestHeader("ShipTuning");
			    AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText",  "For his work on strengthening the armor of the " + XI_ConvertString(RealShips[sti(Pchar.Ship.Type)].BaseName) +
				" the master shipbuilder requires:" + NewStr() + " iron -" + NPChar.Tuning.Matherial1 + "." + NewStr() + " As a deposit, it was paid " + NPChar.Tuning.Money + "gold.");
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_not_now";
                dialog.text = "I don't see the deposit...";
				link.l1 = "I'll come back later.";
				link.l1.go = "Exit";
			}
		break;

		case "ship_tunning_HullArmor_again":
		    if (sti(NPChar.Tuning.ShipType) == sti(Pchar.Ship.Type) && NPChar.Tuning.ShipName      == RealShips[sti(Pchar.Ship.Type)].BaseName)
		    {
                NextDiag.TempNode = "ship_tunning_HullArmor_again";
			    dialog.Text = "The armor is waiting. Did you bring what I asked for?";
			    Link.l1 = "Yes. Something was delivered.";
			    Link.l1.go = "ship_tunning_HullArmor_again_2";
				Link.l2 = "I want to use the shipyard's services.";
				Link.l2.go = "PayToRapair1";
				link.l3 = "No, I'm still in progress.";
				link.l3.go = "exit";
			}
			else
			{
			    DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
			    dialog.Text = "It seems to me, "+ GetSexPhrase("sir", "miss") +", that you have changed the ship since our agreement. We'll have to calculate everything all over again...";
			    Link.l1 = "There was a case. It's a shame that the deposit is gone...";
			    Link.l1.go = "Exit";

			    AddQuestRecord("ShipTuning", "Lose");
			    CloseQuestHeader("ShipTuning");
			}
		break;

		case "ship_tunning_HullArmor_again_2":
		    checkMatherial(Pchar, NPChar, GOOD_IRON, "jewelry12", "chest");

		    if(sti(NPChar.Tuning.Matherial1) < 1 )
			{
				DeleteAttribute(NPChar, "Tuning");
                NextDiag.TempNode = "ship_tunning_again";
                dialog.text = "Everything is fine! Well done! I'm starting work...";
			    link.l1 = "I'm waiting.";
			    link.l1.go = "ship_tunning_HullArmor_complite";
			}
			else
			{
				NextDiag.TempNode = "ship_tunning_HullArmor_again";
                dialog.Text = "You have to bring: iron -"+ sti(NPChar.Tuning.Matherial1) + ".";
				link.l1 = "Good.";
				link.l1.go = "Exit";

                AddQuestRecord("ShipTuning", "t1");
				AddQuestUserData("ShipTuning", "sText", "I need to bring: iron - "+ sti(NPChar.Tuning.Matherial1) + ".");
			}
		break;

		case "ship_tunning_HullArmor_complite":
		    AddTimeToCurrent(12, 30);
		    DeleteAttribute(NPChar, "Tuning");
			SetShipBermudeTuningHullArmor(Pchar);
            NextDiag.TempNode = "ship_tunning_again";
			dialog.Text = "... That seems to be it... You can now catch shells with your sides much more confidently.";
			Link.l1 = "Thank you! I will definitely check it out.";
			Link.l1.go = "Exit";

			AddQuestRecord("ShipTuning", "End");
			CloseQuestHeader("ShipTuning");
		break;
		///////////////////////////////  квестовые ветки
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("What kind of questions?", "What do you want?"), "Just recently, you tried to ask me a question...", "At my shipyard, and indeed in the city, I have never seen such monotonously inquisitive people.",
                          "Well, what kind of questions? My job is to build ships, let's do it.", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("I've changed my mind...", "I have nothing to talk about right now."), "Hmm, something happened to my memory...",
                      "Hmm, however...", "Let's...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
			if (!CheckAttribute(npchar, "Step_Door"))
			{
				link.l2 = "Listen, I want to go through, but the door is closed...";
				link.l2.go = "Step_Door_1";
			}
        // ==> Перехват на квесты
            if (CheckAttribute(pchar, "questTemp.State.SeekBible"))// квест №2, поиск Евангелие
            {
                if (!CheckCharacterItem(pchar, "Bible"))
                {
                    dialog.text = "Tell me...";
        			link.l2 = "Listen, I want to know where a certain man who was fleeing persecution fell into at your shipyard?";
        			link.l2.go = "Step_S2_1";
                }
                else
                {
                    dialog.text = "Oh, good to see you. So it's not for nothing that I let you in there - you survived and got rid me of this stuff.... Tell me, who was there?";
        			link.l2 = "It's better for you not to know, you'll sleep better. Well, goodbye, thanks again for opening the door.";
        			link.l2.go = "exit";
                }
            }
			if (pchar.questTemp.piratesLine == "KillLoy_toAlexus")
			{
				link.l1 = "Did Edward Law come to see you?";
				link.l1.go = "PL_Q3_1";
			}
			if (CheckAttribute(pchar, "questTemp.piratesLine.T1") && !CheckAttribute(npchar, "quest.PQ4"))
            {
				if (CheckAttribute(pchar, "questTemp.BrigSW_gift"))
				{
					link.l1 = "Jackman gave me a 'Sea Wolf'!";
					link.l1.go = "PL_Q3_SW1";
				}
				if (pchar.questTemp.piratesLine.T1 == "KillLoy_LoyIsDied")
				{
					link.l1 = "I don't have very good news about your brig that Law took away...";
					link.l1.go = "PL_Q3_SW2";
				}
			}
			if ((pchar.questTemp.different == "GiveShipLetters") && !CheckAttribute(pchar, "questTemp.different.GiveShipLetters.speakAlexus"))
			{
				link.l1 = "I want to propose a deal.";
				link.l1.go = "Al_ShipLetters_1";// генератор  "Found documents"
				pchar.questTemp.different.GiveShipLetters.speakAlexus = true;
			}
			if (pchar.questTemp.BlueBird == "toSeekBermudes") //шебека Синяя Птица
			{
				link.l1 = "Listen, I need to know one thing about your dungeon...";
				link.l1.go = "BlueBird_1";
			}
			if (CheckCharacterItem(pchar, "Ship_Print_1")) NPChar.print1 = 0;
			if (CheckCharacterItem(pchar, "Ship_Print_2")) NPChar.print2 = 0;
			if (CheckCharacterItem(pchar, "Ship_Print_3")) NPChar.print3 = 0;
			if (CheckCharacterItem(pchar, "Ship_Print_4")) NPChar.print4 = 0;
			if (CheckCharacterItem(pchar, "Ship_Print_5")) NPChar.print5 = 0;
			if (CheckCharacterItem(pchar, "Ship_Print_6")) NPChar.print6 = 0;
			if (CheckCharacterItem(pchar, "Ship_Print_1") || CheckCharacterItem(pchar, "Ship_Print_2") || CheckCharacterItem(pchar, "Ship_Print_3") || CheckCharacterItem(pchar, "Ship_Print_4") || CheckCharacterItem(pchar, "Ship_Print_5") || CheckCharacterItem(pchar, "Ship_Print_6"))
            {
				if (!CheckAttribute(npchar, "questTemp.ShipOrderTime"))
				{
					link.l3 = "I got here something unusual, and it looks very much like a blueprint, would you take a look?";
					link.l3.go = "blueprint";
				}

			}
			if (pchar.location.from_sea == "Pirates_town")
			{
				if (CheckAttribute(RealShips[sti(Pchar.Ship.Type)],"Blueprint") || CheckAttribute(RealShips[sti(Pchar.Ship.Type)],"QuestShip"))
				{
					if (!CheckAttribute(RealShips[sti(Pchar.Ship.Type)],"savesBonus"))
					{
						link.l99 = "Master, not a single shipyarder cannot figure out, where exactly or with what my ship was made. Can you help me?";
						link.l99.go = "setshipmat";
					}
				}
			}
        // <== Перехват на квесты
		break;
		
		case "setshipmat":
			dialog.text = "How so? Let's see... Ha, your ship was made by either pirates or contrabandists. There are a lot of different marks, some of them I can even remember. It is common that their designs are basic, nothing special to see. If you want, I may try to reconstruct it. Do you want it now?"
			link.l1 = "Sure, let's start.";
			link.l1.go = "setshipmat_go";
			link.l2 = "No time right now, I should go.";
			link.l2.go = "exit";
		break;
		
		case "setshipmat_go":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchShipyard3(npchar);
		break;

		case "blueprint":
			dialog.text = "Hmm... You've brought me a rather interesting thing. No, this is not a blueprint in the usual sense of things. This is a complex of constructive solutions in shipbuilding, and it is very unusual. With its help, you can maximize the capabilities of any ship. If you want, I can try to create some kind of ship for a reward. So how's it going?"
			link.l1 = "Of course, let's get started.";
			link.l1.go = "blueprint_ordership";
			link.l2 = "It's certainly very interesting, but perhaps another time.";
			link.l2.go = "exit";
		break;

		case "blueprint_ordership":
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
			LaunchShipyard2(npchar);
		break;
//*************************** Генератор - "Found ship's documents" **************

		case "Al_ShipLetters_1":
			dialog.text = "State the terms.";
			s1 = "By a lucky chance, I turned out to have an absolutely legal package of ship's documents that have not yet been put on the wanted list.";
			s1 = s1 + "The ship is afloat and has not been excluded from the register, do not worry. It's just that the owner has managed to lose these papers...";
			link.l1 = s1;
			link.l1.go = "Al_ShipLetters_2";
		break;
		case "Al_ShipLetters_2":
			s1 = "Let's take a look! Indeed, the marks are quite fresh. Well, luck favours you, "+ GetSexPhrase("sir", "miss") +"! ";
			s1 = s1 + "I just needed just such a set, and I, in turn, offer you " + sti(pchar.questTemp.different.GiveShipLetters.price5) + ". What do you say?";
			dialog.text = s1;
			link.l1 = "Truly a royal generosity! Of course I agree!";
			link.l1.go = "Al_ShipLetters_3";
			link.l2 = "Probably not.";
			link.l2.go = "exit";
		break;
		case "Al_ShipLetters_3":
			TakeItemFromCharacter(pchar, "CaptainBook");
			addMoneyToCharacter(pchar, sti(pchar.questTemp.different.GiveShipLetters.price5));
			ChangeCharacterReputation(pchar, -1);
			OfficersReaction("bad");
			pchar.questTemp.different = "free";
			pchar.quest.GiveShipLetters_null.over = "yes"; //снимаем таймер
			AddQuestRecord("GiveShipLetters", "9");
			CloseQuestHeader("GiveShipLetters");
			DeleteAttribute(pchar, "questTemp.different.GiveShipLetters");
			NextDiag.CurrentNode = NextDiag.TempNode;
			DialogExit();
		break;

//*************************** Пиратка, квест №3, поиски Лоу ***************************
		case "PL_Q3_1":
			dialog.text = "I came in... And why do you need it?";
			link.l1 = "Uh, he and Morgan disagreed on some ethics issues. Don't try my patience, master, I've been chasing him all the way from Martinique.";
			link.l1.go = "PL_Q3_3";
			link.l2 = "I'm his friend, we haven't seen each other for a long time, and then they said in the tavern that he came here...";
			link.l2.go = "PL_Q3_2";
		break;
		case "PL_Q3_2":
			dialog.text = "Yeah. Friend, so. That's good... that's just great! You, that's what, go to Jackman, tell him you're looking for your friend Edward Law. Jackman will help you\nMaybe he still has your friend. Go, go, meet him there.";
			link.l1 = "Thank you! I'm already running!";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "10");
			pchar.questTemp.piratesLine = "KillLoy_toJackman";
		break;
		case "PL_Q3_3":
			dialog.text = "So you're looking for him on Morgan's assignment? Well, at least someone took care of this bastard! Can you imagine? He tricked me! I brought a letter from Jackman, asking him to give the 'Sea Wolf' to the bearer of this letter...";
			link.l1 = "The 'Sea Wolf'?";
			link.l1.go = "PL_Q3_4";
		break;
		case "PL_Q3_4":
			dialog.text = "The Sea Wolf is a brig. My brig! My best creation! I put my whole soul into it. He is beautiful, magnificent and captivating! When the sun shines through its sails at sunset, it is impossible to take your eyes off it. At sunrise, when the handsome man raises his sails, he is like a blooming magnolia flower! And how he glides over the waves...";
			link.l1 = "Hmm, master, let's go back to our Law.";
			link.l1.go = "PL_Q3_5";
		break;
		case "PL_Q3_5":
			dialog.text = "I'm sorry? Oh, yes. Edward Law. I used to make Jackman's brig, I almost finished it, and there he was, Law, with the letter. Anyway, I gave away the brig, and when I went to Jackman for money, it turned out to be a fake letter. Jackman was upset... very... very much. When I got out of there you... Uh...  When I left, Jackman said, 'I respect you, Master, but I have to get the brig back. So I was thinking, since you're looking for this Law anyway, maybe you can find my brig too?";
			link.l1 = "Master, I will help you, of course, but only if you stop wailing and tell me clearly: WHERE IS LAW?!";
			link.l1.go = "PL_Q3_6";
		break;
		case "PL_Q3_6":
			dialog.text = "So ask the tavern owner. Tavern keepers, they're such a people, they know everything about everyone. He probably knows where that scoundrel went on my brig.";
			link.l1 = "Ugh, you've wasted so much time. All right, come on, master.";
			link.l1.go = "exit";
			AddQuestRecord("Pir_Line_3_KillLoy", "11");
			pchar.questTemp.piratesLine = "KillLoy_toTavernAgain";
			//садим капитана Гудли в таверну
			sld = GetCharacter(NPC_GenerateCharacter("CapGoodly", "officer_9", "man", "man", 20, PIRATE, -1, true));
			sld.name = "Captain";
			sld.lastname = "Goodley";
			sld.city = "Pirates";
			sld.dialog.filename   = "Quest\PiratesLine_dialog.c";
			sld.dialog.currentnode   = "CapGoodly";
			sld.greeting = "Gr_EvilPirate";
			LAi_SetSitType(sld);
			LAi_group_MoveCharacter(sld, "PIRATE_CITIZENS");
            FreeSitLocator("Pirates_tavern", "sit2");
         	ChangeCharacterAddressGroup(sld, "Pirates_tavern", "sit", "sit2");
		break;

		case "PL_Q3_SW1":
			dialog.text = "I saw you, I saw you arrive. I couldn't help myself, I ran to the marina to see.\nWorthy gift. Congratulations and thank you! You saved the old man.";
			npchar.quest.PQ4 = true;
			link.l1 = "That's how I saved you...";
			link.l1.go = "exit";
		break;
		case "PL_Q3_SW2":
			npchar.quest.PQ4 = true;
			dialog.text = "Damn it! I knew it... So what?";
			link.l1 = "He drowned, I'm sorry....";
			link.l1.go = "PL_Q3_SW3";
		break;
		case "PL_Q3_SW3":
			dialog.text = "Yeah, it's bad.";
			link.l1 = "Don't worry, everything will be fine...";
			link.l1.go = "exit";
		break;
//*************************** шебека Синяя Птица ***************************
		case "BlueBird_1":
			dialog.text = "It's not mine. On the contrary, I'm trying not to let anyone in there for their own good.";
			link.l1 = "Ah, well, I see. The question actually lies here. Are goods being brought into the city through this dungeon?";
			link.l1.go = "BlueBird_2";
		break;
		case "BlueBird_2":
			dialog.text = "Haha, no, of course not, although the dungeon is through. Judge for yourself, this is what an idiot you have to be to carry loads on yourself through this hole, when you can hire any tartan and calmly bring everything by sea.";
			link.l1 = "Well, what if someone doesn't want it to be known in the city?";
			link.l1.go = "BlueBird_3";
		break;
		case "BlueBird_3":
			dialog.text = "Do you think that if someone starts hauling goods from my shipyard to the city, no one will notice?";
			link.l1 = "Hmm, yes, that's right... So no one is carrying anything through your shipyard?";
			link.l1.go = "BlueBird_4";
		break;
		case "BlueBird_4":
			dialog.text = "No, no one is carrying anything, you can be sure.";
			link.l1 = "Well, thank you very much!";
			link.l1.go = "exit";
			AddQuestRecord("Xebeca_BlueBird", "5");
			pchar.questTemp.BlueBird = "toCavern";
			pchar.quest.BlueBird_inCavern.win_condition.l1 = "locator";
			pchar.quest.BlueBird_inCavern.win_condition.l1.location = "Bermudes_Cavern";
			pchar.quest.BlueBird_inCavern.win_condition.l1.locator_group = "quest";
			pchar.quest.BlueBird_inCavern.win_condition.l1.locator = "BlueBird";
			pchar.quest.BlueBird_inCavern.function = "BlueBird_inCavern";
			LAi_LocationDisableMonstersGen("Bermudes_Cavern", true); //монстров не генерить
			LAi_LocationDisableOfficersGen("Bermudes_Cavern", true); //офицеров не пускать
		break;
//*************************** Открывание двери ***************************
 		case "Step_Door_1":
			dialog.text = "That's right. To get through here, you have to pay me 20,000 coins. If you don't pay me, I won't, I don't have a courtyard here. Besides, you shouldn't be out there...";
			if(sti(pchar.money) >= 20000)
			{
    			link.l1 = "Here's your money, open it.";
    			link.l1.go = "Step_Door_2";
            }
            else
            {
    			link.l1 = "I don't have enough money.";
    			link.l1.go = "Step_Door_3";
            }
            link.l2 = "Here's another one! Paying for some kind of door.";
    		link.l2.go = "exit";
		break;
 		case "Step_Door_2":
			npchar.Step_Door = true; // fix
			dialog.text = "Come in, the door is open. And don't thank me!";
			link.l1 = "Why, thank you, " + npchar.name + "...";
			link.l1.go = "exit";
			LocatorReloadEnterDisable("Pirates_Shipyard", "reload2", false);
            AddMoneyToCharacter(pchar, -20000);
			pchar.quest.CloseBermudesDungeonDoor.win_condition.l1 = "MapEnter";
            pchar.quest.CloseBermudesDungeonDoor.win_condition = "CloseBermudesDungeonDoor";
		break;
 		case "Step_Door_3":
			dialog.text = "Well, when they are, then come. And don't try to persuade - it's useless...";
			link.l1 = "Argh, well, there are just no words!";
			link.l1.go = "exit";
		break;
//*************************** Квест №2, диалоги с Франциско на выходе из Инквизиии ***************************
 		case "Step_S2_1":
			dialog.text = NPCStringReactionRepeat("Ooh, that's a good question... And why do you need it?", "I've already said it all-the door is open.", "I repeat it again-the door is open.", "Listen, how long can I take, huh?", "block", 0, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat("He has a book that doesn't belong to him. He's a thief.", "I got it...", "I see...", "I am incomprehensibly persistent...", npchar, Dialog.CurrentNode);
			link.l1.go = DialogGoNodeRepeat("Step_S2_2", "none", "none", "none", npchar, Dialog.CurrentNode);
		break;
 		case "Step_S2_2":
			dialog.text = "I see. You know, his pursuers, the sailors, have already asked me about it. Then some merchant came and tried to find out too. I didn't tell them anything.";
			link.l1 = "Why?";
			link.l1.go = "Step_S2_3";
		break;
 		case "Step_S2_3":
			dialog.text = "I didn't want to take a sin on my soul - they wouldn't live if they got in there... I can tell you, you're like, "+ GetSexPhrase("not one of the timid ones, and he's strong...", "a fighting girl, you can swing a saber.") +"";
			link.l1 = "Where would they go?";
			link.l1.go = "Step_S2_4";
		break;
 		case "Step_S2_4":
			dialog.text = "Where that thief went. Through this door to my right.";
			link.l1 = "And there, behind the door? And why don't you let anyone in there?";
			link.l1.go = "Step_S2_5";
		break;
 		case "Step_S2_5":
			dialog.text = "Behind the door of the dungeon, who built it - I do not know when I became the owner, it was already there. I do not let because it is dangerous there, very dangerous. The thief seems to have vanished there...\n"+
                          "I don't know who lives there, or what. But sometimes there are screams from there that make the blood run cold...";
			link.l1 = "That's even... hmm, well, start it up.";
			link.l1.go = "Step_S2_6";
		break;
 		case "Step_S2_6":
			dialog.text = "Come in, it's open... Hope to see you again in this world.";
			link.l1 = "You'll see, don't doubt it.";
			link.l1.go = "exit";
            LocatorReloadEnterDisable("Pirates_Shipyard", "reload2", false);
            LocatorReloadEnterDisable("Shore3", "reload2_back", false);
            Pchar.quest.Inquisition_fightInDangeon.win_condition.l1 = "location";
            Pchar.quest.Inquisition_fightInDangeon.win_condition.l1.location = "Bermudes_Dungeon";
            Pchar.quest.Inquisition_fightInDangeon.win_condition = "Inquisition_fightInDangeon";
         	pchar.GenQuestBox.Terks_Grot = true;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry5 = 200;
			pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry17 = 300;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade33 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade28 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade34 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade32 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade23 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.pistol5 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.pistol10 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.pistol6 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.potion2 = 5;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian18 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry7 = 5;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.blade5 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.potion2 = 10;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.potionwine = 3;
			pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry1 = 100;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry2 = 80;
			pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry3 = 60;
			pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry4 = 45;
			pchar.GenQuestBox.Bermudes_Dungeon.box1.items.icollection = 1;
			pchar.GenQuestBox.Bermudes_Dungeon.box1.items.chest = 5;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.jewelry14= 10;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian6 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian11 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian15 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian12 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.indian18 = 1;
            pchar.GenQuestBox.Bermudes_Dungeon.box1.items.Bible = 1;
		break;
		case "PayToRapair1":
			if (pchar.paytorepair != pchar.dayRandom && CheckAttribute(PChar, "paytorepair")) DeleteAttribute(PChar, "paytorepair");
			dialog.text = "Dear, I've already set up everything to improve your ship, do you want me to drop everything like that?!";
			link.l1 = "I understand you. I hope 15,000,000 piastres will settle this?"
			link.l1.go = "PayToRapair2";
			link.l2 = "Okay, nothing is needed.";
			link.l2.go = "exit";
			if (CheckAttribute(PChar, "paytorepair"))
			{
				dialog.text = "You need to handle your ship better! I'm already tired of you today... Okay, what do you need?";
				link.l1 = "I need to fix the ship."
				link.l1.go = "shipyard";
				link.l2 = "I need guns for the ship.";
				link.l2.go = "Cannons";
			}

		break;
		case "PayToRapair2":
			pchar.paytorepair = pchar.dayRandom;
			AddMoneyToCharacter(pchar, -15000);
			dialog.text = "That's another thing! What do you need?";
			link.l1 = "I need to fix the ship."
			link.l1.go = "shipyard";
			link.l2 = "I need guns for the ship.";
			link.l2.go = "Cannons";
		break;

		//Капремонт пса
		case "WarDogUpgrade_1":
			dialog.text = "Really? As if you have no idea that I personally designed it.";
			link.l1 = "So this is your development? I must admit, you've done a great job.";
			link.l1.go = "WarDogUpgrade_2";
		break;

		case "WarDogUpgrade_2":
			dialog.text = "Oh, don't make me laugh. I know it's a wreck. When I made it, I was young, green, stupid. If I started designing it now, I wouldn't make the old mistakes.";
			link.l1 = "Is it really that bad? For me, the boat is still great. Better than many of the boats I've seen in the Caribbean.";
			link.l1.go = "WarDogUpgrade_3";
		break;

		case "WarDogUpgrade_3":
			dialog.text = "He could be much faster, more maneuverable, stronger...";
			link.l1 = "Really? Don't you want to refine it? To work on the mistakes, so to speak.";
			link.l1.go = "WarDogUpgrade_4";
		break;

		case "WarDogUpgrade_4":
			dialog.text = "I don't think so. I have a lot of other things to do right now, and the Hound needs a major overhaul that will take a lot of time and effort. Besides, the years are not the same anymore... Lately I've been reluctant to go back to my old projects. Although, if you do me one favor, miss, so be it, I'll think about it. ";
			link.l1 = "What do you want?";
			link.l1.go = "WarDogUpgrade_5";
		break;

		case "WarDogUpgrade_5":
			dialog.text = "There's a posh ship in the archipelago, I won't point a finger. He thinks he's the best in the world, even better than me, and he brags about it every chance he gets, which is what he doesn't tell people, which is that all his 'brilliant ideas' are actually my old blueprints that I've abandoned for one reason or another.\nYou see, he was my apprentice, until one day he stole my revolutionary shipbuilding sketches and fled to the English Ná recently, claiming to have built an unsinkable ship. 'Titanic' - that's what he called it. How tasteless! Ugh!";
			link.l1 = "'Titanic'... It sounds familiar. Where would I have heard that? However, it doesn't matter, go on.";
			link.l1.go = "WarDogUpgrade_5_1";
		break;

		case "WarDogUpgrade_5_1":
			dialog.text = "He sold it to some English privateer, and he's using it to raid Spanish ports on the mainland. They say this captain has become so cocky that he doesn't care about ships passing by unless they promise to arrive, even if Jolly Roger is waving at him. But if you attack him, I'll make sure he strikes back tenfold.\n I want you to wipe that smug smirk off my student's face and break his toy. Sink 'unsinkable', and I'll work on the 'War Dog' blueprint.";
			link.l1 = "I'm thinking, maybe it's better to try to board him? What if he's really unsinkable?";
			link.l1.go = "WarDogUpgrade_6";

		break;

		case "WarDogUpgrade_6":
			dialog.text = "If my apprentice made a ship of his own, it would be impossible to sink it. But I doubt that this knucklehead was able to understand the intent behind my calculations. Besides, at the time of the theft, this blueprint wasn't completely ready, so it should definitely have vulnerabilities.\nForget about the boarding! Not only is there a team there enough to populate a couple of cities, besides you will only confirm the rumor about its invincibility - they say that you can only defeat him if you cut out the crew. Just put him down.";
			link.l1 = "Do you have any tips on how to do this?";
			link.l1.go = "WarDogUpgrade_6_2";
		break;

		case "WarDogUpgrade_6_2":
			dialog.text = "I don't even know... To be honest, I don't know how to break ships, I've only been building them all my life. But you can try to stuff a couple of brigs with gunpowder and use them as fire-ships, if of course you can convince the crew to risk their lives.";
			link.l1 = "Your methods are too radical. And you say you can't break things. Okay, I'll figure it out somehow. So where can I find this ship?";
			link.l1.go = "WarDogUpgrade_6_1";
		break;

		case "WarDogUpgrade_6_1":
			sld = GetCharacter(NPC_GenerateCharacter("W_Titanic", "off_eng_1", "man", "man", 99, ENGLAND, -1, true));
			FantomMakeCoolestSailor(sld, SHIP_PRINCE, "Titanic", CANNON_TYPE_CANNON_LBS48, 100, 100, 100);
			ref W_Titanic = &RealShips[sti(sld.Ship.Type)];
			W_Titanic.HP = 30000;
			W_Titanic.MastMultiplier = 0.01;
			W_Titanic.TurnRate = 10;
			W_Titanic.MaxCrew = 3500;
			W_Titanic.MinCrew = 3000;
			W_Titanic.SpeedRate = 14;
			W_Titanic.WindAgainstSpeed = 4;
			//W_Titanic.HullArmor = 60;
			W_Titanic.Capacity = 100000;

			SetCrewQuantityFull(sld);
			ProcessHullRepair(sld, 100.0);

			sld.ship.Crew.Morale = 100;
			ChangeCrewExp(sld, "Sailors", 50);
			ChangeCrewExp(sld, "Cannoners", 50);
			ChangeCrewExp(sld, "Soldiers", 50);

			AddItems(sld, "talisman3", 1);
			EquipCharacterByItem(sld, "talisman3");

			SetHalfPerksToChar(sld, true);
			SetCharacterPerk(sld, "CannonProfessional");

			sld.rank = 60;
			SetSelfSkill(sld, 100, 100, 100, 100, 100);
			SetShipSkill(sld, 100, 100, 100, 100, 100, 100, 100, 100, 100);

			SetSPECIAL(sld, 10, 10, 10, 10, 10, 10, 10);
			SetCharacterGoods(sld,GOOD_FOOD,5000);
			SetCharacterGoods(sld,GOOD_BALLS,50000);
			SetCharacterGoods(sld,GOOD_GRAPES,0);
			SetCharacterGoods(sld,GOOD_KNIPPELS,0);
			SetCharacterGoods(sld,GOOD_BOMBS,50000);
			SetCharacterGoods(sld,GOOD_POWDER,100000);
			SetCharacterGoods(sld,GOOD_PLANKS,150);
			SetCharacterGoods(sld,GOOD_SAILCLOTH,150);
			SetCharacterGoods(sld,GOOD_RUM,200);
			SetCharacterGoods(sld,GOOD_WEAPON,600);
			SetCharacterGoods(sld,GOOD_MEDICAMENT,300);

			sld.Abordage.Enable = false;
			sld.Tasks.CanBoarding = false;
			sld.CantBoardEnemies = true;
			//sld.AlwaysFriend = true;
			sld.AlwaysEnemy = true;
			sld.DontRansackCaptain = true;
			sld.AlwaysSandbankManeuver = true;
			Group_FindOrCreateGroup("WarDog");
			Group_SetType("WarDog", "pirate");
			Group_AddCharacter("WarDog", sld.id);
			Group_SetGroupCommander("WarDog", sld.id);
			SetCharacterRelationBoth(sti(sld.index), GetMainCharacterIndex(), RELATION_ENEMY);

			LAi_SetImmortal(sld, false);
			//Group_SetPursuitGroup("WarDog", PLAYER_GROUP);
			Group_SetTaskAttack("WarDog", PLAYER_GROUP);
			Group_LockTask("WarDog");

			WhisperTitanic_ToRandomTown()
			Group_SetAddress("WarDog", pchar.TitanicCity, "Quest_ships", "reload_fort1_siege");
			pchar.TitanicIsKicking = true;
			SetFunctionNPCDeathCondition("WhisperTitanic_Is_Dead", "W_Titanic", false);
			pchar.quest.WhisperTitanic_SurrenderFort.win_condition.l1          = "location";
			pchar.quest.WhisperTitanic_SurrenderFort.win_condition.l1.location = pchar.TitanicCity;
			pchar.quest.WhisperTitanic_SurrenderFort.function             = "WhisperTitanic_SurrenderFort";

			SetQuestHeader("WhisperTitanic");
			AddQuestRecord("WhisperTitanic", 1);
			AddQuestUserData("WhisperTitanic", "sCity", XI_ConvertString("Colony" + pchar.TitanicCity + "Gen"));
			dialog.text = "They say he's besieged now. The Spanish keep sending reinforcements, but the 'Titanic' is sinking the squadron behind the squadron. He'll probably be there for a long time before he ruins the city clean. I told you, you can go ahead before you go, you can order some minor upgrades for the 'War Dog'. They'll be in addition to the overhaul.";
			link.l1 = "Well, let's see what can be improved.";
			link.l1.go = "ship_tunning_start";
		break;

		case "WarDogUpgrade_7":
			dialog.text = "Well, how will you please the old man?";
			link.l1 = "Nothing yet.";
			link.l1.go = "exit";
			if(CheckAttribute(pchar, "TitanicIsDown"))
			{
				link.l1 = "The unsinkable has sunk. Your student's reputation has been trampled into the mud forever. But, I must admit, your student was not a blunder, it was not easy.  ";
				link.l1.go = "WarDogUpgrade_8";
			}
		break;

		case "WarDogUpgrade_8":
			DeleteAttribute(pchar, "TitanicIsDown");
			dialog.text = "Yes, good news. I should have more clients now. They say the Titanic sailed somewhere to the north, where it ran into a huge ice floe, which safely pierced its hull.";
			link.l1 = "Who told you such nonsense? I personally sank it!";
			link.l1.go = "WarDogUpgrade_9";
		break;

		case "WarDogUpgrade_9":
			dialog.text = "Really? Okay, okay, I believe you.";
			link.l1 = "So what about the improvement for the Dog? You promised to take care of him if I helped with the Titanic.";
			link.l1.go = "WarDogUpgrade_10";
		break;

		case "WarDogUpgrade_10":
			dialog.text = "Uh, so... I did couple of new blueprints, considering what to improve\nIt'll take a long time, so it'll cost you a million piastres. ";
			link.l1 = "What?! We didn't agree on this.";
			link.l1.go = "WarDogUpgrade_11";
		break;

		case "WarDogUpgrade_11":
			npchar.WarDogUpgradePayment = true;
			AddQuestRecord("WhisperTitanic", 3);
			dialog.text = "Honey, it's going to take a lot of work. Believe me, under other conditions, I would have demanded much more, and not only money. And this amount is so that I won't starve to death while I'm working.";
			if(sti(pchar.money) >= 1000000)
			{
				link.l1 = "Here's your million, you old miser.";
				link.l1.go = "WarDogUpgrade_pay";
			}
			link.l2 = "Yeah. I need to think about it.";
			link.l2.go = "exit";
		break;

		case "WarDogUpgrade_pay":
			DeleteAttribute(npchar, "WarDogUpgradePayment");
			dialog.text = "Well, let's get started...";
			addMoneyToCharacter(pchar, -1000000);
			link.l1 = "Come on, I'll take a look.";
			link.l1.go = "WarDogUpgrade_pay_2";
		break;

		case "WarDogUpgrade_pay_2":
			AddQuestRecord("WhisperTitanic", 4);
			CloseQuestHeader("WhisperTitanic");
			shTo = &RealShips[sti(Pchar.Ship.Type)];
			iCaliber = sti(shTo.MaxCaliber);
			switch(iCaliber)
			{
				case 28:
					iCaliber = 4;
				break;
				case 32:
					iCaliber = 5;
				break;
			}
			iCaliber = iCaliber + 1;
			switch(iCaliber)
			{
				case 5:
					iCaliber = 32;
				break;
				case 6:
					iCaliber = 36;
				break;
			}
			shTo.MaxCaliber = iCaliber;

			SetShipTuningAdditiveF(Pchar, "SpeedRate", 2.0);
			SetShipTuningAdditiveF(Pchar, "TurnRate", 10.0);
			SetShipTuningAdditiveI(Pchar, "HP", 1000);
			SetShipTuningAdditiveI(Pchar, "MaxCrew", 120);
			SetShipTuningAdditiveF(Pchar, "WindAgainstSpeed", 1.0);
			SetShipTuningAdditiveI(Pchar, "Capacity", 700);
			shTo.HullArmor = 30;
			shTo.OptCrew = 300;

			DeleteAttribute(npchar, "WarDogUpgradePayment");
			DialogExit();
			AddCharacterExpToSkill(pchar, "Repair", 5000);
			SetLaunchFrameFormParam("A month has passed. Watching Alexus work, you learned a lot about shipbuilding and carpentry.", "", 0.1, 15.0);
			LaunchFrameForm();
			WaitDate("",0,0,30, 0, 20);
			npchar.dialog.currentnode = "WarDogUpgrade_pay_1";
		break;

		case "WarDogUpgrade_pay_1":
			dialog.text = "Well, that's it. Now I'm really not ashamed to call 'The Dog' one of my works. You can go to sea now. Just be careful there, I don't want my labors to go to waste.";
			link.l1 = "Okay, I'll go check it out.";
			link.l1.go = "exit";
		break;
		//<-Капремонт пса

		case "DutchmanRepair1":
		    dialog.text = "Oh ho ho, is this really the Flying Dutchman?! I didn't think that I would be able to see him in person one day. To be honest, I never believed in this story, but it's nothing but a real legend! Incredible firepower, a fantastically powerful hull, but at the same time how fast the handsome man walks on the sea surface... Is there something wrong with you?";
            link.l1 = "The ship is magnificent, master, but you can see for yourself what condition it is in. Overgrown sides, cracks in the deck, warped trim - it looks completely unpresentable, and it's unsafe for sailors - it feels like the ship is about to fall apart.";
			link.l1.go = "DutchmanRepair2";
		break;

		case "DutchmanRepair2":
			dialog.text = "Ahem... and it seemed to me that such a view is rather a virtue for captain of your field of activity... Well, anyway, it's not for me to judge. Well, I can repair this ship, but it's very laborious and will require serious resources. I will have to completely redo the skin, make several design changes, except that I will not touch the frame. I'll take a million piastres for this.";
			if (sti(PChar.money) >= 1000000)
			{
				link.l1 = "The price bites, but the welfare of the ship is more important to me. Here's your money.";
				link.l1.go = "DutchmanRepair3";
			}
			link.l2 = "Um, it's a little tight for my purse... I'll come back later.";
			link.l2.go = "exit";
		break;

		case "DutchmanRepair3":
			AddMoneyToCharacter(PChar, -1000000);
			AddTimeToCurrent(8, 0);

			bool HullSpecial = false;
			bool SailsSpecial = false;
			bool CannonsSpecial = false;
			bool CuBot = false;
			bool BotPack = false;
			bool HighBort = false;

			bool TuneHP = false;
			bool TuneMast = false;
			bool TuneSpeed = false;
			bool TuneTurn = false;
			bool TuneWA = false;
			bool TuneCap = false;
			bool TuneMaxCrew = false;

			shTo = &RealShips[sti(Pchar.Ship.Type)];
			int HullArm = shTo.HullArmor;

			Pchar.Ship.Cannons.Borts.cannonl = sti(shTo.lcannon) + 2;
			Pchar.Ship.Cannons.Borts.cannonr = sti(shTo.rcannon) + 2;
			Pchar.Ship.Cannons.Borts.cannonf = sti(shTo.fcannon);
			Pchar.Ship.Cannons.Borts.cannonb = sti(shTo.bcannon);
			Pchar.Ship.Cannons = sti(shTo.CannonsQuantity) + 4;

			aref arTo, arFrom;
			makearef(arFrom, pchar.Ship.Cannons);
			makearef(arTo, pchar.tempFD);
		    CopyAttributes(arTo,arFrom);//запоминаем состояние пушек во временном атрибуте

			float MM = stf(shTo.MastMultiplier);
			string ShipName = pchar.ship.name;

			if (CheckAttribute(shTo, "Tuning.HullSpecial")) HullSpecial = true;
			if (CheckAttribute(shTo, "Tuning.SailsSpecial")) SailsSpecial = true;
			if (CheckAttribute(shTo, "Tuning.CannonsSpecial")) CannonsSpecial = true;
			if (CheckAttribute(shTo, "Tuning.CuBot")) CuBot = true;
			if (CheckAttribute(shTo, "Tuning.BotPack")) BotPack = true;
			if (CheckAttribute(shTo, "Tuning.HighBort")) HighBort = true;

			if (CheckAttribute(shTo, "Tuning.HP")) TuneHP = true;
			if (CheckAttribute(shTo, "Tuning.MastMultiplier")) TuneMast = true;
			if (CheckAttribute(shTo, "Tuning.SpeedRate")) TuneSpeed = true;
			if (CheckAttribute(shTo, "Tuning.TurnRate")) TuneTurn = true;
			if (CheckAttribute(shTo, "Tuning.WindAgainstSpeed")) TuneWA = true;
			if (CheckAttribute(shTo, "Tuning.Capacity")) TuneCap = true;
			if (CheckAttribute(shTo, "Tuning.MaxCrew")) TuneMaxCrew = true;

			pchar.DontRefresh = true;
			pchar.Ship.Type = GenerateShipExt(SHIP_FLYINGDUTCHMAN_N, true, pchar);
			SetBaseShipData(pchar);
			pchar.ship.name = ShipName;
			shTo = &RealShips[sti(Pchar.Ship.Type)];
			//shTo.HullArmor = HullArm;
			shTo.MastMultiplier = MM;

			makearef(arFrom, pchar.tempFD);
			makearef(arTo, pchar.Ship.Cannons);
		    CopyAttributes(arTo,arFrom);//копируем старые пушки на новый корабль
			DeleteAttribute(pchar, "tempFD");//удаляем временный атрибут
			pchar.Ship.Cannons.borts.cannonl.damages.c27 = 1.0; //прописываем 4 дополнительных пушки
			pchar.Ship.Cannons.borts.cannonl.damages.c28 = 1.0;
			pchar.Ship.Cannons.borts.lcannon.damages.c27 = 1.0;
			pchar.Ship.Cannons.borts.lcannon.damages.c28 = 1.0;
			pchar.Ship.Cannons.borts.cannonr.damages.c27 = 1.0;
			pchar.Ship.Cannons.borts.cannonr.damages.c28 = 1.0;
			pchar.Ship.Cannons.borts.rcannon.damages.c27 = 1.0;
			pchar.Ship.Cannons.borts.rcannonl.damages.c28 = 1.0;

			if (TuneHP)
			{
				shTo.HP = sti(shTo.HP) + makeint(sti(shTo.HP)/5);
				shTo.Tuning.HP = true;
			}
			if (TuneMast)
			{
				shTo.Tuning.MastMultiplier = true;
			}
			if (TuneSpeed)
			{
				shTo.SpeedRate = (stf(shTo.SpeedRate) + stf(shTo.SpeedRate)/10.0);
				shTo.Tuning.SpeedRate = true;
			}
			if (TuneTurn)
			{
				shTo.TurnRate = (stf(shTo.TurnRate) + stf(shTo.TurnRate)/5.0);
				shTo.Tuning.TurnRate = true;
			}
			if (TuneWA)
			{
				shTo.WindAgainstSpeed = FloatToString(stf(shTo.WindAgainstSpeed) + 0.5* stf(shTo.WindAgainstSpeed) / stf(shTo.Class) + 0.005, 2);
				shTo.Tuning.WindAgainstSpeed = true;
			}
			if (TuneCap)
			{
				shTo.Capacity = sti(shTo.Capacity) + makeint(sti(shTo.Capacity)/5);
				shTo.Tuning.Capacity = true;
			}
			if (TuneMaxCrew)
			{
				shTo.MaxCrew = sti(shTo.MaxCrew) + makeint(sti(shTo.MaxCrew)/10);
				shTo.Tuning.MaxCrew = true;
			}

			if (HullSpecial)
			{
				shTo.Tuning.HullSpecial = 1;
				shTo.price = makeint(sti(shTo.price)*1.5);
			}
			if (SailsSpecial)
			{
				shTo.Tuning.SailsSpecial = 1;
				shTo.price = makeint(sti(shTo.price)*1.35);
			}
			if (CannonsSpecial)
			{
				shTo.Tuning.CannonsSpecial = 1;
				shTo.price = makeint(sti(shTo.price)*1.25);
			}
			if (CuBot)
			{
				shTo.Tuning.CuBot = 1;
				shTo.price = makeint(sti(shTo.price)*1.4);
			}
			if (BotPack)
			{
				shTo.Tuning.BotPack = 1;
				shTo.price = makeint(sti(shTo.price)*1.5);
				shTo.Capacity = sti(shTo.Capacity) + makeint(sti(shTo.Capacity)/2);
				shTo.HP = sti(shTo.HP) - makeint(sti(shTo.HP)/2);
				if (pchar.ship.hp > sti(shTo.HP)) pchar.ship.hp = sti(shTo.HP);
			}
			if (HighBort)
			{
				shTo.Tuning.HighBort = 1;
				shTo.price = makeint(sti(shTo.price)*1.4);
			}

			dialog.text = "Great, I'm starting to repair it... Ready! Now your legend will shine with new colors.";
			link.l1 = "Thank you, Master.";
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // если где-то выход внутри switch  по return не забыть сделать анлод
}

void checkMatherial(ref Pchar, ref NPChar, int good1, int good2, int good3)
{
    int amount;

    amount = GetSquadronGoods(Pchar, good1) - sti(NPChar.Tuning.Matherial1);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial1);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial1);
    }
    RemoveCharacterGoods(Pchar, good1, amount);
    NPChar.Tuning.Matherial1 = sti(NPChar.Tuning.Matherial1) - amount;

    /* amount = GetCharacterItemWithCabin(pchar, good2, true) - sti(NPChar.Tuning.Matherial2);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial2);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial2);
    }
    TakeNItemsWithCabin(pchar, good2, -amount, false);
    NPChar.Tuning.Matherial2 = sti(NPChar.Tuning.Matherial2) - amount;

    amount = GetCharacterItemWithCabin(pchar, good3, true) - sti(NPChar.Tuning.Matherial3);
    if (amount < 0)
    {
       amount = amount + sti(NPChar.Tuning.Matherial3);
    }
    else
    {
        amount = sti(NPChar.Tuning.Matherial3);
    }
    TakeNItemsWithCabin(pchar, good3, -amount, false);
    NPChar.Tuning.Matherial3 = sti(NPChar.Tuning.Matherial3) - amount; */
}
