
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


/**
 * Set Resource Callback
 *
 * Pushes a value for a resource out to all the nodes
 *
 * @param[in] node_id Disregarded, value is pushed to all nodes
 * @param[in] resource_id ID of resource which is to be updated
 * @param[in] value Value to be set
 */
void cb_set_resource(const char *node_id, 
		     const char *resource_id, 
		     const char *value);
