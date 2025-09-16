This project is a keyboard translator. You can use a new ones keyboard AT model on PC-XT 8088.
Uses a simple STM32C011 with open-drain ports and 5V tolerant inputs, you can install the chip
 only reducing the voltage with a voltage regulator to 3V and connecting directly to keyboar and PC. Or
 just reduce the power supply from 5V to 3V with another methodes like using diodes. There are on folder 
 Fusion360  a schematic and pcb designs. 
 The translator works receiving the keys code from Scan Code Set2 AT keyboard type and translate to Scan Code Set1 XT keyboard.
 The Scan Code Set2 when you press it sends a Make Code and when you release the key, sends de Break Code.
 The Scan Code Set1 only sends the Make Code, and the codes should be translated from Set1 to Set2.

example:
Key 		Set 2 (AT)	Set 1 (XT)
A			0x1C		0x1E
M			0x3A		0x32
Enter		0x5A		0x1C
Left Shift	0x12		0x2A
Right Shift	0x59		0x36




⚠️ Low-voltage equipment: Always follow appropriate isolation practices.
