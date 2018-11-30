## 实现加载Global Descriptor Table (GDT)
 
 
### 段寄存器
一开始看了不少的`GDT`的资料, 基本都是讲其数据结构构成, 以及如何计算得到实际的物理地址, 但一直有很多问题让人头大, 为什么要有`GDT`, 为什么有`保护模式`和`实模式`, 为何一个8字节的`GDT段描述项`结构会如此奇葩, 对`segment` `offset`的概念都变得比较模糊, 越看越不明白. 直到看了[段寄存器](https://blog.csdn.net/michael2012zhao/article/details/5554023)这篇文章, 才算豁然开朗, 理解后再来看其它的简单容易的多.

#### GDT有什么用
GDT是[IA32](https://wiki.osdev.org/IA32_Architecture_Family)架构体系下的, 它的作用是用来提供段式存储机制.
在IA32下, CPU有两种工作方式: `实模式`和`保护模式`.当我们按下开机按钮以后, CPU是工作在实模式下的, 经过某种机制以后才进入保护模式.

#### GDT描述项的构成
![](https://wiki.osdev.org/images/f/f3/GDT_Entry.png)
GDT中的每个描述符中有8个字节组成
- Limit 0:15和16:19->20bit, 寻址最大单元(单位可以是byte或page 4 KiB, 依赖于flags下的Gr标志位)
- Base 0:15+16:23+24:31 -> 32bit, 段基地址 

![](https://wiki.osdev.org/images/6/68/Gdt_bits_fixed.png)

这块各个bit位所表述的意义参见 https://wiki.osdev.org/GDT

定义如下描述项结构体
```CPP
typedef struct gdt_entry_t {
	uint16_t 	limit_low;
	uint16_t 	base_low;
	uint8_t 	base_middle;
	uint8_t 	access;
	uint8_t 	limit_high;
	uint8_t     flags;
	uint8_t 	base_high;
} __attribute__((packed)) gdt_entry_t;

```

初始化方法如下
```CPP
void gdt_entry_init(gdt_entry_t *entry, uint32_t base, uint32_t limit, uint8_t type) {
	uint8_t* target = (uint8_t*)entry;
    if (limit <= 65536) {
        // 16-bit address space
        target[6] = 0x40;
    } else {
        // 32-bit address space
        // Now we have to squeeze the (32-bit) limit into 2.5 regiters (20-bit).
        // This is done by discarding the 12 least significant bits, but this
        // is only legal, if they are all ==1, so they are implicitly still there

        // so if the last bits aren't all 1, we have to set them to 1, but this
        // would increase the limit (cannot do that, because we might go beyond
        // the physical limit or get overlap with other segments) so we have to
        // compensate this by decreasing a higher bit (and might have up to
        // 4095 wasted bytes behind the used memory)
		if ((limit & 0xFFF) != 0xFFF) {
			limit = (limit >> 12)-1;
        } else {
        	limit = limit >> 12;
        }

        target[6] = 0xC0;
    }

    // Encode the limit
    target[0] = limit & 0xFF;
    target[1] = (limit >> 8) & 0xFF;
    target[6] |= (limit >> 16) & 0xF;

    // Encode the base
    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;

    // Type
    target[5] = type;
}
```

#### GDT内存储以下几种描述项
```CPP
typedef struct gdt_t {
	gdt_entry_t null_segment_selector;
	gdt_entry_t code_segment_selector;
	gdt_entry_t data_segment_selector;
} gdt_t;
```

初始化方法哪下
```CPP
/*
                           Pr  Priv  S   Ex  DC   RW   Ac
     0x9A == 1001 1010  == 1   00    1   1   0    1    0
     0x92 == 1001 0010  == 1   00    1   0   0    1    0
   */

void gdt_init() {
    gdt_entry_init(&g_gdt.null_segment_selector, 0, 0, 0);
    gdt_entry_init(&g_gdt.code_segment_selector, 0, 64 * 1024 * 1024, 0x9A);
    gdt_entry_init(&g_gdt.data_segment_selector, 0, 64 * 1024 * 1024, 0x92);

    uint32_t i[2];
    i[1] = (uint32_t)&g_gdt;
    i[0] = sizeof(gdt_t) << 16;

    __asm__ volatile("lgdt (%0)": :"p" (((uint8_t *) i)+2));
     printf("gdt_init\n");
}
```

#### 扩展下printf打印功能
增加输出换行及输出HEX功能, 详见boot/print.c

#### 运行结果
我们在`kernel_main`中调用` gdt_init()` 终端运行`make run`后启动系统会看到在终端打印出
> gdt_init


参考文档 
https://wiki.osdev.org/GDT_Tutorial
https://wiki.osdev.org/GDT




