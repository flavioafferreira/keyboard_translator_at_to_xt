# Keyboard AT → XT Translator

This project is a **keyboard translator** that allows you to use a modern **AT keyboard** on an old **PC-XT 8088**.

## Hardware
- Based on a **STM32C011** microcontroller.  
- Uses **open-drain GPIO** and **5V-tolerant inputs**.  
- Power supply:  
  - You can power the MCU with **3.0 V** using a voltage regulator.  
  - Or reduce from **5 V to 3 V** using other methods (e.g., diodes).  
- Inside the `Fusion360/` folder you will find **schematics and PCB designs**.

## How it Works
- The translator receives **Scan Code Set 2** from an **AT keyboard**.  
- It converts them to **Scan Code Set 1** for the **XT PC**.  

### Scan Code Sets
- **Set 2 (AT)**:  
  - Sends a **Make Code** when the key is pressed.  
  - Sends a **Break Code** (`0xF0` + code) when the key is released.  
- **Set 1 (XT)**:  
  - Sends only the **Make Code**.  
  - Codes differ from Set 2, so translation is needed.

## Example Translation

| Key         | Set 2 (AT) | Set 1 (XT) |
|-------------|------------|------------|
| A           | `0x1C`     | `0x1E`     |
| M           | `0x3A`     | `0x32`     |
| Enter       | `0x5A`     | `0x1C`     |
| Left Shift  | `0x12`     | `0x2A`     |
| Right Shift | `0x59`     | `0x36`     |

---

## Repository Contents
- `Fusion360/` → schematic and PCB design files.  
- `Firmware/` → STM32 source code (keyboard translator).  

---


