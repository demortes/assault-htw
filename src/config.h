/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Ack 2.2 improvements copyright (C) 1994 by Stephen Dooley              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *       _/          _/_/_/     _/    _/     _/    ACK! MUD is modified    *
 *      _/_/        _/          _/  _/       _/    Merc2.0/2.1/2.2 code    *
 *     _/  _/      _/           _/_/         _/    (c)Stephen Zepp 1998    *
 *    _/_/_/_/      _/          _/  _/             Version #: 4.3          *
 *   _/      _/      _/_/_/     _/    _/     _/                            *
 *                                                                         *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

#include "buildings.h"

#define DEC_CONFIG_H        1

/*
 *  Your mud info here :) Zen
 */

#define mudnamecolor                "@@aAssault: High Tech War@@N"
#define mudnamenocolor              "Assault: High Tech War"
#define UPGRADE_REVISION            16
#define WEBSITE                     "http://aoc.slayn.net"
#define admin                       "Grave"
#define admin_email                 "xithen@gmail.com"
/*
 * String and memory management parameters.
 */
#define MAX_KEY_HASH                2048
#define MAX_STRING_LENGTH           8192
#define MSL                         MAX_STRING_LENGTH
#define MAX_INPUT_LENGTH            1280
#define MAX_AREAS                   2000
#define MAX_VNUM                    32767

#define BOOT_DB_ABORT_THRESHOLD     25
#define RUNNING_ABORT_THRESHOLD     10
#define ALARM_FREQUENCY             20

/*
 * Game parameters.
 * Increase the max'es if you add more of something.
 * Adjust the pulse numbers to suit yourself.
 */

#define STARTING_HP         500
#define MAX_CHUNK_WEIGHT        50
#define MAX_BUILDING_LEVEL      5
#define MAX_color           10                              /* eg look, prompt, shout */
#define MAX_ANSI            32                              /* eg red, black, etc */
#define MAX_ALIASES          5
#define MAX_ALLIANCE             15
#define MAX_IGNORES          3
#define MAX_CLASS            17
#define MAX_OBJECT_VALUES        15
#define MAX_BUILDON          6
#define MAX_SKILL           15
#define MAX_LEVEL            90
#define LEVEL_HERO                 (80)
#define LEVEL_GUIDE                  79
#define LEVEL_IMMORTAL               80
#define MAX_QUOTE       200
#define MAX_MAPS        1000
#define MIN_LOAD_OBJ        1000
#define MAX_LOAD_OBJ        1164
#define MAX_HELPER      13
#define BORDER_SIZE     3

#define TERRAIN_NONE        0
#define TERRAIN_BALANCED    1
#define TERRAIN_MOON        2
#define TERRAIN_FOREST      3
#define TERRAIN_ROUGH       4
#define TERRAIN_FROST       5
#define TERRAIN_SAND        6
#define TERRAIN_FIRE        7

#define SPACE_SIZE  450

#define Z_UNDERGROUND   0
#define Z_GROUND    1
#define Z_AIR       2
#define Z_SPACE     3
#define Z_PAINTBALL 4
#define Z_MAX       5
#define Z_NEWBIE    Z_MAX

#define MAX_AMMO    42
#define DAMAGE_ENVIRO   -3
#define DAMAGE_GENERAL  -1
#define DAMAGE_BULLETS  1
#define DAMAGE_BLAST    2
#define DAMAGE_ACID 3
#define DAMAGE_FLAME    4
#define DAMAGE_LASER    5
#define DAMAGE_PAINT    6
#define DAMAGE_SOUND    7
#define DAMAGE_PSYCHIC  8
#define DAMAGE_EMP  9

#define ITEM_IRON   0
#define ITEM_SKIN   1
#define ITEM_COPPER 2
#define ITEM_GOLD   3
#define ITEM_SILVER 4
#define ITEM_ROCK   5
#define ITEM_STICK  6
#define ITEM_LOG    7

#define STATE_SOLID     0
#define STATE_LIQUID        1
#define STATE_GAS       2

#define ELEMENT_CINNABAR_ORE    0
#define ELEMENT_MERCURY     1
#define ELEMENT_GRASS       2
#define ELEMENT_SOIL        3
#define ELEMENT_LEAD        4
#define ELEMENT_SALT        5
#define ELEMENT_SODIUM      6
#define ELEMENT_THORIUM     7

#define VEHICLE_JEEP            0
#define VEHICLE_TANK            1
#define VEHICLE_AIRCRAFT        2
#define VEHICLE_TRUCK           3
#define VEHICLE_CHINOOK         4
#define VEHICLE_BOMBER          5
#define VEHICLE_BBQ         6
#define VEHICLE_LASER           7
#define VEHICLE_MECH            8
#define VEHICLE_SCOUT           9
#define VEHICLE_FIGHTER         10
#define VEHICLE_FRIGATE         11
#define VEHICLE_BATTLECRUISER       12
#define VEHICLE_DESTROYER       13
#define VEHICLE_STARBASE        14
#define VEHICLE_XRAY            15
#define VEHICLE_ALIEN_SCOUT     16
#define VEHICLE_BIO_FLOATER     17
#define VEHICLE_CREEPER         18
#define MAX_VEHICLE         19

