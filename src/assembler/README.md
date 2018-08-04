This is an assembly language used to communicate anvm machine code.

Here is a rough layout of the language.
- A Function is an ordered list of Statement's.
- A Statement is a machine Instruction plus some Arg's.
- An Instruction is the machine word for that particular routine (e.g., ADD, LOAD, STORE, etc.).
- An Arg is something that is passed to an Instruction; it can be a VALUE, REGISTER, REFERENCE, REGISTER\_REFERENCE, or SYMBOL.
- A VALUE is a constant literal number (e.g., #1, #5, etc.).
- A REGISTER is a value that is taken from a register that exists on the virtual anvm CPU (e.g., %r0, %r1, %sp, %pc, etc.).
- A REFERENCE is an index into memory (e.g., @64, @1024, etc.).
- A REGISTER\_REFERENCE is a value taken from an index into memory given by the register (e.g., @%r0, @%r1).
- A SYMBOL resolves to the value of the function or variable (e.g., $foo, $tuna).

Here is a BNFish description of the language.
```
<functions> ::= <function> | <functions> <function>
<function>  ::= <symbol> ":" <EOL> <statements> <EOL>

<statements>  ::= <statement> | <statements> <statement>
<statement>   ::= <instruction> " " <args> <EOL>
<instruction> ::= <symbol> | <instruction> <symbol>

<args>               ::= <arg> | <args> " " <arg>
<arg>                ::= <value> | <register> | <reference> | <reference_register> | <symbol>
<value>              ::= "#" <number>
<register>           ::= "%r" <number> | "%r" "sp" | "%r" "pc"
<reference>          ::= "@" <number>
<reference_register> ::= "@" <register>
<symbol>             ::= "$" <symbol>

<EOL>     ::= "\n" | <comment> "\n"
<comment> ::= ";;" <stuff>

<symbol> ::= [A-Za-z0-9_]+
<number> ::= [0-9]+
<stuff>  ::= [^\n]+
```

When a binary file is written, the parsed functions are written in order of appearance in the .asm file.
The binary file is in big-endian.
