#include "server-core.h"
#include "server-config.h"

#include "crypto/crypto.h"

using namespace std;

int main(int /*argc*/, char** /*argv*/)
{
    welcome();

    init_config("../etc/psc-server.conf");

    string pub_key =  CFG_GET_STRING("rsa_key_prefix") + ".pub";
    string priv_key = CFG_GET_STRING("rsa_key_prefix") + ".pem";
    INFO("debug", "MAIN: TESTING RSA KEYS\n");
    ByteBuffer in, out, out2;
    in << "rsatest";
    RsaEncrypt(pub_key, in, out);
    //out.hexlike();
    in.clear();
    RsaDecrypt(priv_key, out, out2);
    //out2.hexlike(); 
    
    if(in.compare(out2))
        INFO("debug", "MAIN: RSA TEST FAILED\n\n");
    else
        INFO("debug", "MAIN: RSA TEST SUCCESS\n\n");
    
    server_core();

    return 0;
}
