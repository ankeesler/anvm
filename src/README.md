Here are some of the conventions of this VM.

- When referencing the PC register in a statement (such as `LOAD %pc %r0`), the PC value will be taken
  from the beginning of the statement, i.e., the address of the `LOAD` instruction `Word` at the beginning
  of the aforementioned example.
- The arguments to instructions are always in the format `from`, `to`. For example, `LOAD %r0 %r1` will load
  the value in register 0 into register 1; `STORE %r1 @64` will store the value in register 1 into memory
  address 64.
