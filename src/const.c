/*~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-
 ~  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        ~
 ~  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   ~
 ~                                                                         ~
 ~  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          ~
 ~  Chastain, Michael Quan, and Mitchell Tse.                              ~
 ~                                                                         ~
 ~  Ack 2.2 improvements copyright (C) 1994 by Stephen Dooley              ~
 ~  ACK!MUD is modified Merc2.0/2.1/2.2 code (c)Stephen Zepp 1998 Ver: 4.3 ~
 ~                                                                         ~
 ~  In order to use any part of this Merc Diku Mud, you must comply with   ~
 ~  both the original Diku license in 'license.doc' as well the Merc       ~
 ~  license in 'license.txt', and the Ack!Mud license in 'ack_license.txt'.~
 ~  In particular, you may not remove any of these copyright notices.      ~
 ~                                                                         ~
 ~           _______      _____                                            ~
 ~          /  __  /\    / ___ \       222222        PA_MUD by Amnon Kruvi ~
 ~         /______/ /   / /___\ \            2       PA_MUD is modified    ~
 ~        / _______/   / _______ \           2       Ack!Mud, v4.3         ~
 ~       /_/          /_/       \_\        2                               ~
 ~                                      2                                  ~
 ~                                     2222222                             ~
 ~                                                                         ~
 ~                                                                         ~
 ~   Years of work have been invested to create DIKU, Merc, Ack and PA.    ~
 ~   Please show your respect by following the licenses, and issuing       ~
 ~   credits where due.                                                    ~
 ~                                                                         ~
 ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-*/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "ack.h"

struct map_type map_table;
struct alliance_type alliance_table [MAX_ALLIANCE];
struct score_type   score_table[100];
struct rank_type    rank_table[30];
struct load_type    load_list[MAX_BUILDING_LEVEL+1][50];
struct multiplay_type   multiplay_table[30];
struct build_type   build_table[MAX_POSSIBLE_BUILDING];
struct build_help_type  build_help_table[MAX_POSSIBLE_BUILDING];

