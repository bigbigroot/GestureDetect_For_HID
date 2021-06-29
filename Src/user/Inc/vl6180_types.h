/**
 * @file vl6180_types.h
 * @author Weigen Huang (weigen.huang.k7e@fh-zwickau.de)
 * @brief 
 * @version 0.1
 * @date 2021-03-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef VL6180_TYPES_H_
#define VL6180_TYPES_H_


#include <stdint.h>
#include <stddef.h> /* these is for NULL */

#ifndef NULL
#error "review  NULL definition or add required include "
#endif

#if !defined(STDINT_H) &&  !defined(_GCC_STDINT_H) && !defined(__STDINT_DECLS) && !defined(_STDINT) && !defined(_STDINT_H)

#pragma message("Please review  type definition of STDINT define for your platform and add to list above ")

 /*
  *  target platform do not provide stdint or use a different #define than above
  *  to avoid seeing the message below addapt the #define list above or implement
  *  all type and delete these pragma
  */

typedef unsigned int uint32_t;
typedef int int32_t;

typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned char uint8_t;

typedef signed char int8_t;

#endif /* _STDINT_H */

#endif /* VL6180_TYPES_H_ */
