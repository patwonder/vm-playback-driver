/*
 * MessageTypes.h
 *  
 *  Created on: 2010-7-19
 *      Author: pat
 */

#pragma once

#ifndef MESSAGETYPES_H_
#define MESSAGETYPES_H_

#ifdef WIN32

typedef __int64 int64;
typedef unsigned __int64 uint64;

#else

#include <inttypes.h>

namespace va {
typedef int64_t int64;
typedef uint64_t uint64;
}

#endif // WIN32

#endif /* MESSAGETYPES_H_ */
