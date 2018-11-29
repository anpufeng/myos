#include <hardware/idt.h>
#include <hardware/port.h>
#include <boot/print.h>

#define IDT_INTERRUPT_GATE	0xE
#define IDT_DESC_PRESENT	0x80

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define PIC_EOI		0x20		/* End-of-interrupt command code */



idt_t g_idt;

//in assembly
static void interrupt_ignore();
static void handle_interrupt_request0x00();
static void handle_interrupt_request0x01();
static void handle_interrupt_request0x02();
static void handle_interrupt_request0x03();
static void handle_interrupt_request0x04();
static void handle_interrupt_request0x05();
static void handle_interrupt_request0x06();
static void handle_interrupt_request0x07();
static void handle_interrupt_request0x08();
static void handle_interrupt_request0x09();
static void handle_interrupt_request0x0A();
static void handle_interrupt_request0x0B();
static void handle_interrupt_request0x0C();
static void handle_interrupt_request0x0D();
static void handle_interrupt_request0x0E();
static void handle_interrupt_request0x0F();
static void handle_interrupt_request0x31();

static void handle_interrupt_request0x80();

static void handle_exception0x00();
static void handle_exception0x01();
static void handle_exception0x02();
static void handle_exception0x03();
static void handle_exception0x04();
static void handle_exception0x05();
static void handle_exception0x06();
static void handle_exception0x07();
static void handle_exception0x08();
static void handle_exception0x09();
static void handle_exception0x0A();
static void handle_exception0x0B();
static void handle_exception0x0C();
static void handle_exception0x0D();
static void handle_exception0x0E();
static void handle_exception0x0F();
static void handle_exception0x10();
static void handle_exception0x11();
static void handle_exception0x12();
static void handle_exception0x13();


void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = dest;
    for ( ; len != 0; len--) *temp++ = val;
} 
static void __set_idt_entry(uint8_t interrupt, uint16_t code_segment, void (*handler)(), uint8_t level, uint8_t type) {
    // address of pointer to code segment (relative to global descriptor table)
    // and address of the handler (relative to segment)
    g_idt.idt[interrupt].handler_address_low_bits = ((uint32_t) handler) & 0xFFFF;
    g_idt.idt[interrupt].handler_address_high_bits = (((uint32_t) handler) >> 16) & 0xFFFF;
    g_idt.idt[interrupt].gdt_code_segment_selector = code_segment;

    g_idt.idt[interrupt].access = IDT_DESC_PRESENT | ((level & 3) << 5) | type;
    g_idt.idt[interrupt].reserved = 0;
}


