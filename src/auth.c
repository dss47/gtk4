#include <sqlite3.h>
#include <string.h>

int login_user(sqlite3 *db, const char *pseudo, const char *password) {
    // Requête : SELECT id FROM users WHERE pseudo = ? AND password_hash = ?
    // Retourne l'ID de l'utilisateur si succès, 0 sinon.
    return 0; 
}

void register_user(sqlite3 *db, const char *pseudo, const char *email, const char *pass) {
    // Requête : INSERT INTO users (pseudo, email, password_hash) VALUES (?, ?, ?)
}