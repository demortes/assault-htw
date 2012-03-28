#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ack.h"
#include <signal.h>

const struct build_type         build_table[MAX_BUILDING]       =
{

    {
        0, "NOTHING", 0, 0,
        {
            0, 0, 0, 0, 0, 0, 0, 0
        }
        , 0, 1, "Description", "[  ]",
        {
            0, 0, 0, 0, 0, 0
        }
        , 0, 0, 0, 0, 0
    },
    {
        1, "Headquarters", 1000, 1000,
        {
            50, 0, 0, 10, 10, 50, 0, 50
        }
        , 0, 1, "The headquarters is the main structure - This place controls the construction of all the other buildings, and without it, you cannot build any.", "[HQ]",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 3, 0
    },
    {
        2, "Armory", 70, 30,
        {
            10, 0, 5, 0, 5, 0, 0, 5
        }
        , 1, 1, "Sitting at tables of various sizes are people all dressed in white clothing constructing the weapons that are needed for attack and defense. Guns of every type and bombs from the small and delicate to the large and devitating are made here.", "{AA}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 80, 0
    },
    {
        3, "Small Turret", 65, 20,
        {
            15, 1, 0, 0, 0, 5, 0, 1
        }
        , 2, 1, "The inside of the turret is hardly enough for a person to feel comfortable in - It's just a small empty block with some monitors attached to the walls. The turrets defend your base from any, unwanted or not, players. The larger the turret, the farther it fires, and the more damage it does.", ":^^:",
        {
            1, 2, 7, 6, 8, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        4, "Quarry", 50, 20,
        {
            0, 0, 0, 0, 0, 0, 0, 2
        }
        , 1, 1, "A giant hole in the ground is being dug constantly by large robotic drones, carrying large rocks into a storage area. A control tower on the far wall contains three engineers who control the process. In here, you will find rocks (up to 20 can gather at any given time) to use for building.", "{QQ}",
        {
            1, 4, 14, -1, -1, 1
        }
        , 1, 0, 5, 80, 0
    },
    {
        5, "Mine", 60, 30,
        {
            0, 0, 0, 0, 0, 0, 0, 3
        }
        , 1, 1, "A large drill moves through wide tunnels, collapsing the dirt above it while a drone follows it and scans for any useful resources. The resources gathered are gathered in a large pile next to the exit, and more are constantly added. In this structure, you will find gold, copper, silver and iron.", "{MM}",
        {
            4, 14, 8, 6, 4, 4
        }
        , 1, 0, 5, 80, 0
    },
    {
        6, "Storage", 70, 40,
        {
            5, 0, 0, 0, 0, 0, 4, 5
        }
        , 2, 1, "Clips and shells fill carton boxes in the corner of the room. Two men sitting in metal chairs carefully insert bullets into the clips, and fill the cartons. This building generates ammunition for your weapons gathered at the armory.", "{SS}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 80, 0
    },
    {
        7, "Lumberyard", 45, 45,
        {
            5, 0, 0, 0, 0, 0, 0, 0
        }
        , 1, 1, "The ear-bursting sound of high pitched machines never ceases as trees gather up onto the large machine which occupies most of the building. Through a hole in the wall, trees roll into the machine, and come out as clean logs, which you can use for construction.", "{LL}",
        {
            6, 8, 3, 2, -1, 6
        }
        , 1, 0, 5, 80, 0
    },
    {
        8, "Tannery", 50, 35,
        {
            0, 0, 0, 0, 0, 0, 0, 5
        }
        , 1, 1, "The small room on the other side of the entrance is where the skinning work is done - every few minutes, a sheet of skin is thrown through the window onto a gathering pile of skins and furs. The skins can later be used for constructing buildings.", "{TT}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 5, 80, 0
    },
    {
        9, "Turret", 90, 50,
        {
            30, 2, 0, 0, 0, 3, 0, 1
        }
        , 2, 1, "The inside of the turret is hardly enough for a person to feel comfortable in - It's just a small empty block with some monitors attached to the walls. The turrets defend your base from any, unwanted or not, players. The larger the turret, the farther it fires, and the more damage it does.", ":VV:",
        {
            1, 2, 7, 6, 1, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        10, "Large Turret", 100, 60,
        {
            50, 4, 0, 0, 0, 5, 0, 5
        }
        , 13, 1, "The inside of the turret is hardly enough for a person to feel comfortable in - It's just a small empty block with some monitors attached to the walls. The turrets defend your base from any, unwanted or not, players. The larger the turret, the farther it fires, and the more damage it does.", ":!!:",
        {
            1, 2, 7, 6, 1, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        11, "Watchtower", 65, 50,
        {
            0, 0, 0, 0, 0, 10, 5, 15
        }
        , 2, 1, "This wooden structure contains rather primitive forms of defense - Mostly crossbows and bolts. Four torches surround the structure on all four corners, making the tower look impressive from afar and giving it a bright glow.", "^||^",
        {
            8, 4, 3, 7, -1, 8
        }
        , 1, 0, 2, 80, 0
    },
    {
        12, "War Cannon", 70, 70,
        {
            450, 0, 25, 5, 50, 150, 0, 50
        }
        , 13, 1, "A large control panel, equipped with the most high-tech, specially designed, and most sophisticated chair, sits on the far wall. A digital mapping of the area spreads through the entire building. From inside the War Cannon, you can choose to @@eblast@@N any location at a 10X10 radius!", "v||v",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 3, 5, 0
    },
    {
        13, "Weapons Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "The occasional BOOM suggests the development of new, improved weaponry. Papers are scattered all around this large tower; blueprints and designs, mostly... Maybe they're flying around because all the trash cans are already full.", "[WL]",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 4, 10, 0
    },
    {
        14, "Bio Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "This large tower is devided into two floors; one forresearch, which is done in basic laboratories spread throughout the ground floor, and the second is for testing, which is doneinside some of the sterile rooms which make the upper part of the building.", "[BL]",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 4, 10, 0
    },
    {
        15, "Chemical Factory", 80, 60,
        {
            10, 0, 10, 5, 0, 0, 0, 0
        }
        , 14, 1, "The walls are burned through with various acids, creating strange shapes and colors on them. Tubes of strange substances roll around the floor, emitting strange smells and smoke.", "{CC}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 80, 0
    },
    {
        16, "Tech Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "The sights are incredible - In each of the research chambers, new technology makes its first appearance, and at its best! From people warping from one room to another, to flash grenades.", "[TL]",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 4, 10, 0
    },
    {
        17, "Warp", 80, 65,
        {
            10, 0, 5, 5, 0, 0, 0, 5
        }
        , 16, 1, "A large vortex of colored lights floats in the center of the building, controlled by dozens of computers attached to the walls. From this place, you can type @@ewarp@@N to instantly teleport - the radius depends on the structure's level.", "{WW}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 80, 0
    },
    {
        18, "Explosives Supplier", 60, 60,
        {
            5, 0, 2, 4, 3, 0, 0, 0
        }
        , 13, 1, "A back room, quite nicely camouflaged since it has no real visible door, is where the explosive work is done. Every few minutes, a small trapdoor opens, and a hand adds an explosive to the pile. To use grenades, you first @@earm@@N them, then @@ethrow@@N them to the direction you'd like. But be careful, because you only have three seconds to get rid of them!", "{ES}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 80, 0
    },
    {
        19, "Medical Center", 70, 48,
        {
            5, 10, 0, 5, 5, 0, 0, 0
        }
        , 14, 1, "Robotic arms controlled by a few people on the control tower above perform experiments on, mostly, animals. Verious liquids and pills have found their way onto the floor, while the ones that don't kill you have been placed on shelves near the entrance. You can use these med. packs by typing @@eheal@@N.", "{MC}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 80, 0
    },
    {
        20, "Garage", 60, 36,
        {
            15, 0, 0, 0, 0, 5, 0, 5
        }
        , 13, 1, "Large robots perform most of the work here - Some gather materials and sort them by type, the others melt, process, and construct vehicles using them. You can use the parts generated here to @@econstruct@@N vehicles - the better the parts, the better the tank. After you have your vehicle, you can @@eenter@@N it, then @@eexit@@N. You can also come here to @@arepair and restock@@N your vehicle if you need to by placing it inside and... going out for lunch...", "{GG}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 10, 0
    },
    {
        21, "Radar", 64, 41,
        {
            10, 0, 0, 5, 5, 0, 0, 0
        }
        , 16, 1, "A large computer monitor hands on the wall, showing endless information about everything around you. From this building, you can @@escan@@N for other players or buildings - the higher the Radar's level, the more you will find in a larger radius.", "{RR}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        22, "Jammer", 150, 100,
        {
            0, 0, 150, 15, 50, 0, 0, 0
        }
        , 16, 1, "Electrinocs cover entire desks and walls, filling the building with constant beeping. A large satellite monitor hangs on the ceiling, showing a grid of the nearby structures, while a team of men work hard to keep them camoflauged.", "{JJ}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 80, 0
    },
    {
        23, "Warehouse", 40, 250,
        {
            30, 0, 0, 0, 0, 20, 0, 20
        }
        , 2, 1, "The place is beyond messy - There is so much dust and junk all over, that it's almost impossible to see anything else! The building is very large, so searching for useful stuff here might be a little hard.", "[WW]",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 5, 0
    },
    {
        24, "Bot Factory", 90, 70,
        {
            30, 0, 0, 20, 0, 0, 0, 20
        }
        , 14, 1, "Groups of scientists lock themselves up in some of the large room this compound offers, testing robotic devices of all sorts. Small drones float through the buildings, scanning and repairing damage, as well as intruders.", "{BF}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        25, "Forge", 80, 40,
        {
            30, 10, 0, 0, 0, 0, 0, 20
        }
        , 5, 1, "A blazing display of fire makes the far wall, and piles of resources of all types gather next to it. This is where you can @@echunk@@N items together to make large pieces.", "[FR]",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 10, 0
    },
    {
        26, "SCUD Launcher", 800, 800,
        {
            2000, 0, 0, 100, 100, 0, 0, 200
        }
        , 14, 3, "A huge missile stands in the middle of this round room, constantly worked on by a dozen people at any given time. When the missile is ready, you can @@eblast@@N at any coordinates you'd like!", "!SS!",
        {
            4, 4, 4, 4, 4, 4
        }
        , 1, 0, 1, 1, 0
    },
    {
        27, "Armorer", 80, 45,
        {
            5, 0, 0, 5, 0, 0, 0, 5
        }
        , 1, 1, "Vests and helmets hang around the rather large room, while more are added every few minutes by one of the workers, who spend their time in the back room creating the armor. In the corner, a man sits and reads a newspaper, smoking his cigar, while shouting orders to anyone who even tries to rest. Here, you will find armor to use, as well as be able to repair your damaged armor.", "{AR}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 80, 0
    },
    {
        28, "Nuke Launcher", 800, 600,
        {
            2000, 0, 0, 100, 100, 0, 0, 200
        }
        , 16, 3, "Four large missiles corner the large building, each being developped by its own team of technicians. A computer in the center of the room provides quick access to information about the nukes, as well as some games the staff downloaded to keep themselves from getting bored to death building missile after missile. From the nuke launcher, you can @@eblast@@N any coordinates you'd like.", "!NN!",
        {
            4, 4, 4, 4, 4, 4
        }
        , 1, 0, 1, 1, 0
    },
    {
        29, "Airfield", 105, 40,
        {
            15, 0, 0, 0, 0, 5, 0, 5
        }
        , 27, 1, "The airfield is nothing special - just a long paved runway with a small control tower on the side. You can @@econstruct@@N an aircraft using the parts gathered in here. Flying an aircraft is difficult - They are always on the move, and if you run into a building or a wall, it will crash, and you will likely die. You can change the flight direction at any time by going in that direction, and the plane will continue on that path. You can @@eexit@@N the aircraft at any time, or get it back to the airfield to land it.", "{AF}",
        {
            1, 8, 2, 7, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        30, "Warp Tower", 90, 60,
        {
            10, 0, 0, 20, 20, 0, 0, 10
        }
        , 17, 1, "The building is a very tall and narrow tower, consisting of a small control panel on one of the walls, and a strange glowing sphere on the top. A bunch of electrical wires are spread throughout the entire tower.", ":WW:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        31, "Rock Tower", 45, 45,
        {
            0, 0, 0, 0, 0, 40, 20, 0
        }
        , 4, 1, "The tower is nothing more than a tree-house with a small hatch for rocks to be gathered from. It is made of strong logs, and carries a large pile of heavy rocks near the ladder leading outside.", ":%%:",
        {
            1, 4, 4, 1, 1, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        32, "Fire Turret", 65, 20,
        {
            0, 0, 0, 0, 0, 0, 0, 0
        }
        , 0, 1, "Tanks and tanks of napalm are spread around the small structure. A cannon, attached to all the tanks, spins around in circles through a line in the walls, scanning for enemies.", ":ff:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        33, "Cookie Factory", 100, 60,
        {
            500, 0, 0, 0, 0, 100, 0, 300
        }
        , 14, 1, "Ahhh, the small of fresh-baked cookies... What could be more wonderful? How about freshly baked EXPLOSIVE Cookies! You got it, by popular demand, the cookie factory is now here, generating cookie launchers and explosive cookies to load them with. Enjoy, it's one nice weapon.", "{OO}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        34, "Acid Turret", 65, 20,
        {
            0, 0, 0, 0, 0, 0, 0, 0
        }
        , 14, 1, "Barrels leaking acid are spread throughout the turret, dripping slowly into the well burned-out floor. Four cannons, one on each side of the building, scan for enemies.", ":`':",
        {
            1, 2, 7, 8, 1, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        35, "Government Building", 1000, 1000,
        {
            1000, 100, 50, 50, 50, 200, 300, 300
        }
        , 1, 1, "The government building contains the paperwork and designs for civilian buildings. People constantly run around the many offices, yelling and shouting, throwing forms... But most of the people who work here don't do anything. Kinda reminds you of the real world when you think about it...", "[GV]",
        {
            1, 2, 7, 8, 6, 1
        }
        , 0, 0, 0, 1, 0
    },
    {
        36, "Bar", 180, 100,
        {
            5, 0, 0, 0, 0, 10, 0, 300
        }
        , 35, 1, "The lights are always dimmed, and the people are always bikers and whores. And just like on TV, the barkeep here knows everything... He might even be willing to part some information if you @@eleave enough gold here@@N, though the costs might change according to the info he has to give.", "|BR|",
        {
            1, 2, 7, 8, 6, 14
        }
        , 0, 0, 6, 10, 0
    },
    {
        37, "Bank", 140, 70,
        {
            5, 0, 0, 0, 0, 10, 0, 10
        }
        , 35, 1, "Reports, reports, and more reports, written on what could easily be an entire rain-forest of paper, are spread around the entire building - On the floor, on the walls, and in the air. In here, you can see your financial @@estats@@N, and brag about them to your friends.", "|BK|",
        {
            1, 2, 7, 8, 6, 14
        }
        , 0, 0, 6, 10, 0
    },
    {
        38, "Club", 115, 80,
        {
            500, 0, 0, 0, 0, 200, 0, 10
        }
        , 35, 1, "Dim lights, loud music, and tons of alcohol! The underground club is packed with people, enough to render you completely hidden from others' sight, even the bars will not give information of someone who is hiding in a club. ", "|CL|",
        {
            1, 2, 7, 8, 6, 14
        }
        , 0, 0, 6, 10, 0
    },
    {
        39, "Flamespitter", 80, 500,
        {
            1500, 0, 300, 300, 300, 0, 0, 400
        }
        , 32, 3, "Large canisters of napalm are spread throughout the small bunker-like structure, with two giant devices on each side attached to them, automatically focusing on any movement in the area. A small electronic map is attached to all the napalm cannons, marking its fire range. The flamespitter is a powerfull defense, it'll blast in a 7x7 radius, does a lot of damage to people, tanks.", ":FF:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        40, "Laser Tower", 65, 50,
        {
            0, 0, 0, 0, 0, 0, 0, 0
        }
        , 16, 1, "The tower stands tall above the ground, allowing for good visibility. Two types of windows surround the structure - the round ones all have some sort of laser cannons behind them, and the square ones are clear, to allow scouting. For some reason, the tower seems to not be too stable, and it always feels like it's going to collapse.", "|''|",
        {
            3, 4, 7, 8, -1, 3
        }
        , 1, 0, 2, 80, 0
    },
    {
        41, "Laser Workshop", 300, 210,
        {
            300, 0, 0, 500, 0, 0, 0, 200
        }
        , 16, 2, "The building is split into several labs, each holding several scientists meddling with strange devices. The corners of the room are all covered by large cabinets, containing records and other pieces of research. The center of the roof appears to be scorched... Someone's been playing around!", "{LS}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        42, "Solar Facility", 200, 160,
        {
            90, 50, 70, 0, 40, 0, 0, 100
        }
        , 41, 1, "The roof is completely torn out, allowing for as much sunlight as possible to go through to the solar-panel tiles on the floor. A large canister on the far wall contains broken pieces of weaponry, while an even larger one right next to the entrance is full of operational equipment.", "{SF}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        43, "Implant Research", 120, 90,
        {
            100, 0, 50, 50, 40, 0, 30, 0
        }
        , 24, 1, "Robotic arms attached to a rail on the ceiling pass through the different labs, carrying parts and equipment. A small surgery room is on the eastern side of the room, where operations for implants are performed... though from the looks of it, it seems a better idea to do it yourself.", "{IR}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        44, "Atom Bomber", 800, 800,
        {
            0, 0, 0, 0, 0, 0, 0, 0
        }
        , 0, 1, "This underground bunker is hot like hell, and it appears that there aren't any cooling mechanisms installed here either. Researchers dressed in white experiment with napalm, acids, and even radioactive material, trying to create the ultimate missile, which stands partially built on the outside.", "!AA!",
        {
            4, 4, 4, 4, 4, 4
        }
        , 1, 0, 1, 1, 0
    },
    {
        45, "Hunting Lodge", 60, 60,
        {
            0, 0, 0, 0, 0, 0, 0, 100
        }
        , 35, 1, "The lodge is a warm and comfortable place, made mostly out of logs and sticks. The walls are covered with what appears to be slots for trophie heads, as well as pictures and stories about some of the largest animals ever hunted. There is also a dart board on the table you can play with.", "|HL|",
        {
            6, 8, 7, 2, 1, 6
        }
        , 0, 0, 6, 80, 0
    },
    {
        46, "Doomsday Device", 1000, 1000,
        {
            5000, 1000, 2000, 2000, 2000, 1500, 2500, 2500
        }
        , 26, 4, "Hundreds of scientists crawl throughout this huge structure, working in dozens of laboratories, all completely sealed, and all marked hazardous. A control panel on the wall marks the estimated time before the completion of another dose of the deadly SARS virus, you can use the @@eDoom@@N command to set it loose once it is finished.", "!DV!",
        {
            4, 3, 4, 4, 4, 4
        }
        , 1, 0, 1, 1, 0
    },
    {
        47, "Mining Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "The labs seems like a rather deserted place, with only a few people, not even dressed like real scientists, talking in small rooms. Very old blueprints flutter about in the wind, which, due to the lack of walls in certain areas, doesn't have a problem entering.", "[ML]",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 4, 10, 0
    },
    {
        48, "Improved Mine", 65, 35,
        {
            0, 0, 0, 0, 0, 0, 0, 5
        }
        , 47, 1, "A large drill moves through wide tunnels, collapsing the dirt above it while a drone follows it and scans for any useful resources. The resources gathered are gathered in a large pile next to the exit, and more are constantly added. In this structure, you will find gold, copper, silver and iron.", "{IM}",
        {
            4, 7, 1, 2, 8, 14
        }
        , 1, 0, 5, 80, 0
    },
    {
        49, "Refinery", 80, 20,
        {
            200, 0, 30, 30, 30, 0, 0, 5
        }
        , 47, 1, "A large pot, containing what appears to be some sort of really hot acid, rests in the corner of the structure. A special drop zone has been set away from it, where a robotic arm keeps passing by, scanning for items to lift. You can use the building with the @@eRefine@@N command.", "{RF}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 80, 0
    },
    {
        50, "Marketplace", 100, 30,
        {
            200, 0, 70, 0, 0, 100, 0, 0
        }
        , 47, 1, "This place is desguised as an old shack on the outside, but inside, in the basement, hundreds of people are filling the building with noise and items. An electronic screen listing the weapon and armor requests hangs on the wall, updating every number of seconds. Maybe you can @@eSell@@N something.", "{MP}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 1, 0
    },
    {
        51, "Magnet Tower", 90, 45,
        {
            400, 50, 65, 200, 20, 0, 0, 0
        }
        , 47, 1, "A group of extremely high-powered magnets spin in alignment with each other at the very top of this tower.  Fortunately for you, everything you own has been demagnetized to these particular magnets.  Unfortunately for everyone else, everything they own hasn't.  Anyone coming too close to your tower will soon find themselves at a loss for some of the things they brought with them.", ":MT:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        52, "Shield Generator", 100, 600,
        {
            100, 0, 100, 100, 100, 0, 0, 0
        }
        , 47, 1, "Catwalks line the walls of this immense building, providing the only safe passage through the room. Large metallic disks spin around in opposite directions on the ceiling and floor, creating constant lightning strikes between the two circles. A control room against one of the walls houses the 4 scientists running the machine, as well as tons of wires leading off to other places.", "{SG}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        53, "Traffic Jammer", 140, 100,
        {
            1000, 0, 0, 0, 0, 0, 0, 0
        }
        , 47, 1, "", ":TJ:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        54, "Cloning Facility", 200, 150,
        {
            100, 150, 200, 250, 300, 350, 400, 450
        }
        , 14, 2, "Tubes are everywhere, large and small. A table against the wall holds a rack of small test tubes, each of which holds a liquid of a different color. Large floor to ceiling tubes hold partially developed creatures, most of which appear to be humanoid. Several scientists run about the room checking on the various tanks, and their inhabitants.", "{CF}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        55, "Processing Plant", 100, 100,
        {
            200, 0, 30, 30, 30, 0, 0, 5
        }
        , 5, 1, "The plant floor teems with life as many different people in lab coats rush about, in and out of various rooms marked with scientific-sounding names.  Some rooms have smoke wafting out, others emit a soft bubbling sound.  A central area in the plant, built specifically for the refining of chemicals and elements, holds test tubes, beakers, vials, and other items that remind you of your first chemistry set as a kid.  Through chemistry, one can make all sorts of interesting compounds.", "{PP}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 6, 80, 0
    },
    {
        56, "Water Pump", 65, 65,
        {
            10, 0, 0, 0, 0, 15, 0, 0
        }
        , 2, 1, "The roar of water rushing and echoing through this building is almost deafening.  A series of small pipes capture water straight from the source and channel it up to a larger pipe, connected to an extremely powerful pneumatic pump.  This aquatic cannon is capable of swivelling in a complete circle, making anyone that wanders too close very unhappy.", ":__:",
        {
            5, -1, -1, -1, -1, 5
        }
        , 1, 0, 2, 80, 0
    },
    {
        57, "Hydro Pump", 100, 100,
        {
            0, 0, 0, 0, 0, 0, 0, 0
        }
        , 0, 1, "The tangled mess of pipes and storage containers vibrate as water is sucked into them from the conveniently-located water source below.  Water is fed into a short, stubby cannon with an enormously wide barrel.  This specially-made pump cannon is fairly inaccurate compared to others, but the sheer amount of the payload can knock a person out for a brief amount of time.", ":--:",
        {
            5, -1, -1, -1, -1, 5
        }
        , 1, 0, 2, 80, 0
    },
    {
        58, "Tunnel", 75, 60,
        {
            0, 0, 0, 0, 0, 20, 30, 20
        }
        , 5, 1, "Large tunneling drones are excavating a hole here. The tunnel leads rather steeply down underground, supported only by a few wooden beams. Despite the powerful lights lining the tunnel walls, it is impossible to see where the tunnel ends.", "{->}",
        {
            1, 2, 7, -1, -1, 1
        }
        , 1, 0, 0, 80, 0
    },
    {
        59, "Surface Jolter", 200, 200,
        {
            100, 0, 0, 0, 0, 0, 0, 70
        }
        , 58, 1, "Sonar pings as workers dilligently scan the underground for intruders.  Underneath of this structure, a series of high-pressure pneumatic rams are perched, waiting for the signal to start a cave in down below, within their range.", "{>J}",
        {
            1, 2, 4, 7, 8, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        60, "Computer Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "A symphony of humming and beeping welcomes passers-by to the computer lab.  All around you, robots go about their set tasks, being monitored by their programmers.  Computing systems are constantly being designed and tested here, in the hopes that they will one day be set into mass production.  This building truly serves host to the pinnacle of technological accomplishments.", "[CL]",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 4, 10, 0
    },
    {
        61, "Battery", 100, 75,
        {
            20, 0, 0, 30, 0, 0, 0, 0
        }
        , 60, 1, "Conveyor belts drone on as batteries are produced in this streamlined factory.  These batteries are designed specifically to be used in computer systems, allowing one to keep all of his or her systems operational.", "{BT}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 80, 0
    },
    {
        62, "Chip Factory", 105, 90,
        {
            100, 0, 20, 0, 10, 0, 0, 0
        }
        , 60, 1, "Drones flock all about in the factory, each with a set task at hand.  Some are assigned to manufacture cases, others to program the chips directly, and others still to assemble all the hardware that goes into making a computer.  Regardless, every drone's job is essential to having the most top-of-the-line computing systems.", "{CH}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        63, "Web Research", 135, 100,
        {
            200, 0, 5, 2, 2, 0, 0, 0
        }
        , 60, 1, "The walls are lines with the latest high tech computers, and the room is filled with the sound of typing. Every computer has someone working at it, searching the internet for firewall and antivirus upgrades, though they dont seem to care about the legality of how they obtain them.", "{WR}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        64, "Transmission Tower", 105, 60,
        {
            400, 0, 30, 10, 10, 0, 0, 0
        }
        , 60, 3, "Metal poles and beams reach high up into the sky, ending in a long, delicate antenna. A small beige building sits under the tower, housing three of the worlds most powerful computers and four of the worlds best technicians. The central pole of the tower rises out of the center of the control room, leading up to the tower top.", ":TT:",
        {
            1, 2, 7, 8, 8, 1
        }
        , 1, 0, 3, 80, 0
    },
    {
        65, "Online Market", 145, 80,
        {
            350, 0, 50, 25, 50, 0, 0, 0
        }
        , 60, 1, "The tall floor to ceiling walls of many cramped cubicles fill the room, giving everyone secrecy as they go about their own private online activities. Most of the users are participating in the trade of resources, getting rid of what they dont need for something more useful.", "{OM}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 80, 0
    },
    {
        66, "Boom", 50, 50,
        {
            500, 0, 0, 0, 0, 0, 0, 0
        }
        , 13, 3, "Thin and poorly-fired clay bricks form a set of four simple walls here, in the dirt.  Cardboard has been placed atop ricketty wooden trusses as roofing, and atop them, junk plastics and cellophane have been painted black and tacked down, as to resemble roofing.  This 'building' is stacked to the ceiling with crates and drums of explosives, all of which are electronically fused to a single plunger detonator in the center of the room.  One would simply type 'BOOM' to drop the plunger.", ":XX:",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 3, 5, 0
    },
    {
        67, "Hackers Hideout", 120, 73,
        {
            200, 0, 30, 10, 20, 0, 0, 0
        }
        , 60, 2, "Beneath asphalt shingles and ricketty barn-siding, one may come to find the hideout of a hacker.  Here, the inhabitant may seek refuge whilst tracing the effects of their acts of sabotage against their neighbors and enemies.  Little more than a round pinewood tables, two bentwood chairs, and a musty foam rubber bedroll matress furnish this dim and damp establishment.", "{HF}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 1, 0
    },
    {
        68, "Sniper Nest", 40, 50,
        {
            0, 0, 0, 0, 0, 0, 40, 20
        }
        , 2, 1, "A sturdy platform constructed of 2x6 planks rests atop a well-braced series of steel legs, here.  This tower extends up a relatively short distance, and is further concealed by well-placed natural camoflauge, such as shrubs, briar thickets and trees.  This vantage affords the sniper greater control over the surrounding land by offering a better range of fire.", ":->:",
        {
            1, 2, 7, 8, 8, 1
        }
        , 1, 0, 3, 80, 0
    },
    {
        69, "Trap", 60, 50,
        {
            100, 0, 10, 0, 0, 0, 40, 0
        }
        , 1, 1, "", ":():",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        70, "Hackport", 1000, 500,
        {
            2000, 0, 1000, 1000, 1000, 0, 40, 0
        }
        , 60, 4, "In this room is a single computer terminal hooked up to a powerful transmitter. From here you can type @@eDoom@@N to connect to a satellite orbiting the Earth and send a signal to all computers everywhere and implant a trojan horse virus.", "!HP!",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 1, 1, 0
    },
    {
        71, "Underground Turret", 100, 0,
        {
            40, 0, 0, 0, 0, 50, 0, 20
        }
        , 9, 3, "", ":!!:",
        {
            14, -1, -1, -1, -1, 14
        }
        , 1, 0, 2, 100, 0
    },
    {
        72, "Zap", 10, 0,
        {
            0, 0, 0, 0, 0, 0, 0, 2
        }
        , 71, 3, "@@N", "....",
        {
            -1, 14, -1, -1, -1, -1
        }
        , 1, 0, 5, 100, 0
    },
    {
        73, "Dirt Turret", 50, 0,
        {
            50, 0, 0, 0, 0, 50, 0, 20
        }
        , 71, 3, "", ":DT:",
        {
            14, -1, -1, -1, -1, -1
        }
        , 1, 0, 2, 100, 0
    },
    {
        74, "Tracer Turret", 40, 20,
        {
            40, 1, 0, 0, 0, 1, 0, 1
        }
        , 76, 1, "The tracer turret, whilst firing a much weaker barrage than it's larger cousins, is constructed atop a ground-breakingly tall tower, affording it a range more than three times greater than many other defense turrets, giving it a greater ability to volley down fire upon an invading army before they reach the primary base defense systems.", ":$$:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 80, 0
    },
    {
        75, "Snow Digger", 80, 60,
        {
            0, 0, 0, 0, 0, 40, 0, 10
        }
        , 1, 1, "", ":\\:",
        {
            11, -1, 6, -1, -1, -1
        }
        , 1, 0, 2, 100, 0
    },
    {
        76, "Defense Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "The defense lab is organized with the advancement of defensive turrets to suit anyones' needs.  The blueprints and schematics of these devices are posted on the wall pending development.  Scale models have been embedded into a small layout of terrain showing the defensive turrets versatiliaty on any type ground.", "[DL]",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 4, 10, 0
    },
    {
        77, "Stungun", 150, 800,
        {
            20, 0, 0, 0, 5, 0, 0, 0
        }
        , 76, 1, "This short, cramped defense turret bears a large, fork-shaped implement atop it's cylindrical stack, from which an overwhelming amount of static electricity can be emitted towards an enemy at a range slightly greater than that of a small turret, as to stun and incapacitate them.", ":SG:",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 2, 100, 0
    },
    {
        78, "Poison Turret", 220, 110,
        {
            35, 10, 0, 0, 0, 5, 0, 0
        }
        , 76, 1, "A small machine here dips large darts into a vat of liquid. The poison is produced by a larger machine to keep the vat full. When hit by the turret, enemies move much more slowly.", ":P^:",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 2, 100, 0
    },
    {
        79, "Gold Mine", 10, 5,
        {
            -1, -1, -1, -1, -1, -1, -1, -1
        }
        , 0, 1, "", "=GO=",
        {
            0, 0, 0, 0, 0, 0
        }
        , 0, 0, 6, 100, 1
    },
    {
        80, "Computer Store", 10, 5,
        {
            -1, -1, -1, -1, -1, -1, -1, -1
        }
        , 0, 1, "", "=CS=",
        {
            0, 0, 0, 0, 0, 0
        }
        , 0, 0, 6, 100, 1
    },
    {
        81, "Lava Thrower", 115, 90,
        {
            70, 0, 0, 20, 20, 30, 0, 0
        }
        , 2, 1, "A large tube covered in fire-proof material decends into the boiling rock. Lava gets sucked up the tube, then it is transferred to a catapult-like device to hurl the molten rock towards the enemy.", ":)(:",
        {
            9, -1, -1, -1, -1, 9
        }
        , 1, 0, 2, 100, 0
    },
    {
        82, "Cellular Company", 10, 5,
        {
            -1, -1, -1, -1, -1, -1, -1, -1
        }
        , 0, 1, "Once a bustling and succesful enterprise, this telecommunications firm has since been annexed by an invading army, and has been vacated by it's original occupants.  Office furniture, financial data and electronic equipment all remain relatively undisturbed by the exodus of employees.  Perhaps this facility would afford one the ability to transmit a broadcast...", "=CC=",
        {
            0, 0, 0, 0, 0, 0
        }
        , 0, 0, 6, 100, 1
    },
    {
        83, "Encryption Pod", 150, 150,
        {
            30, 0, 10, 15, 20, 0, 0, 0
        }
        , 76, 1, "A powerful machine rests straight up here. On the side is a walkway to a small covered cubical. Inside there, a field is generated to block all incoming computer transmissions to your person, blocking any attempt to connect to your personal computers.", "{EP}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 100, 0
    },
    {
        84, "Armor Factory", 300, 200,
        {
            100, 0, 0, 10, 0, 0, 10, 0
        }
        , 76, 3, "Specialized computers scan all the equipment left on the ground, searching for imperfections and areas which the computer can enhance.  The computers run through millions of calculations on the structural integrity of the armor, estimating where overall defense can be increased without weakening other spots of the armor.  Leave your armor on the ground here and it will slowly improve in quality.", "{A+}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 3, 0
    },
    {
        85, "Subway", 10, 5,
        {
            -1, -1, -1, -1, -1, -1, -1, -1
        }
        , 0, 1, "The long, dark tunnels of an ancient mass-transit system seem to stem in all directions beneath the surface of the ground here.  The engines and railcars once used to transport the residents and laborers of a major city now rest unattended, and though the rails and equipment have been neglected for quite some time, it is clear that one might still use these facilities as a means to transport themselves about the world.", "=SW=",
        {
            0, 0, 0, 0, 0, 0
        }
        , 0, 0, 6, 100, 1
    },
    {
        86, "Shop", 10, 5,
        {
            -1, -1, -1, -1, -1, -1, -1, -1
        }
        , 0, 1, "", "=SP=",
        {
            0, 0, 0, 0, 0, 0
        }
        , 0, 0, 6, 100, 1
    },
    {
        87, "City Hall", 10, 5,
        {
            -1, -1, -1, -1, -1, -1, -1, -1
        }
        , 0, 1, "", "=()=",
        {
            0, 0, 0, 0, 0, 0
        }
        , 0, 0, 6, 100, 1
    },
    {
        88, "Communications Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "", "[UL]",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 4, 10, 0
    },
    {
        89, "Spy Training Center", 200, 100,
        {
            100, 0, 0, 30, 0, 0, 30, 0
        }
        , 88, 1, "Here various equipment and simulations are placed about. From this place all the great things a spy can be taught is learned here.", "{ST}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 10, 0
    },
    {
        90, "Transmitter", 750, 1500,
        {
            2000, 0, 1250, 1000, 1000, 0, 0, 0
        }
        , 88, 3, "", "!TM!",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 3, 1, 0
    },
    {
        91, "Spy Quarters", 180, 100,
        {
            300, 0, 30, 30, 30, 0, 10, 30
        }
        , 89, 3, "", "{SQ}",
        {
            1, 2, 7, 8, 6, -1
        }
        , 1, 0, 1, 1, 0
    },
    {
        92, "Spy Satellite", 100, 80,
        {
            100, 0, 50, 0, 0, 40, 0, 0
        }
        , 89, 1, "", "{SS}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 1, 10, 0
    },
    {
        93, "Projector", 50, 100,
        {
            50, 0, 50, 10, 0, 0, 0, 0
        }
        , 88, 1, "", ":=<:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        94, "Wave Generator", 80, 70,
        {
            25, 0, 25, 5, 0, 10, 0, 0
        }
        , 88, 1, "", ":=$:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        95, "Psychic Lab", 400, 200,
        {
            0, 0, 0, 25, 25, 0, 0, 0
        }
        , 2, 1, "", "[PL]",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 4, 10, 0
    },
    {
        96, "Psychoster", 100, 90,
        {
            40, 30, 30, 0, 0, 10, 0, 0
        }
        , 95, 1, "", ":o0:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        97, "Mind Tower", 90, 110,
        {
            10, 50, 0, 10, 0, 10, 0, 0
        }
        , 95, 1, "", ":<{:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        98, "Psychic Tormentor", 500, 2000,
        {
            1500, 900, 0, 500, 0, 0, 0, 0
        }
        , 95, 3, "", "!0o!",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 1, 1, 0
    },
    {
        99, "Psychic Shield", 130, 150,
        {
            350, 100, 0, 50, 0, 25, 0, 0
        }
        , 95, 1, "", "{0o}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 1, 0
    },
    {
        100, "Cooler", 79, 69,
        {
            120, 75, 5, 10, 0, 0, 10, 0
        }
        , 95, 1, "", "{--}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 100, 0
    },
    {
        101, "Paradrop", 60, 30,
        {
            100, 0, 0, 5, 0, 0, 20, 0
        }
        , 29, 1, "This strip of airfield is where paradrop planes take off, allowing the owner to parachute to a location of choice. The whirring of plane engines can be heard in the background. Use the paradrop command to take off.", "{(=}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 100, 0
    },
    {
        102, "Programmer Shack", 80, 60,
        {
            100, 10, 0, 10, 0, 0, 0, 0
        }
        , 60, 1, "Sleep-deprived hackers sit in front of computer monitors, typing away, writing various hacking programs. The programs they write are then copied to a disk for use by other hackers. Here you will find virus disks, password cracking disks, spoofing disks, scanning disks, as well as format disks.", "{PS}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 10, 0
    },
    {
        103, "Gatherer", 60, 30,
        {
            50, 0, 0, 0, 0, 0, 30, 50
        }
        , 23, 1, "A monolithic and sturdy crane arches high into the air here, reaching out to gather and expedite resources from the land and structures in the shadow of it's mighty arm.  A narrow door leads into the 'pit', from where the arm is controlled by a single operator from a series of four panels.  Several laborers wait upon the floor to unload cargo as the arm retrieves it.", "{GR}",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 5, 100, 0
    },
    {
        104, "Psychic Radar", 80, 80,
        {
            30, 10, 10, 10, 10, 0, 0, 0
        }
        , 95, 1, "", "{PR}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        105, "Dummy", 10, 10,
        {
            50, 50, 10, 10, 10, 10, 10, 10
        }
        , 2, 1, "A shallow framework of 2x2 planks rises up to hold the thin composite siding against the walls of this false structure.  Beneath the paper-thin cellophane shingles, only a sheet of blue plastic covers where the roof sheeting should go.  The entire structure sways slightly in the breeze, though thankfully, not much, as the thin walls are fitted with impact-detonation sensors, which are wired to about five tons of plastic and powder explosives, sitting in crates and kegs, upon the center of the slab floor.", ":VV:",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 2, 100, 0
    },
    {
        106, "Particle Emitter", 70, 50,
        {
            50, 0, 0, 10, 0, 0, 0, 0
        }
        , 16, 1, "", ":PE:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        107, "Sonic Blaster", 100, 55,
        {
            150, 0, 10, 10, 10, 0, 0, 0
        }
        , 88, 1, "This manually-operated weapon creates a sonic pulse so powerful that all enemies within a 7 room radius are damaged. The interior of the building is reinforced with special sound-proofing materials to protect the user. Type blast to activate.", ":SB:",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 3, 5, 0
    },
    {
        108, "EMP Research", 83, 70,
        {
            100, 0, 20, 20, 20, 10, 0, 0
        }
        , 88, 1, "This building houses numerous engineers who constantly design and construct weapons designed for the specific purpose of destroying shields. Electro-Magnetic Pulse technology is used in all of the weapons produced here.", "{EM}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 100, 0
    },
    {
        109, "Earthquaker", 80, 80,
        {
            50, 0, 0, 0, 0, 40, 0, 10
        }
        , 76, 1, "", ":><:",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 2, 100, 0
    },
    {
        110, "Psychic Eyes", 100, 50,
        {
            150, 25, 0, 0, 30, 0, 0, 0
        }
        , 95, 1, "", "{00}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 100, 0
    },
    {
        111, "Psychic Amplifier", 90, 70,
        {
            300, 100, 0, 20, 0, 0, 0, 0
        }
        , 95, 3, "", "{,'}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 3, 5, 0
    },
    {
        112, "Space Center", 120, 220,
        {
            200, 0, 100, 100, 100, 0, 0, 100
        }
        , 35, 1, "The building is shaped as a huge dome - There are workers carting stuff around the laboratories, researchers thinking up new equipment for ships and technichans building them. A large storage area is located on the west section of the structure. Many windows surround the dome, allowing light to shine at the middle, where the main construction area is. Here you can @@eresearch@@N technologies for your spaceship after depositing gold, then @@econstruct@@N some and @@elift@@N into space.", "{SP}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 100, 0
    },
    {
        113, "Teleporter", 80, 150,
        {
            350, 0, 150, 0, 0, 0, 0, 100
        }
        , 112, 1, "", "<-->",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 0, 100, 0
    },
    {
        114, "Missile Defense", 400, 1000,
        {
            2000, 0, 1000, 500, 700, 0, 0, 0
        }
        , 76, 3, "", "!MD!",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 2, 100, 0
    },
    {
        115, "Flash Tower", 140, 100,
        {
            100, 0, 0, 30, 0, 0, 0, 0
        }
        , 76, 1, "", ":FT:",
        {
            1, 2, 7, 8, 6, 14
        }
        , 1, 0, 2, 100, 0
    },
    {
        116, "Statue of Spelguru", 1000, 0,
        {
            10000, 0, 0, 10000, 10000, 0, 0, 0
        }
        , 1, 5, "", "####",
        {
            7, 4, 6, 2, 1, 14
        }
        , 1, 1, 6, 1, 0
    },
    {
        117, "Statue of Cylis", 1000, 0,
        {
            10000, 0, 0, 10000, 10000, 0, 0, 0
        }
        , 1, 5, "", "####",
        {
            6, 7, 1, 2, 14, 4
        }
        , 1, 1, 6, 1, 0
    },
    {
        118, "Engineer Home", 130, 80,
        {
            1000, 0, 200, 0, 200, 0, 0, 0
        }
        , 35, 1, "", "|EH|",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 6, 100, 0
    },
    {
        119, "Weather Machine", 90, 66,
        {
            350, 0, 0, 200, 300, 0, 0, 0
        }
        , 2, 1, "", ":\\/:",
        {
            -1, 11, -1, -1, -1, -1
        }
        , 1, 0, 2, 100, 0
    },
    {
        120, "Blackout", 190, 150,
        {
            1050, 0, 200, 200, 200, 0, 0, 0
        }
        , 16, 1, "", "{BO}",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 0, 100, 0
    },
    {
        121, "Portal", 250, 250,
        {
            0, 0, 0, 0, 0, 0, 0, 0
        }
        , 0, 1, "Inside this squared structure is a high-intenceity field of enery. It is keyed to its owner and his allies, and no one else may pass through it.", "?XY?",
        {
            0, 0, 0, 0, 0, 0
        }
        , 1, 0, 2, 100, 0
    },
    {
        122, "Shockwave", 400, 290,
        {
            500, 0, 0, 100, 0, 0, 0, 300
        }
        , 90, 4, "A large dish anteena sits atop this building pointing towards the sky. From this spot you can @@ySpy@@N to discover and disable ALL of the target players dummy buildings, thus making them non-leathal.", "!SW!",
        {
            1, 2, 7, 8, 6, 1
        }
        , 1, 0, 1, 1, 0
    },
};

const struct build_help_type         build_help_table[MAX_BUILDING]       =
{
};