void idt_init(uint16_t hardware_interrupt_offset, gdt_t *gdt) {
    memory_set(&g_idt, 0, sizeof(gdt_t));
    g_idt.hardware_interrupt_offset = hardware_interrupt_offset;
    g_idt.master_command_port = PIC1_COMMAND;
    g_idt.master_data_port = PIC1_DATA;
    g_idt.slave_command_port = PIC2_COMMAND;
    g_idt.slave_data_port = PIC2_DATA;

	uint32_t code_segment =  gdt_code_segment_selector(gdt);
    for (uint8_t i = 255; i > 0; i--) {
        __set_idt_entry(i, code_segment, &interrupt_ignore, 0, IDT_INTERRUPT_GATE);
        g_idt.handlers[i] = 0;
    }
    __set_idt_entry(0, code_segment, &interrupt_ignore, 0, IDT_INTERRUPT_GATE);
    g_idt.handlers[0] = 0;
    uint16_t offset = g_idt.hardware_interrupt_offset;
    printf("code segment value:");
    printf_hex32(code_segment);
    printf("\n");
    printf("offset: ");
    printf_hex16(offset);
    printf("\n");


#define SET_IDT_ENTRY(interrupt) \
    __set_idt_entry( interrupt, code_segment, &handle_exception##interrupt, 0, IDT_INTERRUPT_GATE);
#define SET_IDT_OFFSET_ENTRY(interrupt) \
    __set_idt_entry(offset + interrupt, code_segment, &handle_interrupt_request##interrupt, 0, IDT_INTERRUPT_GATE);

    SET_IDT_ENTRY(0x00);
    SET_IDT_ENTRY(0x01);
    SET_IDT_ENTRY(0x02);
    SET_IDT_ENTRY(0x03);
    SET_IDT_ENTRY(0x04);
    SET_IDT_ENTRY(0x05);
    SET_IDT_ENTRY(0x06);
    SET_IDT_ENTRY(0x07);
    SET_IDT_ENTRY(0x08);
    SET_IDT_ENTRY(0x09);
    SET_IDT_ENTRY(0x0A);
    SET_IDT_ENTRY(0x0B);
    SET_IDT_ENTRY(0x0C);
    SET_IDT_ENTRY(0x0D);
    SET_IDT_ENTRY(0x0E);
    SET_IDT_ENTRY(0x0F);
    SET_IDT_ENTRY(0x10);
    SET_IDT_ENTRY(0x11);
    SET_IDT_ENTRY(0x12);
    SET_IDT_ENTRY(0x13);

    SET_IDT_OFFSET_ENTRY(0x00);
    SET_IDT_OFFSET_ENTRY(0x01);
    SET_IDT_OFFSET_ENTRY(0x02);
    SET_IDT_OFFSET_ENTRY(0x03);
    SET_IDT_OFFSET_ENTRY(0x04);
    SET_IDT_OFFSET_ENTRY(0x05);
    SET_IDT_OFFSET_ENTRY(0x06);
    SET_IDT_OFFSET_ENTRY(0x07);
    SET_IDT_OFFSET_ENTRY(0x08);
    SET_IDT_OFFSET_ENTRY(0x09);
    SET_IDT_OFFSET_ENTRY(0x0A);
    SET_IDT_OFFSET_ENTRY(0x0B);
    SET_IDT_OFFSET_ENTRY(0x0C);
    SET_IDT_OFFSET_ENTRY(0x0D);
    SET_IDT_OFFSET_ENTRY(0x0E);
    SET_IDT_OFFSET_ENTRY(0x0F);
    __set_idt_entry(0x80, code_segment, &handle_interrupt_request0x80, 0, IDT_INTERRUPT_GATE);
#undef SET_IDT_ENTRY
#undef SET_IDT_OFFSET_ENTRY

/*
    __set_idt_entry(0x00, code_segment, &handle_exception0x00, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x01, code_segment, &handle_exception0x01, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x02, code_segment, &handle_exception0x02, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x03, code_segment, &handle_exception0x03, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x04, code_segment, &handle_exception0x04, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x05, code_segment, &handle_exception0x05, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x06, code_segment, &handle_exception0x06, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x07, code_segment, &handle_exception0x07, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x08, code_segment, &handle_exception0x08, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x09, code_segment, &handle_exception0x09, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x0A, code_segment, &handle_exception0x0A, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x0B, code_segment, &handle_exception0x0B, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x0C, code_segment, &handle_exception0x0C, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x0D, code_segment, &handle_exception0x0D, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x0E, code_segment, &handle_exception0x0E, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x0F, code_segment, &handle_exception0x0F, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x10, code_segment, &handle_exception0x10, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x11, code_segment, &handle_exception0x11, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x12, code_segment, &handle_exception0x12, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(0x13, code_segment, &handle_exception0x13, 0, IDT_INTERRUPT_GATE);

    __set_idt_entry(offset + 0x00, code_segment, &handle_interrupt_request0x00, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x01, code_segment, &handle_interrupt_request0x01, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x02, code_segment, &handle_interrupt_request0x02, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x03, code_segment, &handle_interrupt_request0x03, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x04, code_segment, &handle_interrupt_request0x04, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x05, code_segment, &handle_interrupt_request0x05, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x06, code_segment, &handle_interrupt_request0x06, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x07, code_segment, &handle_interrupt_request0x07, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x08, code_segment, &handle_interrupt_request0x08, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x09, code_segment, &handle_interrupt_request0x09, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x0A, code_segment, &handle_interrupt_request0x0A, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x0B, code_segment, &handle_interrupt_request0x0B, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x0C, code_segment, &handle_interrupt_request0x0C, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x0D, code_segment, &handle_interrupt_request0x0D, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x0E, code_segment, &handle_interrupt_request0x0E, 0, IDT_INTERRUPT_GATE);
    __set_idt_entry(offset + 0x0F, code_segment, &handle_interrupt_request0x0F, 0, IDT_INTERRUPT_GATE);

    __set_idt_entry(          0x80, code_segment, &handle_interrupt_request0x80, 0, IDT_INTERRUPT_GATE);
*/
    port_write8_slow(g_idt.master_command_port, (uint8_t)0x11);
    port_write8_slow(g_idt.slave_command_port, 0x11);
//
    port_write8_slow(g_idt.master_data_port, (uint8_t)offset);
    port_write8_slow(g_idt.slave_data_port, (uint8_t)offset + 8);

    port_write8_slow(g_idt.master_data_port, 0x04);
    port_write8_slow(g_idt.slave_data_port, 0x02);

    port_write8_slow(g_idt.master_data_port, 0x01);
    port_write8_slow(g_idt.slave_data_port, 0x01);

    port_write8_slow(g_idt.master_data_port, 0x00);
    port_write8_slow(g_idt.slave_data_port, 0x00);

    idt_pointer_t idt_pointer;
    idt_pointer.size  = 256*sizeof(gate_descriptor_t) - 1;
    idt_pointer.base  = (uint32_t)g_idt.idt;
    __asm__ volatile("lidt %0" : : "m" (idt_pointer));
    printf("idt_init\n");
}

void idt_active() {
    if (g_idt.activated) {
        idt_deactive();
    }
    g_idt.activated = true;
    __asm__("sti");
    printf("idt_active\n");
}

void idt_deactive() {
    if (g_idt.activated) {
        g_idt.activated = false;
        __asm__("cli");
        printf("idt_deactive\n");
    }
}

static uint32_t __interrupt_manager_do_handle(uint8_t interrupt, uint32_t esp) {
    if(g_idt.handlers[interrupt] != 0) {
//        esp = g_idt.handlers[interrupt]->HandleInterrupt(esp);
    } else if(interrupt != g_idt.hardware_interrupt_offset) {
        printf("UNHANDLED INTERRUPT 0x");
        printf_hex(interrupt);
    }
    
    // if(interrupt == hardwareInterruptOffset) {
    //     esp = (uint32_t)taskg_idt.Schedule((CPUState*)esp);
    // }

    // hardware interrupts must be acknowledged
    if(g_idt.hardware_interrupt_offset <= interrupt && interrupt < g_idt.hardware_interrupt_offset+16) {
        port_write8_slow(g_idt.master_command_port, PIC_EOI);
        printf("port_write8_slow, interrupt: 0X ");
        printf_hex(interrupt);
        printf("\n");
        if(g_idt.hardware_interrupt_offset + 8 <= interrupt) {
            port_write8_slow(g_idt.slave_command_port, PIC_EOI);
        }
    }

    return esp;
}
uint32_t idt_handle(uint8_t interrupt, uint32_t esp) {
    printf("interrupt: 0X");
    printf_hex(interrupt);
    printf("\nidt_handle\n");

    return __interrupt_manager_do_handle(interrupt, esp);
}

void idt_deinit() {
    idt_deactive();
}