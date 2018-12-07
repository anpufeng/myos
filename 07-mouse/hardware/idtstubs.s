

.set IRQ_BASE, 0x20

.section .text

.extern idt_handle


.macro isr_macro num
.global isr\num\()
isr\num\():
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm


.macro irq_macro num
.global irq\num\()
irq\num\():
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm


isr_macro 0x00
isr_macro 0x01
isr_macro 0x02
isr_macro 0x03
isr_macro 0x04
isr_macro 0x05
isr_macro 0x06
isr_macro 0x07
isr_macro 0x08
isr_macro 0x09
isr_macro 0x0A
isr_macro 0x0B
isr_macro 0x0C
isr_macro 0x0D
isr_macro 0x0E
isr_macro 0x0F
isr_macro 0x10
isr_macro 0x11
isr_macro 0x12
isr_macro 0x13

irq_macro 0x00
irq_macro 0x01
irq_macro 0x02
irq_macro 0x03
irq_macro 0x04
irq_macro 0x05
irq_macro 0x06
irq_macro 0x07
irq_macro 0x08
irq_macro 0x09
irq_macro 0x0A
irq_macro 0x0B
irq_macro 0x0C
irq_macro 0x0D
irq_macro 0x0E
irq_macro 0x0F
irq_macro 0x31

irq_macro 0x80


int_bottom:

    # save registers
    #pusha
    #pushl %ds
    #pushl %es
    #pushl %fs
    #pushl %gs

    pushl %ebp
    pushl %edi
    pushl %esi

    pushl %edx
    pushl %ecx
    pushl %ebx
    pushl %eax

    # load ring 0 segment register
    #cld
    #mov $0x10, %eax
    #mov %eax, %eds
    #mov %eax, %ees

    # call C Handler
    pushl %esp
    push (interruptnumber)
    call idt_handle
    #add %esp, 6
    mov %eax, %esp # switch the stack

    # restore registers
    popl %eax
    popl %ebx
    popl %ecx
    popl %edx

    popl %esi
    popl %edi
    popl %ebp
    #pop %gs
    #pop %fs
    #pop %es
    #pop %ds
    #popa

    add $4, %esp

.global interrupt_ignore
interrupt_ignore:

    iret


.data
    interruptnumber: .byte 0
