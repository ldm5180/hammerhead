
# Copyright (c) 2008-2010, Regents of the University of Colorado.
# This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
# NNC07CB47C.


include $(top_srcdir)/common.am


ASN_SOURCE = bionet-protocol-0.asn1



ASN_MODULE_SOURCES =              \
    ResourceFlavor.c              \
    ResourceDataType.c            \
    Value.c                       \
    Datapoint.c                   \
    Resource.c                    \
    StreamDirection.c             \
    Stream.c                      \
    Node.c                        \
    HardwareAbstractor.c          \
    DataManager.c                 \
    ResourceMetadata.c            \
    ResourceDatapoints.c          \
    ResourceDatapointsQuery.c     \
    ResourceDatapointsReply.c     \
    SetResourceValue.c            \
    StreamData.c                  \
    C2H-Message.c                 \
    H2C-Message.c                 \
    BDMNewHab.c                   \
    BDMNewNode.c                  \
    BDM-C2S-Message.c             \
    BDM-S2C-Message.c             \
    BDM-Sync-Metadata-Message.c   \
    BDM-Sync-Datapoints-Message.c \
    BDM-Sync-Ack.c                \
    BDMResourceMetadata.c         \
    BDMResourceDatapoints.c       \
    BDMRecord.c                   \
    BDMSendState.c                \
    BDMSyncRecord.c               \
    ResourceRecord.c              \
    BDM-Sync-Message.c            

# FIXME: autoreconf doesnt work if this variable is called ASN_MODULE_HEADS
ASN_MODULE_HEADS =                \
    ResourceFlavor.h              \
    ResourceDataType.h            \
    Value.h                       \
    Datapoint.h                   \
    Resource.h                    \
    StreamDirection.h             \
    Stream.h                      \
    Node.h                        \
    HardwareAbstractor.h          \
    DataManager.h                 \
    ResourceMetadata.h            \
    ResourceDatapoints.h          \
    ResourceDatapointsQuery.h     \
    ResourceDatapointsReply.h     \
    SetResourceValue.h            \
    StreamData.h                  \
    C2H-Message.h                 \
    H2C-Message.h                 \
    BDMNewHab.h                   \
    BDMNewNode.h                  \
    BDM-C2S-Message.h             \
    BDM-S2C-Message.h             \
    BDM-Sync-Metadata-Message.h   \
    BDM-Sync-Datapoints-Message.h \
    BDM-Sync-Ack.h                \
    BDMRecord.h                   \
    BDMSendState.h                \
    BDMSyncRecord.h               \
    ResourceRecord.h              \
    BDMResourceMetadata.h         \
    BDMResourceDatapoints.h       \
    BDM-Sync-Message.h            


ASN_MODULE_HEADS+=BOOLEAN.h
ASN_MODULE_SOURCES+=BOOLEAN.c
ASN_MODULE_HEADS+=INTEGER.h
ASN_MODULE_HEADS+=NativeEnumerated.h
ASN_MODULE_HEADS+=GeneralizedTime.h
ASN_MODULE_SOURCES+=GeneralizedTime.c
ASN_MODULE_SOURCES+=INTEGER.c
ASN_MODULE_SOURCES+=NativeEnumerated.c
ASN_MODULE_HEADS+=NativeInteger.h
ASN_MODULE_SOURCES+=NativeInteger.c
ASN_MODULE_HEADS+=NativeReal.h
ASN_MODULE_SOURCES+=NativeReal.c
ASN_MODULE_HEADS+=REAL.h
ASN_MODULE_HEADS+=PrintableString.h
ASN_MODULE_SOURCES+=PrintableString.c
ASN_MODULE_SOURCES+=REAL.c
ASN_MODULE_HEADS+=asn_SEQUENCE_OF.h
ASN_MODULE_SOURCES+=asn_SEQUENCE_OF.c
ASN_MODULE_HEADS+=asn_SET_OF.h
ASN_MODULE_SOURCES+=asn_SET_OF.c
ASN_MODULE_HEADS+=constr_CHOICE.h
ASN_MODULE_SOURCES+=constr_CHOICE.c
ASN_MODULE_HEADS+=constr_SEQUENCE.h
ASN_MODULE_SOURCES+=constr_SEQUENCE.c
ASN_MODULE_HEADS+=constr_SEQUENCE_OF.h
ASN_MODULE_SOURCES+=constr_SEQUENCE_OF.c
ASN_MODULE_HEADS+=constr_SET_OF.h
ASN_MODULE_SOURCES+=constr_SET_OF.c
ASN_MODULE_HEADS+=asn_application.h
ASN_MODULE_HEADS+=asn_system.h
ASN_MODULE_HEADS+=asn_codecs.h
ASN_MODULE_HEADS+=asn_internal.h
ASN_MODULE_HEADS+=OCTET_STRING.h
ASN_MODULE_SOURCES+=OCTET_STRING.c
ASN_MODULE_HEADS+=BIT_STRING.h
ASN_MODULE_SOURCES+=BIT_STRING.c
ASN_MODULE_SOURCES+=asn_codecs_prim.c
ASN_MODULE_HEADS+=asn_codecs_prim.h
ASN_MODULE_HEADS+=ber_tlv_length.h
ASN_MODULE_SOURCES+=ber_tlv_length.c
ASN_MODULE_HEADS+=ber_tlv_tag.h
ASN_MODULE_SOURCES+=ber_tlv_tag.c
ASN_MODULE_HEADS+=ber_decoder.h
ASN_MODULE_SOURCES+=ber_decoder.c
ASN_MODULE_HEADS+=der_encoder.h
ASN_MODULE_SOURCES+=der_encoder.c
ASN_MODULE_HEADS+=constr_TYPE.h
ASN_MODULE_SOURCES+=constr_TYPE.c
ASN_MODULE_HEADS+=constraints.h
ASN_MODULE_SOURCES+=constraints.c
ASN_MODULE_HEADS+=xer_support.h
ASN_MODULE_SOURCES+=xer_support.c
ASN_MODULE_HEADS+=xer_decoder.h
ASN_MODULE_SOURCES+=xer_decoder.c
ASN_MODULE_HEADS+=xer_encoder.h
ASN_MODULE_SOURCES+=xer_encoder.c
ASN_MODULE_HEADS+=per_support.h
ASN_MODULE_SOURCES+=per_support.c
ASN_MODULE_HEADS+=per_decoder.h
ASN_MODULE_SOURCES+=per_decoder.c
ASN_MODULE_HEADS+=per_encoder.h
ASN_MODULE_SOURCES+=per_encoder.c

