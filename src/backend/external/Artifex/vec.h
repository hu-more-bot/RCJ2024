/**
* @brief C Vector Library for Artifex
*
* @date 2024.06.xx TODO
* @author SamTheDev
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Vector Component Type
#define AX_VECTOR_TYPE float

// 2D Vector
typedef struct {
    AX_VECTOR_TYPE x, y;
} axVector;

axVector axVectorAdd(axVector a, axVector b);
axVector axVectorSubtract(axVector a, axVector b);
axVector axVectorMultiply(axVector a, axVector b);
axVector axVectorDivide(axVector a, axVector b);

int axVectorCompare(axVector a, axVector b);

AX_VECTOR_TYPE axVectorLength(axVector a);
axVector axVectorNormalize(axVector a);

AX_VECTOR_TYPE axVectorDistance(axVector a, axVector b);
AX_VECTOR_TYPE axVectorDotProduct(axVector a);
axVector axVectorLerp(axVector a, axVector b, float blend);

#ifdef __cplusplus
}
#endif