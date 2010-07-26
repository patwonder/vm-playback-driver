/*
 * common.h
 *
 *  Created on: 2010-7-16
 *      Author: pat
 */

#pragma once

#ifndef COMMON_H_
#define COMMON_H_

//
// Common definitions shared by frontend & backend
//
namespace va {

const int PORT = 8199;
extern const char* const HOST_ADDR;
const int TCP_WINDOW_SIZE = 1048576;

}

#endif /* COMMON_H_ */