const struct clip_type  clip_table[MAX_AMMO]     =
{
    //	Name			Dam	BuildDam		Hit	Explode	Speed  	Type
    {                                                       //Pistol
        "9mm Ammo",       30,     2,          53,     FALSE,  4,  DAMAGE_BULLETS
    },
    {                                                       //Magnum
        "17mm-Remington Ammo",    40,     2,          65,     FALSE,  6,  DAMAGE_BULLETS
    },
    {                                                       //Colt
        "45mm-Colt Ammo",     40,     2,          60,     FALSE,  4,  DAMAGE_BULLETS
    },
    {                                                       //Bazookas
        "Rockets",        230,    90,             35,     TRUE,   40, DAMAGE_BLAST
    },
    {                                                       //Acid Sprayer
        "Acid",           70,     0,          35,     FALSE,  2,  DAMAGE_ACID
    },
    {                                                       //Hmmm... Tanks maybe?
        "Tank Rockets",       100,    30,             75,     TRUE,   30, DAMAGE_BLAST
    },
    {                                                       //Vehicles
        "Railgun",        10,     4,          75,     FALSE,  2,  DAMAGE_BULLETS
    },
    {                                                       //Flamethrower
        "Flame",          40,     20,             35,     FALSE,  2,  DAMAGE_FLAME
    },
    {                                                       //Cookie launcher *sigh*
        "Explosive Cookie",   40,     50,             65,     TRUE,   20, DAMAGE_GENERAL
    },
    {                                                       //Sniper Rifle
        "308 Winchester Ammo",    300,    1,          85,     FALSE,  100,    DAMAGE_BULLETS
    },
    {                                                       //Stealth Floaters
        "Stealth Bomb",       10, 150,            90, TRUE,   0,  DAMAGE_BLAST
    },
    {                                                       //BBQer
        "BBQ Fire",       40,     20,             55,     FALSE,  20,  DAMAGE_FLAME
    },
    {                                                       //Laser Guns
        "Laser",          30,     20,             90,     FALSE,  5,  DAMAGE_LASER
    },
    {                                                       //Laser Machine-Guns
        "Laser",          30,     20,             70,     FALSE,  3,  DAMAGE_LASER
    },
    {                                                       //Sniper Rifle (LS)
        "20x100 Hispano",     400,    1,          99,     FALSE,  70, DAMAGE_BULLETS
    },
    {                                                       //AK47
        "7.62mm Ammo",        30,     20,             50,     FALSE,  2,  DAMAGE_BULLETS
    },
    {                                                       //Paintball
        "Paint",          1,  0,          50,     FALSE,  6,  DAMAGE_PAINT
    },
    {                                                       //Laser Tank
        "Laser Beam",         70,     15,             50,     FALSE,  4,  DAMAGE_LASER
    },
    {                                                       //Fire Bazooka
        "Fire Rocket",        150,    1,          20,     TRUE,   100,    DAMAGE_FLAME
    },
    {                                                       //Shotgun
        "Shotgun Shell",      70,     7,          20,     FALSE,  30, DAMAGE_BULLETS
    },
    {                                                       //Mechs
        "Machinegun",         20,     5,          20,     FALSE,  2,  DAMAGE_BULLETS
    },
    {                                                       //EMP guns
        "EMP Charge",         0,  0,          0,  FALSE,  10, DAMAGE_EMP
    },

    {                                                       //Space
        "lvl 1 Laser Cannon",     50,     0,          50,     FALSE,  3,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 2 Laser Cannon",     75,     0,          60,     FALSE,  4,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 3 Laser Cannon",     100,    0,          70,     FALSE,  5,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 1 Plasma Cannon",    75,     0,          60,     FALSE,  3,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 2 Plasma Cannon",    90,     0,          65,     FALSE,  3,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 3 Plasma Cannon",    110,    0,          75,     FALSE,  4,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 1 Torpedo",      40,     0,          70,     TRUE,   6,  DAMAGE_BLAST
    },
    {                                                       //Space
        "lvl 2 Torpedo",      45,     0,          75,     TRUE,   6,  DAMAGE_BLAST
    },
    {                                                       //Space
        "lvl 3 Torpedo",      60,     0,          80,     TRUE,   7,  DAMAGE_BLAST
    },
    {                                                       //Space
        "lvl 1 Ion Cannon",   90,     0,          75,     FALSE,  4,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 2 Ion Cannon",   110,    0,          85,     FALSE,  4,  DAMAGE_LASER
    },
    {                                                       //Space
        "lvl 3 Ion Cannon",   200,    0,          95,     FALSE,  5,  DAMAGE_LASER
    },

    {                                                       //Psychic Blaster
        "Psychic Blast",      100,    0,          100,    FALSE,  16, DAMAGE_PSYCHIC
    },
    {                                                       //Space
        "Ion Blaster",        8000,   0,          100,    FALSE,  5,  DAMAGE_LASER
    },
    {                                                       //M16
        "5.56mm Ammo",        40,     10,             70,     FALSE,  3,  DAMAGE_BULLETS
    },
    {                                                       //Beanbag Rifle
        "Beanbag",        0,  0,          100,    FALSE,  8,  DAMAGE_BULLETS
    },
    {                                                       //Desert Eagle
        ".50 Action Express Ammo",50, 4,          30, FALSE,  6,  DAMAGE_BULLETS
    },
    {                                                       //G11
        "4.7mm Caseless Ammo",    35, 5,          85, FALSE,  4,  DAMAGE_BULLETS
    },
    {                                                       //XRay van
        "X-Ray",          0,  0,          0,  FALSE,  0,  -1
    },
    {                                                       //Bio floater
        "Bio Spray",      0,  0,          0,  FALSE,  0,  DAMAGE_ACID
    },
};

