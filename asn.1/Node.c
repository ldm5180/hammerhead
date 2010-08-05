/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#include <asn_internal.h>

#include "Node.h"

static asn_TYPE_member_t asn_MBR_resources_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_Resource,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_resources_tags_3[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_resources_specs_3 = {
	sizeof(struct Node__resources),
	offsetof(struct Node__resources, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_resources_3 = {
	"resources",
	"resources",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_resources_tags_3,
	sizeof(asn_DEF_resources_tags_3)
		/sizeof(asn_DEF_resources_tags_3[0]) - 1, /* 1 */
	asn_DEF_resources_tags_3,	/* Same as above */
	sizeof(asn_DEF_resources_tags_3)
		/sizeof(asn_DEF_resources_tags_3[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_resources_3,
	1,	/* Single element */
	&asn_SPC_resources_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_streams_5[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_Stream,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_streams_tags_5[] = {
	(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_streams_specs_5 = {
	sizeof(struct Node__streams),
	offsetof(struct Node__streams, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_streams_5 = {
	"streams",
	"streams",
	SEQUENCE_OF_free,
	SEQUENCE_OF_print,
	SEQUENCE_OF_constraint,
	SEQUENCE_OF_decode_ber,
	SEQUENCE_OF_encode_der,
	SEQUENCE_OF_decode_xer,
	SEQUENCE_OF_encode_xer,
	SEQUENCE_OF_decode_uper,
	SEQUENCE_OF_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_streams_tags_5,
	sizeof(asn_DEF_streams_tags_5)
		/sizeof(asn_DEF_streams_tags_5[0]) - 1, /* 1 */
	asn_DEF_streams_tags_5,	/* Same as above */
	sizeof(asn_DEF_streams_tags_5)
		/sizeof(asn_DEF_streams_tags_5[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_streams_5,
	1,	/* Single element */
	&asn_SPC_streams_specs_5	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_Node_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct Node, id),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"id"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Node, resources),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_resources_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"resources"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct Node, streams),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_streams_5,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"streams"
		},
};
static ber_tlv_tag_t asn_DEF_Node_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_Node_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* id at 71 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* resources at 72 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* streams at 74 */
};
static asn_SEQUENCE_specifics_t asn_SPC_Node_specs_1 = {
	sizeof(struct Node),
	offsetof(struct Node, _asn_ctx),
	asn_MAP_Node_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_Node = {
	"Node",
	"Node",
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
	asn_DEF_Node_tags_1,
	sizeof(asn_DEF_Node_tags_1)
		/sizeof(asn_DEF_Node_tags_1[0]), /* 1 */
	asn_DEF_Node_tags_1,	/* Same as above */
	sizeof(asn_DEF_Node_tags_1)
		/sizeof(asn_DEF_Node_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_Node_1,
	3,	/* Elements count */
	&asn_SPC_Node_specs_1	/* Additional specs */
};

