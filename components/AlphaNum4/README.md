# AlphaNum4 组件说明

## 概述

AlphaNum4 是 Adafruit AlphaNum4 四位 14 段字母数字显示模块(HT16K33 芯片)的 ESP-IDF 驱动组件。

- 芯片:HT16K33(通过 I2C 控制)
- 显示:4 位 14 段字母数字 + 小数点
- 依赖:组件内 `#include "HT16K33.h"`,I2C 总线需先由 `HT16K33` 组件初始化(GPIO4 = SDA,GPIO5 = SCL,100kHz)
- 默认 I2C 地址:`0x70`

> 注意:当前工程(main)中还没有任何代码调用本组件,处于"写好未接入"的待用状态。

## 一、函数说明

| 函数 | 作用 |
|---|---|
| `AlphaNum4()` 构造函数 | 初始化成员:默认 I2C 端口 `I2C_NUM_0`、地址 `0x70`、未初始化标志,并调用 `clear()` 清空显示缓冲 |
| `begin(i2c_port, address)` | 初始化流程:校验地址 ≤0x7F → 保存端口/地址 → 依次发 3 个命令字节:`0x21`(开启内部振荡器)、`0x81`(开启显示、不闪烁)、`0xEF`(亮度设最大 15)。全部成功才置 `_initialized = true` |
| `i2cWrite(data, length)` | 私有封装:校验指针/长度,然后调 `i2c_master_write_to_device` 真正通过 I2C 总线把数据发给芯片,超时 100ms |
| `clear()` | 用 `memset` 把 8 个 `uint16_t` 的显示缓冲全清零(所有段熄灭) |
| `setBrightness(0~15)` | 发亮度命令 `0xE0 \| brightness` |
| `setBlinkRate(0~3)` | 发闪烁命令 `0x80 \| 0x01 \| (blink_rate << 1)`,0=不闪、1=2Hz、2=1Hz、3=0.5Hz |
| `encodeCharacter(char)` | 静态查表:把字符映射成 16 位段码(如 `'0'` → `0x0C3F`、`'1'` → `0x0006`),不认识的字返回 0 |
| `writeDigitRaw(position, bits)` | 把 16 位段码**原样**写入缓冲某个位置(0~3),不做任何编码 |
| `writeDigitAscii(pos, char, dp)` | 字符编码后写入缓冲;`dp=true` 时额外置位 bit14(小数点段) |
| `print(text)` | 清屏后把字符串前 4 个字符依次填到 4 个位置(遇 `\0` 提前结束) |
| `writeDisplay()` | **唯一的"发送"出口**:校验已初始化,把缓冲打包成 17 字节一次发给芯片 |

## 二、底层位操作原理

核心数据模型:**每个字符 = 一个 16 位段码**,bit15~bit0 对应 HT16K33 的 16 个段驱动引脚(14 个字母数字段 + 小数点 + 1 个冗余位)。

### 1. 小数点段:`segment_bits |= (1U << 14)`

置位第 14 位,点亮小数点段。这是写字符时"加小数点"的原理。

### 2. 命令字节拼接:`0xE0 | brightness`

命令/数据拼接:高 4 位 `0xE0` 是指令码(亮度设置),低 4 位是 0~15 亮度值,一字节同时携带"干什么 + 值多少"。

### 3. 闪烁命令:`0x80 | 0x01 | (blink_rate << 1)`

- bit0 固定为 1(保持显示开启)
- bit1~bit2 是闪烁率(0=不闪、1=2Hz、2=1Hz、3=0.5Hz)
- 注意:它把 0x80 的显示命令和 bit0 的"显示开"一起打包,所以 `setBlinkRate` 不会把显示关掉

### 4. 拆字节(小端打包):`writeDisplay()` 中

```c
output[1 + i*2] = _display_buffer[i] & 0xFF;        // 低字节
output[2 + i*2] = (_display_buffer[i] >> 8) & 0xFF; // 高字节
```

- 16 位段码拆成两个 8 位,先低后高,因为 HT16K33 的 RAM 按字节寻址、一个字占连续两个地址
- 首字节 `output[0] = 0x00` 是 RAM 起始地址,告诉芯片"从地址 0 开始连续写 16 字节"

### 5. 缓冲大小:`_display_buffer[8]`

硬件支持 8 个字(可级联两块模块),本类只用前 4 个。

## 三、如何发送 / 读取信息

### 发送(写)——三步走

```cpp
AlphaNum4 display;
display.begin(I2C_NUM_0, 0x70);   // 1. 初始化(必须在前面跑过 i2c_bus_init,GPIO4=SDA/GPIO5=SCL)
display.print("1234");            // 2. 填缓冲(或 writeDigitAscii / writeDigitRaw)
display.writeDisplay();           // 3. 把缓冲刷到硬件,不加这步屏幕上什么都不显示
// setBrightness / setBlinkRate 是即发即生效的命令,不用 writeDisplay
```

### 读取

本组件**没有实现读功能**(与 Adafruit 原版有 `getDisplayBuffer` 不同)。

HT16K33 本身支持回读(读 RAM 地址 0x00 起 16 字节能拿回当前段码)。如需添加,可仿照 `i2cWrite` 用 `i2c_master_read_from_device` 写一个 `i2cRead`;不过对纯显示设备来说回读用处不大。真正需要读的是芯片的按键扫描寄存器(0x40~0x77 命令后读),本组件完全没涉及。

## 附:字符编码表(encodeCharacter)

| 字符 | 段码 | 字符 | 段码 |
|---|---|---|---|
| 空格 | 0x0000 | '7' | 0x0007 |
| '0' | 0x0C3F | '8' | 0x00FF |
| '1' | 0x0006 | '9' | 0x00EF |
| '2' | 0x00DB | 'C' | 0x0039 |
| '3' | 0x00CF | 'E' | 0x00F9 |
| '4' | 0x00E6 | 'O' | 0x003F |
| '5' | 0x00ED | 'P' | 0x00F3 |
| '6' | 0x00FD | 'R' | 0x20F3 |
| | | 'N' | 0x2136 |
