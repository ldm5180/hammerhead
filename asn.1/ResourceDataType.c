/*
 * Generated by asn1c-0.9.21 (http://lionet.info/asn1c)
 * From ASN.1 module "BionetProtocol1"
 * 	found in "bionet-protocol-0.asn1"
 * 	`asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER`
 */

#include <asn_internal.h>

#include "ResourceDataType.h"

int
ResourceDataType_constraint(asn_TYPE_descriptor_t *td, const void *sptr,
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
ResourceDataType_1_inherit_TYPE_descriptor(asn_TYPE_descriptor_t *td) {
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
ResourceDataType_free(asn_TYPE_descriptor_t *td,
		void *struct_ptr, int contents_only) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	td->free_struct(td, struct_ptr, contents_only);
}

int
ResourceDataType_print(asn_TYPE_descriptor_t *td, const void *struct_ptr,
		int ilevel, asn_app_consume_bytes_f *cb, void *app_key) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	return td->print_struct(td, struct_ptr, ilevel, cb, app_key);
}

asn_dec_rval_t
ResourceDataType_decode_ber(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const void *bufptr, size_t size, int tag_mode) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	return td->ber_decoder(opt_codec_ctx, td, structure, bufptr, size, tag_mode);
}

asn_enc_rval_t
ResourceDataType_encode_der(asn_TYPE_descriptor_t *td,
		void *structure, int tag_mode, ber_tlv_tag_t tag,
		asn_app_consume_bytes_f *cb, void *app_key) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	return td->der_encoder(td, structure, tag_mode, tag, cb, app_key);
}

asn_dec_rval_t
ResourceDataType_decode_xer(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		void **structure, const char *opt_mname, const void *bufptr, size_t size) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	return td->xer_decoder(opt_codec_ctx, td, structure, opt_mname, bufptr, size);
}

asn_enc_rval_t
ResourceDataType_encode_xer(asn_TYPE_descriptor_t *td, void *structure,
		int ilevel, enum xer_encoder_flags_e flags,
		asn_app_consume_bytes_f *cb, void *app_key) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	return td->xer_encoder(td, structure, ilevel, flags, cb, app_key);
}

asn_dec_rval_t
ResourceDataType_decode_uper(asn_codec_ctx_t *opt_codec_ctx, asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints, void **structure, asn_per_data_t *per_data) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	return td->uper_decoder(opt_codec_ctx, td, constraints, structure, per_data);
}

asn_enc_rval_t
ResourceDataType_encode_uper(asn_TYPE_descriptor_t *td,
		asn_per_constraints_t *constraints,
		void *structure, asn_per_outp_t *per_out) {
	ResourceDataType_1_inherit_TYPE_descriptor(td);
	return td->uper_encoder(td, constraints, structure, per_out);
}

static asn_INTEGER_enum_map_t asn_MAP_ResourceDataType_value2enum_1[] = {
	{ 0,	6,	"binary" },
	{ 1,	5,	"uint8" },
	{ 2,	4,	"int8" },
	{ 3,	6,	"uint16" },
	{ 4,	5,	"int16" },
	{ 5,	6,	"uint32" },
	{ 6,	5,	"int32" },
	{ 7,	5,	"float" },
	{ 8,	6,	"double" },
	{ 9,	6,	"string" }
};
static unsigned int asn_MAP_ResourceDataType_enum2value_1[] = {
	0,	/* binary(0) */
	8,	/* double(8) */
	7,	/* float(7) */
	4,	/* int16(4) */
	6,	/* int32(6) */
	2,	/* int8(2) */
	9,	/* string(9) */
	3,	/* uint16(3) */
	5,	/* uint32(5) */
	1	/* uint8(1) */
};
static asn_INTEGER_specifics_t asn_SPC_ResourceDataType_specs_1 = {
	asn_MAP_ResourceDataType_value2enum_1,	/* "tag" => N; sorted by tag */
	asn_MAP_ResourceDataType_enum2value_1,	/* N => "tag"; sorted by N */
	10,	/* Number of elements in the maps */
	0,	/* Enumeration is not extensible */
	1	/* Strict enumeration */
};
static ber_tlv_tag_t asn_DEF_ResourceDataType_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (10 << 2))
};
static asn_per_constraints_t asn_PER_ResourceDataType_constr_1 = {
	{ APC_CONSTRAINED,	 4,  4,  0,  9 }	/* (0..9) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 }
};
asn_TYPE_descriptor_t asn_DEF_ResourceDataType = {
	"ResourceDataType",
	"ResourceDataType",
	ResourceDataType_free,
	ResourceDataType_print,
	ResourceDataType_constraint,
	ResourceDataType_decode_ber,
	ResourceDataType_encode_der,
	ResourceDataType_decode_xer,
	ResourceDataType_encode_xer,
	ResourceDataType_decode_uper,
	ResourceDataType_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_ResourceDataType_tags_1,
	sizeof(asn_DEF_ResourceDataType_tags_1)
		/sizeof(asn_DEF_ResourceDataType_tags_1[0]), /* 1 */
	asn_DEF_ResourceDataType_tags_1,	/* Same as above */
	sizeof(asn_DEF_ResourceDataType_tags_1)
		/sizeof(asn_DEF_ResourceDataType_tags_1[0]), /* 1 */
	&asn_PER_ResourceDataType_constr_1,
	0, 0,	/* Defined elsewhere */
	&asn_SPC_ResourceDataType_specs_1	/* Additional specs */
};

