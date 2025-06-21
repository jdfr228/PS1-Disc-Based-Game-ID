# PS1 Disc-Based Game ID
Available in this repository are xdelta patches for PS1 BIOS files that will allow games running off of disc or PSIO to automatically send Game ID data to a MemCard Pro or SD2PSX, as well as a PS1Digital, Retro GEM, and even a BlueRetro receiver, if you have one of these memory cards installed.  Game ID data allows these memory cards to auto-select a virtual memory card page for each game you play, and for HDMI mods and BlueRetro to allow for per-game settings. This doesn't require the user to boot any separate software or exploit, plays the nostalgic PS1 boot splash like a regular startup, and should hopefully maintain full software compatibility.

### A note on PS1Digital/Retro GEM/BlueRetro support:
In the future, these may be able to support Game ID functionality without needing a specific memory card, but for now, since these devices do not send the required ACK signals when receiving a Game ID command, you must have a MemCard Pro or SD2PSX installed.  PS1Digital and Retro GEM are yet to be tested fully.

## NOTE- As of June 2025, these patches have worked well for those using them, but testing remains limited when it comes to covering all hardware/firmware configurations. Users who wish to test these patches should have an easy way to modify their BIOS back to stock in case something goes wrong.  This project takes no liability for any damage incurred when modifying your own console.

## Supported hardware
- 8bitmods MemCard Pro
- SD2PSX
- If you have either of the above, you *should* also gain additional functionality with:
  - PSIO
  - PS1Digital/Retro GEM
  - BlueRetro bluetooth receiver

