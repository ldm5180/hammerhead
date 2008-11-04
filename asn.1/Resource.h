/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#ifndef	_Resource_H_
#define	_Resource_H_


#include <asn_application.h>

/* Including external dependencies */
#include <PrintableString.h>
#include "ResourceFlavor.h"
#include "ResourceDataType.h"
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct Datapoint;

/* Resource */
typedef struct Resource {
	PrintableString_t	 id;
	ResourceFlavor_t	 flavor;
	ResourceDataType_t	 datatype;
	struct datapoints {
		A_SEQUENCE_OF(struct Datapoint) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} datapoints;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} Resource_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_Resource;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "Datapoint.h"

#endif	/* _Resource_H_ */
