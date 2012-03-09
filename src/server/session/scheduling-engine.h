#ifndef _CELL_MANAGER_H
#define _CELL_MANAGER_H

#include "../../shared/typedefs.h"

#define exec_manager      EsecManager::GetInstance()

class ExecManager : public Singleton
{
    public:

    private:
        typedef UNORDERED_MAP<uint32, Cell*> MapCellType;

        ~CellManager();
        ~CellManager();

        Mutex lock;

};


#endif
