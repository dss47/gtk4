#ifndef DATABASE_H
#define DATABASE_H
#include <sqlite3.h>

sqlite3* db_init(const char* db_path);
int user_register(sqlite3 *db, const char *pseudo, const char *email, const char *password);
int user_login(sqlite3 *db, const char *pseudo, const char *password);
void fetch_home_animes(sqlite3 *db);
void get_episodes_ar(sqlite3 *db, int anime_id);
#endif