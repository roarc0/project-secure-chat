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

using namespace std;

enum logger_mask
{
    LOG_NONE        = 0X00001,
    LOG_APPEND      = 0x00002,
    LOG_INCREMENTAL = 0x00004,
    LOG_COLOR       = 0x00008,
    LOG_TIMESTAMP   = 0x00010,
    LOG_CLOSE       = 0x00020,

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

    static logger*          log_singleton;
    pthread_mutex_t  mutex_log;
    vector<log_profile*> vct_profiles;
    uint32 mask;

    logger();

    public:

    ~logger();

    static logger* get_instance()
    {
        if(!log_singleton)
            log_singleton = new logger();
        return log_singleton;
    };

    void          add_profile(log_profile *l_profile);
    log_profile*  get_profile(string profile);
    string        get_filename(string profile, string fname);
    void          set_count(string profile, unsigned int val);
    bool          profile_exists(string profile);

    bool   log(string profile, string fname, const char *s_format, ...);
    bool   log_static(string profile, string fname, const char *str);
    bool   info(string profile, const char *s_format, ...);

    inline void  getlock()
    {
        pthread_mutex_lock(&mutex_log);
    }

    inline void  releaselock()
    {
        pthread_mutex_unlock(&mutex_log);
    }

    /*template <typename T>
    logger& operator <<(logger& log, T const& value)
    {
        variabilestringstram << value;
        return log;
    }*/
};

#endif
