#include <hardware/mouse.h>
#include <hardware/port.h>
#include <boot/print.h>
#include <hardware/idt.h>


#define MOUSE_COMMAND_PORT 0x64
#define MOUSE_DATA_PORT   0X60

uint8_t buffer[3];
uint8_t offset;
uint8_t buttons;

int8_t x, y;


static uint32_t __mouse_handler_callback(uint32_t esp, interrupt_handler_t *arg) {
    uint8_t  status = port_read8(MOUSE_COMMAND_PORT);
    if (!(status & 0x20))
        return esp;

    buffer[offset] = port_read8(MOUSE_DATA_PORT);
    offset = (offset + 1) % 3;

    if(offset == 0)
    {
        if(buffer[1] != 0 || buffer[2] != 0)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;
            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                  | (VideoMemory[80*y+x] & 0xF000) >> 4
                                  | (VideoMemory[80*y+x] & 0x00FF);

            x += buffer[1];
            if(x >= 80) x = 79;
            if(x < 0) x = 0;
            y -= buffer[2];
            if(y >= 25) y = 24;
            if(y < 0) y = 0;

            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                                  | (VideoMemory[80*y+x] & 0xF000) >> 4
                                  | (VideoMemory[80*y+x] & 0x00FF);
        }

        /*
        for(uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            {
                if(buttons & (0x1<<i))
                    handler->OnMouseButtonReleased(i+1);
                else
                    handler->OnMouseButtonPressed(i+1);
            }
        }
        buttons = buffer[0];
        */
    }
    return esp;

}

static interrupt_handler_t g_mouse_handler;


void mouse_init(idt_t *idt) {
    uint16_t* VideoMemory = (uint16_t*)0xb8000;
    offset = 0;
    buttons = 0;
    x = 40;
    y = 12;
    VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0x0F00) << 4
                          | (VideoMemory[80*y+x] & 0xF000) >> 4
                          | (VideoMemory[80*y+x] & 0x00FF);


    g_mouse_handler.interrutpt_number = 0x2C;
    g_mouse_handler.cb_handler = __mouse_handler_callback;
    g_mouse_handler.interrupt_manager = idt;
    idt_register_handler(&g_mouse_handler);

    port_write8(MOUSE_COMMAND_PORT, 0xA8);
    port_write8(MOUSE_COMMAND_PORT, 0x20);
    uint8_t status = port_read8(MOUSE_DATA_PORT) | 2;

    port_write8(MOUSE_COMMAND_PORT, 0x60);
    port_write8(MOUSE_DATA_PORT, status);

    port_write8(MOUSE_COMMAND_PORT, 0xD4);
    port_write8(MOUSE_DATA_PORT, 0xF4);
    port_read8(MOUSE_DATA_PORT);

//    commandport.Write(0xA8);
//    commandport.Write(0x20); // command 0x60 = read controller command byte
//    uint8_t status = dataport.Read() | 2;
//    commandport.Write(0x60); // command 0x60 = set controller command byte
//    dataport.Write(status);
//
//    commandport.Write(0xD4);
//    dataport.Write(0xF4);
//    dataport.Read();

}

