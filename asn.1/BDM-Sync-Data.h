/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#ifndef	_BDM_Sync_Data_H_
#define	_BDM_Sync_Data_H_


#include <asn_application.h>

/* Including external dependencies */
#include "BDM-Sync-Metadata-Message.h"
#include "BDM-Sync-Datapoints-Message.h"
#include <NativeInteger.h>
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum BDM_Sync_Data_PR {
	BDM_Sync_Data_PR_NOTHING,	/* No components present */
	BDM_Sync_Data_PR_metadataMessage,
	BDM_Sync_Data_PR_datapointsMessage,
	BDM_Sync_Data_PR_ackMetadata,
	BDM_Sync_Data_PR_ackDatapoints
} BDM_Sync_Data_PR;

/* BDM-Sync-Data */
typedef struct BDM_Sync_Data {
	BDM_Sync_Data_PR present;
	union BDM_Sync_Data_u {
		BDM_Sync_Metadata_Message_t	 metadataMessage;
		BDM_Sync_Datapoints_Message_t	 datapointsMessage;
		long	 ackMetadata;
		long	 ackDatapoints;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BDM_Sync_Data_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BDM_Sync_Data;

#ifdef __cplusplus
}
#endif

#endif	/* _BDM_Sync_Data_H_ */
