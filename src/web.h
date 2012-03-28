#define WEB_DATA_FILE "../data/web.txt"
#define STAT_WEB_FILE "../../public_html/stat.php"

#define WEB_DATA_KILLED_PIT 1
#define WEB_DATA_KILLS_PIT  2
#define WEB_DATA_HIGHEST_RANK   3
#define WEB_DATA_NEWEST_PLAYER  4
#define WEB_DATA_NUM_PLAYERS    5
#define WEB_DATA_TOT_PLAYERS    6

struct web_data web_data;

struct web_data
{
    char    *last_killed_in_pit;
    char    *last_kills_in_pit;
    char    *highest_ranking_player;
    int highest_rank;
    char    *newest_player;
    int num_players;
    int tot_players;
};