#define VEHICLE_FIRE_RESISTANT      BIT_1
#define VEHICLE_EXPLOSIVE       BIT_2
#define VEHICLE_FLOATS          BIT_3
#define VEHICLE_EATS_FUEL       BIT_4
#define VEHICLE_REGEN           BIT_5
#define VEHICLE_CORROSIVE       BIT_6
#define VEHICLE_CORROSIVE_A     BIT_7
#define VEHICLE_MINING_BEAM     BIT_8
#define VEHICLE_GUARD_LASERS        BIT_9
#define VEHICLE_PSI_SCANNER     BIT_10
#define VEHICLE_OBJ_SENSORS     BIT_11
#define VEHICLE_DRILL           BIT_12
#define VEHICLE_ALIEN_MAGNET        BIT_13
#define VEHICLE_TREASURE_BEAM       BIT_14
#define VEHICLE_SPACE_SCANNER       BIT_15

#define VEHICLE_STATE_NORMAL        0
#define VEHICLE_STATE_EVADE     1
#define VEHICLE_STATE_DEFENSE       2
#define VEHICLE_STATE_OFFENSE       3
#define VEHICLE_STATE_CHARGE        4
#define POWER_SOURCE            0
#define POWER_ENGINE            1
#define POWER_WEAPONS           2
#define POWER_ARMOR         3
#define POWER_REPAIR            4
#define POWER_MAX           5
#define SECTION_CONTROL_ROOM        0
#define SECTION_ENGINE_ROOM     1
#define SECTION_SENSOR_ROOM     2
#define SECTION_SHIELD_ROOM     3

#define RES_WEAPON          5
#define RES_ARMOR           6
#define RES_SHIP            7
#define RESEARCH_W_LASER_1      BIT_1
#define RESEARCH_W_LASER_2      BIT_2
#define RESEARCH_W_LASER_3      BIT_3
#define RESEARCH_W_PLASMA_1     BIT_4
#define RESEARCH_W_PLASMA_2     BIT_5
#define RESEARCH_W_PLASMA_3     BIT_6
#define RESEARCH_W_TORPEDO_1        BIT_7
#define RESEARCH_W_TORPEDO_2        BIT_8
#define RESEARCH_W_TORPEDO_3        BIT_9
#define RESEARCH_W_ION_1        BIT_10
#define RESEARCH_W_ION_2        BIT_11
#define RESEARCH_W_ION_3        BIT_12

#define RESEARCH_A_STEEL_1      BIT_1
#define RESEARCH_A_STEEL_2      BIT_2
#define RESEARCH_A_STEEL_3      BIT_3
#define RESEARCH_A_TITANIUM_1       BIT_4
#define RESEARCH_A_TITANIUM_2       BIT_5
#define RESEARCH_A_TITANIUM_3       BIT_6
#define RESEARCH_A_ALIEN_1      BIT_7
#define RESEARCH_A_ALIEN_2      BIT_8
#define RESEARCH_A_ALIEN_3      BIT_9

#define RESEARCH_S_SCOUT        BIT_1
#define RESEARCH_S_FRIGATE      BIT_2
#define RESEARCH_S_FIGHTER      BIT_3
#define RESEARCH_S_BATTLECRUISER    BIT_4
#define RESEARCH_S_DESTROYER        BIT_5
#define RESEARCH_S_STARBASE     BIT_6

#define SPEC_BLIND      100
#define SPEC_BARIN      30
#define SPEC_SLOW       50
#define SPEC_BLDHEAL        60
#define SPEC_EMP        90
#define SPEC_CONFUSE        120
#define SPEC_ANTIVIR        200
#define SPEC_WARP       300

#define EFFECT_BLIND        BIT_1
#define EFFECT_BARIN        BIT_2
#define EFFECT_SLOW     BIT_3
#define EFFECT_RESOURCEFUL  BIT_4
#define EFFECT_BOMBER       BIT_5
#define EFFECT_CONFUSE      BIT_6
#define EFFECT_POSTAL       BIT_7
#define EFFECT_ENCRYPTION   BIT_8
#define EFFECT_VISION       BIT_9
#define EFFECT_RUNNING      BIT_10
#define EFFECT_TRACER       BIT_11
#define EFFECT_WULFSKIN     BIT_12
#define EFFECT_DRUNK        BIT_13

#define SUIT_NONE           0
#define SUIT_WARP           1
#define SUIT_JUMP           2

#define WEAPON_BLINDING         BIT_4
#define WEAPON_POISON           BIT_5
#define WEAPON_HITS_AIR         BIT_6
#define WEAPON_CONFUSING        BIT_7
#define WEAPON_ALCOHOL          BIT_8

#define INST_NONE           0
#define INST_GPS            BIT_1
#define INST_REFLECTOR          BIT_2
#define INST_INTERN_DEF         BIT_3
#define INST_SATELLITE_UPLINK       BIT_4
#define INST_LASER_AIMS         BIT_5
#define INST_SAFEHOUSE          BIT_6
#define INST_PULSE_NEUTRALIZER      BIT_7
#define INST_DEPLEATED_URANIUM      BIT_8
#define INST_RESOURCE_PURIFIER      BIT_9
#define INST_ANTIVIRUS          BIT_10
#define INST_FIREWALL           BIT_11
#define INST_STUN_GUN           BIT_12
#define INST_PROCESSOR_UPGRADE      BIT_13
#define INST_SPOOF          BIT_14
#define INST_QP             BIT_15
#define INST_ORGANIC_CORE       BIT_16
#define INST_VIRAL_ENHANCER     BIT_17
#define INST_ALIEN_TECHNOLOGY       BIT_18
#define INST_ACID_DEFENSE       BIT_19
#define INST_ALIEN_HIDES        BIT_20

