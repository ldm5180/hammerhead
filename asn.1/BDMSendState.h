/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#ifndef	_BDMSendState_H_
#define	_BDMSendState_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <PrintableString.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BDMSendState */
typedef struct BDMSendState {
	long	 seq;
	PrintableString_t	 topic;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BDMSendState_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BDMSendState;

#ifdef __cplusplus
}
#endif

#endif	/* _BDMSendState_H_ */
