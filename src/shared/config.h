#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <cstdlib>
#include <list>
#include <confuse.h>

#define CFG              config::get_instance()
#define CFG_GET_BOOL(_id)    config::get_instance()->get_bool(_id)
#define CFG_GET_INT(_id)     config::get_instance()->get_int(_id)
#define CFG_GET_STRING(_id)  config::get_instance()->get_string(_id)
#define CFG_GET_FLOAT(_id)   config::get_instance()->get_float(_id)

using namespace std;

class config
{
  public:
                ~config();

    void        open_cfg();
    void        close_cfg();

    void        set_cfg_filename(string);
    string      get_cfg_filename();
    
    void        add_bool(const char *, bool);
    void        add_int(const char *, int);
    void        add_string(const char *, string);
    void        add_float(const char *, float);

    bool        get_bool(const char *);
    int         get_int(const char *);
    string      get_string(const char *);
    float       get_float(const char *);
    
  private:
                config();
    cfg_opt_t*  build_cfg_opt_t();

    static config*    cfg_singleton;
    string            cfg_filename;
    cfg_t*            cfg;
    cfg_opt_t*        opts;
    list<cfg_opt_t>   l_cfgopt_t;

  public:
    static config* get_instance()
    {
        if(!cfg_singleton)
            cfg_singleton = new config();
        return cfg_singleton;
    };
};

#endif
