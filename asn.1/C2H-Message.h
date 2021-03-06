/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#ifndef	_C2H_Message_H_
#define	_C2H_Message_H_


#include <asn_application.h>

/* Including external dependencies */
#include "SetResourceValue.h"
#include "StreamData.h"
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum C2H_Message_PR {
	C2H_Message_PR_NOTHING,	/* No components present */
	C2H_Message_PR_setResourceValue,
	C2H_Message_PR_streamData
} C2H_Message_PR;

/* C2H-Message */
typedef struct C2H_Message {
	C2H_Message_PR present;
	union C2H_Message_u {
		SetResourceValue_t	 setResourceValue;
		StreamData_t	 streamData;
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} C2H_Message_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_C2H_Message;

#ifdef __cplusplus
}
#endif

#endif	/* _C2H_Message_H_ */
