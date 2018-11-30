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
- Pr -> Present bit. 有效的`selector`必须为1
- Privl -> Privilege, 2 bits. 所描述段的特权级，用于特权检查, 0最高(内核)3最低(用户).
- S -> Descriptor type, `code segments`和`data segments`设为1, `system segments` 为0
- Ex->  Executable bit 如果`segment`可执行须置为1 例如` code selector`, 否则置0例如`data selector`

The bit fields are:

- Pr: Present bit. This must be 1 for all valid selectors.
-  Privl: Privilege, 2 bits. Contains the ring level, 0 = highest (kernel), 3 = lowest (user applications).
-  S: Descriptor type. This bit should be set for code or data segments and should be cleared for system segments (eg. a Task State Segment)
-  Ex: Executable bit. If 1 code in this segment can be executed, ie. a code selector. If 0 it is a data selector.
-  DC: Direction bit/Conforming bit.
    - Direction bit for data selectors: Tells the direction. 0 the segment grows up. 1 the segment grows down, ie. the offset has to be greater than the limit.
    - Conforming bit for code selectors:
        - If 1 code in this segment can be executed from an equal or lower privilege level. For example, code in ring 3 can far-jump to conforming code in a ring 2 segment. The privl-bits represent the highest privilege level that is allowed to execute the segment. For example, code in ring 0 cannot far-jump to a conforming code segment with privl==0x2, while code in ring 2 and 3 can. Note that the privilege level remains the same, ie. a far-jump form ring 3 to a privl==2-segment remains in ring 3 after the jump.
        - If 0 code in this segment can only be executed from the ring set in privl.
- RW: Readable bit/Writable bit.
Readable bit for code selectors: Whether read access for this segment is allowed. Write access is never allowed for code segments.
Writable bit for data selectors: Whether write access for this segment is allowed. Read access is always allowed for data segments.
- Ac: Accessed bit. Just set to 0. The CPU sets this to 1 when the segment is accessed.
- Gr: Granularity bit. If 0 the limit is in 1 B blocks (byte granularity), if 1 the limit is in 4 KiB blocks (page granularity).
- Sz: Size bit. If 0 the selector defines 16 bit protected mode. If 1 it defines 32 bit protected mode. You can have both 16 bit and 32 bit selectors at once.

所以我们定义如下描述项结构体
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

#### GDT内应存储以下几种描述项
```CPP
typedef struct gdt_t {
	gdt_entry_t null_segment_selector;
	gdt_entry_t code_segment_selector;
	gdt_entry_t data_segment_selector;
} gdt_t;

```

#### 扩展下printf打印功能
增加输出换行及输出HEX功能, 详见boot/print.c


参考文档 
https://wiki.osdev.org/GDT_Tutorial
https://wiki.osdev.org/GDT




