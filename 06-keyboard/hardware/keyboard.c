#include <hardware/keyboard.h>
#include <hardware/port.h>
#include <boot/print.h>
#include <hardware/idt.h>


#define KEYBOARD_COMMAND_PORT   0x64
#define KEYBOARD_DATA_PORT      0X60

static interrupt_handler_t g_keyboard_handler;

static uint32_t __keyboard_handler_callback(uint32_t esp, interrupt_handler_t *arg) {
    uint8_t key = port_read8(KEYBOARD_DATA_PORT);
    if(key < 0x80) {
        switch(key) {
            case 0x02: printf("1"); break;
            case 0x03: printf("2"); break;
            case 0x04: printf("3"); break;
            case 0x05: printf("4"); break;
            case 0x06: printf("5"); break;
            case 0x07: printf("6"); break;
            case 0x08: printf("7"); break;
            case 0x09: printf("8"); break;
            case 0x0A: printf("9"); break;
            case 0x0B: printf("0"); break;

            case 0x10: printf("q"); break;
            case 0x11: printf("w"); break;
            case 0x12: printf("e"); break;
            case 0x13: printf("r"); break;
            case 0x14: printf("t"); break;
            case 0x15: printf("y"); break;
            case 0x16: printf("u"); break;
            case 0x17: printf("i"); break;
            case 0x18: printf("o"); break;
            case 0x19: printf("p"); break;

            case 0x1E: printf("a"); break;
            case 0x1F: printf("s"); break;
            case 0x20: printf("d"); break;
            case 0x21: printf("f"); break;
            case 0x22: printf("g"); break;
            case 0x23: printf("h"); break;
            case 0x24: printf("j"); break;
            case 0x25: printf("k"); break;
            case 0x26: printf("l"); break;

            case 0x2C: printf("z"); break;
            case 0x2D: printf("x"); break;
            case 0x2E: printf("c"); break;
            case 0x2F: printf("v"); break;
            case 0x30: printf("b"); break;
            case 0x31: printf("n"); break;
            case 0x32: printf("m"); break;
            case 0x33: printf(","); break;
            case 0x34: printf("."); break;
            case 0x35: printf("-"); break;

            case 0x1C: printf("\n"); break;
            case 0x39: printf(" "); break;

            default:  {
                printf("\nKEYBOARD 0x");
                printf_hex(key);
                break;
            }
        }
    }
    return esp;
}

void keyboard_init() {
    g_keyboard_handler.interrutpt_number = 0x21;
    g_keyboard_handler.cb_handler = __keyboard_handler_callback;
    idt_register_handler(&g_keyboard_handler);
    while (port_read8(KEYBOARD_COMMAND_PORT) & 0x1) {
        port_read8(KEYBOARD_DATA_PORT);
    }

    port_write8(KEYBOARD_COMMAND_PORT, 0xae);   // activate interrupts
    port_write8(KEYBOARD_COMMAND_PORT, 0x20);   // command 0x20 = read controller command byte

    uint8_t status = (port_read8(KEYBOARD_DATA_PORT) | 1) & ~0x10;
    port_write8(KEYBOARD_COMMAND_PORT, 0x60);
    port_write8(KEYBOARD_DATA_PORT, status);
    port_write8(KEYBOARD_DATA_PORT, 0xf4);
    printf("keyboard_init\n");
}

