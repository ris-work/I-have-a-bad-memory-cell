RAM is expensive now. It has gone up in price by nearly 3x. There are less things we can do when RAM is this expensive.

### ECC RAM
Be sure to blacklist adjacent addresses too, they use about 72 bits to store 64 bits, so be sure to blacklist around the 128 bits region (16 bytes) minimum for safety. If there are other faulty cells, _you **will** know_.

### Non-ECC RAM
Non-ECC RAM errors are harder to detect, and there is likelihood that more memory cells might latch or become unusable as most cells have comparable write/read cycles. The solutions are the same.

How do I fix the bad RAM issues?
### Linux
Use GRUB2 BadRAM addresses.

### FreeBSD
FreeBSD has two solutions, one is using GRUB2 and the other is using the BadRAM program specified here (Thanks to ...). 

### Microsoft Windows (R)
This is a bit harder. There are multiple BadRAM drivers for Microsoft Windows (R).  
 - [BadMemory](https://github.com/prsyahmi/BadMemory)
 - [Another solution (BCD)](https://github.com/lavrensiyum/windows-bad-ram-solution)

Author: Rishikeshan Sulochana/Lavakumar  
License: CC-BY 4.0
