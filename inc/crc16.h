/*
 *	crc16.h - CRC-16 routine
 *
 * Implements the standard CRC-16:
 *   Width 16
 *   Poly  0x8005 (x^16 + x^15 + x^2 + 1)
 *   Init  0
 *
 * Copyright (c) 2005 Ben Gardner <bgardner@wabtec.com>
 *
 * This source code is licensed under the GNU General Public License,
 * Version 2. See the file COPYING for more details.
 */

#ifndef __CRC16_H
#define __CRC16_H

//#include <linux/types.h>
#include <stdint.h>

//extern uint16_t const crc16_table[256];

uint16_t crc16_8005(uint16_t crc, const uint8_t *buffer, unsigned int len);
uint16_t CRC16_modbus_rtu(uint8_t *puchMsg, uint32_t usDataLen);
uint16_t crc16_1021(unsigned char *buf, int len);

#endif /* __CRC16_H */

