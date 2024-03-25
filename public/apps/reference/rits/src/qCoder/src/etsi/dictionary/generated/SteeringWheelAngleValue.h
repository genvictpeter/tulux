/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./schema/ITS-Container.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_SteeringWheelAngleValue_H_
#define	_SteeringWheelAngleValue_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SteeringWheelAngleValue {
	SteeringWheelAngleValue_straight	= 0,
	SteeringWheelAngleValue_onePointFiveDegreesToRight	= -1,
	SteeringWheelAngleValue_onePointFiveDegreesToLeft	= 1,
	SteeringWheelAngleValue_unavailable	= 512
} e_SteeringWheelAngleValue;

/* SteeringWheelAngleValue */
typedef long	 SteeringWheelAngleValue_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SteeringWheelAngleValue;
asn_struct_free_f SteeringWheelAngleValue_free;
asn_struct_print_f SteeringWheelAngleValue_print;
asn_constr_check_f SteeringWheelAngleValue_constraint;
ber_type_decoder_f SteeringWheelAngleValue_decode_ber;
der_type_encoder_f SteeringWheelAngleValue_encode_der;
xer_type_decoder_f SteeringWheelAngleValue_decode_xer;
xer_type_encoder_f SteeringWheelAngleValue_encode_xer;
per_type_decoder_f SteeringWheelAngleValue_decode_uper;
per_type_encoder_f SteeringWheelAngleValue_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _SteeringWheelAngleValue_H_ */
#include <asn_internal.h>
