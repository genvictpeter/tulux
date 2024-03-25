/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "AM-PDU-Descriptions"
 * 	found in "schema/CAM.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_CAM_H_
#define	_CAM_H_


#include <asn_application.h>

/* Including external dependencies */
#include "ItsPduHeader.h"
#include "CoopAwareness.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CAM */
typedef struct CAM {
	ItsPduHeader_t	 header;
	CoopAwareness_t	 cam;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} CAM_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_CAM;

#ifdef __cplusplus
}
#endif

#endif	/* _CAM_H_ */
#include <asn_internal.h>
