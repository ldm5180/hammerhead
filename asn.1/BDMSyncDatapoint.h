/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#ifndef	_BDMSyncDatapoint_H_
#define	_BDMSyncDatapoint_H_


#include <asn_application.h>

/* Including external dependencies */
#include "Datapoint.h"
#include <GeneralizedTime.h>
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BDMSyncDatapoint */
typedef struct BDMSyncDatapoint {
	Datapoint_t	 datapoint;
	GeneralizedTime_t	 timestamp;
	long	 entrySeq;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BDMSyncDatapoint_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BDMSyncDatapoint;

#ifdef __cplusplus
}
#endif

#endif	/* _BDMSyncDatapoint_H_ */
