#include "database-manager.h"

DatabaseManager::DatabaseManager()
{

}

DatabaseManager::DatabaseManager(std::string filename)
{
    this->filename = filename;
    OpenDb();
}

DatabaseManager::~DatabaseManager()
{
    CloseDb();
}

void DatabaseManager::SetFilename(string filename)
{
    this->filename = filename;
}

void DatabaseManager::CloseDb()
{
    sqlite3_close(handle);
}

void DatabaseManager::OpenDb()
{
    int ret;
    bool newdb = false;

    if (!file_exists(filename.c_str()))
    {
        newdb = true;
        INFO("debug", "DATABASE_MANAGER: creating new db ... \"%s\"\n", filename.c_str());
    }
    else
        INFO("debug", "DATABASE_MANAGER: opening db ... \"%s\"\n", filename.c_str());

    ret = sqlite3_open(filename.c_str(), &handle);
    if (ret)
    { 
        INFO("debug", "DATABASE_MANAGER: can't open db ... \"%s\"\n", sqlite3_errmsg(handle));
        sqlite3_close(handle);
        exit(1);
    }
    
    if (newdb)
    {
        sqlite3_exec(handle, "create table user ( \
                          id        INTEGER PRIMARY KEY, \
                          username  TEXT UNIQUE NOT NULL, \
                          password  TEXT NOT NULL, \
                          utype     INTEGER, \
                          regdate   DATE \
                          );", 0, 0, NULL);
    }
}

bool DatabaseManager::CheckUser(const string& username)
{
    bool exists = 0;
    string query = "select * from user where username=\"" + username + "\"";
    sqlite3_prepare_v2(handle, query.c_str(), query.length(), &result, NULL);

    if(sqlite3_step(result) == SQLITE_ROW)
    {
        INFO("debug", "DATABASE_MANAGER: user \"%s\" exist\n", username.c_str());
        exists = true;
    }
    else
        INFO("debug", "DATABASE_MANAGER: user \"%s\" doesn't exist\n", username.c_str());
    sqlite3_finalize(result);
    return exists;
}
