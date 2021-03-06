/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#include <asn_internal.h>

#include "BDM-C2S-Message.h"

static asn_TYPE_member_t asn_MBR_BDM_C2S_Message_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_C2S_Message, choice.resourceDatapointsQuery),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ResourceDatapointsQuery,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"resourceDatapointsQuery"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_C2S_Message, choice.subscribeHab),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"subscribeHab"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_C2S_Message, choice.subscribeNode),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"subscribeNode"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_C2S_Message, choice.subscribeDatapoints),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PrintableString,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"subscribeDatapoints"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_C2S_Message, choice.sendState),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BDMSendState,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"sendState"
		},
};
static asn_TYPE_tag2member_t asn_MAP_BDM_C2S_Message_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* resourceDatapointsQuery at 245 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* subscribeHab at 246 */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* subscribeNode at 247 */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* subscribeDatapoints at 248 */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 } /* sendState at 250 */
};
static asn_CHOICE_specifics_t asn_SPC_BDM_C2S_Message_specs_1 = {
	sizeof(struct BDM_C2S_Message),
	offsetof(struct BDM_C2S_Message, _asn_ctx),
	offsetof(struct BDM_C2S_Message, present),
	sizeof(((struct BDM_C2S_Message *)0)->present),
	asn_MAP_BDM_C2S_Message_tag2el_1,
	5,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
static asn_per_constraints_t asn_PER_BDM_C2S_Message_constr_1 = {
	{ APC_CONSTRAINED,	 3,  3,  0,  4 }	/* (0..4) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
asn_TYPE_descriptor_t asn_DEF_BDM_C2S_Message = {
	"BDM-C2S-Message",
	"BDM-C2S-Message",
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
	&asn_PER_BDM_C2S_Message_constr_1,
	asn_MBR_BDM_C2S_Message_1,
	5,	/* Elements count */
	&asn_SPC_BDM_C2S_Message_specs_1	/* Additional specs */
};

