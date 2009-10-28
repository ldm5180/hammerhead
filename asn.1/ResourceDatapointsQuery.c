/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "ResourceDatapointsQuery.h"

static asn_TYPE_member_t asn_MBR_ResourceDatapointsQuery_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, habType),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"habType"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, habId),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"habId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, nodeId),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"nodeId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, resourceId),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"resourceId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, datapointStartTime),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GeneralizedTime,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"datapointStartTime"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, datapointEndTime),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_GeneralizedTime,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"datapointEndTime"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, entryStart),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"entryStart"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceDatapointsQuery, entryEnd),
		(ASN_TAG_CLASS_CONTEXT | (7 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"entryEnd"
		},
};
static ber_tlv_tag_t asn_DEF_ResourceDatapointsQuery_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ResourceDatapointsQuery_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* habType at 145 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* habId at 146 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* nodeId at 147 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* resourceId at 148 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* datapointStartTime at 149 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* datapointEndTime at 150 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 }, /* entryStart at 151 */
    { (ASN_TAG_CLASS_CONTEXT | (7 << 2)), 7, 0, 0 } /* entryEnd at 153 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ResourceDatapointsQuery_specs_1 = {
	sizeof(struct ResourceDatapointsQuery),
	offsetof(struct ResourceDatapointsQuery, _asn_ctx),
	asn_MAP_ResourceDatapointsQuery_tag2el_1,
	8,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ResourceDatapointsQuery = {
	"ResourceDatapointsQuery",
	"ResourceDatapointsQuery",
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
	asn_DEF_ResourceDatapointsQuery_tags_1,
	sizeof(asn_DEF_ResourceDatapointsQuery_tags_1)
		/sizeof(asn_DEF_ResourceDatapointsQuery_tags_1[0]), /* 1 */
	asn_DEF_ResourceDatapointsQuery_tags_1,	/* Same as above */
	sizeof(asn_DEF_ResourceDatapointsQuery_tags_1)
		/sizeof(asn_DEF_ResourceDatapointsQuery_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ResourceDatapointsQuery_1,
	8,	/* Elements count */
	&asn_SPC_ResourceDatapointsQuery_specs_1	/* Additional specs */
};

