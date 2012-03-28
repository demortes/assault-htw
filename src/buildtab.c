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
 ~  In order to use any part of this  PA  Diku Mud, you must comply with   ~
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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ack.h"

/* The tables in here are:
     Object item type    :    tab_item_types      : number
     Object extra flags  :    tab_obj_flags       : bit_vector
     Object wear flags   :    tab_wear_flags      : bit_vector
     Wear locations      :    tab_wear_loc        : number

*/

const struct lookup_type tab_value_meanings[]=
{
    { "Unused",                                         15, 0 },
    { "Unused",                                         16, 0 },
    { "Hours of light, -1 = infinite",                  17, 5 },
    { "Unused",                                         18, 0 },
    { "Unused",                                         19, 0 },
    { "Rarity",                                         20, 0 },
    { "Replacer vnum",                                  21, 0 },
    { "Forced extract timer",                           22, 0 },
    { "Unused",                                         23, 0 },
    { "Unused",                                         24, 0 },
    { "Unused",                                         25, 0 },
    { "Unused",                                         26, 0 },
    { "Unused",                                         27, 0 },
    { "Unused",                                         28, 0 },
    { "Unused",                                         29, 0 },

    { "AmmoType",                                       30, 0 },
    { "Amount",                                         31,  0 },
    { "Unused",                                         32,  0 },
    { "Unused",                                         33,  0 },
    { "Unused",                                         34, 0 },
    { "Building",                                       35, 0 },
    { "Replacer vnum",                                  36, 0 },
    { "Forced extract timer",                           37, 0 },
    { "Unused",                                         38, 0 },
    { "Unused",                                         39, 0 },
    { "Unused",                                         40, 0 },
    { "Unused",                                         41, 0 },
    { "Unused",                                         42, 0 },
    { "Unused",                                         43, 0 },
    { "Unused",                                         44, 0 },

    { "Timer",                                          45, 0 },
    { "Armed",                                          46, 0 },
    { "Damage Dice",                                    47, 0 },
    { "Special",                                        48, 0 },
    { "Grenade?",                                       49, 0 },
    { "Building",                                       50, 0 },
    { "Dud Chance",                                     51, 0 },
    { "Research Progress",                              52, 0 },
    { "Silent",                                         53, 0 },
    { "Unused",                                         54, 0 },
    { "Unused",                                         55, 0 },
    { "Unused",                                         56, 0 },
    { "Unused",                                         57, 0 },
    { "Unused",                                         58, 0 },
    { "Unused",                                         59, 0 },

    { "Unused",                                         60, 0 },
    { "Unused",                                         61, 0 },
    { "Unused",                                     62, 0 },
    { "Unused",                                         63, 0 },
    { "Unused",                                         64, 0 },
    { "Building",                                       65, 0 },
    { "Replacer vnum",                                  66, 0 },
    { "Forced extract timer",                           67, 0 },
    { "Unused",                                         68, 0 },
    { "Unused",                                     69, 0 },
    { "Unused",                                     70, 0 },
    { "Unused",                                     71, 0 },
    { "Unused",                                     72, 0 },
    { "Unused",                                     73, 0 },
    { "Unused",                                     74, 0 },

    { "Ammo",                                           75, 0 },
    { "Max Ammo",                                       76, 0 },
    { "AmmoType",                                       77, 0 },
    { "Affects",                                        78, 0 },
    { "Range",                                          79, 0 },
    { "Sector",                                         80, 0 },
    { "Scope",                                          81, 0 },
    { "+Dam",                                           82, 0 },
    { "+BuildDam",                                      83, 0 },
    { "+Speed",                                     84, 0 },
    { "+Accuracy",                                      85, 0 },
    { "Unused",                                     86, 0 },
    { "Unused",                                     87, 0 },
    { "Unused",                                     88, 0 },
    { "Unused",                                     89, 0 },

    { "Suit",                                           90, 0 },
    { "Quality",                                        91, 0 },
    { "Unused",                                         92, 0 },
    { "Unused",                                         93, 0 },
    { "Unused",                                         94, 0 },
    { "Building",                                       95, 0 },
    { "Replacer vnum",                                  96, 0 },
    { "Forced extract timer",                           97, 0 },
    { "Unused",                                         98, 0 },
    { "Unused",                                         99, 0 },
    { "Unused",                                         100, 0 },
    { "Unused",                                         101, 0 },
    { "Unused",                                         102, 0 },
    { "Unused",                                         103, 0 },
    { "Unused",                                         104, 0 },

    { "Quality",                                        105, 0 },
    { "Special",                                        106, 0 },
    { "Unused",                                         107, 0 },
    { "Unused",                                         108, 0 },
    { "Unused",                                         109, 0 },
    { "Building",                                       110, 0 },
    { "Replacer vnum",                                  111, 0 },
    { "Forced extract timer",                           112, 0 },
    { "Unused",                                         113, 0 },
    { "Unused",                                         114, 0 },

    { "Type",                                           120, 50 },
    { "Quality",                                        121, -25 },
    { "Unused",                                         122, -25 },
    { "Unused",                                         123, -25 },
    { "Unused",                             124, 0 },
    { "Building",                                       125, 0 },
    { "Replacer vnum",                                  126, 0 },
    { "Forced extract timer",                           127, 0 },
    { "Unused",                                         128, 0 },
    { "Unused",                                         129, 0 },
    { "Unused",                                         130, 0 },
    { "Unused",                                         131, 0 },
    { "Unused",                                         132, 0 },
    { "Unused",                                         133, 0 },
    { "Unused",                                         134, 0 },

    { "Main Type",                                  135, 10 },
    { "Damage",                                 136, 0 },
    { "vs. General",                                137, 0 },
    { "vs. Bullets",                                138, 0 },
    { "vs. Blast",                                      139, 0 },
    { "vs. Acid",                                       140, 0 },
    { "vs. Fire",                                       141, 0 },
    { "vs. Laser",                                      142, 0 },
    { "vs. Sound",                                      143, 0 },
    { "Unused",                                         144, 0 },
    { "Unused",                                         145, 0 },
    { "Unused",                                         146, 0 },
    { "Unused",                                         147, 0 },
    { "Unused",                                         148, 0 },
    { "Unused",                                         149, 0 },

    { "Range",                                          150, 0 },
    { "Building Type",                                  151, 0 },
    { "Unused",                                         152, 0 },
    { "Unused",                                         153, 0 },
    { "Unused",                                         154, 0 },
    { "Unused",                                         155, 0 },
    { "Replacer vnum",                                  156, 0 },
    { "Forced extract timer",                           157, 0 },
    { "Unused",                                         158, 0 },
    { "Unused",                                         159, 0 },
    { "Unused",                                         160, 0 },
    { "Unused",                                         161, 0 },
    { "Unused",                                         162, 0 },
    { "Unused",                                         163, 0 },
    { "Unused",                                         164, 0 },

    { "InstType",                                       165, 10 },
    { "Building 1",                                     166, 0 },
    { "Building 2",                                     167, 0 },
    { "Building 3",                                     168, 0 },
    { "Building 4",                                     169, 0 },
    { "Unused",                                         170, 0 },
    { "Replacer vnum",                                  171, 0 },
    { "Forced extract timer",                           172, 0 },
    { "Unused",                                         173, 0 },
    { "Unused",                                         174, 0 },
    { "Unused",                                         175, 0 },
    { "Unused",                                         176, 0 },
    { "Unused",                                         177, 0 },
    { "Unused",                                         178, 0 },
    { "Unused",                                         179, 0 },

    { "Type",                                           180, 10 },
    { "Value1",                                         181, 0 },
    { "Value2",                                         182, 0 },
    { "Bit value",                                      183, 0 },
    { "Unused",                                         184, 0 },
    { "Building",                                       185, 0 },
    { "Replacer vnum",                                  186, 0 },
    { "Forced extract timer",                           187, 0 },
    { "Unused",                                         188, 0 },
    { "Unused",                                         189, 0 },
    { "Unused",                                         190, 0 },
    { "Unused",                                         191, 0 },
    { "Unused",                                         192, 0 },
    { "Unused",                                         193, 0 },
    { "Unused",                                         194, 0 },

    { "Type",                                           225, 10 },
    { "State",                                          226, 0 },
    { "Unused",                                         227, 0 },
    { "Unused",                                         228, 0 },
    { "Unused",                                         229, 0 },
    { "Unused",                                         230, 0 },
    { "Replacer vnum",                                  231, 0 },
    { "Forced extract timer",                           232, 0 },
    { "Unused",                                         233, 0 },
    { "Unused",                                         234, 0 },
    { "Unused",                                         235, 0 },
    { "Unused",                                         236, 0 },
    { "Unused",                                         237, 0 },
    { "Unused",                                         238, 0 },
    { "Unused",                                         239, 0 },

    { "Solids",                                         240, 10 },
    { "Liquids",                                        241, 0 },
    { "Gasses",                                         242, 0 },
    { "Unused",                                         243, 0 },
    { "Unused",                                         244, 0 },
    { "Unused",                                         245, 0 },
    { "Replacer vnum",                                  246, 0 },
    { "Forced extract timer",                           247, 0 },
    { "Unused",                                         248, 0 },
    { "Unused",                                         249, 0 },
    { "Unused",                                         250, 0 },
    { "Unused",                                         251, 0 },
    { "Unused",                                         252, 0 },
    { "Unused",                                         253, 0 },
    { "Unused",                                         254, 0 },

    { "Type",                                           255, 10 },
    { "Quality",                                        256, 0 },
    { "Unused",                                         257, 0 },
    { "Unused",                                         258, 0 },
    { "Unused",                                         259, 0 },
    { "Unused",                                         260, 0 },
    { "Replacer vnum",                                  261, 0 },
    { "Forced extract timer",                           262, 0 },
    { "Unused",                                         263, 0 },
    { "Unused",                                         264, 0 },
    { "Unused",                                         265, 0 },
    { "Unused",                                         266, 0 },
    { "Unused",                                         267, 0 },
    { "Unused",                                         268, 0 },
    { "Unused",                                         269, 0 },

    { "Connect",                                        270, 10 },
    { "Result",                                         271, 0 },
    { "Connect",                                        272, 0 },
    { "Result",                                         273, 0 },
    { "Unused",                                         274, 0 },
    { "Unused",                                         275, 0 },
    { "Unused",                                         276, 0 },
    { "Unused",                                         277, 0 },
    { "Unused",                                         278, 0 },
    { "Unused",                                         279, 0 },
    { "Unused",                                         280, 0 },
    { "Unused",                                         281, 0 },
    { "Unused",                                         282, 0 },
    { "Unused",                                         283, 0 },
    { "Unused",                                         284, 0 },

    { "Quality",                                        285, 10 },
    { "Battery",                                        286, 0 },
    { "Antenna",                                        287, 0 },
    { "Backdoor",                                       288, 0 },
    { "Speed",                                          289, 0 },
    { "VirusVersion",                                   290, 0 },
    { "CrackerVersion",                                 291, 0 },
    { "Modem",                                          292, 0 },
    { "BatteryType",                                    293, 0 },
    { "Unused",                                         294, 0 },
    { "Unused",                                         295, 0 },
    { "Unused",                                         296, 0 },
    { "Unused",                                         297, 0 },
    { "Unused",                                         298, 0 },
    { "Unused",                                         299, 0 },

    { "Range",                                          300, 10 },
    { "Battery",                                        301, 0 },
    { "Unused",                                         302, 0 },
    { "Unused",                                         303, 0 },
    { "Unused",                                         304, 0 },
    { "Unused",                                         305, 0 },
    { "Replacer vnum",                                  306, 0 },
    { "Forced extract timer",                           307, 0 },
    { "Unused",                                         308, 0 },
    { "Unused",                                         309, 0 },
    { "Unused",                                         310, 0 },
    { "Unused",                                         311, 0 },
    { "Unused",                                         312, 0 },
    { "Unused",                                         313, 0 },
    { "Unused",                                         314, 0 },

    { "Skill",                                          315, 10 },
    { "Amount",                                         316, 0 },
    { "Unused",                                         317, 0 },
    { "Unused",                                         318, 0 },
    { "Unused",                                         319, 0 },
    { "Unused",                                         320, 0 },
    { "Replacer vnum",                                  321, 0 },
    { "Forced extract timer",                           322, 0 },
    { "Unused",                                         323, 0 },
    { "Unused",                                         324, 0 },
    { "Unused",                                         325, 0 },
    { "Unused",                                         326, 0 },
    { "Unused",                                         327, 0 },
    { "Unused",                                         328, 0 },
    { "Unused",                                         329, 0 },

    { "Type",                                           330, 0 },
    { "+HP",                                            331, 0 },
    { "+Speed",                                         332, 0 },
    { "+Fuel",                                          333, 0 },
    { "+Ammo",                                          334, 0 },
    { "AmmoType",                                       335, 0 },
    { "Flags",                                          336, 0 },
    { "Unused",                                         337, 0 },
    { "Unused",                                         338, 0 },
    { "Unused",                                         339, 0 },
    { "Unused",                                         340, 0 },
    { "Unused",                                         341, 0 },
    { "Unused",                                         342, 0 },
    { "Unused",                                         343, 0 },
    { "Unused",                                         344, 0 },

    { "Type",                                           345, 0 },
    { "Version",                                        346, 0 },
    { "Unused",                                         347, 0 },
    { "Unused",                                         348, 0 },
    { "Unused",                                         349, 0 },
    { "Unused",                                         350, 0 },
    { "Unused",                                         351, 0 },
    { "Unused",                                         352, 0 },
    { "Unused",                                         353, 0 },
    { "Unused",                                         354, 0 },
    { "Unused",                                         355, 0 },
    { "Unused",                                         356, 0 },
    { "Unused",                                         357, 0 },
    { "Unused",                                         358, 0 },
    { "Unused",                                         359, 0 },

    { "Unused",                                         360, 0 },
    { "Unused",                                         361, 0 },
    { "Unused",                                         362, 0 },
    { "Unused",                                         363, 0 },
    { "Unused",                                         364, 0 },
    { "Unused",                                         365, 0 },
    { "Unused",                                         366, 0 },
    { "Unused",                                         367, 0 },
    { "Unused",                                         368, 0 },
    { "Unused",                                         369, 0 },
    { "Unused",                                         370, 0 },
    { "Unused",                                         371, 0 },
    { "Unused",                                         372, 0 },
    { "Unused",                                         373, 0 },
    { "Unused",                                         374, 0 },

    { "Type",                                           375, 0 },
    { "Amount",                                         376, 0 },
    { "Unused",                                         377, 0 },
    { "Unused",                                         378, 0 },
    { "Unused",                                         379, 0 },
    { "Unused",                                         380, 0 },
    { "Unused",                                         381, 0 },
    { "Unused",                                         382, 0 },
    { "Unused",                                         383, 0 },
    { "Unused",                                         384, 0 },
    { "Unused",                                         385, 0 },
    { "Unused",                                         386, 0 },
    { "Unused",                                         387, 0 },
    { "Unused",                                         388, 0 },
    { "Unused",                                         389, 0 },

    { "V0",                                             390, 0 },
    { "V1",                                             391, 0 },
    { "V2",                                             392, 0 },
    { "V3",                                             393, 0 },
    { "V4",                                             394, 0 },
    { "V5",                                             395, 0 },
    { "V6",                                             396, 0 },
    { "V7",                                             397, 0 },
    { "V8",                                             398, 0 },
    { "V9",                                             399, 0 },
    { "Unused",                                         400, 0 },
    { "Unused",                                         401, 0 },
    { "Unused",                                         402, 0 },
    { "Unused",                                         403, 0 },
    { "Unused",                                         404, 0 },

    { "No. of days message will last.",                 405, 10 },
    { "Minimum level to write board (read?).",          406, 0 },
    { "Minimum level to look board (write?).",          407, 0 },
    { "BOARD vnum... set to object's vnum.",            408, 0 },
    { "Alliance",                                       409, 0 },
    { "Building",                                       410, 0 },
    { "Unused",                                         411, 0 },
    { "Unused",                                         412, 0 },
    { "Unused",                                         413, 0 },
    { "Unused",                                         414, 0 },
    { "Unused",                                         415, 0 },
    { "Unused",                                         416, 0 },
    { "Unused",                                         417, 0 },
    { "Unused",                                         418, 0 },
    { "Unused",                                         419, 0 },

    { "Type",                                           420, 0 },
    { "Quality",                                        421, 0 },
    { "Vehicle",                                        422, 0 },
    { "Vehicle",                                        423, 0 },
    { "Vehicle",                                        424, 0 },
    { "Vehicle",                                        425, 0 },
    { "Vehicle",                                        426, 0 },
    { "Vehicle",                                        427, 0 },
    { "Vehicle",                                        428, 0 },
    { "Bit Value",                                      429, 0 },
    { "Unused",                                         430, 0 },
    { "Unused",                                         431, 0 },
    { "Unused",                                         432, 0 },
    { "Unused",                                         433, 0 },
    { "Unused",                                         434, 0 },

    { "GWire",                                          435, 0 },
    { "BWire",                                          436, 0 },
    { "RWire",                                          437, 0 },
    { "YWire",                                          438, 0 },
    { "GWire2",                                         439, 0 },
    { "BWire2",                                         440, 0 },
    { "RWire2",                                         441, 0 },
    { "YWire2",                                         442, 0 },
    { "Unused",                                         443, 0 },
    { "Unused",                                         444, 0 },
    { "Unused",                                         445, 0 },
    { "Unused",                                         446, 0 },
    { "Unused",                                         447, 0 },
    { "Unused",                                         448, 0 },
    { "Unused",                                         449, 0 },

    { "Building",                                       450, 0 },
    { "Limit",                                          451, 0 },
    { "Unused",                                         452, 0 },
    { "Unused",                                         453, 0 },
    { "Unused",                                         454, 0 },
    { "Unused",                                         455, 0 },
    { "Unused",                                         456, 0 },
    { "Unused",                                         457, 0 },
    { "Unused",                                         458, 0 },
    { "Unused",                                         459, 0 },
    { "Unused",                                         460, 0 },
    { "Unused",                                         461, 0 },
    { "Unused",                                         462, 0 },
    { "Unused",                                         463, 0 },
    { "Unused",                                         464, 0 },

    { "Research Time",                                  465, 0 },
    { "Item #1",                                        466, 0 },
    { "Item #2",                                        467, 0 },
    { "Item #3",                                        468, 0 },
    { "Item #4",                                        469, 0 },
    { "Item #5",                                        470, 0 },
    { "Item #6",                                        471, 0 },
    { "Unused",                                         472, 0 },
    { "Unused",                                         473, 0 },
    { "Unused",                                         474, 0 },
    { "Unused",                                         475, 0 },
    { "Unused",                                         476, 0 },
    { "Unused",                                         477, 0 },
    { "Unused",                                         478, 0 },
    { "Unused",                                         479, 0 },

    { "Type",                                           480, 0 },
    { "Range",                                          481, 0 },
    { "Coord_x",                                        482, 0 },
    { "Coord_y",                                        483, 0 },
    { "Unused",                                         484, 0 },
    { "Unused",                                         485, 0 },
    { "Unused",                                         486, 0 },
    { "Unused",                                         487, 0 },
    { "Unused",                                         488, 0 },
    { "Unused",                                         489, 0 },
    { "Unused",                                         490, 0 },
    { "Unused",                                         491, 0 },
    { "Unused",                                         492, 0 },
    { "Unused",                                         493, 0 },
    { "Unused",                                         494, 0 },

    { "Quality",                                        480, 0 },
    { "Charge",                                         481, 0 },
    { "Unused",                                         482, 0 },
    { "Unused",                                         483, 0 },
    { "Unused",                                         484, 0 },
    { "Unused",                                         485, 0 },
    { "Unused",                                         486, 0 },
    { "Unused",                                         487, 0 },
    { "Unused",                                         488, 0 },
    { "Unused",                                         489, 0 },
    { "Unused",                                         490, 0 },
    { "Unused",                                         491, 0 },
    { "Unused",                                         492, 0 },
    { "Unused",                                         493, 0 },
    { "Unused",                                         494, 0 },

    { NULL, 0 }
};

