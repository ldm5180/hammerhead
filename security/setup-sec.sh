#!/bin/bash

SECDIR=$2
SIGN_SERVER=$1

print_usage() {
    me=`basename $0`
    echo "$me creates a security dir for any bionet application"
    echo ""
    echo "usage: $me <signing-server> <dir-to-create> [ca-server-dir]"
    echo ""
    echo "    signing-server    the hostname/address of a server setup with"
    echo "                      the ca-serve server and a certificate authority "
    echo "		      certificate"
    echo ""
    echo "    dir-to-create     The directory to create"
    echo ""
    echo "    ca-server-dir     The directory on the signing-server to use."
    echo "                      Default: /data/bionet-ca"
    echo
}


if [[ "$0" =~ ^/ ]]; then
    MYLOC=`dirname $0`
else
    MYLOC=`dirname $PWD/$0`
fi


if [ -z "$SECDIR" ]; then
    print_usage
    echo "Please specify a security directory to create"
    exit 1
fi
if [ -z "$SIGN_SERVER" ]; then
    print_usage
    echo "Please specify a signing server to use"
    exit 1
fi

CASRVDIR=$3
if [ -z "$CASRVDIR" ]; then
    CASRVDIR='/data/bionet-ca'
fi

mkdir -p $SECDIR
cd $SECDIR || exit 1

openssl req -config $MYLOC/bionet-ssl.conf -batch -newkey rsa:2048 -nodes -out pub.csr

# Send the CRS to the signing server
ssh $SIGN_SERVER "$CASRVDIR/ca-serve" < pub.csr > pub.cert
rm pub.csr

#get the public certificat from this CA
if [ ! -d ca-dir ]; then 
	mkdir ca-dir
	ssh $SIGN_SERVER "$CASRVDIR/ca-serve --get-ca" > ca-dir/bionet-ca.pem
	c_rehash ca-dir
fi;

#verify the cert returned with our CA list
if ! openssl verify -CApath ca-dir pub.cert | grep -q '^error '; then
	echo "Verify OK!"
else
	# Verify failed; re-generate error
	openssl verify -CApath ca-dir pub.cert
	rm pub.cert
fi
