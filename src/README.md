Here are some of the conventions of this VM.

- When referencing the PC register in a statement (such as `LOAD %pc %r0`), the PC value will be taken
  from the beginning of the statement, i.e., the address of the `LOAD` instruction `Word` at the beginning
  of the aforementioned example.
