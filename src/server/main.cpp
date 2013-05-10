#include "server-core.h"
#include "server-config.h"

#include "crypto/crypto.h"

using namespace std;

int main(int /*argc*/, char** /*argv*/)
{
    welcome();

    init_config("../etc/psc-server.conf");

    string pub_file = CFG_GET_STRING("rsa_prefix") + 
                      CFG_GET_STRING("rsa_my_keys") + ".pub";
    string pem_file = CFG_GET_STRING("rsa_prefix") + 
                      CFG_GET_STRING("rsa_my_keys") + ".pem";
    
    INFO("debug", "MAIN: TESTING RSA KEYS\n");
    
    if(RsaTest(pem_file.c_str(), pub_file.c_str(), NULL))
        INFO("debug", "MAIN: RSA TEST SUCCEEDED\n\n");
    else
        INFO("debug", "MAIN: RSA TEST FAILED\n\n");
    
    server_core();

    return 0;
}