const struct lookup_type tab_item_types[] =
{
    /* { "nada",			0, 0 },  */
    /* { "placeholder",		0, 0 },   */
    { "light",                    1,   20},
    { "ammo",                     2, 1000},
    { "bomb",                     3, 2000},
    { "blueprints",               4, 5000},
    { "weapon",                   5,  500},
    { "suit",                     6, 5000},
    { "medpack",                  7,    0},
    { "drone",                    8,    0},
    { "armor",                    9,  500},
    { "teleporter",              10, 1000},
    { "installation",            11,    0},
    { "implant",                 12,   20},
    { "flag",                    13,    0},
    { "dartboard",               14,    0},
    { "element",                 15,    0},
    { "container",               16,    0},
    { "weapon upgrade",          17,    0},
    { "piece",                   18,    0},
    { "computer",                19,    0},
    { "item locator",            20,    0},
    { "skill upgrade",           21,    0},
    { "part",                    22,    0},
    { "disk",                    23,    0},
    { "portal",                  24,    0},
    { "asteroid",                25,    0},
    { "backup disk",             26,    0},
    { "board",                   27,    0},
    { "vehicle addon",           28,    0},
    { "toolkit",                 29,    0},
    { "scaffold",                30,    0},
    { "ore",                     31,    0},
    { "biotunnel",               32,    0},
    { "battery",                 33,    0},
    { "unused",                  34,    0},
    { "token",     35, NO_USE },
    { "unused",                  36,    0},
    { "unused",                  37,    0},
    { "unused",                  38,    0},
    { "unused",                  39,    0},
    { "material",     40, 0 },
    { NULL, 0}
};

