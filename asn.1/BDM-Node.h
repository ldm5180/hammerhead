/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#ifndef	_BDM_Node_H_
#define	_BDM_Node_H_


#include <asn_application.h>

/* Including external dependencies */
#include <PrintableString.h>
#include <OCTET_STRING.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct BDM_Resource;
struct Stream;
struct BDM_Event;

/* BDM-Node */
typedef struct BDM_Node {
	PrintableString_t	 id;
	OCTET_STRING_t	 uid;
	struct BDM_Node__resources {
		A_SEQUENCE_OF(struct BDM_Resource) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} resources;
	struct BDM_Node__streams {
		A_SEQUENCE_OF(struct Stream) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} streams;
	struct BDM_Node__events {
		A_SEQUENCE_OF(struct BDM_Event) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} events;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BDM_Node_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BDM_Node;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "BDM-Resource.h"
#include "Stream.h"
#include "BDM-Event.h"

#endif	/* _BDM_Node_H_ */
