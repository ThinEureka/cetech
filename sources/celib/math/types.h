/***********************************************************************
**** Math types
***********************************************************************/

#ifndef CETECH_MATH_TYPES_H
#define CETECH_MATH_TYPES_H


/***********************************************************************
**** Vectors 2, 3, 4
***********************************************************************/

typedef float vec2f_t[2];
typedef float vec3f_t[3];
typedef float vec4f_t[4];

/***********************************************************************
**** Quaternion
***********************************************************************/

typedef float quatf_t[4];


/***********************************************************************
**** Matrix 3x3, 4x4
***********************************************************************/

typedef float mat33f_t[3 * 3];
typedef float mat44f_t[4 * 4];

#endif //CETECH_MATH_TYPES_H
