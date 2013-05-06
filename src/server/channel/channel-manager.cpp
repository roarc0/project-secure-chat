#include "channel-manager.h"
#include "crypto/crypto.h"

ChannelManager::ChannelManager()
{    
    i_timer.SetInterval(50); //TODO dal config
}

ChannelManager::~ChannelManager()
{
    if (m_updater.IsActivate())
        m_updater.Wait();

    while (!m_channels.empty())
    {
        // delete m_channels.begin()->second; Smart
        m_channels.erase(m_channels.begin());
    }
}

SmartChannel ChannelManager::CreateChannel(std::string& c_name, std::string& c_pass)
{
    Lock guard(m_mutex);

    if (c_name == "Default")
        return SmartChannel(NULL);    

    SmartChannel cha = FindChannel(c_name);
    if (cha.get())
        return SmartChannel(NULL);

    cha = SmartChannel(new Channel(c_name, c_pass));
    m_channels[c_name] = cha;
    return cha;        
}

SmartChannel ChannelManager::FindChannel(const std::string& c_name)
{
    if (c_name == "")
        return SmartChannel(NULL);
    mapChannel::const_iterator iter = m_channels.find(c_name);
    return (iter == m_channels.end() ? SmartChannel(NULL) : iter->second);
}

void ChannelManager::getChannelList(std::string& info)
{
    std::string c_pass = "";
    std::string c_pass_sha256;
    SHA256_digest (c_pass.c_str(), c_pass.length(), c_pass_sha256);
    Session_smart ses(NULL);

    stringstream ss;
    ss << "Channel list:" << endl;

    mapChannel::iterator iter = m_channels.begin();
    for (; iter != m_channels.end(); ++iter)
    {
        ss << " " << iter->second->GetName() << " (" << iter->second->getSessionNumer() << ") ";
        if (!(iter->second->CanEnterSession(ses, c_pass_sha256)))
        {
          ss << "*";  
        }
        ss << endl;
    }
    info += ss.str();
}

int ChannelManager::RemoveChannel(std::string& c_name)
{
    Lock guard(m_mutex);

    mapChannel::iterator iter = m_channels.find(c_name);
    if (iter != m_channels.end())
    {
        // delete (iter->second); Smart
        m_channels.erase(iter);
    }

    return 0;
}

void ChannelManager::Update(uint32 diff)
{
    i_timer.Update(diff);
    if (!i_timer.Passed())
        return;
    
    // Gestione della parte ThreadSafe dei Canali
    mapChannel::iterator iter = m_channels.begin();
    for (; iter != m_channels.end(); ++iter)
    {
        if (m_updater.IsActivate())
            m_updater.ScheduleUpdate(*iter->second, uint32(i_timer.GetCurrent()));
        else
            iter->second->Update(uint32(i_timer.GetCurrent()));
    }
    if (m_updater.IsActivate())
        m_updater.Wait();

    // Gestione della parte ThreadUnsafe dei Canali
    for (iter = m_channels.begin(); iter != m_channels.end(); ++iter)
        if (!iter->second->DelayedUpdate(uint32(i_timer.GetCurrent())))
        {
            INFO ("debug", "CHANNEL-MANAGER: Removing channel\n");
            //delete iter->second; Smart
            m_channels.erase(iter);
        }

    i_timer.SetCurrent(0);
}

void ChannelManager::JoinDefaultChannel(Session_smart ses)
{
    std::string c_name = "Default";    
    SmartChannel cha = FindChannel(c_name);
    if (!cha.get())
    {
        std::string c_pass = "";
        std::string c_pass_sha256;
        SHA256_digest (c_pass.c_str(), c_pass.length(), c_pass_sha256);
        cha = SmartChannel(new Channel(c_name, c_pass_sha256));
        m_channels[c_name] = cha;
    }
    cha->AddSession(ses);
    ses->setChannel(cha);
    ses->SendSysMessage("Default channel joined");
}
