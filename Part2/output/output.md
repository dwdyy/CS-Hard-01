```
// 开始初始化申请了 4kb 空白块
=========================== init ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 4096
Allocated: 1 0

Address: 0x55555555b020
Size: 0
Allocated: 1
=========================== init ===========================
// 申请8000实际为8016,先扩展堆,在合并产生8016+4096的空白块，空白快前8016再分配出去
=========================== add p1 8000 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 8016
Allocated: 1 1

Address: 0x55555555bf70
Size: 4096
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== add p1 8000 ===========================
// 申请200字节 从4096的空白快取出
=========================== add p2 200 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 8016
Allocated: 1 1

Address: 0x55555555bf70
Size: 208
Allocated: 1 1

Address: 0x55555555c040
Size: 3888
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== add p2 200 ===========================
// 申请50字节 从3888的空白快取出
=========================== add p3 50 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 8016
Allocated: 1 1

Address: 0x55555555bf70
Size: 208
Allocated: 1 1

Address: 0x55555555c040
Size: 64
Allocated: 1 1

Address: 0x55555555c080
Size: 3824
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== add p3 50 ===========================
// 208 的块被释放，无法继续合并
=========================== free p2 200 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 8016
Allocated: 1 1

Address: 0x55555555bf70
Size: 208
Allocated: 1 0

Address: 0x55555555c040
Size: 64
Allocated: 0 1

Address: 0x55555555c080
Size: 3824
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== free p2 200 ===========================
// 加一个150的块，最优适配从208的块释放
=========================== add p4 150 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 8016
Allocated: 1 1

Address: 0x55555555bf70
Size: 160
Allocated: 1 1

Address: 0x55555555c010
Size: 48
Allocated: 1 0

Address: 0x55555555c040
Size: 64
Allocated: 0 1

Address: 0x55555555c080
Size: 3824
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== add p4 150 ===========================
// 释放8008 无法合并
=========================== free p1 8000 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 8016
Allocated: 1 0

Address: 0x55555555bf70
Size: 160
Allocated: 0 1

Address: 0x55555555c010
Size: 48
Allocated: 1 0

Address: 0x55555555c040
Size: 64
Allocated: 0 1

Address: 0x55555555c080
Size: 3824
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== free p1 8000 ===========================
// 释放150 合并
=========================== free p3 150 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 8016
Allocated: 1 0

Address: 0x55555555bf70
Size: 160
Allocated: 0 1

Address: 0x55555555c010
Size: 3936
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== free p3 150 ===========================
// 最后全部合并
=========================== free p4 150 ===========================
Address: 0x55555555a010
Size: 16
Allocated: 1 1

Address: 0x55555555a020
Size: 12112
Allocated: 1 0

Address: 0x55555555cf70
Size: 0
Allocated: 1
=========================== free p4 150 ===========================
```