#define GUNNER_NUCLEAR          BIT_1                       //Adds nuclear fallout to EQ
#define GUNNER_POISON           BIT_2                       //Adds poison effect
#define GUNNER_ROCKETS          BIT_3                       //Stronger vs buildings
#define GUNNER_PSYCHIC          BIT_4                       //Pushes around randomly
#define GUNNER_CHAOS            BIT_5                       //Random effect
#define GUNNER_TRACER           BIT_6                       //Trace effect - always shows up on "where"

#define IMPLANT_METAL_CHEST_1       BIT_1
#define IMPLANT_METAL_CHEST_2       BIT_2
#define IMPLANT_METAL_CHEST_3       BIT_3
#define IMPLANT_METAL_ARM_1     BIT_4
#define IMPLANT_METAL_ARM_2     BIT_5
#define IMPLANT_METAL_ARM_3     BIT_6
#define IMPLANT_METAL_ABS_1     BIT_7
#define IMPLANT_METAL_ABS_2     BIT_8
#define IMPLANT_METAL_ABS_3     BIT_9

#define PIT_BORDER_X            (MAX_MAPS-10)
#define PIT_BORDER_Y            (MAX_MAPS-10)
#define MEDAL_BORDER_X          52
#define MEDAL_BORDER_Y          22

/*
 * Extended bitvector stuff.
 */
#ifndef INT_BITS
// #define INT_BITS                   32
#define INT_BITS                   64
#endif
#define XBM                        31
#define RSV                         5                       /* log2( INT_BITS )     */
#define XBI                         2                       /* int's in a bitvector */
#define MAX_BITS                  ( XBI * INT_BITS )

#define TYPE_UNDEFINED  -1

#define C_TYPE_MISC     0
#define C_TYPE_COMM     1
#define C_TYPE_CONFIG   2
#define C_TYPE_INFO     3
#define C_TYPE_ACTION   4
#define C_TYPE_OBJECT   5
#define C_TYPE_ALLI 6
#define C_TYPE_IMM  7

#define C_SHOW_NEVER    -1
#define C_SHOW_ALWAYS     0
#define C_SHOW_SKILL      1

#define PULSE_PER_SECOND     8
#define PULSE_VIOLENCE    (  2 * PULSE_PER_SECOND )
#define PULSE_OBJFUN      (  4 * PULSE_PER_SECOND )
#define PULSE_TICK        ( 60 * PULSE_PER_SECOND )
#define PULSE_ROOMS       ( 10 * PULSE_PER_SECOND )
#define PULSE_AREA        ( 80 * PULSE_PER_SECOND )
#define PULSE_AUCTION     ( 30 * PULSE_PER_SECOND )
#define PULSE_BACKUP      ( 1800 * PULSE_PER_SECOND )
#define PULSE_TIME        ( 3600 * PULSE_PER_SECOND )
#define PULSE_REMAP       ( 7200 * PULSE_PER_SECOND )
#define PULSE_OBJECTS     ( PULSE_PER_SECOND * 5 )
#define PULSE_BOMB        ( PULSE_PER_SECOND )
#define PULSE_QUEST   ( 15 * PULSE_PER_SECOND )
#define PULSE_SPEC        ( PULSE_PER_SECOND * 10 )

/*
 * Well known object virtual numbers.
 * Defined in #OBJECTS.
 */
