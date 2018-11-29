

.set IRQ_BASE, 0x20

.section .text

.extern idt_handle


.macro handle_exception_macro num
.global handle_exception\num\()
handle_exception\num\():
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm


.macro handle_interrupt_request_macro num
.global handle_interrupt_request\num\()
handle_interrupt_request\num\():
    movb $\num + IRQ_BASE, (interruptnumber)
    pushl $0
    jmp int_bottom
.endm


handle_exception_macro 0x00
handle_exception_macro 0x01
handle_exception_macro 0x02
handle_exception_macro 0x03
handle_exception_macro 0x04
handle_exception_macro 0x05
handle_exception_macro 0x06
handle_exception_macro 0x07
handle_exception_macro 0x08
handle_exception_macro 0x09
handle_exception_macro 0x0A
handle_exception_macro 0x0B
handle_exception_macro 0x0C
handle_exception_macro 0x0D
handle_exception_macro 0x0E
handle_exception_macro 0x0F
handle_exception_macro 0x10
handle_exception_macro 0x11
handle_exception_macro 0x12
handle_exception_macro 0x13

handle_interrupt_request_macro 0x00
handle_interrupt_request_macro 0x01
handle_interrupt_request_macro 0x02
handle_interrupt_request_macro 0x03
handle_interrupt_request_macro 0x04
handle_interrupt_request_macro 0x05
handle_interrupt_request_macro 0x06
handle_interrupt_request_macro 0x07
handle_interrupt_request_macro 0x08
handle_interrupt_request_macro 0x09
handle_interrupt_request_macro 0x0A
handle_interrupt_request_macro 0x0B
handle_interrupt_request_macro 0x0C
handle_interrupt_request_macro 0x0D
handle_interrupt_request_macro 0x0E
handle_interrupt_request_macro 0x0F
handle_interrupt_request_macro 0x31

handle_interrupt_request_macro 0x80


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

    # call C++ Handler
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
