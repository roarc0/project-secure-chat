#!/bin/sh

openssl genrsa -out server.pem 1024
openssl rsa -in server.pem -pubout > server.pub

openssl genrsa -out client.pem 1024
openssl rsa -in client.pem -pubout > client.pub