const struct lookup_type tab_obj_flags[] =
{
    { "nada"   ,   0, 0 },
    { "nuclear"    ,   1, 0 },
    { "sticky" ,   2, 0 },
    { "noqp"   ,   4, 0 },
    { "nodrop"        ,         128, 20},
    { "noremove"      ,        4096, 100},
    { "inventory"     ,        8192, 0},
    { "nosave"        ,       16384, 1000},
    { "nosac"      ,    8388608, 100 },
    { "unique"     ,   16777216, 100 },
    { NULL,0}
};

const struct lookup_type tab_weapon_flags[] =
{
    { "nada",  0, 0 },
    { NULL,0}
};

const struct lookup_type tab_wear_flags[] =
{
    { "head",                BIT_4, 40 },
    { "face",                BIT_5, 50 },
    { "ear",                 BIT_7, 40 },
    { "neck",                BIT_8, 20 },
    { "shoulders",          BIT_10, 40 },
    { "arms",               BIT_11, 100 },
    { "wrist",              BIT_12, 20 },
    { "hands",              BIT_13, 50 },
    { "finger",             BIT_14, 40 },
    { "hold",               BIT_16, 100 },
    { "about",              BIT_17, 30 },
    { "waist",              BIT_18, 20 },
    { "body",               BIT_19, 100 },
    { "legs",               BIT_21, 100 },
    { "feet",               BIT_22, 900 },
    { "take",               BIT_24, 900 },
    { "eyes",               BIT_26, 900 },
    { NULL, 0 }
};

