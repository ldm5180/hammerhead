/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "BDM-S2C-Message.h"

static asn_TYPE_member_t asn_MBR_BDM_S2C_Message_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_S2C_Message, choice.resourceDatapointsReply),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ResourceDatapointsReply,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"resourceDatapointsReply"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_S2C_Message, choice.resourceMetadata),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BDMResourceMetadata,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"resourceMetadata"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_S2C_Message, choice.datapointsUpdate),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BDMResourceDatapoints,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"datapointsUpdate"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_S2C_Message, choice.newHab),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Node,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"newHab"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_S2C_Message, choice.lostHab),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"lostHab"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_S2C_Message, choice.newNode),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Node,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"newNode"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_S2C_Message, choice.lostNode),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"lostNode"
		},
};
static asn_TYPE_tag2member_t asn_MAP_BDM_S2C_Message_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* resourceDatapointsReply at 228 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* resourceMetadata at 229 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* datapointsUpdate at 230 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* newHab at 231 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* lostHab at 232 */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* newNode at 233 */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 } /* lostNode at 234 */
};
static asn_CHOICE_specifics_t asn_SPC_BDM_S2C_Message_specs_1 = {
	sizeof(struct BDM_S2C_Message),
	offsetof(struct BDM_S2C_Message, _asn_ctx),
	offsetof(struct BDM_S2C_Message, present),
	sizeof(((struct BDM_S2C_Message *)0)->present),
	asn_MAP_BDM_S2C_Message_tag2el_1,
	7,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
static asn_per_constraints_t asn_PER_BDM_S2C_Message_constr_1 = {
	{ APC_CONSTRAINED,	 3,  3,  0,  6 }	/* (0..6) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
asn_TYPE_descriptor_t asn_DEF_BDM_S2C_Message = {
	"BDM-S2C-Message",
	"BDM-S2C-Message",
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
	&asn_PER_BDM_S2C_Message_constr_1,
	asn_MBR_BDM_S2C_Message_1,
	7,	/* Elements count */
	&asn_SPC_BDM_S2C_Message_specs_1	/* Additional specs */
};