#define OBJ_VNUM_MATERIAL   32699
#define OBJ_VNUM_TELEPORTER 32698
#define OBJ_VNUM_LOCATOR    32678
#define OBJ_VNUM_BLUEPRINTS 32693
#define OBJ_VNUM_ACID_TURRET_U  32694
#define OBJ_VNUM_FIRE_TURRET_U  32696
#define OBJ_VNUM_LASER_TOWER_U  32697
#define OBJ_VNUM_IDUP       32695
#define OBJ_VNUM_ACID_SPRAY 1010
#define OBJ_VNUM_CANNONBALL 32686
#define OBJ_VNUM_GRANADE    1012
#define OBJ_VNUM_SUIT_WARP  1013
#define OBJ_VNUM_SUIT_JUMP  1144
#define OBJ_VNUM_SCUD       32687
#define OBJ_VNUM_FLASH_GRENADE  1030
#define OBJ_VNUM_REFLECTOR  1031
#define OBJ_VNUM_COOKIE_LAUNCH  1032
#define OBJ_VNUM_COOKIE_AMMO    1033
#define OBJ_VNUM_MAIN_BOARD 1039
#define OBJ_VNUM_ATOM_BOMB  32692
#define OBJ_VNUM_CORPSE     32691
#define OBJ_VNUM_FLAG       32690
#define OBJ_VNUM_DART_BOARD 32689
#define OBJ_VNUM_QP_TOKEN   32688
#define OBJ_VNUM_ELEMENT    32682
#define OBJ_VNUM_CHINESE_TEA    32685
#define OBJ_VNUM_SMOKE_BOMB 32684
#define OBJ_VNUM_POISON_TEA 32683
#define OBJ_VNUM_CONTAINER  1077
#define OBJ_VNUM_LEAD_BOMB  999
#define OBJ_VNUM_BLACK_POWDER   998
#define OBJ_VNUM_BIO_GRENADE    997
#define OBJ_VNUM_BURN_GRENADE   996
#define OBJ_VNUM_SAFEHOUSE_INST 995
#define OBJ_VNUM_RESOURCE_PURE  994
#define OBJ_VNUM_DEPLEATED_URA  993
#define OBJ_VNUM_PULSE_NEUTRAL  992
#define OBJ_VNUM_COMPUTER   10
#define OBJ_VNUM_ALLI_BOARD 10000
#define OBJ_VNUM_PAINT_GUN  32679
#define OBJ_VNUM_STUN_GUN   989
#define OBJ_VNUM_PROCESSOR_UP   987
#define OBJ_VNUM_DISK_V     1138
#define OBJ_VNUM_DISK_C     1139
#define OBJ_VNUM_DISK_F     1140
#define OBJ_VNUM_DISK_S     1141
#define OBJ_VNUM_DISK_P     1145
#define OBJ_VNUM_SHOCK_BOMB 1150
#define OBJ_VNUM_MEDAL      32676
#define OBJ_VNUM_TOOLKIT    32669
#define OBJ_VNUM_DIRTY_BOMB 986
#define OBJ_VNUM_SCAFFOLD   32667
#define OBJ_VNUM_BROKEN_BONE    509
#define OBJ_VNUM_AIR2GROUNDBOMB 32686

#define MAX_QUEST_ITEMS     20
#define QUEST_ITEM_COST     1000
#define MIN_QUEST_OBJ       2
#define MAX_QUEST_OBJ       14

#define helper0 "Remember to read the three main help files: Help getting started, Help FAQ, Help Suggestions."
#define helper1 "Trouble creating a vehicle? Read: Help vehicle requirements."
#define helper2 "Found a GPS, Satellite uplink, or another installation or blueprint? You can IDENTIFY it."
#define helper3 "The quest items don't move. It's the clues that change. You need to compare your map to the clue map."
#define helper4 "A @@ccyan@@N-colored base means that its owner is a newbie, and should not be attacked. Read Help Rules for more detail."
#define helper5 "You'll get these helper messages for the first hour of play time. Don't ignore them. The reason you can't turn them off is because 99% of the questions people ask are explained in the help files."
#define helper6 "You will be attacked, and you will lose your base. If that happens, don't give up, start over, and build better defenses."
#define helper7 "Remember, this is a PK MUD, so no whining about someone killing you. Just like other people can kill you, you can kill others."
#define helper8 "Don't forget to visit the website (And vote :P ) - fredrik.homelinux.org/Amnon (With the capital A)"
#define helper9 "Want custom text in your Flag on the who list? Just ask Amnon for it!"
#define helper10 "Getting spammed out (Disconnected)? You don't have to! Check out 'help spammed out' to find out about a program that can help you."
#define helper11 "Tired of your char? Want to recreate? Want to leave the game and never come back :( ? Use the pdelete command to delete your char."
#define helper12 "Players can create their OWN custom attacks! See 'help special' for info!"

/*
 * Well known room virtual numbers.
 * Defined in #ROOMS.
 */
#define ROOM_VNUM_WMAP                3
#define ROOM_VNUM_LIMBO               2
#define ROOM_VNUM_JAIL            1

#define C_SHOW_NEVER    -1
#define C_SHOW_ALWAYS     0
#define C_SHOW_SKILL      1

/*
 * God Levels
 */
#define L_GOD           MAX_LEVEL
#define L_SUP           L_GOD - 1
#define L_DEI           L_SUP - 1
#define L_ANG           L_DEI - 1
#define L_HER           L_ANG - 1

/*
 * Time and weather stuff.
 */
#define SUN_DARK                    0
#define SUN_RISE                    1
#define SUN_LIGHT                   2
#define SUN_SET                     3

#define SKY_CLOUDLESS               0
#define SKY_CLOUDY                  1
#define SKY_RAINING                 2
#define SKY_LIGHTNING               3
#define SKY_MAX             4

#define MOON_DOWN   0
#define MOON_RISE   1
#define MOON_LOW    2
#define MOON_PEAK   3
#define MOON_FALL   4
#define MOON_SET    5

#define MOON_NEW    0
#define MOON_WAX_CRE    1
#define MOON_WAX_HALF   2
#define MOON_WAX_GIB    3
#define MOON_FULL   4
#define MOON_WAN_GIB    5
#define MOON_WAN_HALF   6
#define MOON_WAN_CRE    7

/*
 * More Time and weather stuff. - Wyn
 */

/* Overall time */
#define HOURS_PER_DAY   24
#define DAYS_PER_WEEK    7
#define DAYS_PER_MONTH  30
#define MONTHS_PER_YEAR 10
#define DAYS_PER_YEAR   (DAYS_PER_MONTH * MONTHS_PER_YEAR)

/* PaB: Hours of the day */
/* Notes: Night is half of the day, so sunrise is 1/4 of the way
 * through the day, and sunset 3/4 of the day.
 */
