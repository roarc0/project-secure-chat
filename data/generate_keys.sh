#!/bin/sh

SIZE=2048
CIPHER="-aes256"
PASSOUT="-passout pass:password"
PASSIN="-passin pass:password"

genkey()
{
    openssl genrsa $2  $3 -out $1.pem $SIZE
    openssl rsa $4 -in $1.pem -pubout > $1.pub
}

genkey "server"

genkey "client_viralex" $CIPHER "$PASSOUT" "$PASSIN"
genkey "client_paradox" $CIPHER "$PASSOUT" "$PASSIN"
genkey "client_gufo"    $CIPHER "$PASSOUT" "$PASSIN"

mv *.pem *.pub keys/
