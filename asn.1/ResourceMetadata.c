/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "ResourceMetadata.h"

static asn_TYPE_member_t asn_MBR_ResourceMetadata_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceMetadata, nodeId),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"nodeId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceMetadata, resourceId),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"resourceId"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceMetadata, flavor),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ResourceFlavor,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"flavor"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct ResourceMetadata, datatype),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ResourceDataType,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"datatype"
		},
};
static ber_tlv_tag_t asn_DEF_ResourceMetadata_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_TYPE_tag2member_t asn_MAP_ResourceMetadata_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* nodeId at 65 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* resourceId at 66 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* flavor at 67 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* datatype at 69 */
};
static asn_SEQUENCE_specifics_t asn_SPC_ResourceMetadata_specs_1 = {
	sizeof(struct ResourceMetadata),
	offsetof(struct ResourceMetadata, _asn_ctx),
	asn_MAP_ResourceMetadata_tag2el_1,
	4,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_ResourceMetadata = {
	"ResourceMetadata",
	"ResourceMetadata",
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
	asn_DEF_ResourceMetadata_tags_1,
	sizeof(asn_DEF_ResourceMetadata_tags_1)
		/sizeof(asn_DEF_ResourceMetadata_tags_1[0]), /* 1 */
	asn_DEF_ResourceMetadata_tags_1,	/* Same as above */
	sizeof(asn_DEF_ResourceMetadata_tags_1)
		/sizeof(asn_DEF_ResourceMetadata_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_ResourceMetadata_1,
	4,	/* Elements count */
	&asn_SPC_ResourceMetadata_specs_1	/* Additional specs */
};

