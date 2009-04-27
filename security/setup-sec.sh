#!/bin/sh

SECDIR=$1
SIGN_SERVER=bioserve.local
MYLOC=`dirname $0`

if [ -z "$SECDIR" ]; then
	echo "Please specify a security directory to create"
	exit 1
fi

mkdir -p $SECDIR
cd $SECDIR || exit 1

openssl req -config $MYLOC/bionet-ssl.conf -batch -newkey rsa:2048 -nodes -out pub.csr

# Send the CRS to the signing server
ssh $SIGN_SERVER "/data/bionet-ca/ca-serve" < pub.csr > pub.cert
rm pub.csr

#get the public certificat from this CA
if [ ! -d ca-dir ]; then 
	mkdir ca-dir
	ssh $SIGN_SERVER "/data/bionet-ca/ca-serve --get-ca" > ca-dir/bionet-ca.pem
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
