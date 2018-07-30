This is an assembly language used to communicate anvm machine code.

Here is a rough layout of the language.
- A Function is an ordered list of Statement's.
- A Statement is a machine Instruction plus some Arg's.
- An Instruction is the machine word for that particular routine (e.g., ADD, LOAD, STORE, etc.).
- An Arg is something that is passed to an Instruction; it can be a LITERAL, REGISTER, REFERENCE, or REGISTER\_REFERENCE.
- A LITERAL is a constant value (e.g., 1, 5, etc.).
- A REGISTER is a value that is taken from a register that exists on the virtual anvm CPU (e.g., %r0, %r1, %sp, %pc, etc.).
- A REFERENCE is an index into memory (e.g., @64, @1024, etc.).
- A REGISTER\_REFERENCE is a value taken from an index into memory given by the register (e.g., @%r0, @%r1).

Here is a BNFish description of the language.
```
<function> ::= <name> ":" <EOL> <statements> <EOL>

<statements> ::= <statement> | <statements> <statement>
<statement>  ::= <instruction> <space> <args> <EOL>

<args> ::= <arg> | <args> <space> <arg>
<arg>  ::= <number> | "@" <number> | "%r" <number> | "@%r" <number> | "%rsp" | "@%rsp"

<name>         ::= <name-stuff> | <word> <name-stuff>
<name-stuff>   ::= [A-Za-z0-9_]
<number-stuff> ::= [0-9]
<number>       ::= <number> | <number-stuff> <number>
<space>        ::= " "
```

When a binary file is written, the parsed functions are written in order of appearance in the .asm file.
The binary file is in big-endian.
