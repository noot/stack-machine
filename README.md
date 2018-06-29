# stack-machine
simple c implementation of a virtual stack machine

# to run

./stackVM program.S

./stackVM program.S program1.S

and so on.

# opcodes

```

00 halt
01 push
02 pop
03 add
04 sub
05 mod

10 b
11 bnz
12 bz
13 ret
1E swap
1F cmp

FF nop

```
any invalid opcode is assumed to be a nop
