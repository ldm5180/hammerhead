/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "H2C-Message.h"

static asn_TYPE_member_t asn_MBR_H2C_Message_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct H2C_Message, choice.newNode),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Node,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"newNode"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct H2C_Message, choice.lostNode),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"lostNode"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct H2C_Message, choice.resourceMetadata),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ResourceMetadata,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"resourceMetadata"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct H2C_Message, choice.datapointsUpdate),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ResourceDatapoints,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"datapointsUpdate"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct H2C_Message, choice.streamData),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_StreamData,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"streamData"
		},
};
static asn_TYPE_tag2member_t asn_MAP_H2C_Message_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* newNode at 181 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* lostNode at 182 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* resourceMetadata at 183 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* datapointsUpdate at 184 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* streamData at 186 */
};
static asn_CHOICE_specifics_t asn_SPC_H2C_Message_specs_1 = {
	sizeof(struct H2C_Message),
	offsetof(struct H2C_Message, _asn_ctx),
	offsetof(struct H2C_Message, present),
	sizeof(((struct H2C_Message *)0)->present),
	asn_MAP_H2C_Message_tag2el_1,
	5,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
static asn_per_constraints_t asn_PER_H2C_Message_constr_1 = {
	{ APC_CONSTRAINED,	 3,  3,  0,  4 }	/* (0..4) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
asn_TYPE_descriptor_t asn_DEF_H2C_Message = {
	"H2C-Message",
	"H2C-Message",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	CHOICE_decode_uper,
	CHOICE_encode_uper,
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	&asn_PER_H2C_Message_constr_1,
	asn_MBR_H2C_Message_1,
	5,	/* Elements count */
	&asn_SPC_H2C_Message_specs_1	/* Additional specs */
};

