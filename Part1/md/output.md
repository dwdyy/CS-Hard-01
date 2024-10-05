```c
// 这里用户申请一个8000字节的空间，实际分配 8000 + 8(脚部头部) = 8008
=========================== mm_malloc : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 8008
Allocated: 1

Address: 0x5609ec31cf58
Size: 4280
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_malloc : out ===========================

// 这里用户申请一个200字节的空间，实际分配200 + 8(脚部头部) = 208

=========================== mm_malloc : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 8008
Allocated: 1

Address: 0x5609ec31cf58
Size: 208
Allocated: 1

Address: 0x5609ec31d028
Size: 4072
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_malloc : out ===========================
// 这里用户申请一个50字节的空间，实际分配56 + 8(脚部头部) = 64
=========================== mm_malloc : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 8008
Allocated: 1

Address: 0x5609ec31cf58
Size: 208
Allocated: 1

Address: 0x5609ec31d028
Size: 64
Allocated: 1

Address: 0x5609ec31d068
Size: 4008
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_malloc : out ===========================
// 这里释放了200字节的空间
=========================== mm_free : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 8008
Allocated: 1

Address: 0x5609ec31cf58
Size: 208
Allocated: 0

Address: 0x5609ec31d028
Size: 64
Allocated: 1

Address: 0x5609ec31d068
Size: 4008
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_free : out ===========================
// 这里申请了150字节的空间实际 152 + 8 = 160 大小为208的空白快切割为 160 + 48;
=========================== mm_malloc : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 8008
Allocated: 1

Address: 0x5609ec31cf58
Size: 160
Allocated: 1

Address: 0x5609ec31cff8
Size: 48
Allocated: 0

Address: 0x5609ec31d028
Size: 64
Allocated: 1

Address: 0x5609ec31d068
Size: 4008
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_malloc : out ===========================
// 这里释放大小8000 + 8 字节空间 空白快无法合并
=========================== mm_free : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 8008
Allocated: 0

Address: 0x5609ec31cf58
Size: 160
Allocated: 1

Address: 0x5609ec31cff8
Size: 48
Allocated: 0

Address: 0x5609ec31d028
Size: 64
Allocated: 1

Address: 0x5609ec31d068
Size: 4008
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_free : out ===========================
// 这里释放大小56 + 8 = 64字节空间  与上下空白块合并为 48 + 64 + 4008 = 4120
=========================== mm_free : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 8008
Allocated: 0

Address: 0x5609ec31cf58
Size: 160
Allocated: 1

Address: 0x5609ec31cff8
Size: 4120
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_free : out ===========================
//最后释放160字节块 全部合并 为 12288
=========================== mm_free : out ===========================
Address: 0x5609ec31b008
Size: 8
Allocated: 1

Address: 0x5609ec31b010
Size: 12288
Allocated: 0

Address: 0x5609ec31e010
Size: 0
Allocated: 1
=========================== mm_free : out ===========================