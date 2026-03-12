#include "database.h"
#include <stdio.h>

sqlite3* db_init(const char* db_path) {
    sqlite3 *db;
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        fprintf(stderr, "Erreur DB: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    printf("Connecté à Nippon DB avec succès.\n");
    return db;
}

int user_register(sqlite3 *db, const char *pseudo, const char *email, const char *password) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO users (pseudo, email, password_hash) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    sqlite3_bind_text(stmt, 1, pseudo, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, password, -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? 1 : 0;
}