#ifndef __GDT_H__
#define __GDT_H__

#include <types.h>

//https://wiki.osdev.org/GDT_Tutorial

typedef struct segment_descriptor_t {
	uint16_t 	limit_lo;
	uint16_t 	base_lo;
	uint8_t 	base_hi;
	uint8_t 	type;
	uint8_t 	limit_hi;
	uint8_t 	base_vhi;

} __attribute__((packed)) segment_descriptor_t;

typedef struct gdt_t {
	segment_descriptor_t null_segment_selector;
	segment_descriptor_t unused_segment_selector;
	segment_descriptor_t code_segment_selector;
	segment_descriptor_t data_segment_selector;
} gdt_t;

void segment_descriptor_init(segment_descriptor_t *sd, uint32_t base, uint32_t limit, uint8_t type);
uint32_t segment_descriptor_base(segment_descriptor_t *sd);
uint32_t segment_descriptor_limit(segment_descriptor_t *sd);
void segment_descriptor_deinit(segment_descriptor_t *sd);


extern gdt_t g_gdt;


void gdt_init();
uint16_t gdt_code_segment_selector();
uint16_t gdt_data_segment_selector();
void gdt_deinit();

#endif