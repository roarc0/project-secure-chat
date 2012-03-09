#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include  <sqlite3.h>
#include  "../../shared/utility/logger.h"
#include  "../../shared/utility/file.h"
#include  "../../shared/singleton.h"

#define db_manager    DatabaseManager::GetInstance()

using namespace std;

class DatabaseManager : public Singleton
{
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

#endif
