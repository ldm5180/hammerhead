#!/bin/sh

aclocal -I build-aux/m4
autoreconf --force --install -I build-aux/m4
