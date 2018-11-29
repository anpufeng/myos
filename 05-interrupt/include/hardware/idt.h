#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <types.h>
#include <boot/gdt.h>


typedef struct interrupt_handler_t {
	uint8_t 	interrutpt_number;
	void *		interrupt_manager;
} interrupt_handler_t;

//https://wiki.osdev.org/Descriptor https://wiki.osdev.org/IDT#Structure_IA-32
//todo rename variable name
typedef struct gate_descriptor_t {
	uint16_t	handler_address_low_bits;
	uint16_t 	gdt_code_segment_selector;
	uint8_t 	reserved;
	uint8_t 	access;
  	uint16_t 	handler_address_high_bits;
} __attribute__((packed)) gate_descriptor_t;

typedef struct idt_pointer_t {
	uint16_t 	size;
	uint32_t 	base;
} __attribute__((packed)) idt_pointer_t;

typedef struct idt_t {
	interrupt_handler_t *handlers[256];
	gate_descriptor_t	idt[256];		//interrupt descriptor table
	uint16_t			hardware_interrupt_offset;
	uint16_t			master_command_port;
	uint16_t			master_data_port;
	uint16_t			slave_command_port;
	uint16_t			slave_data_port;
	bool				activated;

} idt_t;

void idt_init(uint16_t hardware_interrupt_offset, gdt_t *gdt);
void idt_active();
void idt_deactive();
uint32_t idt_handle(uint8_t interrupt, uint32_t esp);
void idt_deinit();

#endif 
