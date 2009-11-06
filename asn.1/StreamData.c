/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "StreamData.h"

static asn_TYPE_member_t asn_MBR_StreamData_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct StreamData, nodeId),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"nodeId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct StreamData, streamId),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"streamId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct StreamData, data),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"data"
		},
};
static ber_tlv_tag_t asn_DEF_StreamData_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_StreamData_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* nodeId at 196 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* streamId at 197 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* data at 198 */
};
static asn_SEQUENCE_specifics_t asn_SPC_StreamData_specs_1 = {
	sizeof(struct StreamData),
	offsetof(struct StreamData, _asn_ctx),
	asn_MAP_StreamData_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_StreamData = {
	"StreamData",
	"StreamData",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_StreamData_tags_1,
	sizeof(asn_DEF_StreamData_tags_1)
		/sizeof(asn_DEF_StreamData_tags_1[0]), /* 1 */
	asn_DEF_StreamData_tags_1,	/* Same as above */
	sizeof(asn_DEF_StreamData_tags_1)
		/sizeof(asn_DEF_StreamData_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_StreamData_1,
	3,	/* Elements count */
	&asn_SPC_StreamData_specs_1	/* Additional specs */
};

