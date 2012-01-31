#include <map>
#include "session.h"

// Classe di gestione delle sessioni aperte
class SessionManager
{
    public:    
        void addSession (const UserSession&);
        void deleteSession (uint32 id);
        
        
        UserSession& getNextSessionToServe();
        UserSession& getNextSessionToExecute();
        /*
        
        Funzioni elaborazione pacchetto specifiche sull'utente, tipo cambio livello moderazione ecc..
        
        */
        
    private:
        std::map<uint32, UserSession> sessions;
};
