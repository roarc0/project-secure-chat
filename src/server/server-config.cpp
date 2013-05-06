#include "server-config.h"

void init_config(string filename)
{
    if (!file_exists(filename.c_str()))
        cout << "config file <" << filename << "> does not exists!" << endl;

    CFG->set_cfg_filename(filename);
    CFG->add_bool("debug", false);
    CFG->add_bool("log", false);
    CFG->add_string("log_path", "logs");
    CFG->add_int("server_port", 7777);
    CFG->add_string("db_filename", "../etc/database.sqlite");
    CFG->add_string("rsa_key_prefix", "../etc/keys/server");
    
    CFG->add_int("ThreadNet", 2);
    CFG->add_int("ThreadExec", 2);

    CFG->add_int("SessionActiveLimit", 100);
    CFG->add_int("SessionQueueLimit", 50);

    CFG->open_cfg();
    post_init_config();
}

void post_init_config()
{
    if (!dir_exists(CFG_GET_STRING("log_path")))
        mkdir(CFG_GET_STRING("log_path").c_str(), 0777);
    else
    {
        stringstream str;
        str << "rm -f " << CFG_GET_STRING("log_path") << "/*.log";
        int ret = system(str.str().c_str());
    }

    check_config();
    init_log_profiles();
}

void init_log_profiles()
{
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

int load_args(int argc, char **argv)
{
    int opt/*, tmp_i*/;
    opterr = 0;

    while ((opt = getopt (argc, argv, "c:hd")) != -1)
        switch (opt)
        {
            case 'h':
                help_args();
                exit(0);
            break;
            case 'd':
                // debug true
            break;
            case 'c':
                // new config file = optarg;
                if (!file_exists(optarg))
                {
                    cout << "config file " << optarg << " does not exists!" << endl;
                    exit(1);
                }
                cout << "* loading config from " << optarg << endl; //TODO INFO
                init_config(optarg);
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

void help_args()
{
    cout << "help: " << endl;
    cout << "-h                  :  help" << endl;
    cout << "-d                  :  debug mode on" << endl;
    cout << "-c <filename>       :  alternative config filename" << endl;
    cout << endl;
}

void check_config() // TODO inserire controlli
{
    /*if (CFG_GET_INT("thread_slots") <= 0)
    {
        //set_thread_slot_to -->>> = sysconf( _SC_NPROCESSORS_ONLN ) + 1;
    }*/

}
