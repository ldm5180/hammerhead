
// Copyright (c) 2008-2010, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.


/**
 * @mainpage Bionet API Documentation
 *
 * @section intro Intro
 *
 * There are three parts to the Bionet API, the Client API (libbionet),the
 * Hardware Abstractor (HAB) API (libhab), and the Bionet Data Manager API (libbdm).
 *
 * Each of these APIs makes use of the Utility API (libutil) in their dealings 
 * with common datatypes.
 *
 * @if dotsec
 * @dot
 *  digraph example {
 *     node [shape=record, fontname=Helvetica, fontsize=10];
 *     C [ label="libbionet" URL="\ref client"];
 *     H [ label="libhab" URL="\ref hab"];
 *     U [ label="libutil" URL="\ref util"];
 *     B [ label="libbdm" URL="ref bdm"];
 *     U -> C [ arrowhead="open" ];
 *     U -> H [ arrowhead="open" ];
 *     U -> B [ arrowhead="open" ];
 *  }
 * @enddot
 * @endif
 *
 * @section client Bionet Client API (libbionet)
 *
 * Used by Client applications to subscribe to data in Bionet coming from 
 * Hardware Abstractors. 
 *
 * Full documentation:@n
 * @li bionet.h
 *
 * @note Python bindings available in client/py-libbionet. Contains the entire
 * Utility API within the module.
 *
 * @section hab Hardware Abstractor "HAB" API (libhab)
 *
 * Used by HAB applications to publish data from real hardware to Bionet. 
 * 
 * Full documentation:@n 
 * @li hardware-abstractor.h
 *
 * @note Python bindings available in hab/py-libhab. Contains the entire
 * Utility API within the module.
 *
 * @section bdm Bionet Data Manager API (libbdm)
 *
 * Used by Bionet Data Manager Client applications to subscribe to data recorded by the 
 * Bionet Data Manager and originally from Hardware Abstractors. 
 *
 * Full documentation:@n
 * @li bdm-client.h
 *
 * @note Python bindings available in data-mananger/client/py-libbdm. Contains the entire
 * Bionet Utility API within the module.

 * @section util Utility API (libutil)
 *
 * Used by Client applications and HAB application to create, read, or modify 
 * Bionet datatypes. 
 *
 * Full documentation:@n
 * @li bionet-hab.h
 * @li bionet-node.h
 * @li bionet-resource.h
 * @li bionet-datapoint.h
 * @li bionet-value.h
 * @li bionet-stream.h
 * @li bionet-util.h
 */
