#ifndef __PORT_H__
#define __PORT_H__

#include <types.h>

uint8_t port_read8(uint16_t port);
void port_write8(uint16_t port, uint8_t data);
void port_write8_slow(uint16_t port, uint8_t data);

uint16_t port_read16(uint16_t port);
void port_write16(uint16_t port, uint16_t data);

uint32_t port_read32(uint16_t port);
void port_write32(uint16_t port, uint32_t data);

#endif