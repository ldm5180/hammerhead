
//
// Copyright (C) 2004-2008, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of GNU General Public License version 2, as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//


#include <stdio.h>


void print_help(void) {
    printf("usage: csa-cp-hab\n\
\n\
    command line arguments are: \n\
\n\
        -?, --help \n\
          prints this help\n\
\n\
        -i ID, --id ID \n\
          set the HAB-ID to ID, ex. \'-i \"briar-patch\"\'\n\
          defaults to hostname if omitted\n\
\n\
        --no-files\n\
          Do not record the raw CSA-CP data to disk.\n\
\n\
        -d DF, --device-file DF\n\
          talk to relay node through devie file DF\n\
          required\n\
\n\
        -c, --clear-files\n\
          clears the files on the device\n\
          (default is off)\n\
\n\
        --debug\n\
          Enable debugging output.\n\
\n");
}