/* This not used at present.  Thought i'd bung details in though :) */
/* Yeah it is, you lied! - Wyn */
const struct lookup_type tab_item_apply[] =
{

    { "nada",                 1, 0 },
    { NULL, 0 }
};

const struct lookup_type tab_wear_loc[] =
{
    { "nada",         0, 0},
    { "head",                   4, 20},
    { "face",                   5, 20},
    { "ear_l",                     7, 40},
    { "ear_r",                     8, 40},
    { "neck_1",                     9, 40},
    { "neck_2",                     10, 40},
    { "shoulders",                    12, 20},
    { "arms",                    13, 20},
    { "wrist_l",                  14, 50},
    { "wrist_r",                   15, 20},
    { "hands",                   16, 20},
    { "finger_l",                 17, 30},
    { "finger_r",                 18, 30},
    { "hold_l",                   20, 100},
    { "hold_r",                    21, 40},
    { "cape",                    22, 20},
    { "waist",                     23, 20},
    { "body",                  24, 40},
    { "legs",                     26, 20},
    { "feet",                  27, 40},
    { "eyes",         29, 20},
    { "max_wear",                36,  0},
    { NULL, 0}
};

const struct lookup_type tab_player_flags[] =
{
    { "nada",         0,  0},
    { "AFK",                      2,  0},
    { "ECHAN",            BIT_26, 0},
    { NULL,0}
};

