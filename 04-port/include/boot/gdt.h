#ifndef __GDT_H__
#define __GDT_H__

#include <types.h>

//https://wiki.osdev.org/GDT_Tutorial

typedef struct segment_descriptor_t {
	uint16_t 	limit_lo;
	uint16_t 	base_lo;
	uint8_t 	base_hi;
	uint8_t 	access;
	uint8_t 	limit_hi;
	uint8_t 	base_vhi;

} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_t {
	gdt_entry_t null_segment_selector;
	gdt_entry_t unused_segment_selector;
	gdt_entry_t code_segment_selector;
	gdt_entry_t data_segment_selector;
} gdt_t;

void gdt_entry_init(gdt_entry_t *entry, uint32_t base, uint32_t limit, uint8_t type);
uint32_t gdt_entry_base(gdt_entry_t *entry);
uint32_t gdt_entry_limit(gdt_entry_t *entry);
void gdt_entry_deinit(gdt_entry_t *entry);


extern gdt_t g_gdt;


void gdt_init();
uint16_t gdt_code_segment_selector();
uint16_t gdt_data_segment_selector();
void gdt_deinit();

#endif