#define HOUR_DAY_BEGIN      (HOURS_PER_DAY / 4 - 1)
#define HOUR_SUNRISE        (HOUR_DAY_BEGIN + 1)
#define HOUR_NOON           (HOURS_PER_DAY / 2)
#define HOUR_SUNSET         ((HOURS_PER_DAY / 4) * 3 + 1)
#define HOUR_NIGHT_BEGIN    (HOUR_SUNSET + 1)
#define HOUR_MIDNIGHT       HOURS_PER_DAY

/* PaB: Seasons */
/* Notes: Each season will be arbitrarily set at 1/4 of the year.
 */
#define SEASON_WINTER       0
#define SEASON_SPRING       1
#define SEASON_SUMMER       2
#define SEASON_FALL         3
#define SEASON_MAX         4

/*
 * Connected state for a channel.
 */

/* These values referenced by users command, BTW */

#define CON_PLAYING                      0
#define CON_GET_NAME                     -1
#define CON_GET_OLD_PASSWORD             -2
#define CON_CONFIRM_NEW_NAME             -3
#define CON_GET_NEW_PASSWORD             -4
#define CON_CONFIRM_NEW_PASSWORD         -5
#define CON_READ_MOTD                   -10
#define CON_FINISHED                -12
#define CON_MENU                        -13
#define CON_COPYOVER_RECOVER            -14
/* For Hotreboot */
#define CON_QUITTING                  -15
#define CON_RECONNECTING                -16
#define CON_GET_NEW_CLASS       -17
#define CON_GET_ANSI            -18
#define CON_GET_RECREATION      -19
#define CON_GET_SEX         -20
#define CON_GET_BONUS           -21
#define CON_GET_NEW_PLANET      -22
#define CON_READ_RULES          -23
#define CON_GET_RESET           -24
#define CON_GET_NEW_MODE        -25
#define CON_SETTING_STATS             1

/*
 * TO types for act.
 */
#define TO_ROOM             0
#define TO_NOTVICT          1
#define TO_VICT             2
#define TO_CHAR             3

/*
 * Room flags.
 * Used in #ROOMS.
 */
#define ROOM_NO_MOB             BIT_3
#define ROOM_INDOORS            BIT_4

/*
 * Directions.
 * Used in #ROOMS.
 */
#define DIR_NORTH                     0
#define DIR_EAST                      1
#define DIR_SOUTH                     2
#define DIR_WEST                      3

/*
 * Sector types.
 * Used in #ROOMS.
 */

#define SECT_NULL                     0
#define SECT_MAX                   17

#define SECT_ROCK           1
#define SECT_SAND           2
#define SECT_HILLS          3
#define SECT_MOUNTAIN           4
#define SECT_WATER          5
#define SECT_SNOW           6
#define SECT_FIELD          7
#define SECT_FOREST         8
#define SECT_LAVA           9
#define SECT_BURNED         10
#define SECT_SNOW_BLIZZARD      11
#define SECT_ASH            12
#define SECT_AIR            13
#define SECT_UNDERGROUND        14
#define SECT_ICE            15
#define SECT_MAGMA          16

/*
 * Equpiment wear locations.
 * Used in #RESETS.
 */
#define WEAR_NONE               -1
#define WEAR_HEAD               0
#define WEAR_EYES       1
#define WEAR_FACE               2
#define WEAR_EAR_L              3
#define WEAR_EAR_R              4
#define WEAR_NECK_1             5
#define WEAR_NECK_2             6
#define WEAR_SHOULDERS          7
#define WEAR_ARMS               8
#define WEAR_WRIST_L            9
#define WEAR_WRIST_R            10
#define WEAR_HANDS              11
#define WEAR_FINGER_L           12
#define WEAR_FINGER_R           13
#define WEAR_HOLD_HAND_L        14
#define WEAR_HOLD_HAND_R        15
#define WEAR_ABOUT              16
#define WEAR_WAIST              17
#define WEAR_BODY               18
#define WEAR_LEGS               19
#define WEAR_FEET               20
#define MAX_WEAR                     21

/*
 * Positions.
 */
#define POS_DEAD                      0
#define POS_MORTAL                    1
#define POS_INCAP                     2
#define POS_STUNNED                   3
#define POS_SLEEPING                  4
#define POS_RESTING                   5
#define POS_SNEAKING                  6
#define POS_STANDING                  7
#define POS_WRITING                   8
#define POS_BUILDING                  9
#define POS_HACKING          10
#define POS_SPACE_COM            11
#define POS_ENGINEERING          12
#define POS_NUKEM            13
#define POS_PAGER            14

/*
 *  Configuration Bits for players
 */