/* Now for the functions */

unsigned long int table_lookup(const struct lookup_type * table,char * name)
{
    int a;

    if (name[0]=='\0')
        return /* table[0].value-1 */0;

    for (a=0; table[a].text != NULL; a++)
        if (!str_prefix(name,table[a].text))
            return  ( ( !str_cmp(table[a].text, "nada" ) ) ? 0 : table[a].value );
    return /* table[0].value-1  */0;
}

unsigned long int multi_table_lookup(const struct lookup_type * table,char * name)
{
    int a;
    long_int retval;
    char curname[MSL];

    if (name[0]=='\0')
        return 0;

    retval = 0;
    name=one_argument(name,curname);
    for ( ; ; )
    {
        if ( curname[0] == '\0')
            break;

        for (a=0; table[a].text != NULL; a++)
        {
            if (!str_prefix(curname,table[a].text)
                || !str_cmp(curname,table[a].text))
                retval ^= ( ( !str_cmp(table[a].text, "nada" ) ) ? 0 : table[a].value );
        }
        name=one_argument(name,curname);
    }

    return retval;
}

char * rev_table_lookup(const struct lookup_type * table, unsigned long int number)
{
    int a;

    for (a=0; table[a].text != NULL; a++)
        if (table[a].value==number) return table[a].text;
    return "";
}

