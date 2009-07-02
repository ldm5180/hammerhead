/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "BDM-Sync-Message.h"

static asn_TYPE_member_t asn_MBR_BDM_Sync_Message_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_Sync_Message, choice.metadataMessage),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BDM_Sync_Metadata_Message,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"metadataMessage"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct BDM_Sync_Message, choice.datapointsMessage),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_BDM_Sync_Datapoints_Message,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"datapointsMessage"
		},
};
static asn_TYPE_tag2member_t asn_MAP_BDM_Sync_Message_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* metadataMessage at 232 */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 } /* datapointsMessage at 234 */
};
static asn_CHOICE_specifics_t asn_SPC_BDM_Sync_Message_specs_1 = {
	sizeof(struct BDM_Sync_Message),
	offsetof(struct BDM_Sync_Message, _asn_ctx),
	offsetof(struct BDM_Sync_Message, present),
	sizeof(((struct BDM_Sync_Message *)0)->present),
	asn_MAP_BDM_Sync_Message_tag2el_1,
	2,	/* Count of tags in the map */
	0,
	-1	/* Extensions start */
};
static asn_per_constraints_t asn_PER_BDM_Sync_Message_constr_1 = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
asn_TYPE_descriptor_t asn_DEF_BDM_Sync_Message = {
	"BDM-Sync-Message",
	"BDM-Sync-Message",
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
	&asn_PER_BDM_Sync_Message_constr_1,
	asn_MBR_BDM_Sync_Message_1,
	2,	/* Elements count */
	&asn_SPC_BDM_Sync_Message_specs_1	/* Additional specs */
};
