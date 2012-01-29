#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <vector>

#include "../typedefs.h"

#define LOG_PTR  logger::get_instance()
#define LOG(_profile, _fname, _fmt, ...) logger::get_instance()->log(_profile, _fname, _fmt, ##__VA_ARGS__)
#define LOG_STATIC(_profile, _fname, _str) logger::get_instance()->log_static(_profile, _fname, _str)
#define INFO(_profile, _fmt, ...) logger::get_instance()->info(_profile, _fmt, ##__VA_ARGS__)

#define BSIZE 4096

using namespace std;

enum logger_mask
{
    L_APPEND      = 0x00001,
    L_DEBUG       = 0x00002,
    L_VERBOSE     = 0x00004,
    L_INCREMENTAL = 0x00008,
    L_COLOR       = 0x00010,
    L_TIMESTAMP   = 0x00020,
    L_CLOSE       = 0x00040,
    L_LOG         = 0X00080
};

struct log_profile
{
    string name, path;
    ofstream ff;
    pthread_mutex_t  l_mutex;

    uint32 l_mask;
    uint32 color, count;

    log_profile(string l_name, string l_path)
    {
        name = l_name;
        path = l_path;
        count = color = 0;
        l_mask = 0;
        pthread_mutex_init(&l_mutex, NULL);
    }

    void set_opt(uint32 l_m)
    {
        l_mask |= l_m;
    }

    bool get_opt(uint32 l_m)
    {
        return l_mask & l_m;
    }

    void lock()
    {
        pthread_mutex_lock(&l_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&l_mutex);
    }
};

class logger
{
    private:

    static logger*          l_singleton;
    pthread_mutex_t  mutex_log;
    vector<log_profile*> vct_profiles;
    uint32 mask;

    logger();
    bool  check_profile(log_profile *l_profile);

    public:

    ~logger();

    static logger* get_instance()
    {
        if(!l_singleton)
            l_singleton = new logger();
        return l_singleton;
    };

    void          add_profile(log_profile *l_profile);
    log_profile*  get_profile(string profile);
    string        get_filename(string profile, string fname);
    void          set_count(string profile, unsigned int val);
    bool          profile_exists(string profile);

    bool   log(string profile, string fname, const char *s_format, ...);
    bool   log_static(string profile, string fname, const char *str);
    bool   info(string profile, const char *s_format, ...);


    inline void set_opt(uint32 msk)
    {
        mask |= msk;
    }

    inline bool get_opt(uint32 msk)
    {
        return mask & msk;
    }

    inline void  getlock()
    {
        pthread_mutex_lock(&mutex_log);
    }

    inline void  releaselock()
    {
        pthread_mutex_unlock(&mutex_log);
    }
};

#endif
