#include <types.h>
#include <boot/gdt.h>
#include <boot/print.h>
#include <hardware/idt.h>

typedef void (*constructor)();
constructor start_ctors;
constructor end_ctors;
void call_constructors()
{
    for(constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

//http://www.jamesmolloy.co.uk/tutorial_html/

void kernel_main(void) {
    printf("Hello World! --- this is my os\n");

    gdt_init();
    idt_init(0x20, &g_gdt);
    idt_active();

    while (1);

    idt_deinit();
    gdt_deinit();

}