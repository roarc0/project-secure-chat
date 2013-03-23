#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "chat_handler.h"

ChatCommand* ChatHandler::getCommandTable()
{ 
    static ChatCommand channelCommandTable[] =
    {
        { "join",        SEC_USER,            &ChatHandler::HandleJoinChannelCommand,          "", NULL },
        { "create",      SEC_USER,            &ChatHandler::HandleCreateChannelCommand,        "", NULL },
        { "leave",       SEC_USER,            &ChatHandler::HandleLeaveChannelCommand,         "", NULL },
        { "kick",        SEC_MODERATOR,       &ChatHandler::HandleKickCommand,                 "", NULL },
        { NULL,          0,                   NULL,                                            "", NULL }
    };
    
    static ChatCommand utilityCommandTable[] =
    {
        
        { NULL,          0,                   NULL,                                            "", NULL }
    };

    static ChatCommand commandTable[] =
    {
        { "channel",      SEC_USER,     NULL,                 "", channelCommandTable     },
        { "utility",      SEC_USER,     NULL,                 "", utilityCommandTable     },
        { "ping",         SEC_USER,     &ChatHandler::HandlePingCommand,         "", NULL },
        { NULL,           0,            NULL,                 "", NULL                    }
    };

    return commandTable;
};

bool ChatHandler::isAvailable(ChatCommand const& cmd) const
{
    // controlla il security level della sessione
    //return m_session->GetSecurity() >= AccountTypes(cmd.SecurityLevel);
    return true;
}

bool ChatHandler::hasStringAbbr(const char* name, const char* part)
{
    // non "" command
    if (*name)
    {
        // "" part from non-"" command
        if (!*part)
            return false;
        for (;;)
        {
            if (!*part)
                return true;
            else if (!*name)
                return false;
            else if (tolower(*name) != tolower(*part))
                return false;
            ++name; ++part;
        }
    }
    // allow with any for ""

    return true;
}

int ChatHandler::ParseCommands(const char* text)
{
    assert(text);
    assert(*text);

    std::string fullcmd = text;

    /// chat case (/command or !command format)
    if (m_session)
    {
        if (text[0] != '\\')
            return 0;
    }

    /// ignore single \ and ! in line
    if (strlen(text) < 2)
        return 0;
    // original `text` can't be used. It content destroyed in command code processing.

    /// ignore messages staring from many dots.
    if ((text[0] == '\\' && text[1] == '\\'))
        return 0;

    /// skip first \ or ! (in console allowed use command with \ and ! and without its)
    if (text[0] == '\\')
        ++text;

    if (!ExecuteCommandInTable(getCommandTable(), text, fullcmd))
    {
        SendSysMessage("There is no such command");
    }
    return 1;
}

bool ChatHandler::ExecuteCommandInTable(ChatCommand* table, const char* text, const std::string& fullcmd)
{
    char const* oldtext = text;
    std::string cmd = "";

    while (*text != ' ' && *text != '\0')
    {
        cmd += *text;
        ++text;
    }
    
    while (*text == ' ') ++text;
    for (uint32 i = 0; table[i].Name != NULL; ++i)
    {
        if (!hasStringAbbr(table[i].Name, cmd.c_str()))
            continue;
        bool match = false;
        if (strlen(table[i].Name) > cmd.length())
        {
            for (uint32 j = 0; table[j].Name != NULL; ++j)
            {
                if (!hasStringAbbr(table[j].Name, cmd.c_str()))
                    continue;

                if (strcmp(table[j].Name, cmd.c_str()) != 0)
                    continue;
                else
                {
                    match = true;
                    break;
                }
            }
        }
        if (match)
            continue;
        // select subcommand from child commands list
        if (table[i].ChildCommands != NULL)
        {
            if (!ExecuteCommandInTable(table[i].ChildCommands, text, fullcmd))
            {
                if (text && text[0] != '\0')
                {
                    // Mandare risposta al client
                    SendSysMessage("There is no such subcommand.");
                }
                else
                {
                    // Mandare risposta al client
                    SendSysMessage("Incorrect syntax.");
                }

                ShowHelpForCommand(table[i].ChildCommands, text);
            }

            return true;
        }

        // must be available and have handler
        if (!table[i].Handler || !isAvailable(table[i]))
            continue;

        SetSentErrorMessage(false);
        // table[i].Name == "" is special case: send original command to handler
        if ((this->*(table[i].Handler))(table[i].Name[0] != '\0' ? text : oldtext))
        {
            // Comando eseguito
        }
        // I comandi possono aver mandato loro errori interni
        else if(!HasSentErrorMessage()) 
        {
            if (!table[i].Help.empty())
            {
                // Mandare messagio di errore
                SendSysMessage(table[i].Help.c_str());
            }
            else
            {
                SendSysMessage("Incorrect syntax.");
            }
        }

        return true;
    }

    return false;
}

