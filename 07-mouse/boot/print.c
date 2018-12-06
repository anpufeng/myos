#include <boot/print.h>


void printf(char *str) {
	uint16_t white = 0xFF00;
	static uint16_t *video_memory = (uint16_t*)0xb8000;
    static uint8_t x = 0,y = 0;
    static const size_t WIDTH = 80;
    static const size_t HEIGHT = 25;
    for(int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                video_memory[WIDTH*y+x] = (video_memory[WIDTH*y+x] & white) | str[i];
                x++;
                break;
        }

        if(x >= WIDTH) {
            x = 0;
            y++;
        }

        if(y >= HEIGHT) {
            for(y = 0; y < HEIGHT; y++) {
                for(x = 0; x < WIDTH; x++) {
                    video_memory[WIDTH*y+x] = (video_memory[WIDTH*y+x] & white) | ' ';
                }
            }
            x = 0;
            y = 0;
        }
    }
}

void printf_hex(uint8_t key)
{
	char *foo = "00";
	char *hex = "0123456789ABCDEF";
	foo[0] = hex[(key >> 4) & 0xF];
	foo[1] = hex[key & 0xF];
	printf(foo);
}

void printf_hex16(uint16_t key)
{
    printf_hex((key >> 8) & 0xFF);
    printf_hex( key & 0xFF);
}
void printf_hex32(uint32_t key)
{
    printf_hex((key >> 24) & 0xFF);
    printf_hex((key >> 16) & 0xFF);
    printf_hex((key >> 8) & 0xFF);
    printf_hex( key & 0xFF);
}