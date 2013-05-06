#!/bin/sh

SIZE=2048

genkey()
{
    openssl genrsa $2 -out $1.pem $SIZE
    openssl rsa -in $1.pem -pubout > $1.pub
}

#-outform DER

genkey "server"
genkey "client" "-aes256"
