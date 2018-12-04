## I/O依据端口读写8/16/32-bit值 

#### 读写8bit
`inb` `outb`是往对应的I/O端口读写数据的指令, 对于内嵌式汇编的语法参考[Extended Asm](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html)   
```CPP
uint8_t port_read8(uint16_t port) {
	uint8_t result;
	__asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void port_write8(uint16_t port, uint8_t data) {
	__asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

void port_write8_slow(uint16_t port, uint8_t data) {
	__asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (port));
}

```

`inb %1,%0` 意思是将`%1`到`%0`, 即`port`的数据至`result`

`outb`同理是将`data`写至`port`

这里有个特殊地方`jmp 1f\n1: jmp 1f\n1:`是指`IO_WAIT`, 强制CPU等待I/O操作完成.

#### 16位32位

```CPP
uint16_t port_read16(uint16_t port) {
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void port_write16(uint16_t port, uint16_t data) {
	__asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t port_read32(uint16_t port) {
	uint32_t result;
	__asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void port_write32(uint16_t port, uint32_t data) {
	__asm__ volatile("outl %0, %1" : : "a"(data), "Nd" (port));
}
```

本节的操作主要是为后续其它章节服务, 比如下一节中断的实现

参考文档 https://wiki.osdev.org/Inline_Assembly/Examples#I.2FO_access





