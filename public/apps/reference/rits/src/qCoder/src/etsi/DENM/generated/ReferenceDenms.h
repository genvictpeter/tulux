/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "DENM-PDU-Descriptions"
 * 	found in "schema/DENM.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_ReferenceDenms_H_
#define	_ReferenceDenms_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct ActionID;

/* ReferenceDenms */
typedef struct ReferenceDenms {
	A_SEQUENCE_OF(struct ActionID) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ReferenceDenms_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ReferenceDenms;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "ActionID.h"

#endif	/* _ReferenceDenms_H_ */
#include <asn_internal.h>