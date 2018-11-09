void printf(char *str) {
	static unsigned short *video_memory = (unsigned short *)0xb8000;
	
	unsigned short white = 0xFF00;
	for (int i = 0; str[i] != '\0'; i++) {
		video_memory[i] = (video_memory[0] & white) | str[i];
	}
}



typedef void (*constructor)();
constructor start_ctors;
constructor end_ctors;
void call_constructors()
{
    for(constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

void kernel_main(void) {
	 printf("Hello World! --- this is my os");
}