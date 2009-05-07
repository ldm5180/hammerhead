MYPWD=$PWD

cd /opt/petalinux
source ./settings.sh
cd $MYPWD

chmod 0400 glib-2.18.1/mb.cache avahi-0.6.23/mb.cache  libdaemon-0.13/mb.cache expat-2.0.1/mb.cache dbus-1.2.4/mb.cache

export PATH=$PETALINUX/software/petalinux-dist/tools/ucfront:$PATH

echo ""
echo Building expat...
echo ""
cd $MYPWD/expat*
./easy_configure.sh && make clean && make && make install

echo ""
echo Building libdaemon
echo ""
cd $MYPWD/libdaemon*
./easy_configure.sh && make clean && make && make install


echo ""
echo Building dbus
echo ""
cd $MYPWD/dbus*
./easy_configure.sh && make clean && make && make install


echo ""
echo Building glib
echo ""
cd $MYPWD/glib*
./easy_configure.sh && make clean && make && make install


echo ""
echo Building avahi
echo ""
cd $MYPWD/avahi*
./easy_configure.sh && make clean && make && make install

echo ""
echo Building OpenSSL
echo ""
cd $MYPWD/openssl*
./easy_configure.sh && make clean && make && make install

cd $MYPWD