/* spec: fixed to not assume contiguous bit use */

char * bit_table_lookup(const struct lookup_type * table, unsigned long int number)
{
    int a;
    static char buf[MAX_STRING_LENGTH];

    buf[0]='\0';

    for ( a=0; number && table[a].text; a++ )
    {
        if ( ( number & table[a].value)==table[a].value &&
            str_cmp( table[a].text, "nada" ) && str_cmp( table[a].text, "placeholder" ) )
        {
            safe_strcat( MSL, buf,table[a].text);
            //	  safe_strcat( MSL, buf,", ");
            safe_strcat( MSL, buf," ");
            number &= ~table[a].value;
        }
    }

    if (buf[0]=='\0')
        safe_strcat( MSL, buf, "none" );
    else
    {
        a=strlen(buf);
        //	  buf[a-2]='.';
        //	  buf[a-1]='\0';
        //	  buf[a-2]='.';
        buf[a-1]='\0';
    }
    return buf;
}

void table_printout(const struct lookup_type * table,char * buf)
{
    int a;

    a=0;
    buf[0]='\0';

    for (a=0;table[a].text!=NULL;a++)
    {
        if ( ( strcmp(table[a].text,"nada") )
            &&   ( strcmp(table[a].text,"placeholder") ) )  /* If not an invalid choice */
        {
            safe_strcat( MSL, buf,"          ");
            safe_strcat( MSL, buf,table[a].text);
            safe_strcat( MSL, buf,"\n\r");
        }
    }

    return;
}

