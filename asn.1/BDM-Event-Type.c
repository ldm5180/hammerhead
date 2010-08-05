/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#include <asn_internal.h>

#include "BDM-Event-Type.h"

int
BDM_Event_Type_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	/* Replace with underlying type checker */
	td->check_constraints = asn_DEF_NativeEnumerated.check_constraints;
	return td->check_constraints(td, sptr, ctfailcb, app_key);
}

/*
 * This type is implemented using NativeEnumerated,
 * so here we adjust the DEF accordingly.
 */
static void
BDM_Event_Type_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
	td->free_struct    = asn_DEF_NativeEnumerated.free_struct;
	td->print_struct   = asn_DEF_NativeEnumerated.print_struct;
	td->ber_decoder    = asn_DEF_NativeEnumerated.ber_decoder;
	td->der_encoder    = asn_DEF_NativeEnumerated.der_encoder;
	td->xer_decoder    = asn_DEF_NativeEnumerated.xer_decoder;
	td->xer_encoder    = asn_DEF_NativeEnumerated.xer_encoder;
	td->uper_decoder   = asn_DEF_NativeEnumerated.uper_decoder;
	td->uper_encoder   = asn_DEF_NativeEnumerated.uper_encoder;
	if(!td->per_constraints)
		td->per_constraints = asn_DEF_NativeEnumerated.per_constraints;
	td->elements       = asn_DEF_NativeEnumerated.elements;
	td->elements_count = asn_DEF_NativeEnumerated.elements_count;
     /* td->specifics      = asn_DEF_NativeEnumerated.specifics;	// Defined explicitly */
}

void
BDM_Event_Type_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
BDM_Event_Type_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
BDM_Event_Type_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
BDM_Event_Type_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
BDM_Event_Type_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
BDM_Event_Type_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

asn_dec_rval_t
BDM_Event_Type_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

asn_enc_rval_t
BDM_Event_Type_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	BDM_Event_Type_1_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

static asn_INTEGER_enum_map_t asn_MAP_BDM_Event_Type_value2enum_1[] = {
	{ 0,	3,	"new" },
	{ 1,	4,	"lost" }
};
static unsigned int asn_MAP_BDM_Event_Type_enum2value_1[] = {
	1,	/* lost(1) */
	0	/* new(0) */
};
static asn_INTEGER_specifics_t asn_SPC_BDM_Event_Type_specs_1 = {
	asn_MAP_BDM_Event_Type_value2enum_1,	/* "tag" => N; sorted by tag */
	asn_MAP_BDM_Event_Type_enum2value_1,	/* N => "tag"; sorted by N */
	2,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1	/* Strict enumeration */
};
static ber_tlv_tag_t asn_DEF_BDM_Event_Type_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
static asn_per_constraints_t asn_PER_BDM_Event_Type_constr_1 = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
asn_TYPE_descriptor_t asn_DEF_BDM_Event_Type = {
	"BDM-Event-Type",
	"BDM-Event-Type",
	BDM_Event_Type_free,
	BDM_Event_Type_print,
	BDM_Event_Type_constraint,
	BDM_Event_Type_decode_ber,
	BDM_Event_Type_encode_der,
	BDM_Event_Type_decode_xer,
	BDM_Event_Type_encode_xer,
	BDM_Event_Type_decode_uper,
	BDM_Event_Type_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_BDM_Event_Type_tags_1,
	sizeof(asn_DEF_BDM_Event_Type_tags_1)
		/sizeof(asn_DEF_BDM_Event_Type_tags_1[0]), /* 1 */
	asn_DEF_BDM_Event_Type_tags_1,	/* Same as above */
	sizeof(asn_DEF_BDM_Event_Type_tags_1)
		/sizeof(asn_DEF_BDM_Event_Type_tags_1[0]), /* 1 */
	&asn_PER_BDM_Event_Type_constr_1,
	0, 0,	/* Defined elsewhere */
	&asn_SPC_BDM_Event_Type_specs_1	/* Additional specs */
};

