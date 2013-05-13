#include "server-config.h"

void HelpArgs()
{
    cout << "help: " << endl;
    cout << "-h                  :  help" << endl;
    cout << "-d                  :  debug mode on" << endl;
    cout << "-c <filename>       :  alternative config filename" << endl;
    cout << endl;
}

void CheckConfig()
{

}

void InitConfig(string filename)
{
    if (!file_exists(filename.c_str()))
        cout << "config file <" << filename << "> does not exists!" << endl;

    CFG->set_cfg_filename(filename);
    CFG->add_bool("debug", false);
    CFG->add_bool("log", false);
    CFG->add_string("log_path", "logs");
    CFG->add_int("server_port", 7777);
    CFG->add_int("key_refresh_interval", 900);
    CFG->add_string("db_filename", "../etc/database.sqlite");
    
    CFG->add_string("rsa_prefix", "../etc/keys/");
    CFG->add_string("rsa_my_keys", "server");
    CFG->add_string("rsa_client_pub_key", "client_");

    CFG->add_int("ThreadNet", 2);
    CFG->add_int("ThreadExec", 2);

    CFG->add_int("SessionActiveLimit", 100);
    CFG->add_int("SessionQueueLimit", 50);

    CFG->open_cfg();

    CheckConfig();
}

void InitLog()
{
    if (!dir_exists(CFG_GET_STRING("log_path")))
        mkdir(CFG_GET_STRING("log_path").c_str(), 0777);
    else
    {
        stringstream str;
        str << "rm -f " << CFG_GET_STRING("log_path") << "/*.log";
        int ret = system(str.str().c_str());
    }

    log_profile *l_profile;

    if (CFG_GET_BOOL("debug"))
    {
        l_profile = new log_profile("debug", "");
        l_profile->set_opt(LOG_NONE);
        LOG_PTR->add_profile(l_profile);
    }

    if (CFG_GET_BOOL("log"))
    {
        l_profile = new log_profile("log", "");
        l_profile->set_opt(LOG_APPEND);
        LOG_PTR->add_profile(l_profile);
    }
}

void Init(int argc, char **argv)
{
    int opt;
    bool init_config_done = false;
    opterr = 0;

    while ((opt = getopt (argc, argv, "c:hd")) != -1)
        switch (opt)
        {
            case 'h':
                HelpArgs();
                exit(0);
            break;
            case 'd':
                // debug true
            break;
            case 'c':
                if (!file_exists(optarg))
                {
                    cout << "CONFIG: config file " << optarg << " does not exists!" << endl;
                    break;
                }
                cout << "CONFIG: loading config from " << optarg << endl; //TODO INFO
                InitConfig(optarg);
                init_config_done = true;
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

    if(!init_config_done)
    {
        InitConfig(DEFAULT_CONFIG);
    }

    InitLog();
}