const struct bonus_type bonus_table[] =
{
    { "Gold", "+30 Gold", 32681 },
    { "QP", "A 30-Quest-Points bonus", 32680 },
    { "Safehouse", "Install it in a building, and while inside, you will be undetectable by the bar (Player locator)", 995 },
    { "Laptop", "Comes with a fully charged battery. Use it for implanting viruses in enemy buildings!", 10 },
    { "Dr. Norton", "A 1-time complete virus-clear installation for the entire base.", 11 },
    { "Sniper Rifle", "You wanna defend yourself? They say attacking is the best defense.", 1122 },
    { "Teleporter", "Takes you back home in a 100-room range.", 32594 },
    { "", "", -1 },
};
const struct formula_type formula_table[] =
{
    { 1,        4,      5,  OBJ_VNUM_SMOKE_BOMB,    1,  "Smoke Bomb - 1 Solid Mercury, 4 Liquid Mercury, 5 Gas Mercury."     },
    { 500300,       1,      0,  985,            10, "White Powder - 3 Solid Soil, 5 Solid Sodium, 1 Liquid Mercury."    },
    { 1000,     201000,     0,  OBJ_VNUM_LEAD_BOMB, 30, "Lead Bomb - 1 Solid Lead, 1 Liquid Lead, 2 Liquid Sodium." },
    { 500300,       0,      1,  OBJ_VNUM_BLACK_POWDER,  100,    "Black Powder - 3 Solid Soil, 5 Solid Sodium, 1 Gas Mercury."   },
    { 40020,        0,      3,  OBJ_VNUM_BIO_GRENADE,   150,    "Bio Grenade - 2 Solid Grass, 4 Solid Salt, 3 Gas Mercury." },
    { 25000,        0,      5,  OBJ_VNUM_BURN_GRENADE,  250,    "Burn Grenade - 5 Solid Lead, 2 Solid Salt, 5 Gas Mercury." },
    { 0,        4003,       1002,   32599,          350,    "Black Poison - 4 Liquid + 1 Gas Lead, 3 Liquid +2 Gas Mercury." },
    { 1000000,      1202000,    0,  OBJ_VNUM_DIRTY_BOMB,    400,    "Dirty Bomb - 2 Liquid Lead, 2 Liquid Sodium, 1 Liquid Thorium, 1 Solid Thorium."    },
    {                                                       //End marker
        -1, -1, -1, -1, -1, ""
    },
};

const   struct color_type      color_table    [MAX_color]            =
{
    { "say",              0 },
    { "tell",             1 },
    { "gossip",           2 },
    { "music",            4 },
    { "flame",            5 },
    { "info",             6 },
    { "stats",            7 },
    { "objects",          8 },
    { "ooc",              9 },
};

const   struct  ansi_type       ansi_table      [MAX_ANSI]              =
{
    { "gray",            "\033[0;37m",    0,     'g',    7 },
    { "red",             "\033[0;31m",    1,     'R',    7 },
    { "green",           "\033[0;32m",    2,     'G',    7 },
    { "brown",           "\033[0;33m",    3,     'b',    7 },
    { "blue",            "\033[0;34m",    4,     'B',    7 },
    { "magenta",         "\033[0;35m",    5,     'm',    7 },
    { "cyan",            "\033[0;36m",    6,     'c',    7 },
    {                                                       /* was 0;33 */
        "black",           "\033[0;30m",    7,     'k',    7
    },
    { "yellow",          "\033[1;33m",    8,     'y',    7 },
    { "white",           "\033[1;37m",    9,     'W',    7 },
    //   { "normal",          "\033[0;0m\033[0;40m",      10,	'N',	13 },
    { "color reset",           "\033[0;0m\033[0;49m",      10,   'N',    13 },
    { "purple",          "\033[1;35m",   11,     'p',    7 },
    { "dark_grey",       "\033[1;30m",   12,     'd',    7 },
    { "light_blue",      "\033[1;34m",   13,     'l',    7 },
    { "light_green",     "\033[1;32m",   14,     'r',    7 },
    { "light_cyan",      "\033[1;36m",   15,     'a',    7 },
    { "light_red",       "\033[1;31m",   16,     'e',    7 },
    { "bold",            "\033[1m",    17,       'x',    4 },
    { "flashing",        "\033[5m",    18,       'f',    4 },
    { "inverse",         "\033[7m",    19,       'i',    4 },
    { "back_red",             "\033[0;41m",   20,    '2',    7 },
    { "back_green",           "\033[0;42m",   21,    '3',    7 },
    { "back_yellow",           "\033[0;43m",  22,    '4',    7 },
    { "back_blue",            "\033[0;44m",   23,    '1',    7 },
    { "back_magenta",         "\033[0;45m",   24,    '5',    7 },
    { "back_cyan",            "\033[0;46m",   25,    '6',    7 },
    { "back_black",           "\033[0;40m",   26,    '0',    7 },
    { "back_white",           "\033[1;47m",    27,   '7',    7 },
    { "@@CR@@Ca@@Cn@@Cd@@Co@@Cm@@N",     "",         28, 'C',    7 },
    { "blue",            "\033[0;34m",    29,     'J',    7 },
    { "red",             "\033[0;31m",    30,     'Q',    7 },
    { "gray",            "\033[0;37m",    31,     'o',    7 },
};

/*
 * Class table.
 */