#define CONFIG_SMALLMAP     BIT_1
#define CONFIG_QUESTS       BIT_2
#define CONFIG_EXITS        BIT_3
#define CONFIG_NOCOLORS     BIT_4
#define CONFIG_NOFOLLOW     BIT_5
#define CONFIG_COMBINE          BIT_6
#define CONFIG_PROMPT           BIT_7
#define CONFIG_TELNET_GA        BIT_8
#define CONFIG_COLOR            BIT_9
#define CONFIG_COMPRESS     BIT_10
#define CONFIG_FULL_ANSI        BIT_11
#define CONFIG_MXP      BIT_12
#define CONFIG_BLIND        BIT_13
#define CONFIG_PUBMAIL      BIT_14
#define CONFIG_LARGEMAP     BIT_15
#define CONFIG_MINCOLORS    BIT_16
#define CONFIG_SOUND        BIT_17
#define CONFIG_ECHAN        BIT_18
#define CONFIG_HELPER       BIT_19
#define CONFIG_CLIENT       BIT_20
#define CONFIG_BRIEF        BIT_21
#define CONFIG_IMAGE        BIT_22
#define CONFIG_COMPRESS2    BIT_23
#define CONFIG_TINYMAP      BIT_24
#define CONFIG_NOBLACK      BIT_25
#define CONFIG_WHITEBG      BIT_26
#define CONFIG_INVERSE      BIT_27
#define CONFIG_NOLEGEND     BIT_28

/*
 * ACT bits for players.
 */
#define PFLAG_AFK           BIT_1
#define PFLAG_SNOOP         BIT_2
#define PFLAG_PRACTICE          BIT_3
#define PFLAG_ALIAS         BIT_4
#define PFLAG_RAD_SIL           BIT_5
#define PFLAG_HELPING           BIT_6
#define PLR_PDELETER            BIT_7
#define PLR_BASIC           BIT_8
#define PLR_HOLYLIGHT                   BIT_13
#define PLR_WIZINVIS                    BIT_14
#define PLR_BUILDER                     BIT_15              /* Is able to use the OLC */
#define PLR_SILENCE                     BIT_16
#define PLR_NO_EMOTE                    BIT_17
#define PLR_NO_TELL                 BIT_19
#define PLR_LOG                     BIT_20
#define PLR_DENY                    BIT_21
#define PLR_FREEZE                  BIT_22
#define PLR_TAG             BIT_27                          /* For Tag */
#define PLR_ASS             BIT_28
#define PLR_INCOG           BIT_31

/*
 * Obsolete bits.
 */
#if 0
#define PLR_AUCTION                   4                     /* Obsolete     */
#define PLR_CHAT                    256                     /* Obsolete     */
#define PLR_NO_SHOUT             131072                     /* Obsolete     */
#endif

/*
 * Channel bits.
 */
#define CHANNEL_ALLIANCE        BIT_1
#define CHANNEL_GOSSIP          BIT_2
#define CHANNEL_MUSIC           BIT_3
#define CHANNEL_IMMTALK         BIT_4
#define CHANNEL_NEWBIE          BIT_5
#define CHANNEL_QUESTION        BIT_6
#define CHANNEL_SHOUT           BIT_7
#define CHANNEL_POLITICS        BIT_8
#define CHANNEL_FLAME           BIT_9
#define CHANNEL_ZZZ             BIT_10
#define CHANNEL_RACE            BIT_11
#define CHANNEL_CLAN            BIT_12
#define CHANNEL_NOTIFY          BIT_13
#define CHANNEL_INFO            BIT_14
#define CHANNEL_LOG     BIT_15
#define CHANNEL_CREATOR     BIT_16
#define CHANNEL_ALLALLI     BIT_17
#define CHANNEL_ALLRACE     BIT_18
#define CHANNEL_HERMIT      BIT_19                          /* Turns off ALL channels */
#define CHANNEL_BEEP        BIT_20
#define CHANNEL_FAMILY      BIT_21
#define CHANNEL_DIPLOMAT    BIT_22
#define CHANNEL_CRUSADE     BIT_23
#define CHANNEL_REMORTTALK  BIT_24
#define CHANNEL_HOWL            BIT_25
#define CHANNEL_ADEPT           BIT_26
#define CHANNEL_OOC             BIT_27
#define CHANNEL_QUEST           BIT_28
#define CHANNEL_CODE        BIT_29
#define CHANNEL_GAME        BIT_30

#define CHANNEL2_AFFIL      BIT_1
#define CHANNEL2_ALLAFFIL   BIT_2
#define CHANNEL2_PKOK       BIT_3
#define CHANNEL2_GUIDE      BIT_4
#define CHANNEL2_LANG       BIT_5

/* NOTE 32 is the last allowable channel ZEN */

/* Monitor channels - for imms to select what mud-based info they receive */
#define MONITOR_CONNECT     BIT_1
#define MONITOR_AREA_UPDATE BIT_2
#define MONITOR_AREA_BUGS   BIT_3
#define MONITOR_AREA_SAVING BIT_4
#define MONITOR_GEN_IMM     BIT_5
#define MONITOR_GEN_MORT    BIT_6
#define MONITOR_COMBAT      BIT_7
#define MONITOR_BUILD       BIT_8
#define MONITOR_OBJ     BIT_9
#define MONITOR_ROOM        BIT_10
#define MONITOR_BAD     BIT_11
#define MONITOR_DEBUG       BIT_12
#define MONITOR_SYSTEM      BIT_13
#define MONITOR_LDEBUG      BIT_14
#define MONITOR_FAKE        BIT_15

/* build bits for OLC -S- */
#define ACT_BUILD_NOWT                0                     /* not doing anything   */
#define ACT_BUILD_REDIT               1                     /* editing rooms        */
#define ACT_BUILD_OEDIT               2                     /* editing objects      */
#define ACT_BUILD_BEDIT               3                     /* editing buildings    */
#define ACT_BUILD_MPEDIT          4                         /* editing mprogs	*/
#define ACT_BUILD_CEDIT               5                     /* editing the clan table */
#define NO_USE             -999                             /* this table entry can	*/
/* NOT be used, except  */
/* by a Creator		*/