bool ChatHandler::ShowHelpForCommand(ChatCommand* table, const char* cmd)
{
    if (*cmd)
    {
        for (uint32 i = 0; table[i].Name != NULL; ++i)
        {
            // must be available (ignore handler existence for show command with possibe avalable subcomands
            if (!isAvailable(table[i]))
                continue;

            if (!hasStringAbbr(table[i].Name, cmd))
                continue;

            // have subcommand
            char const* subcmd = (*cmd) ? strtok(NULL, " ") : "";

            if (table[i].ChildCommands && subcmd && *subcmd)
            {
                if (ShowHelpForCommand(table[i].ChildCommands, subcmd))
                    return true;
            }

            if (!table[i].Help.empty())
                SendSysMessage(table[i].Help.c_str());

            if (table[i].ChildCommands)
                if (ShowHelpForSubCommands(table[i].ChildCommands, table[i].Name, subcmd ? subcmd : ""))
                    return true;

            return !table[i].Help.empty();
        }
    }
    else
    {
        for (uint32 i = 0; table[i].Name != NULL; ++i)
        {
            // must be available (ignore handler existence for show command with possibe avalable subcomands
            if (!isAvailable(table[i]))
                continue;

            if (strlen(table[i].Name))
                continue;

            if (!table[i].Help.empty())
                SendSysMessage(table[i].Help.c_str());

            if (table[i].ChildCommands)
                if (ShowHelpForSubCommands(table[i].ChildCommands, "", ""))
                    return true;

            return !table[i].Help.empty();
        }
    }

    return ShowHelpForSubCommands(table, "", cmd);
}

bool ChatHandler::ShowHelpForSubCommands(ChatCommand* table, char const* cmd, char const* subcmd)
{
    std::string list;
    for (uint32 i = 0; table[i].Name != NULL; ++i)
    {
        // must be available (ignore handler existence for show command with possibe avalable subcomands
        if (!isAvailable(table[i]))
            continue;

        // for empty subcmd show all available
        if (*subcmd && !hasStringAbbr(table[i].Name, subcmd))
            continue;

        if (m_session)
            list += "\n    ";
        else
            list += "\n\r    ";

        list += table[i].Name;

        if (table[i].ChildCommands)
            list += " ...";
    }

    if (list.empty())
        return false;

    if (table == getCommandTable())
    {
        SendSysMessage("Commands available to you:");
        PSendSysMessage("%s", list.c_str());
    }
    else
        PSendSysMessage("Command %s have subcommands:%s", cmd, list.c_str());

    return true;
}

void ChatHandler::SendSysMessage(const char *str)
{
    INFO ("debug", "%s \n", str);
    // TODO inseririre eventuali header appostiti
    Packet data;    
    data << str;
    m_session->SendPacket(&data);
}

void ChatHandler::PSendSysMessage(const char *format, ...)
{
    va_list ap;
    char str [2048];
    va_start(ap, format);
    vsnprintf(str, 2048, format, ap);
    va_end(ap);
    SendSysMessage(str);
}

bool ChatHandler::HandleJoinChannelCommand(const char *args) 
{ 
    return true; 
}

bool ChatHandler::HandleCreateChannelCommand(const char *args)
{ 
    return true; 
}

bool ChatHandler::HandleLeaveChannelCommand(const char *args)
{ 
    return true; 
}

bool ChatHandler::HandleKickCommand(const char *args)
{ 
    return true; 
}

bool ChatHandler::HandlePingCommand(const char *args)
{ 
    // TODO inseririre eventuali header appostiti
    Packet data;    
    data << "Pong";
    m_session->SendPacket(&data);
    return true; 
}
