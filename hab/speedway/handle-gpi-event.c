
#include <errno.h>
#include <stdio.h>

#include <glib.h>

#include "ltkc.h"
#include "speedway.h"


void handle_gpi_event(LLRP_tSGPIEvent *pGPIEvent) {
    g_message("gpi event!  GPI %d goes to %d", pGPIEvent->GPIPortNumber, pGPIEvent->GPIEvent);
}

