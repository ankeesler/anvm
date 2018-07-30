```
anvm = Andrew's Virtual Machine
       ^^       ^       ^
```

Object model:
- A Program is a bunch of executable instructions and area for an execution stack.
- A CPU has a program counter, a stack pointer, a status register, and some general purpose registers.
- A System is a thing that runs Program's on a CPU.

To run the tests you need to download [bazel](https://github.com/bazelbuild/bazel/releases). Then, run:
```bash
./script/test.sh
```

To build the assembler, you need to download [bazel](https://github.com/bazelbuild/bazel/releases). Then, run:
```bash
bazel build //src/assembler:anasm
```

This project uses at least C++11.
