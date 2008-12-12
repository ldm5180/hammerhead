
//
// Copyright (C) 2008, Regents of the University of Colorado.
//

/**
 * @mainpage Bionet API Documentation
 *
 * @section intro Intro
 *
 * There are two parts to the Bionet API, the Client API (libbionet) and the
 * Hardware Abstractor (HAB) API (libhab).
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
 *     U -> C [ arrowhead="open" ];
 *     U -> H [ arrowhead="open" ];
 *  }
 * @enddot
 * @endif
 *
 * @section client Bionet Client API
 *
 * Used by Client applications to subscribe to data in Bionet coming from 
 * Hardware Abstractors. 
 *
 * Full documentation:@n
 * @li bionet.h
 *
 * @section hab Hardware Abstractor (HAB) API
 *
 * Used by HAB applications to publish data from real hardware to Bionet. 
 * 
 * Full documentation:@n 
 * @li hardware-abstractor.h
 *
 * @section util Utility API (libutil)
 *
 * Used by Client applications and HAB application to create, read, or modify 
 * Bionet datatypes. 
 *
 * Full documentation:@n
 * @li bionet-hab.h
 * @li bionet-node.h
 * @li bionet-resource.h
 * @li bionet-stream.h
 * @li bionet-util.h
 */
