export CC=$PWD/ucfront.pl
export LDFLAGS='Wl, -elf2flt'
 ./Configure no-shared no-dso no-krb5 --prefix=$PETALINUX/software/petalinux-dist/openssl 386 gcc 