#define SEX_MALE                      1
#define SEX_FEMALE                    2

/*
 * Item types.
 * Used in #OBJECTS.
 */
#define ITEM_LIGHT                    1
#define ITEM_AMMO                     2
#define ITEM_BOMB                     3
#define ITEM_BLUEPRINT                4
#define ITEM_WEAPON                   5
#define ITEM_SUIT             6
#define ITEM_MEDPACK              7
#define ITEM_DRONE            8
#define ITEM_ARMOR                    9
#define ITEM_TELEPORTER          10
#define ITEM_INSTALLATION            11
#define ITEM_IMPLANT             12
#define ITEM_FLAG            13
#define ITEM_DART_BOARD          14
#define ITEM_ELEMENT             15
#define ITEM_CONTAINER           16
#define ITEM_WEAPON_UP           17
#define ITEM_PIECE           18
#define ITEM_COMPUTER            19
#define ITEM_LOCATOR             20
#define ITEM_SKILL_UP            21
#define ITEM_PART            22
#define ITEM_DISK            23
#define ITEM_TRASH           24
#define ITEM_ASTEROID            25
#define ITEM_BACKUP_DISK         26
#define ITEM_BOARD           27
#define ITEM_VEHICLE_UP          28
#define ITEM_TOOLKIT             29
#define ITEM_SCAFFOLD            30
#define ITEM_ORE             31
#define ITEM_BIOTUNNEL           32
#define ITEM_BATTERY             33
#define ITEM_TOKEN           35
#define ITEM_MATERIAL            40

/*
 * Extra flags.
 * Used in #OBJECTS.
 */
#define ITEM_NUCLEAR            1
#define ITEM_STICKY         2
#define ITEM_NOQP           4
#define ITEM_INVIS          32
#define ITEM_NODROP         128
#define ITEM_NOREMOVE           4096
#define ITEM_INVENTORY          8192
#define ITEM_NOSAVE         16384                           /* For "quest" items :) */
#define ITEM_RARE           1048576
#define ITEM_NOLOOT         4194304
#define ITEM_UNIQUE         16777216

#define CLASS_ENGINEER      0
#define CLASS_DARKOP        1
#define CLASS_MINER     2
#define CLASS_DRIVER        3
#define CLASS_SUICIDE_BOMBER    4
#define CLASS_PILOT     5
#define CLASS_SNIPER        6
#define CLASS_ROBOTIC       7
#define CLASS_SCIENTIST     8
#define CLASS_HACKER        9
#define CLASS_SCANNER       10
#define CLASS_SPRINTER      11
#define CLASS_PROJECTOR     12
#define CLASS_GENIUS        13
#define CLASS_MECHANIC      14
#define CLASS_MEDIC     15
#define CLASS_SPY       16

/*
 * Wear flags.
 * Used in #OBJECTS.
 */
#define ITEM_WEAR_NONE          BIT_0
#define ITEM_WEAR_HALO          BIT_1
#define ITEM_WEAR_AURA          BIT_2
#define ITEM_WEAR_HORNS         BIT_3
#define ITEM_WEAR_HEAD          BIT_4
#define ITEM_WEAR_FACE          BIT_5
#define ITEM_WEAR_BEAK          BIT_6
#define ITEM_WEAR_EAR           BIT_7
#define ITEM_WEAR_NECK          BIT_8
#define ITEM_WEAR_WINGS         BIT_9
#define ITEM_WEAR_SHOULDERS     BIT_10
#define ITEM_WEAR_ARMS          BIT_11
#define ITEM_WEAR_WRIST         BIT_12
#define ITEM_WEAR_HANDS         BIT_13
#define ITEM_WEAR_FINGER        BIT_14
#define ITEM_WEAR_CLAWS         BIT_15
#define ITEM_WEAR_HOLD_HAND     BIT_16
#define ITEM_WEAR_ABOUT         BIT_17
#define ITEM_WEAR_WAIST         BIT_18
#define ITEM_WEAR_BODY          BIT_19
#define ITEM_WEAR_TAIL          BIT_20
#define ITEM_WEAR_LEGS          BIT_21
#define ITEM_WEAR_FEET          BIT_22
#define ITEM_WEAR_HOOVES        BIT_23
#define ITEM_TAKE               BIT_24
#define ITEM_WEAR_EYES      BIT_26
#define ITEM_WEAR_CBADGE    BIT_27
#define ITEM_WEAR_UTAIL     BIT_28
/*
 * Apply types (for affects).
 * Used in #OBJECTS.
 */
#define APPLY_NONE                    0
#define APPLY_STR                     1
#define APPLY_DEX                     2
#define APPLY_INT                     3
#define APPLY_WIS                     4
#define APPLY_CON                     5
#define APPLY_SEX                     6
#define APPLY_CLASS                   7
#define APPLY_LEVEL                   8
#define APPLY_AGE                     9
#define APPLY_HEIGHT                 10
#define APPLY_WEIGHT                 11
#define APPLY_MANA                   12
#define APPLY_HIT                    13
#define APPLY_MOVE                   14
#define APPLY_GOLD                   15
#define APPLY_EXP                    16
#define APPLY_AC                     17
#define APPLY_HITROLL                18
#define APPLY_DAMROLL                19
#define APPLY_SAVING_PARA            20
#define APPLY_SAVING_ROD             21
#define APPLY_SAVING_PETRI           22
#define APPLY_SAVING_BREATH          23
#define APPLY_SAVING_SPELL           24

