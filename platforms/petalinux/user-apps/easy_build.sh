MYPWD=$PWD

cd /opt/petalinux
source ./settings.sh
cd $MYPWD

export PATH=$PETALINUX/software/petalinux-dist/tools/ucfront:$PATH


cd $MYPWD/expat*
./easy_configure.sh && make clean && make && make install

cd $MYPWD/libdaemon*
./easy_configure.sh && make clean && make && make install

cd $MYPWD/dbus*
./easy_configure.sh && make clean && make && make install

cd $MYPWD/glib*
./easy_configure.sh && make clean && make && make install

cd $MYPWD/avahi*
./easy_configure.sh && make clean && make && make install

