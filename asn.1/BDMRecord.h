/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#ifndef	_BDMRecord_H_
#define	_BDMRecord_H_


#include <asn_application.h>

/* Including external dependencies */
#include <GeneralizedTime.h>
#include <OCTET_STRING.h>
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct Datapoint;

/* BDMRecord */
typedef struct BDMRecord {
	GeneralizedTime_t	 entryTimestamp;
	OCTET_STRING_t	 resourceKey;
	struct syncDatapoints {
		A_SEQUENCE_OF(struct Datapoint) list;
		
		/* Context for parsing across buffer boundaries */
		asn_struct_ctx_t _asn_ctx;
	} syncDatapoints;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BDMRecord_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BDMRecord;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "Datapoint.h"

#endif	/* _BDMRecord_H_ */
