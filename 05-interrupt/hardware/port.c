#include <hardware/port.h>


uint8_t port_read8(uint16_t port) {
	uint8_t result;
	__asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void port_write8(uint16_t port, uint8_t data) {
	__asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

void port_write8_slow(uint16_t port, uint8_t data) {
	__asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (port));
}

uint16_t port_read16(uint16_t port) {
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void port_write16(uint16_t port, uint16_t data) {
	__asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t port_read32(uint16_t port) {
	uint32_t result;
	__asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void port_write32(uint16_t port, uint32_t data) {
	__asm__ volatile("outl %0, %1" : : "a"(data), "Nd" (port));
}