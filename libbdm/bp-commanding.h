
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

// This library is free software. You can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as 
// published by the Free Software Foundation, version 2.1 of the License.
// This library is distributed in the hope that it will be useful, but 
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details. A copy of the GNU 
// Lesser General Public License v 2.1 can be found in the file named 
// "COPYING.LESSER".  You should have received a copy of the GNU Lesser 
// General Public License along with this library; if not, write to the 
// Free Software Foundation, Inc., 
// 51 Franklin Street, Fifth Floor, 
// Boston, MA 02110-1301 USA.
 
// You may contact the Automation Group at:
// bionet@bioserve.colorado.edu
 
// Dr. Kevin Gifford
// University of Colorado
// Engineering Center, ECAE 1B08
// Boulder, CO 80309
 
// Because BioNet was developed at a university, we ask that you provide
// attribution to the BioNet authors in any redistribution, modification, 
// work, or article based on this library.
 
// You may contribute modifications or suggestions to the University of
// Colorado for the purpose of discussing and improving this software.
// Before your modifications are incorporated into the master version 
// distributed by the University of Colorado, we must have a contributor
// license agreement on file from each contributor. If you wish to supply
// the University with your modifications, please join our mailing list.
// Instructions can be found on our website at 
// http://bioserve.colorado.edu/developers-corner.

#ifndef __BP_COMMANDING_H
#define __BP_COMMANDING_H


#include <stdint.h>

#include <glib.h>

#include "bionet-util.h"
#include "bionet-bdm.h"


/**
 * @file bp-commanding.h
 * Describes the API for sending bionet commands over BP.
 */


/**
 * @brief Connects to the Bundle Agent and does other setup needed to pass bundles.
 *
 * @param[in] source_eid The Source Endpoint Identifier to send future bundles from.
 *
 * @retval 0 on success
 * @retval -1 on failure
 */
int bionet_bp_start(char *source_eid);


/**
 * @brief Disconnects from the Bundle Agent and frees memory used for the Bundle Protocol.
 */
void bionet_bp_stop(void);


/**
 * @brief Sends a bundle to the specified proxy, asking that the named Resource be set to the specified value.
 *
 * @param[in] dest_eid The Endpoint Identifier of the Bionet BP Commanding Proxy.
 * @param[in] bundle_ttl Bundle lifetime
 * @param[in] resource_name The name of the Resource to command (may include wildcards).
 * @param[in] value The value to set the Resource to.
 *
 * @retval 0 on success
 * @retval -1 on failure
 */
int bionet_bp_set_resource_by_name(const char *dest_eid, int bundle_ttl, const char *resource_name, const char *value);




#endif