## Requirements
- A supported original BIOS dump (.bin format)
- A replacement EEPROM chip for your system's BIOS and an adapter board to match the console pinout
  - SST 39VF040 recommended
  - Order your own boards [from OSHPark](https://oshpark.com/shared_projects/F5tR8dhr) and flash the chip yourself
  - Another adapter board option is now available, particular for 40-pin BIOS - https://github.com/Electroanalog/PSX-Flash-BIOS/
  - Most systems' original BIOS will have a 32-pin BIOS chip, though some older models use a 40-pin chip (see Supported Hardware section below for systems that potentially do not have a 32-pin chip)
- If you have blank chips, an EEPROM programmer (The CD-based flasher is deprecated), OR a kind individual selling pre-flashed chips in some capacity.
  - For blank chips, if you have no way of flashing an individual SST 39VF040 chip, you can purchase a [FlashCatUSB XPORT](https://www.embeddedcomputers.net/products/FlashcatUSB_XPORT/) plus a [TSOP-32 (Type-B) adapter](https://www.embeddedcomputers.net/products/ParallelAdapters/), or use mi213's [TL866 adapter](https://oshpark.com/shared_projects/b8rGXXJj) (+ a hot air station to remove the flash chip after flashing) for TL866ii family flashers readily available from Aliexpress (or the [socketed](https://oshpark.com/shared_projects/coznPX7v) versions of mi213's boards to avoid needing a hot air station).
  
## Installing replacement BIOS PCB

### Note that this is intended for pre-flashed chips.

1) Locate your BIOS chip (IC102) and make sure it is a 32-pin chip (32 marking in one of the corners)

![image](https://user-images.githubusercontent.com/24398594/222358899-5d3cfce6-00b1-449d-8af3-7e4354929205.png)

2) Carefully remove the chip with a hot air rework station, or if you do not have one, by clipping the legs of each pin with a pair of sharp flush cutters

![image](https://user-images.githubusercontent.com/24398594/222359016-1ceaa052-a1f3-44a4-be67-9b52fcc1d3e9.png)

3) If you used flush cutters, use your soldering iron to wipe away any leftover feet, and then clean away the old solder with a soldering wick and clean the area with isopropyl alcohol

![image](https://user-images.githubusercontent.com/24398594/222359108-7788d434-35e0-4e5b-9ddb-62c2668c530b.png)

4) Prepare your adapter PCB by aligning the flash chip as best you can, tacking down one corner with lots of flux and a little bit of solder (I used too much on one side here), checking the alignment for the other side, and tacking down the opposite corner.  If alignment looks good, continue to work the rest of the pins, again using a lot of flux and *very* little solder

![image](https://user-images.githubusercontent.com/24398594/222359200-ad92b1c9-ce72-4350-94fd-572e476bb923.png)

5) Check for shorts on any adjacent pins of the flash chip. Most pins on the adapter are next to each other. Bridge your pad for /WE and VCC if necessary, and solder your adapter in place on the PS1 motherboard. Again use lots of flux, and plenty of solder this time (much harder to bridge these pins). Clean the surrounding areas of the motherboard, and the pins on the flash chip with alcohol when you are done.

![image](https://user-images.githubusercontent.com/24398594/222359309-a4336171-9620-463c-a6a1-3f30acb46b05.png)

## How to patch your BIOS dump .bin

Apply the supplied xdelta patch file to your original .bin BIOS dump using [DeltaPatcher](https://www.romhacking.net/utilities/704/), or another xdelta patching tool.

### Final checksums after patching will be (Release v1.0)
| BIOS version | Original CRC-32 | Patched CRC-32 |
| --- | --- | --- |
| SCPH-1001 (NTSC-U/C) | 37157331 | 398AAD29 |
| SCPH-3500 (NTSC-J) | BC190209 | 9D5271AC |
| SCPH-5000 (NTSC-J) | 24FC7E17 | 05B70DB2 |
| SCPH-5500 (NTSC-J) | FF3EEB8C | DE759829 |
| SCPH-5501 (NTSC-U/C) | 8D8CB7E4 | 831369FC |
| SCPH-5502/5552 (PAL) | D786F0B9 | D9192EA1 |
| SCPH-7000 (NTSC-J) | EC541CD0 | CD1F6F75 |
| SCPH-7001/7501/7503/9001 (NTSC-U/C) | 502224B6 | 5EBDFAAE |
| SCPH-7002/7502/9002 (PAL) | 318178BF | 3F1EA6A7 |
| SCPH-7003 (NTSC-J) | 8D8CB7E4 | 831369FC |
| SCPH-100 (NTSC-J) | F2AF798B | FC30A793 |
| SCPH-101 (NTSC-U/C) | 171BDCEC | 198402F4 |
| SCPH-102A (PAL) | 0BAD7EA9 | 0532A0B1 |
| SCPH-102B (PAL) | 76B880E5 | 78275EFD |

## Supported firmware
## Tested on hardware
- SCPH-1001** (US)
- SCPH-5502/5552 (PAL)
- SCPH-7002 (PAL)
- SCPH-7001/7501 (should work for 7503 and 9001 as well)
- SCPH-7502 (PAL)
- SCPH-101 (US)

## Untested but should work
### NTSC-U/C
- SCPH-5501

### NTSC-J
- SCPH-3500**
- SCPH-5000 (may be a bad dump per PSXDEV)
- SCPH-5500
- SCPH-7000 (should work for 9000 as well)
- SCPH-7003 (identical to NTSC-U/C SCPH-5501)
- SCPH-100

### PAL
- SCPH-9002 (should be identical to SCPH-7002/7502, likely works!)
- SCPH-102A
- SCPH-102B

** - Motherboards for these models may contain a 40-pin BIOS chip. The newer adapter board from the [PSX-Flash-BIOS project](https://github.com/Electroanalog/PSX-Flash-BIOS/) will need to be used.

## Unsupported firmware
- Requires an alternative code injection point
  - SCPH-1000 (NTSC-J)
  - SCPH-1002 (PAL)
  - SCPH-3000 (NTSC-J)
- Unverified dumps (per PSXDEV)
  - SCPH-5903 (NTSC-J?)
  - SCPH-7000W (NTSC-J?)
  - SCPH-7500 (NTSC-J)
  - SCPH-7503 (NTSC-J?)
  - SCPH-9000 (NTSC-J)
  - SCPH-9001 (NTSC-U/C)
  - SCPH-9003 (NTSC-J?)
  - SCPH-102 (PAL)
  - SCPH-102C (PAL)
  - SCPH-103 (NTSC-J?)

You can very likely use any firmware you want on any hardware revision (barring of course special models like the Net Yaroze or other devkits)

## Todo
- Additional hardware verification

## Special thanks
- nocash for his amazing help with developing this patch and for all his work on no$psx and extensive PSX documentation.  This project would not exist without him, and as he is currently forced to stay in a small room with family, I'd highly recommend supporting [his Patreon](https://www.patreon.com/martin_korth) or [donating directly](http://problemkaputt.de/donate.htm) if you are able!
- 8bitmods for developing the wonderful MemCard Pro and even offering me an advance unit for testing this patch with!
- megavolt85 for developing and releasing the source for their BIOS patching program, even though it didn't end up working. The source included in earlier releases has been modified by me to remove copyrighted BIOS files and add support for the SST 39VF040 flash chip.
- mi213 for their PS1 Flash BIOS project and for graciously providing BIOS boards and helping with testing. See all their OSHPark uploads [here](https://oshpark.com/profiles/mi213).
- bbsan for developing the SD2PSX, arranging to send one to me for testing, and helping test himself.
- Electroanalog for developing a new adapter board for older 40-pin models.

---

<details>
  <summary>The CD flashing option is deprecated since it did not work in testing.  Attempt at your own risk</summary>

  ## How to insert patched BIOS .bin into flasher CD
1) Download the latest release of [mkpsxiso](https://github.com/Lameguy64/mkpsxiso) and the latest modified flashing program from this repo's releases page.

2) Move your .bin and .cue files for the flashing program to your extracted mkpsxiso bin directory.

![image](https://user-images.githubusercontent.com/24398594/222319093-cf6d3f3c-e81b-4cf9-abdb-2391f12bb28d.png)

3) Open a command line and navigate to the bin folder, and run the following command:
```
dumpsxiso.exe -s ./flasher.xml BIOSFlasher.bin
```

4) The files from the flashing CD should now be in the bin directory:

![image](https://user-images.githubusercontent.com/24398594/222319502-39e69eb5-abb2-4387-8cd9-49522e032455.png)

5) Rename your patched BIOS file to PSX-XBOO.BIN, replace the file in the bin directory, and then run the following command in the command line:
```
mkpsxiso.exe flasher.xml -o BIOSFlasherModified.bin -c BIOSFlasherModified.cue
```

6) Test the BIOSFlasherModified.cue image in an emulator, and then burn it to a CD using the .cue file. I recommend IMGBurn and, for a modern CD burner, MAX speed setting. Newer burners are optimized for the highest speed and often give worse results burning at forced lower speeds!

![image](https://user-images.githubusercontent.com/24398594/222325783-5263a90b-f1d4-411d-8848-72e5b5e49f54.png)

### Currently supported replacement BIOS ROM chips for software flashing:
- AMD 29F040B
- ATMEL 29C040A
- SST 28SF040
- SST 39SF040
- SST 39VF040
- WINBOND 29C040

Adding support for additional flash chips is quite simple, if they support byte-mode writing with an 8-bit data bus. Let me know if you're using anything that is unsupported, has a datasheet with chip erase and sector write byte commands available, and want it added to the software. If you are capable of updating the software yourself and building it with Psy-Q, the source is available [here](https://web.archive.org/web/20220517114908/https://www.obscuregamers.com/threads/bios-flasher.771/).

## How to modify BIOS replacement PCB to enable writing:
1) Locate the /WE pin on your flash ROM chip- for SST 39VF040 this is pin 7

![image](https://user-images.githubusercontent.com/24398594/222040812-030bf715-0dc9-44e3-92a2-a3c49bead369.png)

The recommended OSHPark board made for SST 39VF040 flash chips exposes the /WE pin on a convenient pad (that needs to be bridged afterwards)

![image](https://user-images.githubusercontent.com/24398594/222325916-9b7b5976-c01b-4ef8-be39-c64d0664b4c8.png)

2) **(Will's board only)** Disconnect this pin from VCC
A board from Will's Console Modifications requires that pin 7 be carefully lifted and soldered to. I was unable to do this personally, which is why I don't recommend his board. Attempting to sever the trace may be easier.
3) Wire this pin to the CPU's /SWR0 or /WR pin (pin 100)

((picture forthcoming of hopefully convenient test point on Slim PSOne))

((picture forthcoming of pin on serial port for earlier models))

4) Boot up your modified flasher disc via FreePSXBoot, a modchip, or the disc swap method and follow its instructions (press L1 to detect BIOS chip, select PSX-XBOO.BIN file)
5) Reconnect /WE pin to VCC and test your new BIOS!
</details>
