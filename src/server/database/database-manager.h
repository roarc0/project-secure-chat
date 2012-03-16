#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include  <sqlite3.h>
#include  "../../shared/utility/logger.h"
#include  "../../shared/utility/file.h"
#include  "../../shared/singleton.h"

using namespace std;

class DatabaseManager
{
    friend class Singleton<DatabaseManager>;
  private:

    string              db_filename;
    sqlite3*            handle;
    sqlite3_stmt*       res;

  public:

    ~DatabaseManager();

    void SetDbFilename(string filename)
    {
        db_filename = filename;
    }

    void InitDb();
};

#define db_manager Singleton<DatabaseManager>::GetInstance()

#endif
