#include "cell.h"

void Cell::Insert(Session* m_pSes)
{
    //Lock guard(_lock_list);
    c_sessions.insert(m_pSes);
}

void Cell::Remove(Session* m_pSes)
{
    //Lock guard(_lock_list);
    c_sessions.remove(m_pSes);
}

virtual bool Cell::Update()
{
    bool locked;
    TryLock guard(_lock_session, locked);

    if (!locked)
        return false;

    std::list<Session*>::iterator itr = c_sessions.begin();
    for ( ; itr != c_sessions.end() ; itr++)
    {
        CellSessionFilter updater(*itr);
        itr->Update(updater);
    }

    return true; 
}
