# CallDump
A tool to dump informations about the caller of a function.

Use case: Find out what calls a function if you can't attach a debugger.


To dump informations just follow these steps:

0. Specify the path to the log file.
1. Get the address of the function you want to know the caller informations.
2. Hook the target function with one of the predefined functions or create your own and call `DumpAddressInfo` from there.

The dumped file will look like this:
```
Found Return Address: 0x63b8e6f

Module: CallDump.dll (0x6310000)   Offset: 0xa8e6f

Instructions:
00000000063b8e50   push eax
00000000063b8e51   call 0x0634B38D
00000000063b8e56   add esp, 0x0C
00000000063b8e59   mov esi, esp
00000000063b8e5b   push 0x00000000
00000000063b8e5d   push 0x063C59AC
00000000063b8e62   push 0x063C59AC
00000000063b8e67   push 0x00000000
00000000063b8e69   call  [0x063CE218]
00000000063b8e6f   cmp esi, esp
00000000063b8e71   call 0x0634A8B6
00000000063b8e76   mov eax, 0x00000001
00000000063b8e7b   jmp 0x063B8E7F
00000000063b8e7d   xor eax, eax
00000000063b8e7f   pop edi
00000000063b8e80   pop esi
00000000063b8e81   pop ebx

Hex Dump:
00000000063b8e07  cc cc cc cc cc cc cc cc  cc cc cc cc cc cc cc cc  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ
00000000063b8e17  cc cc cc cc cc cc cc cc  cc 55 8b ec 81 ec c0 00  ÌÌÌÌÌÌÌÌÌU‹ììÀ.
00000000063b8e27  00 00 53 56 57 8d bd 40  ff ff ff b9 30 00 00 00  ..SVW½@ÿÿÿ¹0...
00000000063b8e37  b8 cc cc cc cc f3 ab 83  7d 0c 01 75 39 cc 05 68  ¸ÌÌÌÌó«ƒ}..u9Ì.h
00000000063b8e47  e7 b8 34 06 a1 18 e2 3c  06 50 e8 37 25 f9 ff 83  ç¸4.¡.â<.Pè7%ùÿƒ
00000000063b8e57  c4 0c 8b f4 6a 00 68 ac  59 3c 06 68 ac 59 3c 06  Ä.‹ôj.h¬Y<.h¬Y<.
00000000063b8e67  6a 00 ff 15 18 e2 3c 06  3b f4 e8 40 1a f9 ff b8  j.ÿ..â<.;ôè@.ùÿ¸
00000000063b8e77  01 00 00 00 eb 02 33 c0  5f 5e 5b 81 c4 c0 00 00  ....ë.3À_^[ÄÀ..
00000000063b8e87  00 3b ec e8 27 1a f9 ff  8b e5 5d c2 0c 00 cc cc  .;ìè'.ùÿ‹å]Â..ÌÌ
00000000063b8e97  cc cc cc cc cc cc cc cc  cc cc cc cc cc cc cc cc  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ
00000000063b8ea7  cc cc cc cc cc cc cc cc  cc cc cc cc cc cc cc cc  ÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌÌ
00000000063b8eb7  cc cc cc cc cc cc cc cc  cc 55 8b ec 81 ec e8 00  ÌÌÌÌÌÌÌÌÌU‹ììè.
00000000063b8ec7  00 00 53 56 57 8d bd 18  ff ff ff b9 3a 00 00 00  ..SVW½.ÿÿÿ¹:...
```