const   struct  class_type      class_table     [MAX_CLASS]     =
{
    { "Eng",  "Engineer", "Buildings can start at L2, get more C4. (Recommended)", TRUE,0 },
    { "Dop",  "Dark-Op", "33%% Invisibility on map, can evade guard turrets.", FALSE, 0 },
    { "Min",  "Miner", "Resource buildings generate larger chunks of materials. (Recommended)", TRUE, 0 },
    { "Drv",  "Driver", "Faster driving in vehicles, vehicles regenerate.", FALSE, 2 },
    { "Scd",  "S. Bomber", "Suicide bombers can blow themselves up, and live through it. Arming bombs doesn't drop them.", FALSE, 2 },
    { "Plt",  "Pilot", "Can survive plane crashes with much greater ease.", FALSE, 0 },
    { "Snp",  "Sniper", "+1 Range to all weapons.", FALSE, 5 },
    { "Rob",  "Robotic", "Can use all level implants without meeting the minimum requirements, +Max Carry Weight (Implants require BIO Lab).", FALSE, 0 },
    { "Sci",  "Scientist", "Your explosive formulas won't blow up on you. Also, sometimes get chemistry items with a wrong formula.", FALSE, 8 },
    { "Hak",  "Hacker", "Protect yourself against hackers and viruses easily, and bypass firewalls to upload them yourself.", FALSE, 0 },
    { "Scn",  "Scanner", "Locate objects at great distances without the use of an item locator.", FALSE, 10 },
    { "Spr",  "Sprinter", "Run quickly in fight mode.", FALSE, 10 },
    { "Prj",  "Projector", "When alone in a room, you appear as 3 people on the map!", FALSE, 10 },
    { "Gen",  "Genius", "-100 Carry Weight, perform actions and skills twice as fast.", FALSE, 20 },
    { "Mec",  "Mechanic", "Install as many Extra parts on your vehicles as you want.", FALSE, 0 },
    { "Med",  "Medic", "Increased form of healing using the Heal command with no need for medpacks.", FALSE, 50 },
    { "Spy",  "Spy", "Receive additional building stats when using the INFO command.", FALSE, 10 },
};

const struct wildmap_type wildmap_table[SECT_MAX] =
{
    //  Color  Symbol   ID    Name   		Heat	Speed	Fuel

    { "@@k", "++++", "00", "Wall",        0,  0,  0   },
    { "@@d", "_-_-", "01", "Rock",        0,  -4,     1   },
    { "@@y", "._._", "02", "Sand",        6,  4,  3   },
    { "@@b", ".--.", "03", "Hills",       -2,     2,  3   },
    { "@@b", "_~~_", "04", "Mountain",        -3,     4,  4   },
    { "@@l", "~~~~", "05", "Water",       -4,     1,  0   },
    { "@@W", "~~~~", "06", "Snow",        -6,     2,  2   },
    { "@@r", "....", "07", "Field",       1,  -4,     1   },
    { "@@G", "^^^^", "08", "Forest",      1,  2,  1   },
    { "@@e", "~o~~", "09", "Lava",        20,     2,  6   },
    { "@@d", "^v^v", "10", "Burned Ground",   0,  0,  1   },
    { "@@W", "~~~~", "11", "Snow - Blizzard",     -20,    3,  4   },
    { "@@d", "~~~~", "12", "Ash",         5,  0,  2   },
    { "@@a", "----", "13", "Air",         0,  1,  0   },
    { "@@b", "....", "14", "Underground",     0,  0,  3   },
    { "@@a", "~~~~", "15", "Ice",         -10,    10,     0   },
    { "@@e", "~oo~", "16", "Magma",       150,    0,  0   },
};

