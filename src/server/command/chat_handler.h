#ifndef CHAT_HANDLER_H
#define CHAT_HANDLER_H

#include "common.h"
#include "session.h"

#if COMPILER == COMPILER_GNU
#  define ATTR_PRINTF(F, V) __attribute__ ((format (printf, F, V)))
#else //COMPILER != COMPILER_GNU
#  define ATTR_PRINTF(F, V)
#endif //COMPILER == COMPILER_GNU

enum AccountTypes
{
    SEC_USER           = 0,
    SEC_MODERATOR      = 1,
    SEC_ADMINISTRATOR  = 2
};

class ChatHandler;

class ChatCommand
{
    public:
        const char *       Name;
        uint32             SecurityLevel;
        bool (ChatHandler::*Handler)(const char* args);
        std::string        Help;
        ChatCommand*       ChildCommands;
};

class ChatHandler
{
        Session* m_session;
        bool sentErrorMessage;
    public:
        Session* GetSession() { return m_session; }
        explicit ChatHandler(Session* session) : m_session(session) {}
        ~ChatHandler() {}
        
        // Funzione chiamata per vedere se c'è un comando, e se c'è lo esegue
        int ParseCommands(const char* text);
        
    private:
        bool isAvailable(ChatCommand const& cmd) const;
        bool hasStringAbbr(const char* name, const char* part);
        bool ExecuteCommandInTable(ChatCommand* table, const char* text, const std::string& fullcmd);
        
        bool ShowHelpForCommand(ChatCommand* table, const char* cmd);
        bool ShowHelpForSubCommands(ChatCommand* table, char const* cmd, char const* subcmd);
        
        bool HasSentErrorMessage() const { return sentErrorMessage;}
        void SetSentErrorMessage(bool val){ sentErrorMessage = val;};
        
        void SendSysMessage(const char *str);
        void SendSysMessage(uint32 entry);
        void PSendSysMessage(const char *format, ...) ATTR_PRINTF(2, 3);
        void PSendSysMessage(uint32 entry, ...);

        static ChatCommand* getCommandTable();
        
        // Se ritornano false appare il messaggio di incorretta sintassi o di Help se presente
        bool HandleCommandsCommand(const char* args);
        bool HandleHelpCommand(const char* args);
        bool HandleJoinChannelCommand(const char *args);
        bool HandleCreateChannelCommand(const char *args);
        bool HandleLeaveChannelCommand(const char *args);
        bool HandleKickCommand(const char *args);
        bool HandlePingCommand(const char *args);
 
 };
 
 #endif // CHAT_HANDLER_H
