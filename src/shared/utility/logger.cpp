#include "logger.h"

logger::logger()
{
    pthread_mutex_init(&mutex_log, NULL);
}

logger::~logger()
{
    vector<log_profile*>::iterator itr;

    getlock();
    for (itr = vct_profiles.begin(); itr != vct_profiles.end(); ++itr)
    {
        (*itr)->ff.close();
        delete (*itr);
    }

    vct_profiles.clear();

    releaselock();
}

void logger::add_profile(log_profile *l_profile)
{
    if(!l_profile)
        return;

    getlock();
    vct_profiles.push_back(l_profile);
    releaselock();
}

log_profile* logger::get_profile(string profile)
{
    vector<log_profile*>::iterator itr;

    getlock();
    for (itr = vct_profiles.begin(); itr != vct_profiles.end(); ++itr)
    {
        if ((*itr)->name == profile)
        {
            releaselock();
            return (*itr);
        }
    }

    releaselock();
    return NULL;
}

bool logger::profile_exists(string profile)
{
   return !(get_profile(profile) == 0);
}

string logger::get_filename(string profile, string fname)
{
    string file;
    log_profile *l_profile = get_profile(profile);

    if(!l_profile)
        return "";

    file  = l_profile->path;
    file += "/";
    file += fname;
    if (l_profile->get_opt(L_INCREMENTAL))
    {
        stringstream str;
        
        str << l_profile->count;
        file += str.str();
    }
    file += ".log";
    return file;
}

void logger::set_count(string profile, unsigned int val)
{
    log_profile *l_profile = get_profile(profile);

    if (l_profile && l_profile->get_opt(L_INCREMENTAL))
        l_profile->count = val;
}

bool logger::check_profile(log_profile *l_profile)
{
    if (!l_profile)
        return false;

    if (!get_opt(L_LOG) && l_profile->get_opt(L_LOG))
        return false;

    if (!get_opt(L_DEBUG) && l_profile->get_opt(L_DEBUG))
        return false;

    if (!get_opt(L_VERBOSE) && l_profile->get_opt(L_VERBOSE))
        return false;

    return true;
}

bool logger::log(string profile, string fname, const char *fmt, ...)
{
    static char buffer[BSIZE];
    int ret;

    if(!check_profile(get_profile(profile)))
        return false;

    va_list ap;
    va_start(ap, fmt);
    ret = vsnprintf(buffer, BSIZE, fmt, ap);
    va_end(ap);

    if (ret)
        return log_static(profile, fname, buffer);
    else
    {
        perror("vsnprintf");
        return 0;
    }
}


bool logger::log_static(string profile, string fname, const char *str)
{
    log_profile *l_profile = get_profile(profile);

    if (!check_profile(l_profile))
        return false;
    
    l_profile->lock();
    if (!l_profile->ff.is_open())
    {
        if (l_profile->get_opt(L_APPEND))
            l_profile->ff.open(get_filename(profile, fname).c_str(), ios::out | ios::app);
        else
            l_profile->ff.open(get_filename(profile, fname).c_str(), ios::out);
    }

    if (l_profile->ff.is_open())
    {        
        l_profile->ff << str << endl;
        if (l_profile->get_opt(L_CLOSE))
            l_profile->ff.close();        
    }
    else
    {
        perror("logger");
        l_profile->unlock();
        return true;
    }
    l_profile->unlock();

    return false;
}

bool logger::info(string profile, const char *fmt, ...)
{
    log_profile *l_profile = get_profile(profile);
    static char buffer[BSIZE];
    int ret;

    if(!check_profile(l_profile))
        return false;

    va_list ap;
    va_start(ap, fmt);
    ret = vsnprintf(buffer, BSIZE, fmt, ap);
    va_end(ap);

    if (ret)
    {
        l_profile->lock();
        cout << buffer;
        cout.flush();
        l_profile->unlock();
    }
    else
    {
        perror("vsnprintf");
        return 0;
    }
}