BUILT_SOURCES = $(ASN_MODULE_SOURCES) $(ASN_MODULE_HEADS)

SRC = $(ASN_MODULE_SOURCES) bionet-asn-helpers.c

lib_LTLIBRARIES=libbionet-asn.la

libbionet_asn_la_SOURCES=$(SRC) $(ASN_MODULE_HEADS)

libbionet_asn_la_CPPFLAGS = $(AM_CPPFLAGS) -DBUILDING_LIBBIONET_ASN

libbionet_asn_la_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) $(CFLAGS_BIONET_UTIL) -fno-strict-aliasing

libbionet_asn_la_LIBADD = $(AM_LDADD) $(LIBS_BIONET_UTIL) $(GLIB_LIBS) -lm 

if ENABLE_WIN32
libbionet_asn_la_LIBADD += -lmsvcrt
endif

libbionet_asn_la_LDFLAGS = $(AM_LDFLAGS) $(LDFLAGS_BIONET_UTIL) 


if ENABLE_EXE

bin_PROGRAMS = test-asn-h2c test-asn-c2h test-asn-bdm-c2s test-asn-bdm-s2c

test_asn_h2c_DEPENDENCIES = libbionet-asn.la
test_asn_h2c_SOURCES = converter-sample.c pdu_collection.c
test_asn_h2c_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) -I../util -DASN_PDU_COLLECTION -DPDU=H2C_Message 
test_asn_h2c_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) -L../util
test_asn_h2c_LDADD = $(AM_LDADD) -lbionet-util -lbionet-asn -lm

test_asn_c2h_DEPENDENCIES = libbionet-asn.la
test_asn_c2h_SOURCES = converter-sample.c pdu_collection.c
test_asn_c2h_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) -I../util -DASN_PDU_COLLECTION -DPDU=C2H_Message 
test_asn_c2h_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) -L../util
test_asn_c2h_LDADD = $(AM_LDADD) -lbionet-util -lbionet-asn -lm

test_asn_bdm_c2s_DEPENDENCIES = libbionet-asn.la
test_asn_bdm_c2s_SOURCES = converter-sample.c pdu_collection.c
test_asn_bdm_c2s_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) -I../util -DASN_PDU_COLLECTION -DPDU=BDM_C2S_Message 
test_asn_bdm_c2s_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) -L../util
test_asn_bdm_c2s_LDADD = $(AM_LDADD) -lbionet-util -lbionet-asn -lm

test_asn_bdm_s2c_DEPENDENCIES = libbionet-asn.la
test_asn_bdm_s2c_SOURCES = converter-sample.c pdu_collection.c
test_asn_bdm_s2c_CFLAGS = $(AM_CFLAGS) $(GLIB_CFLAGS) -I../util -DASN_PDU_COLLECTION -DPDU=BDM_S2C_Message 
test_asn_bdm_s2c_LDFLAGS = $(AM_LDFLAGS) $(GLIB_LIBS) -L../util
test_asn_bdm_s2c_LDADD = $(AM_LDADD) -lbionet-util -lbionet-asn -lm



endif

# 
# this rule removes all the autogenerated code from the asn1c compiler
#

clean-asn1: clean
	rm -f converter-sample.c pdu_collection.c ${ASN_MODULE_SOURCES} ${ASN_MODULE_HEADS}


# 
# this rule re-runs the asn1c compiler to re-produce all the ASN.1 C code,
# then applies our local patches
#

# Uncomment the following to have asn.c code be regenerated automatically
#$(BUILT_SOURCES): asn1-stamp

rebuild-asn1: asn1-stamp
asn1-stamp: $(ASN_SOURCE)
	asn1c -Werror -fnative-types -fskeletons-copy -pdu=auto -gen-PER $(ASN_SOURCE)
	patch --quiet -p1 < local-patch
	touch $@

