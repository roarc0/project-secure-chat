#!/bin/sh

SIZE=2048

genkey()
{
    openssl genrsa $2 -out $1.pem $SIZE
    openssl rsa -in $1.pem -outform DER -pubout > $1.pub
}

genkey "server"
genkey "client" "-aes256"
