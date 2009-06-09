/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#ifndef	_BDM_Sync_Datapoints_Message_H_
#define	_BDM_Sync_Datapoints_Message_H_


#include <asn_application.h>

/* Including external dependencies */
#include "BDMRecord.h"
#include <PrintableString.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BDM-Sync-Datapoints-Message */
typedef struct BDM_Sync_Datapoints_Message {
	BDMRecord_t	 bdmRecord;
	PrintableString_t	 bdmID;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BDM_Sync_Datapoints_Message_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BDM_Sync_Datapoints_Message;

#ifdef __cplusplus
}
#endif

#endif	/* _BDM_Sync_Datapoints_Message_H_ */
