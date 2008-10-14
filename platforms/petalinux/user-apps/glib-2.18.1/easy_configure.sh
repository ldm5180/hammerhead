mkdir -p $PETALINUX/software/petalinux-dist/glib/include/glib-2.0
cp glibconfig.h /opt/petalinux/software/petalinux-dist/glib/include/glib-2.0/.
./configure -C --cache-file=mb.cache --enable-static --disable-shared --disable-xattr --disable-selinux --prefix=$PETALINUX/software/petalinux-dist/glib --build=i386 --host=none-uclinux-uclibc CC='ucfront microblaze-uclinux-gcc' LDFLAGS='-Wl, -elf2flt'

