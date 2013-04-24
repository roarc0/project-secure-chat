#include "config.h"
#include <string.h>

config* config::cfg_singleton = NULL;

config::config()
{
    cfg = NULL;
    opts = NULL;
}

config::~config()
{
    close_cfg();
}

void config::set_cfg_filename(string filename)
{
    cfg_filename = filename;
}

string config::get_cfg_filename()
{
    return cfg_filename;
}

void config::delete_cfg_opt_t()
{
    if(!opts)
        return;

    for (int i = 0; opts[i].type != CFGT_NONE; i++)
    { 
        if(opts[i].type == CFGT_STR)
            delete[] opts[i].def.string;
    }
    
    delete[] opts;
}

cfg_opt_t *config::build_cfg_opt_t()
{
    delete_cfg_opt_t();

    opts = new cfg_opt_t[l_cfgopt_t.size() + 1];
    list<cfg_opt_t>::iterator itr = l_cfgopt_t.begin();
    int i = 0;

    for (; itr != l_cfgopt_t.end(); itr++)
    {
        opts[i] = *(itr);
        i++;
    }

    opts[i] = CFG_END();

    return opts;
}

void config::open_cfg()
{
    if(cfg)
        close_cfg();

    build_cfg_opt_t();

    cfg = cfg_init(opts, CFGF_NONE);

    if (cfg_parse(cfg, cfg_filename.c_str()) == CFG_PARSE_ERROR)
    {
        cfg_free(cfg);
        exit(0);
    }
}

void config::close_cfg()
{
    if(cfg)
        cfg_free(cfg);
    cfg = NULL;
   
    delete_cfg_opt_t();
    opts = NULL;
}

void config::add_bool(const char *id, bool value)
{
    cfg_opt_t cfg_tmp = CFG_BOOL((char*)id, (cfg_bool_t)value, CFGF_NONE);
    l_cfgopt_t.push_back(cfg_tmp);
}

void config::add_int(const char *id, int value)
{
    cfg_opt_t cfg_tmp = CFG_INT((char*)id, value, CFGF_NONE);
    l_cfgopt_t.push_back(cfg_tmp);
}

void config::add_string(const char *id, string value)
{
    char *str = new char[value.length() + 1];
    strcpy(str,value.c_str());
    cfg_opt_t cfg_tmp = CFG_STR((char*)id, str, CFGF_NONE);
    l_cfgopt_t.push_back(cfg_tmp);
}

void config::add_float(const char *id, float value)
{
    cfg_opt_t cfg_tmp = CFG_FLOAT((char*)id, value, CFGF_NONE);
    l_cfgopt_t.push_back(cfg_tmp);
}

bool config::get_bool(const char *id)
{
    return cfg_getbool(cfg, id);
}

int config::get_int(const char *id)
{
    return cfg_getint(cfg, id);
}

string config::get_string(const char *id)
{
    return cfg_getstr(cfg, id);
}

float config::get_float(const char *id)
{
    return cfg_getfloat(cfg, id);
}
