/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "ITS-Container"
 * 	found in "./schema/ITS-Container.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_PathHistory_H_
#define	_PathHistory_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct PathPoint;

/* PathHistory */
typedef struct PathHistory {
	A_SEQUENCE_OF(struct PathPoint) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} PathHistory_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_PathHistory;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "PathPoint.h"

#endif	/* _PathHistory_H_ */
#include <asn_internal.h>
