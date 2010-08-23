/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#ifndef	_BDM_S2C_Message_H_
#define	_BDM_S2C_Message_H_


#include <asn_application.h>

/* Including external dependencies */
#include "ResourceDatapointsReply.h"
#include "BDMResourceMetadata.h"
#include "BDMResourceDatapoints.h"
#include "BDMNewLostHab.h"
#include "BDMNewLostNode.h"
#include "BDMSendState.h"
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum BDM_S2C_Message_PR {
	BDM_S2C_Message_PR_NOTHING,	/* No components present */
	BDM_S2C_Message_PR_resourceDatapointsReply,
	BDM_S2C_Message_PR_resourceMetadata,
	BDM_S2C_Message_PR_datapointsUpdate,
	BDM_S2C_Message_PR_newHab,
	BDM_S2C_Message_PR_lostHab,
	BDM_S2C_Message_PR_newNode,
	BDM_S2C_Message_PR_lostNode,
	BDM_S2C_Message_PR_sendState
} BDM_S2C_Message_PR;

/* BDM-S2C-Message */
typedef struct BDM_S2C_Message {
	BDM_S2C_Message_PR present;
	union BDM_S2C_Message_u {
		ResourceDatapointsReply_t	 resourceDatapointsReply;
		BDMResourceMetadata_t	 resourceMetadata;
		BDMResourceDatapoints_t	 datapointsUpdate;
		BDMNewLostHab_t	 newHab;
		BDMNewLostHab_t	 lostHab;
		BDMNewLostNode_t	 newNode;
		BDMNewLostNode_t	 lostNode;
		BDMSendState_t	 sendState;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BDM_S2C_Message_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BDM_S2C_Message;

#ifdef __cplusplus
}
#endif

#endif	/* _BDM_S2C_Message_H_ */
