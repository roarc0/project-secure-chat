#include "server-core.h"
#include "server-config.h"

#include "crypto/crypto.h"

using namespace std;

int main(int /*argc*/, char** /*argv*/)
{
    welcome();

    init_config("../etc/psc-server.conf");

    cout << "stupido" << endl;
    INFO("debug", "what?\n");
    ByteBuffer in, out;
    
    cout << "stupido" << endl;  
    in << "gufogufogufo";
    INFO("debug", "RSA ENC: %s\n", (char*) in.contents());
    RsaEncrypt("../etc/server.pub", in, out);
    INFO("debug", "RSA DEC:\n");
    out.hexlike();
    

    server_core();

    return 0;
}
