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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "ack.h"
#include "tables.h"

// All the map-generating code goes here

void create_full_map args( ( CHAR_DATA *ch ) );
void create_map(CHAR_DATA *ch, int type) {
	if (type == TERRAIN_BALANCED)
		create_full_map(ch);
	return;
}
void create_full_map(CHAR_DATA *ch) {
	int xx, yy, x, y, i, j, k, m, n, z = ch->z;
	int sdefault = SECT_OCEAN;
	char buf[MSL];
	int fuzz = 3;

	for (x = 0; x < MAX_MAPS; x++)
		for (y = 0; y < MAX_MAPS; y++)
			map_table.type[x][y][z] = sdefault;

	buf[0] = '\0';
	j = number_range(MAX_MAPS / 40, MAX_MAPS / 20); /* Generate Snow */

	sprintf(buf + strlen(buf), "%d Snow\n\r", j);
	for (i = 0; i < j; i++) {
		m = number_range(20, MAX_MAPS / 10);
		n = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		for (xx = n - m; xx < n; xx++) {
			for (yy = k - m; yy < k; yy++) {
				x = xx;
				y = yy;
				real_coords(&x, &y);
//					if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
//						continue;
				if ((xx + fuzz >= n || xx - fuzz <= n - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if ((yy + fuzz >= k || yy - fuzz <= k - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if (map_table.type[x][y][z] == sdefault)
					map_table.type[x][y][z] = SECT_SNOW;
			}
		}
	}
	j = number_range(MAX_MAPS / 10, MAX_MAPS / 5); /* Generate Rock Fields */
	sprintf(buf + strlen(buf), "%d Rock Fields\n\r", j);
	for (i = 0; i < j; i++) {
		m = number_range(20, MAX_MAPS / 20);
		n = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		for (xx = n - m; xx < n; xx++) {
			for (yy = k - m; yy < k; yy++) {
				x = xx;
				y = yy;
				real_coords(&x, &y);
//					if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
//						continue;
				if ((xx + fuzz >= n || xx - fuzz <= n - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if ((yy + fuzz >= k || yy - fuzz <= k - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if (map_table.type[x][y][z] == sdefault)
					map_table.type[x][y][z] = SECT_ROCK;
			}
		}
	}
	j = number_range(MAX_MAPS / 20, MAX_MAPS / 14); /* Generate Forests */
	sprintf(buf + strlen(buf), "%d Forests\n\r", j);
	for (i = 0; i < j; i++) {
		m = number_range(20, MAX_MAPS / 15);
		n = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		for (xx = n - m; xx < n; xx++) {
			for (yy = k - m; yy < k; yy++) {
				x = xx;
				y = yy;
				real_coords(&x, &y);
//					if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
//						continue;
				if ((xx + fuzz >= n || xx - fuzz <= n - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if ((yy + fuzz >= k || yy - fuzz <= k - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if (map_table.type[x][y][z] == sdefault)
					map_table.type[x][y][z] = SECT_FOREST;
			}
		}
	}
	j = number_range(MAX_MAPS / 40, MAX_MAPS / 20); /* Generate Deserts */
	sprintf(buf + strlen(buf), "%d Deserts\n\r", j);
	for (i = 0; i < j; i++) {
		m = number_range(20, MAX_MAPS / 10);
		n = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		for (xx = n - m; xx < n; xx++) {
			for (yy = k - m; yy < k; yy++) {
				x = xx;
				y = yy;
				real_coords(&x, &y);
//					if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
//						continue;
				if ((xx + fuzz >= n || xx - fuzz <= n - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if ((yy + fuzz >= k || yy - fuzz <= k - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if (map_table.type[x][y][z] == sdefault)
					map_table.type[x][y][z] = SECT_SAND;
			}
		}
	}
	j = number_range(MAX_MAPS / 40, MAX_MAPS / 20); /* Generate Fields */
	sprintf(buf + strlen(buf), "%d Fields\n\r", j);
	for (i = 0; i < j; i++) {
		m = number_range(20, MAX_MAPS / 10);
		n = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		for (xx = n - m; xx < n; xx++) {
			for (yy = k - m; yy < k; yy++) {
				x = xx;
				y = yy;
				real_coords(&x, &y);
//					if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
//						continue;
				if ((xx + fuzz >= n || xx - fuzz <= n - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if ((yy + fuzz >= k || yy - fuzz <= k - m)
						&& number_range(1, fuzz) == 1)
					continue;
				if (map_table.type[x][y][z] == sdefault)
					map_table.type[x][y][z] = SECT_FIELD;
			}
		}
	}
	j = number_range(1, MAX_MAPS / 20); /* Generate Lakes */
	sprintf(buf + strlen(buf), "%d Lakes 1\n\r", j);
	for (i = 1; i < j; i++) {
		x = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		yy = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(1, 3);
		for (; yy < k; yy++) {
//				if ( x - BORDER_SIZE < 0 || x + m + BORDER_SIZE >= MAX_MAPS )
//					break;
			y = yy;
			real_coords(&x, &y);

			for (n = 1; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_LAVA)
					map_table.type[x + n][y][z] = SECT_ASH;
				else if (map_table.type[x + n][y][z] != SECT_OCEAN)
					map_table.type[x + n][y][z] = SECT_WATER;
			}
			x = number_range(x - 1, x + 1);
		}
	}
	j = number_range(1, MAX_MAPS / 20);
	sprintf(buf + strlen(buf), "%d Lakes 2\n\r", j);
	for (i = 1; i < j; i++) {
		y = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		xx = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(1, 3);
		for (; xx < k; xx++) {
//				if ( y < 0 || y + m >= MAX_MAPS )
//					continue;
			x = xx;
			real_coords(&x, &y);

			for (n = 1; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_LAVA)
					map_table.type[x + n][y][z] = SECT_ASH;
				else if (map_table.type[x + n][y][z] != SECT_OCEAN)
					map_table.type[x + n][y][z] = SECT_WATER;
			}
			y = number_range(y - 1, y + 1);
		}

	}
	j = number_range(1, MAX_MAPS / 20);
	sprintf(buf + strlen(buf), "%d Lakes 3\n\r", j);
	for (i = 1; i < j; i++) {
		x = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		yy = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(1, 3);
		for (; yy < k; yy++) {
//				if ( x - BORDER_SIZE < 0 || x + m + BORDER_SIZE >= MAX_MAPS )
//					break;
			y = yy;
			real_coords(&x, &y);

			for (n = 1; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_LAVA)
					map_table.type[x + n][y][z] = SECT_ASH;
				else if (map_table.type[x + n][y][z] != SECT_OCEAN)
					map_table.type[x + n][y][z] = SECT_WATER;
			}
			x = number_range(x - 1, x + 1);
		}
	}
	j = number_range(1, MAX_MAPS / 20);
	sprintf(buf + strlen(buf), "%d Lakes 4\n\r", j);
	for (i = 1; i < j; i++) {
		y = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		xx = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(1, 3);
		for (; xx < k; xx++) {
//				if ( y - BORDER_SIZE < 0 || y + m + BORDER_SIZE >= MAX_MAPS )
//					break;
			x = xx;
			real_coords(&x, &y);

			for (n = 0; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_LAVA)
					map_table.type[x + n][y][z] = SECT_ASH;
				else if (map_table.type[x + n][y][z] != SECT_OCEAN)
					map_table.type[x + n][y][z] = SECT_WATER;
			}
			y = number_range(y - 1, y + 1);
		}
	}
	j = number_range(1, MAX_MAPS / 20); /* Generate Mountain Ridges */
	sprintf(buf + strlen(buf), "%d Mountain Ridges 1\n\r", j);
	for (i = 1; i < j; i++) {
		x = number_range(BORDER_SIZE, MAX_MAPS);
		y = number_range(BORDER_SIZE, MAX_MAPS / 2);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		yy = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(2, 4);
		for (; yy < k; yy++) {
//				if ( x < 0 || x + m >= MAX_MAPS )
//					break;
			y = yy;
			real_coords(&x, &y);
			for (n = 1; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_OCEAN)
					continue;
				map_table.type[x + n][y][z] = SECT_MOUNTAIN;
			}
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x + n][y][z] = SECT_HILLS;
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x][y][z] = SECT_HILLS;
			x = number_range(x - 1, x + 1);
		}
	}
	j = number_range(1, MAX_MAPS / 20);
	sprintf(buf + strlen(buf), "%d Mountain Ridges 2\n\r", j);
	for (i = 1; i < j; i++) {
		y = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		xx = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(2, 4);
		for (; xx < k; xx++) {
//				if ( y < 0 || y + m >= MAX_MAPS )
//					break;
			x = xx;
			real_coords(&x, &y);
			for (n = 1; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_OCEAN)
					continue;
				map_table.type[x + n][y][z] = SECT_MOUNTAIN;
			}
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x + n][y][z] = SECT_HILLS;
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x][y][z] = SECT_HILLS;
			y = number_range(y - 1, y + 1);
		}

	}
	j = number_range(1, MAX_MAPS / 20);
	sprintf(buf + strlen(buf), "%d Mountain Ridges 3\n\r", j);
	for (i = 1; i < j; i++) {
		x = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		yy = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(2, 4);
		for (; yy < k; yy++) {
//				if ( x < 0 || x + m >= MAX_MAPS )
//					break;
			y = yy;
			real_coords(&x, &y);
			for (n = 1; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_OCEAN)
					continue;
				map_table.type[x + n][y][z] = SECT_MOUNTAIN;
			}
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x + n][y][z] = SECT_HILLS;
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x][y][z] = SECT_HILLS;
			x = number_range(x - 1, x + 1);
		}
	}
	j = number_range(1, MAX_MAPS / 20);
	sprintf(buf + strlen(buf), "%d Mountain Ridges 4\n\r", j);
	for (i = 1; i < j; i++) {
		y = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		xx = number_range(BORDER_SIZE, MAX_MAPS);
		m = number_range(2, 4);
		for (; xx < k; xx++) {
//				if ( y < 0 || y + m >= MAX_MAPS )
//					break;
			x = xx;
			real_coords(&x, &y);
			for (n = 1; n < m; n++) {
				if (map_table.type[x + n][y][z] == SECT_OCEAN)
					continue;
				map_table.type[x + n][y][z] = SECT_MOUNTAIN;
			}
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x + n][y][z] = SECT_HILLS;
			if (map_table.type[x + n][y][z] != SECT_OCEAN)
				map_table.type[x][y][z] = SECT_HILLS;
			y = number_range(y - 1, y + 1);
		}
	}
	j = MAX_MAPS / 1; /* Generate Volcanos */
	sprintf(buf + strlen(buf), "%d Volcanos\n\r", j);
	for (i = 0; i < j; i++) {
		m = number_range(6, MAX_MAPS - 6);
		n = number_range(6, MAX_MAPS - 6);
		k = number_range(3, 6);
		for (x = m; x < m + k; x++) {
			for (y = n; y < n + k; y++) {
				if (x < 0 || y < 0 || x >= MAX_MAPS || y >= MAX_MAPS)
					continue;
				map_table.type[x][y][z] = SECT_LAVA;
			}
		}
		for (x = m; x < m + k; x++) {
			if (x < 0 || x >= MAX_MAPS)
				continue;
			if (map_table.type[x][n - 1][z] != SECT_LAVA)
				map_table.type[x][n][z] = SECT_MOUNTAIN;
			if (map_table.type[x][n + k + 1][z] != SECT_LAVA)
				map_table.type[x][n + k][z] = SECT_MOUNTAIN;
		}
		for (x = n; x < n + k; x++) {
			if (x < 0 || x >= MAX_MAPS)
				continue;

			if (map_table.type[m - 1][x][z] != SECT_LAVA)
				map_table.type[m][x][z] = SECT_MOUNTAIN;
			if (map_table.type[m + k + 1][x][z] != SECT_LAVA)
				map_table.type[m + k][x][z] = SECT_MOUNTAIN;
		}
	}

	/* Generate Borders */
	for (x = 0; x < BORDER_SIZE; x++)
		for (y = 0; y < MAX_MAPS; y++) {
			map_table.type[x][y][z] = sdefault;
			map_table.type[y][x][z] = sdefault;
		}
	for (x = MAX_MAPS - BORDER_SIZE; x < MAX_MAPS; x++)
		for (y = 0; y < MAX_MAPS; y++) {
			map_table.type[x][y][z] = sdefault;
			map_table.type[y][x][z] = sdefault;
		}
	send_to_char(buf, ch);
	return;
}

void init_fields() {
	int fuzz = 3, z = Z_PAINTBALL;
	int i, j, m, n, k, x, y, sdefault = SECT_FOREST;

	for (x = 0; x < MAX_MAPS; x++) {
		for (y = 0; y < MAX_MAPS; y++) {
			if ((y == 199 || y == 301 || x == 199 || x == 301) && x >= 199
					&& x <= 301 && y >= 199 && y <= 301)
				map_table.type[x][y][z] = SECT_NULL;//Paintball field borders.
			else if (y == 0 || x == 0)
				map_table.type[x][y][z] = SECT_NULL;	// General field border
			else
				map_table.type[x][y][z] = sdefault;	//Otherwise, make it a forest

		}
	}
	j = number_range(MAX_MAPS / 40, MAX_MAPS / 20); /* Generate Fields */
	for (i = 0; i < j; i++) {
		m = number_range(20, MAX_MAPS / 10);
		n = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		for (x = n - m; x < n; x++) {
			for (y = k - m; y < k; y++) {
				if (x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS)
					continue;
				if ((x + 4 > n || x - 4 < n - m) && number_range(1, fuzz) == 1)
					continue;
				if (y == number_fuzzy(k))
					continue;
				if (y == number_fuzzy(k - m))
					continue;
				if (map_table.type[x][y][z] == sdefault)
					map_table.type[x][y][z] = SECT_FIELD;
			}
		}
	}
	j = number_range(MAX_MAPS / 40, MAX_MAPS / 20); /* Generate Snow */
	for (i = 0; i < j; i++) {
		m = number_range(20, MAX_MAPS / 10);
		n = number_range(BORDER_SIZE, MAX_MAPS);
		k = number_range(BORDER_SIZE, MAX_MAPS);
		for (x = n - m; x < n; x++) {
			for (y = k - m; y < k; y++) {
				if (x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS)
					continue;
				if ((x + 4 > n || x - 4 < n - m) && number_range(1, fuzz) == 1)
					continue;
				if (y == number_fuzzy(k))
					continue;
				if (y == number_fuzzy(k - m))
					continue;
				if (map_table.type[x][y][z] == sdefault)
					map_table.type[x][y][z] = SECT_SNOW;
			}
		}
	}
	for (x = PIT_BORDER_X; x < MAX_MAPS; x++) {						//Generate the PIT!
		for (y = PIT_BORDER_Y; y < MAX_MAPS; y++) {
			if (x == PIT_BORDER_X || y == PIT_BORDER_Y || y == MAX_MAPS-1 || x == MAX_MAPS-1)
				map_table.type[x][y][Z_PAINTBALL] = SECT_NULL;
			else
				map_table.type[x][y][Z_PAINTBALL] = SECT_BURNED;
		}
	}
	// Medal arena...
	for (x = BORDER_SIZE + 1; x <= MEDAL_BORDER_X; x++)
		for (y = BORDER_SIZE + 1; y <= MEDAL_BORDER_Y; y++) {
			map_table.type[x][y][z] = SECT_BURNED;
			map_table.resource[x][y][z] = -1;
		}

	for (x = BORDER_SIZE; x <= MEDAL_BORDER_X; x++) {
		map_table.type[x][MEDAL_BORDER_Y][z] = SECT_NULL;
		map_table.type[x][BORDER_SIZE][z] = SECT_NULL;
	}
	for (y = BORDER_SIZE; y <= MEDAL_BORDER_Y; y++) {
		map_table.type[MEDAL_BORDER_X][y][z] = SECT_NULL;
		map_table.type[BORDER_SIZE][y][z] = SECT_NULL;
	}

	return;
}

void create_special_map() {
	int x, y, z = Z_PAINTBALL;

	return; // Init fields took this over?
	for (x = BORDER_SIZE + 1; x <= MEDAL_BORDER_X; x++)
		for (y = BORDER_SIZE + 1; y <= MEDAL_BORDER_Y; y++) {
			map_table.type[x][y][z] = SECT_BURNED;
			map_table.resource[x][y][z] = -1;
		}

	for (x = BORDER_SIZE; x < MEDAL_BORDER_X; x++) {
		map_table.type[x][MEDAL_BORDER_Y + 1][z] = SECT_NULL;
		map_table.type[x][BORDER_SIZE][z] = SECT_NULL;
	}
	for (y = BORDER_SIZE; y < MEDAL_BORDER_Y; y++) {
		map_table.type[MEDAL_BORDER_X + 1][y][z] = SECT_NULL;
		map_table.type[BORDER_SIZE][y][z] = SECT_NULL;
	}
	return;
}
