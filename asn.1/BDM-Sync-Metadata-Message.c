/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol0"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto`
 */

#include <asn_internal.h>

#include "BDM-Sync-Metadata-Message.h"

static asn_TYPE_member_t asn_MBR_BDM_Sync_Metadata_Message_1[] = {
	{ ATF_POINTER, 0, 0,
		(ASN_TAG_CLASS_UNIVERSAL | (16 << 2)),
		0,
		&asn_DEF_DataManager,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		""
		},
};
static ber_tlv_tag_t asn_DEF_BDM_Sync_Metadata_Message_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static asn_SET_OF_specifics_t asn_SPC_BDM_Sync_Metadata_Message_specs_1 = {
	sizeof(struct BDM_Sync_Metadata_Message),
	offsetof(struct BDM_Sync_Metadata_Message, _asn_ctx),
	0,	/* XER encoding is XMLDelimitedItemList */
};
asn_TYPE_descriptor_t asn_DEF_BDM_Sync_Metadata_Message = {
	"BDM-Sync-Metadata-Message",
	"BDM-Sync-Metadata-Message",
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
	asn_DEF_BDM_Sync_Metadata_Message_tags_1,
	sizeof(asn_DEF_BDM_Sync_Metadata_Message_tags_1)
		/sizeof(asn_DEF_BDM_Sync_Metadata_Message_tags_1[0]), /* 1 */
	asn_DEF_BDM_Sync_Metadata_Message_tags_1,	/* Same as above */
	sizeof(asn_DEF_BDM_Sync_Metadata_Message_tags_1)
		/sizeof(asn_DEF_BDM_Sync_Metadata_Message_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_BDM_Sync_Metadata_Message_1,
	1,	/* Single element */
	&asn_SPC_BDM_Sync_Metadata_Message_specs_1	/* Additional specs */
};

