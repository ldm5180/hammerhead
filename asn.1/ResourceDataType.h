/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#ifndef	_ResourceDataType_H_
#define	_ResourceDataType_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeEnumerated.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum ResourceDataType {
	ResourceDataType_binary	= 0,
	ResourceDataType_uint8	= 1,
	ResourceDataType_int8	= 2,
	ResourceDataType_uint16	= 3,
	ResourceDataType_int16	= 4,
	ResourceDataType_uint32	= 5,
	ResourceDataType_int32	= 6,
	ResourceDataType_float	= 7,
	ResourceDataType_double	= 8,
	ResourceDataType_string	= 9
} e_ResourceDataType;

/* ResourceDataType */
typedef long	 ResourceDataType_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ResourceDataType;
asn_struct_free_f ResourceDataType_free;
asn_struct_print_f ResourceDataType_print;
asn_constr_check_f ResourceDataType_constraint;
ber_type_decoder_f ResourceDataType_decode_ber;
der_type_encoder_f ResourceDataType_encode_der;
xer_type_decoder_f ResourceDataType_decode_xer;
xer_type_encoder_f ResourceDataType_encode_xer;
per_type_decoder_f ResourceDataType_decode_uper;
per_type_encoder_f ResourceDataType_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _ResourceDataType_H_ */
