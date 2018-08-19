```
anvm = Andrew's Virtual Machine
       ^^       ^       ^
```

Object model:
- A Program is a bunch of executable instructions and area for an execution stack.
- A CPU has a program counter, a stack pointer, a status register, and some general purpose registers.
- A System is a thing that runs Program's on a CPU.

To do anything in this repo, first run the [`bootstrap.sh`](script/bootstrap.sh) script to download dependencies. Then, use the Makefile to run the tests.
```bash
./script/bootstrap.sh
make test
```

To build the assembler, run this command (after the above step).
```bash
bazel build //src/asm:anasm
```

This project uses at least C++11.
