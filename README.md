# PS1 Disc-Based Game ID
Available in this repository are IPS patches for PS1 BIOS files that will allow games running off of disc to automatically send Game ID data to a MemCard Pro, allowing it to auto-select a virtual memory card page for each game you play. This doesn't require the user to boot any separate software or exploit, plays the nostalgic PS1 boot splash like a regular startup, and should hopefully maintain full software compatibility.

### A note on PS1Digital support:
The PS1Digital can also utilize the Game ID data in order to automatically select video settings on a per-game basis. Unfortunately, I do not have one to test with or a working PS1 model that would support it anyways. If you have access to a PS1Digital and would like to test this feature, please let me know! As it stands, I expect that Game ID for PS1Digital would work correctly if you have a MemCard Pro inserted, but will likely NOT work with regular memory cards. This is because of the way that the MemCard Pro acknowledges the Game ID commands being sent to it, pinging back data that lets the patched BIOS know to continue. The PS1Digital cannot send this acknowledgement ping back, so I'm unsure of how to properly send the entire message without a MemCard Pro involved (though I suspect it's possible since I believe a PS1Digital + XStation combo can do so).

## WARNING- As of the time of publishing these patches are *untested* on real hardware. Users who wish to test these patches should have an easy way to modify their BIOS back to stock in case something goes wrong.

## Requirements
- A supported original BIOS dump (.bin format)
- A replacement EEPROM chip for your system's BIOS and an adapter board to match the console pinout
  - SST 39VF040 recommended
  - Order your own boards [from OSHPark](https://oshpark.com/shared_projects/F5tR8dhr) and see flashing options below
  - OR if you want to try lifting an individual TSOP pin you can try [Will's Console Modifications in the UK](https://www.willsconsolemodifications.co.uk/index.php?route=product/product&product_id=55) for a board + flash chip with stock BIOS.
  - Both of these **require your system to have a 32-pin BIOS chip** (see Supported Hardware section below for systems that potentially do not have a 32-pin chip)
- Either an EEPROM programmer if you have a blank flash chip OR a way to boot a burned CD-R/homebrew on your PS1 (FreePSXBoot, modchip, disc swap method) if you have an existing BIOS mod or flash chip pre-flashed with stock BIOS
  - **Blank flash chip-** If you have no way of flashing an individual SST 39VF040 chip, you can purchase a [FlashCatUSB XPORT](https://www.embeddedcomputers.net/products/FlashcatUSB_XPORT/) plus a [TSOP-32 (Type-B) adapter](https://www.embeddedcomputers.net/products/ParallelAdapters/), or use mi213's [TL866 adapter](https://oshpark.com/shared_projects/b8rGXXJj) (+ a hot air station to remove the flash chip after flashing) for TL866ii family flashers readily available from Aliexpress (or the [socketed](https://oshpark.com/shared_projects/coznPX7v) versions of mi213's boards to avoid needing a hot air station).
  - **Pre-flashed chip-** Follow instructions below to install a wire to allow your flash chip to be re-written with the patched BIOS.  Unfortunately, as the BIOS images are 512KB in size, a burned CD-R is required and the flashing program cannot be loaded fully through FreePSXBoot

## How to patch your BIOS dump .bin

Apply the supplied xdelta patch file to your original .bin BIOS dump using [DeltaPatcher](https://www.romhacking.net/utilities/704/), or another xdelta patching tool.

### Final checksums after patching will be
| BIOS version | CRC-32 |
| --- | --- |
| SCPH-1000 (NTSC-J) | |
| SCPH-1001 (NTSC-U/C) | |
| SCPH-1002 (PAL) | |
| SCPH-3000 (NTSC-J) | |
| SCPH-3500 (NTSC-J) | |
| SCPH-5500 (NTSC-J) | |
| SCPH-5501 (NTSC-U/C) | |
| SCPH-5502/5552 (PAL) | |
| SCPH-7000 (NTSC-J) | |
| SCPH-7001/7501/7503/9001 (NTSC-U/C) | | 
| SCPH-7002/7502/9002 (PAL) | |
| SCPH-7003 (NTSC-J) | |
| SCPH-100 (NTSC-J) | |
| SCPH-101 (NTSC-U/C) | 3A5B1E02 |
| SCPH-102A (PAL) | |
| SCPH-102B (PAL) | |

## How to insert patched BIOS .bin into flasher CD
Documentation forthcoming.

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

![image](https://user-images.githubusercontent.com/24398594/222043663-48d4d6f4-c71e-4ab5-bfca-150cd81e77d3.png)

2) **(Will's board only)** Disconnect this pin from VCC
A board from Will's Console Modifications requires that pin 7 be carefully lifted and soldered to. I was unable to do this personally, which is why I don't recommend his board. Attempting to sever the trace may be easier.
3) Wire this pin to the CPU's /SWR0 or /WR pin (pin 100)
((picture forthcoming of hopefully convenient test point on Slim PSOne))
((picture forthcoming of pin on serial port for earlier models))
4) Boot up your modified flasher disc via FreePSXBoot, a modchip, or the disc swap method and follow its instructions
5) Reconnect /WE pin to VCC and test your new BIOS!

## Supported hardware
## Tested

## Untested
### NTSC-U/C
- SCPH-1001**
- SCPH-5501
- SCPH-7001/7501 (should work for 7503 and 9001 as well)
- SCPH-101


### NTSC-J
- SCPH-1000**
- SCPH-3000**
- SCPH-3500**
- SCPH-5500
- SCPH-7000 (should work for 9000 as well)
- SCPH-7003 (identical to NTSC-U/C SCPH-5501)
- SCPH-100

### PAL
- SCPH-1002**
- SCPH-5502/5552
- SCPH-7002/7502/9002
- SCPH-102A
- SCPH-102B

** - Motherboards for these models may contain a 40-pin BIOS chip. I am not currently aware of any readily available replacement PCBs for these units, and the significant pinout differences would necessitate an adapter PCB.

## Todo
- Hardware verification
- Creating and verifying patches on each available BIOS revision
- Create documentation for modifying megavolt85's BIOS flashing program
- Add pictures to documentation for BIOS flashing modifications

## Special thanks
- nocash for his amazing help with developing this patch and for all his work on no$psx and extensive PSX documentation.  This project would not exist without him, and as he is currently forced to stay in a small room with family, I'd highly recommend supporting [his Patreon](https://www.patreon.com/martin_korth) or [donating directly](http://problemkaputt.de/donate.htm) if you are able!
- 8bitmods for developing the wonderful MemCard Pro and even offering me an advance unit for testing this patch with!
- megavolt85 for developing and releasing the source for their BIOS patching program.
- mi213 for their PS1 Flash BIOS project. See all their OSHPark uploads [here](https://oshpark.com/profiles/mi213).
