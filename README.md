RAM is expensive now. It has gone up in price by nearly 3x. There are less things we can do when RAM is this expensive. It used to cost $4/GiB, now about $16/GiB (Sri Lanka). The good news is you can likely still use the RAM module with one or more broken cells. The bad news is that in that for integrity it is better if you have ECC, but for normal use it is still likely okay to do this, just don't run bank databases or bridge simulations on this.

### Step 0: Confirm the faulty addresses by using MemTest86+.
MemTest86+ can be downloaded [here](https://github.com/memtest86plus/memtest86plus). Please note down the faulty addresses too, it _is important_. Check thoroughly. If your tests _passed_ but the computer shows symptoms of _RAM errors_, the error is likely in the buses (motherboard, likely in non-RAM areas and less likely but possibly in RAM bus too) or cables (HDD). Use a filesystem like ZFS and a volume manager like ... ZFS (ZFS is both) so the CPU knows that the data is damaged in the _disk/drive_ or _while in transit_. For anything remotely safety-critical, _do not use_ **NTFS/EXT4**, use a modern one like **ZFS** or **HAMMER2**, but only with **ECC RAM**.

### Step 1 (non-ECC RAM steps included)

#### ECC RAM
Be sure to blacklist adjacent addresses too, they use about 72 bits to store 64 bits, so be sure to blacklist around the 128 bits region (16 bytes) minimum for safety. If there are other faulty cells, _you **will** know_ as you already have ECC and are on a [SECDED](https://cr.yp.to/hardware/ecc.html) system.

#### Non-ECC RAM
Non-ECC RAM errors are harder to detect, and there is likelihood that more memory cells might latch or become unusable as most cells have comparable write/read cycles. DO NOT USE ZFS WITH THIS. The solutions are the same.

How do I fix the bad RAM issues?
#### Linux or GNU Mach
Use GRUB2 [BadRAM](https://www.gnu.org/software/grub/manual/grub/html_node/badram.html) addresses.
```
GRUB_BADRAM="ADDRESS,MASK,[ADDRESS,MASK[,ADDRESS,MASK]]"
#GRUB_BADRAM="0x00005555,0xffff0000" - RESERVE the 64K region where 0x00005555 is, as BAD.
```

#### FreeBSD
FreeBSD has two solutions, one is using GRUB2 and the other is using the BadRAM program specified here (Thanks to Kim Øyhus).   
The Kim Øyhus' program is run like this:  
```sh
gcc -o BadRAM BadRAM-FreeBSD_v0.0.c
# ./BadRAM 0x11111000 0xffffff00 &
./BadRAM [address] [mask]
```


#### Microsoft Windows (R)
This is a bit harder. There are multiple BadRAM drivers for Microsoft Windows (R).  
 - [BadMemory](https://github.com/prsyahmi/BadMemory) (GPL)
 - [badramwin](https://github.com/kwolekr/badramwin) by Ryan Kwolek, works on older MS Windows (R), untested on Windows 10 but likely will work (MIT/BSD)
 - [Another solution (BCD)](https://github.com/lavrensiyum/windows-bad-ram-solution) (Apache2)

Note: The article/text itself is not _AI slop_ but the images refered might be.  
Author: Rishikeshan Sulochana/Lavakumar  
License: CC-BY 4.0
