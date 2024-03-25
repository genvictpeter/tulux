/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./schema/ITS-Container.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#include "DeltaReferencePosition.h"

static asn_TYPE_member_t asn_MBR_DeltaReferencePosition_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct DeltaReferencePosition, deltaLatitude),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DeltaLatitude,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"deltaLatitude"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DeltaReferencePosition, deltaLongitude),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DeltaLongitude,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"deltaLongitude"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct DeltaReferencePosition, deltaAltitude),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DeltaAltitude,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"deltaAltitude"
		},
};
static const ber_tlv_tag_t asn_DEF_DeltaReferencePosition_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_DeltaReferencePosition_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* deltaLatitude */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* deltaLongitude */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* deltaAltitude */
};
static asn_SEQUENCE_specifics_t asn_SPC_DeltaReferencePosition_specs_1 = {
	sizeof(struct DeltaReferencePosition),
	offsetof(struct DeltaReferencePosition, _asn_ctx),
	asn_MAP_DeltaReferencePosition_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_DeltaReferencePosition = {
	"DeltaReferencePosition",
	"DeltaReferencePosition",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0, 0,	/* No OER support, use "-gen-OER" to enable */
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_DeltaReferencePosition_tags_1,
	sizeof(asn_DEF_DeltaReferencePosition_tags_1)
		/sizeof(asn_DEF_DeltaReferencePosition_tags_1[0]), /* 1 */
	asn_DEF_DeltaReferencePosition_tags_1,	/* Same as above */
	sizeof(asn_DEF_DeltaReferencePosition_tags_1)
		/sizeof(asn_DEF_DeltaReferencePosition_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_DeltaReferencePosition_1,
	3,	/* Elements count */
	&asn_SPC_DeltaReferencePosition_specs_1	/* Additional specs */
};

