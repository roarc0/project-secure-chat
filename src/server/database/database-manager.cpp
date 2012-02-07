#include "database-manager.h"

database_manager* database_manager::db_singleton = NULL;

database_manager::database_manager()
{

}

database_manager::~database_manager()
{
    sqlite3_close(db);
}

void database_manager::init_db()
{
    int ret;
    bool new_db = false;

    if (!file_exists(db_filename.c_str()))
    {
        new_db = true;
        INFO("debug", "\ncreating new db ... %s\n", db_filename.c_str());
    }

    ret = sqlite3_open(db_filename.c_str(), &db);
    if (ret)
    { 
        INFO("debug", "Can't open database_manager: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    if (new_db)
    {
        sqlite3_exec(db, "create table user ( \
                          id       INTEGER PRIMARY KEY, \
                          name     TEXT UNIQUE NOT NULL, \
                          pwd      TEXT NOT NULL, \
                          utype    INTEGER, \
                          regdate  DATE \
                          );", 0, 0, NULL);
    }
}

