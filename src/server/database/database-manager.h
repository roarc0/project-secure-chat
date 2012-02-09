#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include  <sqlite3.h>
#include  "../../shared/utility/logger.h"
#include  "../../shared/utility/file.h"

#define db_manager                     database_manager::get_instance()

using namespace std;

class database_manager
{
  private:

    database_manager();

    static database_manager*    db_singleton;
    string              db_filename;
    sqlite3*            handle;
    sqlite3_stmt*       res;

  public:

    ~database_manager();
    static database_manager* get_instance()
    {
        if(!db_singleton)
            db_singleton = new database_manager();
        return db_singleton;
    };

    void set_dbfilename(string filename)
    {
        db_filename = filename;
    }

    void init_db();
};

#endif