void wide_table_printout(const struct lookup_type * table,char * buf)
{
    /* Like table_printout, but formats into columns */

    char tmp[MAX_STRING_LENGTH];
    int a;
    int foo;                                                /* work out how many values shown in a row */

    a=0;
    foo = 0;
    buf[0]='\0';

    for (a=0;table[a].text!=NULL;a++)
    {
        if (  ( strcmp(table[a].text,"nada") )              /* If not an invalid choice */
            && ( strcmp(table[a].text,"nada") )   )
        {
            safe_strcat( MSL, buf,"     ");
            sprintf( tmp, "%12s", table[a].text );
            safe_strcat( MSL, buf,tmp );
            if ( ++foo % 4 == 0 )
                safe_strcat( MSL, buf,"\n\r");
        }
    }
    safe_strcat( MSL, buf, "\n\r" );
    return;
}

char * show_values(const struct lookup_type * table,int value, bool fBit)
{

    char tmp[MAX_STRING_LENGTH];
    static char buf[MAX_STRING_LENGTH];
    int a;
    int foo;                                                /* work out how many values shown in a row */

    a=0;
    foo = 0;
    buf[0]='\0';

    for (a=0;table[a].text!=NULL;a++)
    {
        if (  ( strcmp(table[a].text,"nada") )              /* If not an invalid choice */
            && ( strcmp(table[a].text,"placeholder") )  )
        {
            safe_strcat( MSL, buf,"     ");
            sprintf( tmp, "%s%-13s",
                fBit?  (IS_SET( value, table[a].value ) ? "@@y*" : "@@g ") :
            (value == table[a].value ? "@@y*" : "@@g "),
                table[a].text );
            safe_strcat( MSL, buf,tmp );
            if ( ++foo % 4 == 0 )
                safe_strcat( MSL, buf,"\n\r");
        }
    }
    safe_strcat( MSL, buf, "@@g\n\r" );
    return( buf );
}
