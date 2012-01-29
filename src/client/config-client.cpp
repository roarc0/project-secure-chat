#include "config-client.h"

void init_config(string filename)
{
    //if (!file_exists(filename.c_str()))
    //    return false;

    CFG->add_bool("debug", false);
    CFG->add_bool("log", false);
    CFG->add_string("log_path", "logs");
    CFG->add_string("nickname", "user");
    CFG->add_string("server_host", "127.0.0.1");
    CFG->add_string("server_port", "7777");

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

    if (CFG_GET_BOOL("debug"))  // deprecated
        LOG_PTR->set_opt(L_DEBUG);

    if (CFG_GET_BOOL("log")) // deprecated
        LOG_PTR->set_opt(L_LOG);
    else 
        return;

    if (CFG_GET_BOOL("debug"))
    {
        l_profile = new log_profile("debug", "");
        l_profile->set_opt(L_VERBOSE | L_DEBUG | L_COLOR);
        LOG_PTR->add_profile(l_profile);
    }
}


int load_args(int argc, char **argv)
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

void check_config() // TODO inserire i controlli
{

}
