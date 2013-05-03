#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <sqlite3.h>
#include <string>
#include "utility/logger.h"
#include "utility/file.h"
#include "utility/singleton.h"

using namespace std;

class DatabaseManager
{
    friend class Singleton<DatabaseManager>;
    std::string    filename;
    sqlite3*       handle;
    sqlite3_stmt*  result;

  public:
    DatabaseManager(); 
    DatabaseManager(string);
    ~DatabaseManager();

    void SetFilename(string);

    void OpenDb();
    void CloseDb();
    
    bool CheckUser(const string&);
};

#define db_manager Singleton<DatabaseManager>::GetInstance()

#endif
