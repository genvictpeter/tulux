/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./schema/ITS-Container.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_SemiAxisLength_H_
#define	_SemiAxisLength_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum SemiAxisLength {
	SemiAxisLength_oneCentimeter	= 1,
	SemiAxisLength_outOfRange	= 4094,
	SemiAxisLength_unavailable	= 4095
} e_SemiAxisLength;

/* SemiAxisLength */
typedef long	 SemiAxisLength_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_SemiAxisLength;
asn_struct_free_f SemiAxisLength_free;
asn_struct_print_f SemiAxisLength_print;
asn_constr_check_f SemiAxisLength_constraint;
ber_type_decoder_f SemiAxisLength_decode_ber;
der_type_encoder_f SemiAxisLength_encode_der;
xer_type_decoder_f SemiAxisLength_decode_xer;
xer_type_encoder_f SemiAxisLength_encode_xer;
per_type_decoder_f SemiAxisLength_decode_uper;
per_type_encoder_f SemiAxisLength_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _SemiAxisLength_H_ */
#include <asn_internal.h>
