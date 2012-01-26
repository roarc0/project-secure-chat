#include "config-server.h"

cfg_opt_t opts[] =
{   
    CFG_BOOL((char*)"debug", (cfg_bool_t)false, CFGF_NONE),
    CFG_STR((char*)"conf_filename",(char*)"logs",CFGF_NONE),
    CFG_BOOL((char*)"log_enable",(cfg_bool_t)true, CFGF_NONE),
    CFG_STR((char*)"log_path",(char*)"logs",CFGF_NONE),
    CFG_INT((char*)"thread_slots",  4, CFGF_NONE),   

    CFG_END()
};

cfg_t* config::open_cfg()
{
    cfg_t *cfg = cfg_init(opts, CFGF_NONE);

    if (cfg_parse(cfg, config_string[CONFIG_CONF_FILENAME].c_str()) == CFG_PARSE_ERROR)
    {
        cfg_free(cfg);
        exit(0);
    }

    return cfg;
}

config::config()
{
    config_string[CONFIG_CONF_FILENAME] = "pschat-server.conf";
    load_config();
}

bool config::load_config()
{
    if (!file_exists(config_string[CONFIG_CONF_FILENAME].c_str()))
        return false;

    cfg_t *cfg = open_cfg();

    config_bool[CONFIG_LOG]        = cfg_getbool(cfg, "log_enable");
    config_string[CONFIG_LOG_PATH] = cfg_getstr(cfg, "conf_filename");
    config_string[CONFIG_LOG_PATH] = cfg_getstr(cfg, "log_path");
    config_bool[CONFIG_DEBUG]      = cfg_getbool(cfg, "debug");
    config_int[CONFIG_THREAD_SLOTS]     = cfg_getint(cfg, "thread_slots");

    cfg_free(cfg);

    return true;
}

void config::post_init_config()
{
    if (!dir_exists(config_string[CONFIG_LOG_PATH]))
        mkdir(config_string[CONFIG_LOG_PATH].c_str(), 0777);
    else
    {
        stringstream str;
        str << "rm -f " << config_string[CONFIG_LOG_PATH] << "/*.log";
        int ret = system(str.str().c_str()); // FIXME check ret
    }

    check_config();
    init_log_profiles();
}

void config::init_log_profiles()
{
    log_profile *l_profile;

    if (config_bool[CONFIG_DEBUG])  // deprecated
        LOG_PTR->set_opt(L_DEBUG);

    if (config_bool[CONFIG_LOG]) // deprecated
        LOG_PTR->set_opt(L_LOG);
    else 
        return;

    if (config_bool[CONFIG_DEBUG])
    {
        l_profile = new log_profile("debug", "");
        l_profile->set_opt(L_VERBOSE | L_DEBUG | L_COLOR);
        LOG_PTR->add_profile(l_profile);
    }
}

int config::load_args(int argc, char **argv)
{
    int opt, tmp_i;
    opterr = 0;

    while ((opt = getopt (argc, argv, "c:hd")) != -1)
        switch (opt)
        {
            case 'h':
                help_args();
                exit(0);
            break;
            case 'd':
                config_bool[CONFIG_DEBUG] = true;
            break;
            case 'c':
                config_string[CONFIG_CONF_FILENAME] = optarg;
                if (!file_exists(optarg))
                {
                    cout << "config file " << optarg << " does not exists!" << endl;
                    exit(1);
                }
                cout << "* loading config from " << optarg << endl;
                load_config();
            break;
            case '?':
                if (optopt == 'c')
                fprintf (stderr, "option -%c requires an argument.\n\n", optopt);
                else
                fprintf (stderr,
                        "unknown option character `\\x%x'.\n\n",
                        optopt);
                break;
            default:
                break;
        }    

    for (int index = optind; index < argc; index++)
        cout << "non-option argument " << argv[index] << endl;

    post_init_config();

    return 0;
}

void config::help_args()
{
    cout << "help: " << endl;
    cout << "-h                  :  help" << endl;
    cout << "-d                  :  debug mode on" << endl;
    cout << "-c <filename>       :  alternative config filename" << endl;
    cout << endl;
}

void config::check_config() // TODO inserire altri controlli
{
    if (config_int[CONFIG_THREAD_SLOTS] <= 0)
    {
        config_int[CONFIG_THREAD_SLOTS] = sysconf( _SC_NPROCESSORS_ONLN ) + 1;
    }

}

bool config::get_bool_config(enum config_bool e_conf)
{
    if (e_conf >= CONFIG_MAX_BOOL)
        exit(1); // Errore

    return config_bool[e_conf];
}

int config::get_int_config(enum config_int e_conf)
{
    if (e_conf >= CONFIG_MAX_INT)
        exit(1); // Errore

    return config_int[e_conf];
}

std::string config::get_string_config(enum config_string e_conf)
{
    if (e_conf >= CONFIG_MAX_STRING)
        exit(1); // Errore
    
    return config_string[e_conf];
}

float config::get_float_config(enum config_float e_conf)
{
    if (e_conf >= CONFIG_MAX_FLOAT)
        exit(1); // Errore

    return config_float[e_conf];
}