char * const helper  [MAX_HELPER] =
{
    "MCCP is an extremely useful feature in the game. If your client doesn't support it (Gmud, Telnet, and other old clients), take a look at the \"Help MCClient\" help file.\n\rTo check whether your client supports MCCP, type \"score\".",
    "If you don't know what an item does, try using the \"Identify\" command. If you don't know how to use it, try the \"Use\" command.",
    "A @@ccyan@@N-colored base means its owner is a newbie, it's up to you to decide whether you want to attack it, but... it's illegal to do so (You wouldn't want a high-ranking player attacking you in your first hours, right?).",
    "You will be attacked, and you will lose your base. If that happens, don't give up, start over, and build better defenses. Everyone has been there.",
    "Remember, this is a PK MUD, so no whining about someone killing you. Just like other people can kill you, you can kill others.",
    "Want custom text in your Flag on the who list? Just ask Amnon for it!",
    "Getting spammed out (Disconnected)? You don't have to! Check out 'help spammed out' and 'help mcclient' to find out about a program that can help you.",
    "Tired of your char? Want to recreate? Want to leave the game and never come back :( ? Use the pdelete command to delete your char.",
    "Someone is bugging you? Making your stay unpleasant? Attacking you just as you start? Tell an imm, or mail me at assault_htw@yahoo.com fast!",
    "Need some practice? Enter the @@epaintball@@N arena, where you can kill or get killed freely with no real loss (Type 'where' to see where everyone is in paintball!)",
    "Remember, the game is constantly under development. There are, there will be, and... there will be more... bugs, imbalances, etc. Just hang on to your chair if something happens :P",
    "To turn off a channel, type \"channel -channelname\". To turn it back on, type: \"channel +channelname\".",
    "Type \"config\" for a list of configuration options!",
};

const struct planet_type planet_table[]  =
{
    //	     Name     System  Gravity      Z Value         Terrain   		Description
    { "Underground",0,  1,  Z_UNDERGROUND,  TERRAIN_NONE,           "" },
    { "Earth",  1,  1,  Z_GROUND,   TERRAIN_BALANCED,   "Basic playing grid." },
    { "Air",    0,  1,  Z_AIR,      TERRAIN_NONE,       "" },
    { "Space",  0,  5,  Z_SPACE,    TERRAIN_NONE,       "" },
    { "Game Arena", 0,  3,  Z_PAINTBALL,    TERRAIN_NONE,       "" },
    { NULL,     0,  0,  0,      0,          "" },
};

