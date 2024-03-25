/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "AM-PDU-Descriptions"
 * 	found in "schema/CAM.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#include "SpecialVehicleContainer.h"

static asn_per_constraints_t asn_PER_type_SpecialVehicleContainer_constr_1 GCC_NOTUSED = {
	{ APC_CONSTRAINED | APC_EXTENSIBLE,  3,  3,  0,  6 }	/* (0..6,...) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_SpecialVehicleContainer_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialVehicleContainer, choice.publicTransportContainer),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_PublicTransportContainer,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"publicTransportContainer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialVehicleContainer, choice.specialTransportContainer),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SpecialTransportContainer,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"specialTransportContainer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialVehicleContainer, choice.dangerousGoodsContainer),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_DangerousGoodsContainer,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"dangerousGoodsContainer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialVehicleContainer, choice.roadWorksContainerBasic),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RoadWorksContainerBasic,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"roadWorksContainerBasic"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialVehicleContainer, choice.rescueContainer),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_RescueContainer,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"rescueContainer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialVehicleContainer, choice.emergencyContainer),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_EmergencyContainer,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"emergencyContainer"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SpecialVehicleContainer, choice.safetyCarContainer),
		(ASN_TAG_CLASS_CONTEXT | (6 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SafetyCarContainer,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"safetyCarContainer"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_SpecialVehicleContainer_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* publicTransportContainer */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* specialTransportContainer */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* dangerousGoodsContainer */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* roadWorksContainerBasic */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* rescueContainer */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 }, /* emergencyContainer */
    { (ASN_TAG_CLASS_CONTEXT | (6 << 2)), 6, 0, 0 } /* safetyCarContainer */
};
static asn_CHOICE_specifics_t asn_SPC_SpecialVehicleContainer_specs_1 = {
	sizeof(struct SpecialVehicleContainer),
	offsetof(struct SpecialVehicleContainer, _asn_ctx),
	offsetof(struct SpecialVehicleContainer, present),
	sizeof(((struct SpecialVehicleContainer *)0)->present),
	asn_MAP_SpecialVehicleContainer_tag2el_1,
	7,	/* Count of tags in the map */
	0,
	7	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_SpecialVehicleContainer = {
	"SpecialVehicleContainer",
	"SpecialVehicleContainer",
	CHOICE_free,
	CHOICE_print,
	CHOICE_constraint,
	CHOICE_decode_ber,
	CHOICE_encode_der,
	CHOICE_decode_xer,
	CHOICE_encode_xer,
	CHOICE_decode_uper,
	CHOICE_encode_uper,
	0, 0,	/* No OER support, use "-gen-OER" to enable */
	CHOICE_outmost_tag,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	&asn_PER_type_SpecialVehicleContainer_constr_1,
	asn_MBR_SpecialVehicleContainer_1,
	7,	/* Elements count */
	&asn_SPC_SpecialVehicleContainer_specs_1	/* Additional specs */
};

