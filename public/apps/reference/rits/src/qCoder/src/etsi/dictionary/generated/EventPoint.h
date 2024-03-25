/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./schema/ITS-Container.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_EventPoint_H_
#define	_EventPoint_H_


#include <asn_application.h>

/* Including external dependencies */
#include "DeltaReferencePosition.h"
#include "PathDeltaTime.h"
#include "InformationQuality.h"
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* EventPoint */
typedef struct EventPoint {
	DeltaReferencePosition_t	 eventPosition;
	PathDeltaTime_t	*eventDeltaTime	/* OPTIONAL */;
	InformationQuality_t	 informationQuality;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} EventPoint_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_EventPoint;

#ifdef __cplusplus
}
#endif

#endif	/* _EventPoint_H_ */
#include <asn_internal.h>
