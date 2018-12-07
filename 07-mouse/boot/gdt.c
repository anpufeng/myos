#include <boot/gdt.h>
#include <boot/print.h>

gdt_t g_gdt;

void gdt_entry_init(gdt_entry_t *entry, uint32_t base, uint32_t limit, uint8_t type) {
	uint8_t* target = (uint8_t*)entry;
    if (limit <= 65536) {
        // 16-bit address space
        target[6] = 0x40;
    } else {
        // 32-bit address space
        // Now we have to squeeze the (32-bit) limit into 2.5 regiters (20-bit).
        // This is done by discarding the 12 least significant bits, but this
        // is only legal, if they are all ==1, so they are implicitly still there

        // so if the last bits aren't all 1, we have to set them to 1, but this
        // would increase the limit (cannot do that, because we might go beyond
        // the physical limit or get overlap with other segments) so we have to
        // compensate this by decreasing a higher bit (and might have up to
        // 4095 wasted bytes behind the used memory)
		if ((limit & 0xFFF) != 0xFFF) {
			limit = (limit >> 12)-1;
        } else {
        	limit = limit >> 12;
        }

        target[6] = 0xC0;
    }

    // Encode the limit
    target[0] = limit & 0xFF;
    target[1] = (limit >> 8) & 0xFF;
    target[6] |= (limit >> 16) & 0xF;

    // Encode the base
    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;

    // Type
    target[5] = type;
}

uint32_t gdt_entry_base(gdt_entry_t *entry) {
	uint8_t *target = (uint8_t*)entry;

    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];

    return result;
}

uint32_t gdt_entry_limit(gdt_entry_t *entry) {
	uint8_t *target = (uint8_t*)entry;

    uint32_t result = target[6] & 0xF;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];

    if((target[6] & 0xC0) == 0xC0) {
        result = (result << 12) | 0xFFF;
    }

    return result;
}

/*
                           Pr  Priv  S   Ex  DC   RW   Ac
     0x9A == 1001 1010  == 1   00    1   1   0    1    0
     0x92 == 1001 0010  == 1   00    1   0   0    1    0
   */
void gdt_init() {
    gdt_entry_init(&g_gdt.null_segment_selector, 0, 0, 0);
    gdt_entry_init(&g_gdt.code_segment_selector, 0, 64 * 1024 * 1024, 0x9A);
    gdt_entry_init(&g_gdt.data_segment_selector, 0, 64 * 1024 * 1024, 0x92);

    uint32_t i[2];
    i[1] = (uint32_t)&g_gdt;
    i[0] = sizeof(gdt_t) << 16;

    __asm__ volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));
     printf("gdt_init\n");
}

uint16_t gdt_code_segment_selector() {
	return (uint8_t*)&g_gdt.code_segment_selector - (uint8_t*)&g_gdt;
}

uint16_t gdt_data_segment_selector() {
	return (uint8_t*)&g_gdt.data_segment_selector - (uint8_t*)&g_gdt;
}