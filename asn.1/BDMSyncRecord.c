/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "BDMSyncRecord.h"

static asn_TYPE_member_t asn_MBR_syncResources_3[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_ResourceRecord,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_syncResources_tags_3[] = {
	(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_syncResources_specs_3 = {
	sizeof(struct syncResources),
	offsetof(struct syncResources, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
static /* Use -fall-defs-global to expose */
asn_TYPE_descriptor_t asn_DEF_syncResources_3 = {
	"syncResources",
	"syncResources",
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
	asn_DEF_syncResources_tags_3,
	sizeof(asn_DEF_syncResources_tags_3)
		/sizeof(asn_DEF_syncResources_tags_3[0]) - 1, /* 1 */
	asn_DEF_syncResources_tags_3,	/* Same as above */
	sizeof(asn_DEF_syncResources_tags_3)
		/sizeof(asn_DEF_syncResources_tags_3[0]), /* 2 */
	0,	/* No PER visible constraints */
	asn_MBR_syncResources_3,
	1,	/* Single element */
	&asn_SPC_syncResources_specs_3	/* Additional specs */
};

static asn_TYPE_member_t asn_MBR_BDMSyncRecord_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct BDMSyncRecord, bdmID),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"bdmID"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDMSyncRecord, syncResources),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_syncResources_3,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"syncResources"
		},
};
static ber_tlv_tag_t asn_DEF_BDMSyncRecord_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_BDMSyncRecord_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* bdmID at 95 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* syncResources at 97 */
};
static asn_SEQUENCE_specifics_t asn_SPC_BDMSyncRecord_specs_1 = {
	sizeof(struct BDMSyncRecord),
	offsetof(struct BDMSyncRecord, _asn_ctx),
	asn_MAP_BDMSyncRecord_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_BDMSyncRecord = {
	"BDMSyncRecord",
	"BDMSyncRecord",
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
	asn_DEF_BDMSyncRecord_tags_1,
	sizeof(asn_DEF_BDMSyncRecord_tags_1)
		/sizeof(asn_DEF_BDMSyncRecord_tags_1[0]), /* 1 */
	asn_DEF_BDMSyncRecord_tags_1,	/* Same as above */
	sizeof(asn_DEF_BDMSyncRecord_tags_1)
		/sizeof(asn_DEF_BDMSyncRecord_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BDMSyncRecord_1,
	2,	/* Elements count */
	&asn_SPC_BDMSyncRecord_specs_1	/* Additional specs */
};

