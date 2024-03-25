/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./schema/ITS-Container.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_HazardousLocation_DangerousCurveSubCauseCode_H_
#define	_HazardousLocation_DangerousCurveSubCauseCode_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum HazardousLocation_DangerousCurveSubCauseCode {
	HazardousLocation_DangerousCurveSubCauseCode_unavailable	= 0,
	HazardousLocation_DangerousCurveSubCauseCode_dangerousLeftTurnCurve	= 1,
	HazardousLocation_DangerousCurveSubCauseCode_dangerousRightTurnCurve	= 2,
	HazardousLocation_DangerousCurveSubCauseCode_multipleCurvesStartingWithUnknownTurningDirection	= 3,
	HazardousLocation_DangerousCurveSubCauseCode_multipleCurvesStartingWithLeftTurn	= 4,
	HazardousLocation_DangerousCurveSubCauseCode_multipleCurvesStartingWithRightTurn	= 5
} e_HazardousLocation_DangerousCurveSubCauseCode;

/* HazardousLocation-DangerousCurveSubCauseCode */
typedef long	 HazardousLocation_DangerousCurveSubCauseCode_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_HazardousLocation_DangerousCurveSubCauseCode;
asn_struct_free_f HazardousLocation_DangerousCurveSubCauseCode_free;
asn_struct_print_f HazardousLocation_DangerousCurveSubCauseCode_print;
asn_constr_check_f HazardousLocation_DangerousCurveSubCauseCode_constraint;
ber_type_decoder_f HazardousLocation_DangerousCurveSubCauseCode_decode_ber;
der_type_encoder_f HazardousLocation_DangerousCurveSubCauseCode_encode_der;
xer_type_decoder_f HazardousLocation_DangerousCurveSubCauseCode_decode_xer;
xer_type_encoder_f HazardousLocation_DangerousCurveSubCauseCode_encode_xer;
per_type_decoder_f HazardousLocation_DangerousCurveSubCauseCode_decode_uper;
per_type_encoder_f HazardousLocation_DangerousCurveSubCauseCode_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _HazardousLocation_DangerousCurveSubCauseCode_H_ */
#include <asn_internal.h>
