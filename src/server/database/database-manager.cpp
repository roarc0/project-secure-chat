#include "database-manager.h"

DatabaseManager::~DatabaseManager()
{
    sqlite3_close(handle);
}

void DatabaseManager::InitDb()
{
    int ret;
    bool new_db = false;

    if (!file_exists(db_filename.c_str()))
    {
        new_db = true;
        INFO("debug", "* creating new db ... \"%s\"\n", db_filename.c_str());
    }
    else
        INFO("debug", "* opening db ... \"%s\"\n", db_filename.c_str());

    ret = sqlite3_open(db_filename.c_str(), &handle);
    if (ret)
    { 
        INFO("debug", "* can't open db ... \"%s\"\n", sqlite3_errmsg(handle));
        sqlite3_close(handle);
        exit(1);
    }
    
    if (new_db)
    {
        sqlite3_exec(handle, "create table user ( \
                          id       INTEGER PRIMARY KEY, \
                          name     TEXT UNIQUE NOT NULL, \
                          pwd      TEXT NOT NULL, \
                          utype    INTEGER, \
                          regdate  DATE \
                          );", 0, 0, NULL);
    }
}

