# ESP32-S3 Drink Dispenser

这是参考 `DrinkDispenser` 项目结构创建的 ESP32-S3 / ESP-IDF 初始工程。

## 环境要求

- ESP-IDF 5.2 或更高版本
- ESP32-S3 开发板
- 8 MB 或更大的 Flash

## 构建

先加载 ESP-IDF 环境，然后执行：

```bash
idf.py set-target esp32s3
idf.py build
```

## 烧录和查看日志

将 `<PORT>` 替换为开发板串口：

```bash
idf.py -p <PORT> flash monitor
```

按 `Ctrl+]` 退出串口监视器。

## 当前功能

- 输出芯片核心数、芯片版本和 Flash 容量
- 每 5 秒输出一次运行心跳日志
- 提供 8 MB Flash 的基础分区表，其中包含约 4.94 MB 的 SPIFFS 数据分区