const struct s_res_type s_res_table[]    =
{
    { "Laser Cannons 1",      RESEARCH_W_LASER_1,     RES_WEAPON,     0,          1,  22, 2,  VEHICLE_SCOUT,  1 },
    { "Laser Cannons 2",      RESEARCH_W_LASER_2,     RES_WEAPON,     RESEARCH_W_LASER_1, 3,  23, 3,  VEHICLE_SCOUT,  1 },
    { "Laser Cannons 3",      RESEARCH_W_LASER_3,     RES_WEAPON,     RESEARCH_W_LASER_2, 5,  24, 5,  VEHICLE_FIGHTER,    2 },
    { "Plasma Cannons 1",     RESEARCH_W_PLASMA_1,    RES_WEAPON,     RESEARCH_W_LASER_1, 3,  25, 1,  VEHICLE_SCOUT,  2 },
    { "Plasma Cannons 2",     RESEARCH_W_PLASMA_2,    RES_WEAPON,     RESEARCH_W_PLASMA_1,    5,  26, 3,  VEHICLE_FIGHTER,    2 },
    { "Plasma Cannons 3",     RESEARCH_W_PLASMA_3,    RES_WEAPON,     RESEARCH_W_PLASMA_2,    7,  27, 5,  VEHICLE_FIGHTER,    3 },
    { "Torpedo Cannons 1",    RESEARCH_W_TORPEDO_1,   RES_WEAPON,     RESEARCH_W_PLASMA_1,    5,  28, 3,  VEHICLE_FRIGATE,    3 },
    { "Torpedo Cannons 2",    RESEARCH_W_TORPEDO_2,   RES_WEAPON,     RESEARCH_W_TORPEDO_1,   7,  29, 4,  VEHICLE_FRIGATE,    3 },
    { "Torpedo Cannons 3",    RESEARCH_W_TORPEDO_3,   RES_WEAPON,     RESEARCH_W_TORPEDO_2,   9,  30, 5,  VEHICLE_BATTLECRUISER,  4 },
    { "Ion Cannons 1",        RESEARCH_W_ION_1,   RES_WEAPON,     RESEARCH_W_TORPEDO_1,   7,  31, 3,  VEHICLE_BATTLECRUISER,  4 },
    { "Ion Cannons 2",        RESEARCH_W_ION_2,   RES_WEAPON,     RESEARCH_W_ION_1,   9,  32, 3,  VEHICLE_DESTROYER,  4 },
    { "Ion Cannons 3",        RESEARCH_W_ION_3,   RES_WEAPON,     RESEARCH_W_ION_2,   11, 33, 6,  VEHICLE_STARBASE,   5 },
    { "Steel Armor 1",        RESEARCH_A_STEEL_1,     RES_ARMOR,  0,          2,  10000,  0,  VEHICLE_SCOUT,  1 },
    { "Steel Armor 2",        RESEARCH_A_STEEL_2,     RES_ARMOR,  RESEARCH_A_STEEL_1, 4,  20000,  0,  VEHICLE_SCOUT,  2 },
    { "Steel Armor 3",        RESEARCH_A_STEEL_3,     RES_ARMOR,  RESEARCH_A_STEEL_2, 6,  30000,  0,  VEHICLE_FRIGATE,    3 },
    { "Scout",            RESEARCH_S_SCOUT,   RES_SHIP,   0,          4,  4,  10, 0,          1 },
    { "Fighter",          RESEARCH_S_FIGHTER,     RES_SHIP,   RESEARCH_S_SCOUT,   4,  2,  5,  1,      1 },
    { "Frigate",          RESEARCH_S_FRIGATE,     RES_SHIP,   RESEARCH_S_FIGHTER, 8,  4,  6,  2,      2 },
    { "Battlecruiser",        RESEARCH_S_BATTLECRUISER, RES_SHIP,     RESEARCH_S_FRIGATE, 16, 2,  10, 4,      3 },
    { "Destroyer",        RESEARCH_S_DESTROYER,   RES_SHIP,   RESEARCH_S_BATTLECRUISER, 50,   3,  12, 7,      4 },
    { "Starbase",         RESEARCH_S_STARBASE,    RES_SHIP,   RESEARCH_S_DESTROYER,   100,    1,  15, 10,     5 },
    { "Titanium Armor 1",     RESEARCH_A_TITANIUM_1,  RES_ARMOR,  RESEARCH_A_STEEL_2, 4,  15000,  0,  VEHICLE_SCOUT,  2 },
    { "Titanium Armor 2",     RESEARCH_A_TITANIUM_2,  RES_ARMOR,  RESEARCH_A_TITANIUM_1,  6,  18000,  0,  VEHICLE_FIGHTER,    3 },
    { "Titanium Armor 3",     RESEARCH_A_TITANIUM_3,  RES_ARMOR,  RESEARCH_A_TITANIUM_2,  8,  40000,  0,  VEHICLE_FRIGATE,    4 },
    { "Alien Armor 1",        RESEARCH_A_ALIEN_1,     RES_ARMOR,  RESEARCH_A_TITANIUM_1,  6,  17000,  0,  VEHICLE_FRIGATE,    3 },
    { "Alien Armor 2",        RESEARCH_A_ALIEN_2,     RES_ARMOR,  RESEARCH_A_ALIEN_1, 8,  25000,  0,  VEHICLE_DESTROYER,  4 },
    { "Alien Armor 3",        RESEARCH_A_ALIEN_3,     RES_ARMOR,  RESEARCH_A_ALIEN_2, 10, 60000,  0,  VEHICLE_STARBASE,   5 },
    { NULL,           -1,         0,      0,          0,  0,  0,  0,  0 },
};

int const kill_groups [] =
{ 1,10,50,999,-1};

char * const vehicle_name [MAX_VEHICLE] =
{
    "Jeep Vehicle",
    "Tank Vehicle",
    "Laser Floater Vehicle",
    "Truck Vehicle",
    "Chinook Chopper Helicopter Vehicle",
    "Stealth Floater Vehicle",
    "BBQ Tank Vehicle",
    "Laser Tank Vehicle",
    "Mech Vehicle",
    "Scout Spaceship Vehicle ship",
    "Fighter Spaceship Vehicle ship",
    "Frigate Spaceship Vehicle ship",
    "Battlecruiser Spaceship Vehicle ship",
    "Destroyer Spaceship Vehicle ship",
    "Starbase Spaceship Vehicle ship",
    "xRay ray van Vehicle",
    "Alien Scout Ship",
    "Bio Floater Vehicle",
    "Creeper Vehicle",
};

char * const vehicle_desc [MAX_VEHICLE] =
{
    "A dirty Jeep",
    "A large Tank",
    "A Laser-Floater",
    "A worn-out Truck",
    "A Chinook",
    "A Stealth-Floater",
    "A BBQ Tank",
    "A Laser Tank",
    "A Mech",
    "A Scout ship",
    "A Fighter ship",
    "A Frigate ship",
    "A Battlecruiser ship",
    "A Destroyer ship",
    "A Starbase",
    "An X-Ray van",
    "An Alien Scout",
    "A Bio-Floater",
    "A Creeper",
};
