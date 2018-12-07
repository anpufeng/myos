## 加载IDT(Interrupt Descriptor Table)

#### IDT概述

`IDT`是一个保护模式下的中断描述符表, 实模式下与之对应的为Interrupt Vector Table([`IVT`](https://wiki.osdev.org/IVT)). `IDT`用来告诉CPU [Interrupt Service Routines(ISR)](https://wiki.osdev.org/Interrupt_Service_Routines) 的位置, 和之前讲的[`GDT`](https://github.com/anpufeng/myos/tree/master/03-gdt)有类似之处, `IDT`的位置(地址和大小)保存在寄存器`IDTR`, 加载指令为`LIDT`, 当有中断发生时, CPU会根据中断号查看`IDT`表(一共有256个中断, 所以IDT表是一个包含有256个元素的数组), 找到并执行需要调用的方法.

#### IDT结构
 `IDT`内每个元素称之为`GATE`, 结构如下 
```CPP
typedef struct idt_descr_t {
	uint16_t    offset_1;   // offset bits 0..15
	uint16_t    selector;   // a code segment selector in GDT or LDT
	uint8_t     zero;       // unused, set to 0
	uint8_t     type_attr;  // type and attributes, see below
	uint16_t    offset_2;   // offset bits 16..31
} __attribute__((packed)) idt_descr_t;
```

定义`IDT`为如下结构体
```CPP
typedef struct idt_t {
	idt_descr_t     idts[256];		//interrupt descriptor table
	uint16_t        hardware_interrupt_offset;
	uint16_t        master_command_port;
	uint16_t        master_data_port;
	uint16_t        slave_command_port;
	uint16_t        slave_data_port;
	bool            activated;
} idt_t;
```

初始化方法如下

```CPP
static void __set_idt_entry(uint8_t interrupt, uint16_t code_segment, void (*handler)(), uint8_t level, uint8_t type) {
    // address of pointer to code segment (relative to global descriptor table)
    // and address of the handler (relative to segment)
    g_idt.idts[interrupt].offset_1 = ((uint32_t) handler) & 0xFFFF;
    g_idt.idts[interrupt].offset_2 = (((uint32_t) handler) >> 16) & 0xFFFF;
    g_idt.idts[interrupt].selector = code_segment;

    g_idt.idts[interrupt].type_attr = IDT_DESC_PRESENT | ((level & 3) << 5) | type;
    g_idt.idts[interrupt].zero = 0;
}


void idt_init(uint16_t hardware_interrupt_offset, gdt_t *gdt) {
    g_idt.hardware_interrupt_offset = hardware_interrupt_offset;
    g_idt.master_command_port = PIC1_COMMAND;
    g_idt.master_data_port = PIC1_DATA;
    g_idt.slave_command_port = PIC2_COMMAND;
    g_idt.slave_data_port = PIC2_DATA;

	uint32_t code_segment =  gdt_code_segment_selector(gdt);
    for (uint8_t i = 255; i > 0; --i) {
        __set_idt_entry(i, code_segment, &interrupt_ignore, 0, IDT_INTERRUPT_GATE);
    }
    __set_idt_entry(0, code_segment, &interrupt_ignore, 0, IDT_INTERRUPT_GATE);
    uint16_t offset = g_idt.hardware_interrupt_offset;

#define SET_ISR(interrupt) \
    __set_idt_entry(interrupt, code_segment, &handle_exception##interrupt, 0, IDT_INTERRUPT_GATE);
#define SET_IRQ(interrupt) \
    __set_idt_entry(offset + interrupt, code_segment, &irq##interrupt, 0, IDT_INTERRUPT_GATE);

//    SET_ISR(0x00);
//    SET_ISR(0x01);
//    SET_ISR(0x02);
//    SET_ISR(0x03);
//    SET_ISR(0x04);
//    SET_ISR(0x05);
//    SET_ISR(0x06);
//    SET_ISR(0x07);
//    SET_ISR(0x08);
//    SET_ISR(0x09);
//    SET_ISR(0x0A);
//    SET_ISR(0x0B);
//    SET_ISR(0x0C);
//    SET_ISR(0x0D);
//    SET_ISR(0x0E);
//    SET_ISR(0x0F);
//    SET_ISR(0x10);
//    SET_ISR(0x11);
//    SET_ISR(0x12);
//    SET_ISR(0x13);

    SET_IRQ(0x00);
    SET_IRQ(0x01);
    SET_IRQ(0x02);
    SET_IRQ(0x03);
    SET_IRQ(0x04);
    SET_IRQ(0x05);
    SET_IRQ(0x06);
    SET_IRQ(0x07);
    SET_IRQ(0x08);
    SET_IRQ(0x09);
    SET_IRQ(0x0A);
    SET_IRQ(0x0B);
    SET_IRQ(0x0C);
    SET_IRQ(0x0D);
    SET_IRQ(0x0E);
    SET_IRQ(0x0F);
    __set_idt_entry(0x80, code_segment, &irq0x80, 0, IDT_INTERRUPT_GATE);
#undef SET_ISR
#undef SET_IRQ

    /* remapping the PIC */
    port_write8_slow(g_idt.master_command_port, 0x11);
    port_write8_slow(g_idt.slave_command_port, 0x11);
//
    port_write8_slow(g_idt.master_data_port, offset);
    port_write8_slow(g_idt.slave_data_port, offset + 8);

    port_write8_slow(g_idt.master_data_port, 0x04);
    port_write8_slow(g_idt.slave_data_port, 0x02);

    port_write8_slow(g_idt.master_data_port, 0x01);
    port_write8_slow(g_idt.slave_data_port, 0x01);

    port_write8_slow(g_idt.master_data_port, 0x00);
    port_write8_slow(g_idt.slave_data_port, 0x00);

    idt_pointer_t idt_pointer;
    idt_pointer.size  = 256*sizeof(idt_descr_t) - 1;
    idt_pointer.base  = (uint32_t)g_idt.idts;
    __asm__ volatile("lidt %0" : : "m" (idt_pointer));
    printf("idt_init\n");
}
```


 
声明了一些需要处理的中断函数, 并初始化在对应的IDT索引内, 
我们先初始化了`PIC`的`Master`和`Slave`的端口号, 
实模式下

    | Chip        | Interrupt numbers (IRQ) | Vector offset |  Interrupt Numbers|
    | ------------| ----------------------- |---------------|------------------ |
    | Master PIC  | 0 to 7                  | 0x08          | 0x08 to 0x0F      |
    | Slave PIC   | 8 to 15                 | 0x70          | 0x70 to 0x77      |
我们这里使用的是保护模式下的, 因为0到0x1F中断已经被INTERL保留使用了, 所以这里使用以0X20为offset(IRQs 0..0xF -> INT 0x20..0x2F), 因为要整除8所以`master offset` 0x20 `slave offset` 0x28
同时我们需要初始化`PIC`, 至于为何设置这些数据及其意义所在, 参见https://wiki.osdev.org/8259_PIC#Initialisation
`idt.c`只是声明了中断处理函数, 具体函数的定义在`idtstubs.s`内
主要中断逻辑就是先保存当前一些寄存器状态, 然后调用了在`idt.c`内的中断处理函数, 后面恢复寄存器状态.


#### 运行结果
我们收到了中断信号, 至于为何只收到一次, 是因为我们还没有回复`EOI`end of interrupt, 这个留待后续处理
<img src="https://github.com/anpufeng/myos/blob/master/image/05-idt.jpg" width="600" height="500">





