#!/bin/sh

SIZE=2048

genkey()
{
    openssl genrsa $2 -out $1.pem $SIZE
    openssl rsa -in $1.pem -pubout > $1.pub
}

genkey "server"

genkey "client_viralex" "-aes256"
genkey "client_paradox" "-aes256"
genkey "client_gufo" "-aes256"

mv *.pem *.pub keys/
