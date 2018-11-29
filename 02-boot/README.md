## 实现一个可启动的系统并安装至VirtualBox
 

#### 文件概览
* loader.s -  设置处理器环境的内核入口点
* kernel.c -  内核运行程序
* linker.ld - 用以链接以上文件
* makefile -  编译的makefile

####启动操作系统内核
要想启动一个操作系统, 我们需要一个`bootloader`来引导, 它是在操作系统内核运行之前运行。可以初始化硬件设备、建立内存空间映射图，从而将系统的软硬件环境带到一个合适状态，以便为最终调用操作系统内核准备好正确的环境.
我们这里使用[`GRUB`](https://wiki.osdev.org/GRUB "GRUB")而非自己去编写一个`bootloader`, (后面有时间可以再去学习下如何自己实现一个可供我们直接使用的)
所以我们按照[`Multiboot header`](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#OS-image-format "Multiboot")的标准来启动内核, 通过放置一些全局的`magic values`来让`bootloader`识别
这里使用汇编实现, 编译工具采用的[`AS`](https://sourceware.org/binutils/docs/as/)
```nasm
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM


.section .text
.extern kernel_main
.extern call_constructors
.global loader


loader:
    mov $kernel_stack, %esp
    call call_constructors
    push %eax
    push %ebx
    call kernel_main


_stop:
    cli
    hlt
    jmp _stop


.section .bss
.space 2*1024*1024; # 2 MiB
kernel_stack:
```
在loader块调用了我们在`kernel.c`中定义的`kernel_main`方法
###启动内核后输出"hello world"
内存中从0xB8000h起始的一段地址叫做图像缓存（video buffer）。
0xB8000开始的字符位于屏幕左上角，整个屏幕一共25行，80列。每个字符占用两个字节，前一个字节是ASCII码，后一个字节属性.
```C
void printf(char *str) {
	static unsigned short *video_memory = (unsigned short *)0xb8000;

	unsigned short white = 0xFF00;
	for (int i = 0; str[i] != '\0'; i++) {
		video_memory[i] = (video_memory[0] & white) | str[i];
	}
}

```

### 链接内核
现在我们可以通过`gcc` `as`命令分别编译`kernel.c`和`loader.s`, 同时生成`kernel.o`和`loader.o`文件, 我们最终要把两个文件链接生成为可供`bootloader`使用的内核可执行文件
```nasm
ENTRY(loader)
OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)

SECTIONS
{
  . = 0x0100000;

  .text :
  {
    *(.multiboot)
    *(.text*)
    *(.rodata)
  }

  .data  :
  {
    start_ctors = .;
    KEEP(*( .init_array ));
    KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
    end_ctors = .;

    *(.data)
  }

  .bss  :
  {
    *(.bss)
  }

  /DISCARD/ : { *(.fini_array*) *(.comment) }
}
```

### 编写makefile
详见makefile文件, 里面定义了一些`gcc`及`as`的编译参数及编译依赖规则
 
### 生成可供虚拟机启动的操作系统镜像
参考`/boot/grub/grub.cfg` 我们添加一个`menuentry`, 并通过[grub-mkrescue](https://www.gnu.org/software/grub/manual/grub/html_node/Making-a-GRUB-bootable-CD_002dROM.html#Making-a-GRUB-bootable-CD_002dROM)命令生成iso
```bash
mkdir -p iso/boot/grub
cp mykernel.bin iso/boot/mykernel.bin
echo 'set timeout=2'                      > iso/boot/grub/grub.cfg
echo 'set default=0'                     >> iso/boot/grub/grub.cfg
echo ''                                  >> iso/boot/grub/grub.cfg
echo 'menuentry "My Operating System" {' >> iso/boot/grub/grub.cfg
echo '  multiboot /boot/mykernel.bin'    >> iso/boot/grub/grub.cfg
echo '  boot'                            >> iso/boot/grub/grub.cfg
echo '}'                                 >> iso/boot/grub/grub.cfg
grub-mkrescue --output=mykernel.iso iso
```
### 在VirtualBox中安装mykernel.iso
- 新建一个虚拟机, 填写如下图所示

<img src="https://github.com/anpufeng/myos/blob/master/image/02-boot-virtualbox.png" width="500" height="400">

- 下一步使用默认64M, 下一步选择`do not add a virtual hard drive`点击创建, 然后start此系统, 此时会让你选择镜像目录, 选择我们makefile中生成的iso所在的目录即可.

- 最终结果如下

<img src="https://github.com/anpufeng/myos/blob/master/image/02-boot.gif" width="600" height="350">

参考文档 https://wiki.osdev.org/Bare_Bones 



