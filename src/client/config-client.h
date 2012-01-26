#ifndef CONFIG_CLIENT_H
#define CONFIG_CLIENT_H

#include <confuse.h>
#include "../shared/common.h"
#include "../shared/logger/logger.h"

using namespace std;

enum config_bool
{
    CONFIG_DEBUG = 0,
    CONFIG_LOG,
    CONFIG_MAX_BOOL
};

enum config_int
{
    CONFIG_PORT = 0,
    CONFIG_MAX_INT
};

enum config_string
{
    CONFIG_CONF_FILENAME = 0,
    CONFIG_LOG_PATH,
    CONFIG_SERVER_HOST,
    CONFIG_NICKNAME,
    CONFIG_MAX_STRING
};

enum config_float
{
    CONFIG_MAX_FLOAT,
};

class config
{
    bool         config_bool[CONFIG_MAX_BOOL];
    int          config_int[CONFIG_MAX_INT];
    std::string  config_string[CONFIG_MAX_STRING];
    float        config_float[CONFIG_MAX_FLOAT];

    cfg_t* open_cfg();

    public: 
    config();
    ~config();

    bool        get_bool_config(enum config_bool);
    int         get_int_config(enum config_int);
    std::string get_string_config(enum config_string);
    float       get_float_config(enum config_float);

    bool load_config();
    void post_init_config();
    void init_log_profiles();
    void help_config();
    void check_config();

    int  load_args(int argc, char **argv);
    void help_args();
};

#endif