/*
 * Values for containers (value[1]).
 * Used in #OBJECTS.
 */
#define CONT_CLOSEABLE                1
#define CONT_PICKPROOF                2
#define CONT_CLOSED                   4
#define CONT_LOCKED                   8

/*
 * Data files used by the server.
 *
 * AREA_LIST contains a list of areas to boot.
 * All files are read in completely at bootup.
 * Most output files (bug, idea, typo, shutdown) are append-only.
 *
 * The NULL_FILE is held open so that we have a stream handle in reserve,
 *   so players can go ahead and telnet to all the other descriptors.
 * Then we close it whenever we need to open a file (e.g. a save file).
 */
#if defined(macintosh)
#define LOG_DIR         ""                                  /* Log files                 */
#define PLAYER_DIR      ""                                  /* Player files                 */
#define SITE_DIR    ""
#define NULL_FILE       "proto.are"                         /* To reserve one stream        */
#endif

#if defined(MSDOS)
#define LOG_DIR         ""                                  /* Log files                 */
#define PLAYER_DIR      ""                                  /* Player files                 */
#define SITE_DIR    ""
#define NULL_FILE       "nul"                               /* To reserve one stream        */
#endif

#if defined(unix)
#define PLAYER_DIR      "../player/"                        /* Player files                 */
#define LOG_DIR         "../log/"                           /* Log files                 */
#define SITE_DIR    "../../public_html/"                    /* For online who list	*/
#define NULL_FILE       "/dev/null"                         /* To reserve one stream        */
#endif

#if defined(linux)
#define PLAYER_DIR      "../player/"                        /* Player files                 */
#define LOG_DIR         "../log/"                           /* Log files                 */
#define SITE_DIR    "../../public_html/"
#define NULL_FILE       "/dev/null"                         /* To reserve one stream        */
#endif

#define AREA_LIST       "area.lst"                          /* List of areas                */

#define MAIL_DIR            "../mail/"
#define DATA_DIR            "../data/"
#define BUG_DIR             "../reports/"
#define LOG_DIR             "../log/"
#define INFO_DIR	    "../information/"

#define BUG_FILE            BUG_DIR  "bugs.txt"           /* Game bugs    */
#define CHANGES_FILE        DATA_DIR  "changes.txt"        /* For Changes list                 */
#define LOG_FILE            DATA_DIR  "logs.txt"           /* For 'idea', 'typo', and 'bug'    */
#define HELP_FILE           BUG_DIR   "helps.txt"                /* For missing help files      */
#define SNOOP_FILE          LOG_DIR  "watch.txt"                 /* For players who need to be watched   */
#define SHUTDOWN_FILE       BUG_DIR  "shutdown.txt"             /* For 'shutdown'               */
#define DISABLED_FILE       DATA_DIR "disabled.txt"             /* disabled commands - Wyn */
#define PLAYER_LIST_FILE    DATA_DIR "playerlist.txt"          /* Player list */

#define OBJECTS_FILE        DATA_DIR "objects.lst"
#define OBJECTS_FEST_FILE   DATA_DIR "objects.fst"
#define OBJECTS_BACKUP_FILE DATA_DIR "objects.bak"
#define QUOTE_FILE  DATA_DIR "quotes.txt"
#define BANS_FILE   DATA_DIR "bans.lst"
#define BRANDS_FILE DATA_DIR "brands.lst"
#define MAP_FILE    DATA_DIR "map.txt"
#define BUILDING_TABLE_FILE DATA_DIR "building_table.txt"
#define BUILDING_FILE   DATA_DIR "buildings.txt"
#define BUILDING_FEST_FILE  DATA_DIR "buildings.fst"
#define BUILDING_BACKUP_FILE    DATA_DIR "buildings.bak"
#define VEHICLE_FILE    DATA_DIR  "vehicles.txt"
#define VEHICLE_FEST_FILE   DATA_DIR  "vehicles.fst"
#define VEHICLE_BACKUP_FILE DATA_DIR  "vehicles.bak"
#define MAP_BACKUP_FILE DATA_DIR  "map.bak"
#define SCORE_FILE  DATA_DIR "scores.txt"
#define RANK_FILE   DATA_DIR "ranks.txt"
#define SYSDAT_FILE DATA_DIR "system.dat"
#define ALLIANCES_FILE  DATA_DIR "alliances.txt"
#define PLANET_FILE DATA_DIR "planets.txt"
#define MAX_PLAYERS_FILE DATA_DIR "players.txt"
#define MULTIPLAY_FILE  DATA_DIR "multiplay.txt"

/* Other Stuff - Flar */
#define COPYOVER_FILE   "COPYOVER.TXT"                      /* Temp data file used for copyover */
#define EXE_FILE        "../src/ack"                        /* The one that runs the ACK! */

/* stuff for Quests */
#define QROOM_VNUM "299"
#define CLAN_MONEY 1039
