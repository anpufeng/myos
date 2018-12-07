#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>
#include <boot/gdt.h>

//refer: https://wiki.osdev.org/Descriptor#Interrupt_Gate_Descriptor https://wiki.osdev.org/IDT#Structure_IA-32

typedef struct idt_descr_t {
	uint16_t    offset_1;   // offset bits 0..15
	uint16_t    selector;   // a code segment selector in GDT or LDT
	uint8_t     zero;       // unused, set to 0
	uint8_t     type_attr;  // type and attributes, see below
	uint16_t    offset_2;   // offset bits 16..31
} __attribute__((packed)) idt_descr_t;

typedef struct idt_pointer_t {
	uint16_t 	size;
	uint32_t 	base;
} __attribute__((packed)) idt_pointer_t;

typedef struct idt_t {
	idt_descr_t         idts[256];		//interrupt descriptor table
	uint16_t			hardware_interrupt_offset;
	bool				activated;
} idt_t;

void idt_init(uint16_t hardware_interrupt_offset, gdt_t *gdt);
void idt_active();
void idt_deactive();
uint32_t idt_handle(uint8_t interrupt, uint32_t esp);

#endif 
