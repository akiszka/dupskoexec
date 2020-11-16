# dupskoexec
> This is a programming language similar to Assembler in syntax. It compiles to bytecode. Right now, there is a compiler, a bytecode interpreter and a JIT compiler.
## Building
### Dependencies
Building dupskoexec requires the following:
* [the tup build system](http://gittup.org/tup/index.html)
* any C compiler (by default clang is used)
### How to build dupskoexec?
If you're building dupskoexec for the first time, run:
```
$ tup init
```
After that, building is as simple as running:
```
$ tup
```
To test if dupskoexec works, you can try one of the examples:
```
$ ./dupcc -f examples/<anyexample>.du -o a.out
$ ./dupexec -f a.out
```
## Usage
### The regular compiler
The regular compiler is called dupcc and it compiles source code to bytecode. It accepts two command line arguments:

argument | description
:---: | :---
-f | input file name
-o | output file name

The compilation process works in two steps:
1. The source file is analyzed line by line and instruction codes are generated. When the compiler encounters a function, it takes note of that. When the compiler encounters a call to a function, it checks if a function was defined earlier. If it wasn't, it stores the call for later.
2. If there are any calls left to fill at the end of the compilation, the compiler goes through them and compares them with the defined functions. If there is a function defined, it replaces a dummy address with the address of the function.

#### The instructions and their underlying instruction codes (opcodes)

instruction | generated opcode (hex) | description
--- | --- | ---
pop | 0 | removes the upmost value from the stack
nop | 1 | does nothing
end | 2 | ends the program
add | 3 | pops two upmost values from the stack, adds them, pushes the result
sub | 4 | pops two upmost values from the stack, subtracts them, pushes the result
mul | 5 | pops two upmost values from the stack, multiplies them, pushes the result
div | 6 | pops two upmost values from the stack, divides them, pushes the result
and | 7 | pops two upmost values from the stack, performs a bitwise AND on them, pushes the result
or | 8  | pops two upmost values from the stack, performs a bitwise OR on them, pushes the result
xor | 9 | pops two upmost values from the stack, performs a bitwise XOR on them, pushes the result
not | A | pops the upmost value from the stack, persorms a bitwise NOT on it, pushes the result
read | B| reads one char from the keyboard and pushes it on the stack
write|C|  pops the upmost character from the stack, prints it ont the screen
fun \<name> | \<none> | defines a function called \<name>
call \<name> | DXX| calls the function called \<name>, the Xs in the opcode are the function's address
push \<value> |EXX| pushes \<value> (one byte in size) on the stack, the Xs in the opcode are the value to push
pushc \<char> |EXX| pushes the numerical value of the character \<char> on the stack, the Xs as above
pushs \<string> |EXXEXXEXX...| pushes the string \<string> to the stack character by character, the Xs are the values of the consequent characters to push
return | F| returns from the current function
times \<number> \<instruction> | ? | has the same effect as typing \<instruction> exactly \<number> times

### The bytecode interpreter
If you want to execute any program compiled with dupcc, you have to use the bytecode interpreter, which understands the afore-mentioned opcodes. It's called dupexec and accepts only one command-line argument.

argument | description
:---: | :---
-f | input file name

### The JIT (Just-In-Time) compiler
In case you don't want to compile your code and just execute the source file right away, there is always the JIT compiler. It's called dupjit and accepts only one command-line argument:

argument | description
:---: | :---
-f | input file name

The JIT compiler works in a series of steps:
1. Get a line from the input file
2. Compile the single line
3. Execute the single line
4. Go to step 1 until the input file is finished

This way, the JIT compiler can compile code while it's executed.

