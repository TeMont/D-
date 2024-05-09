# X-lang
Compiled programming language created with support of basic features.
The compiler is made using NASM & GoLink.

# done
- [X] Variables
- [X] Declaring Without Value
- [X] Return Statement
- [X] String Variables
- [X] Integer Variables
- [X] Bool Variables
- [X] Char Variables
- [X] Math operations with precedence
- [X] Logical Operators
- [X] If Statement
- [X] Elif Statement
- [X] Else Statement
- [X] One-Line Comments
- [X] Multi-Line Comments
- [X] Output
- [X] Input
- [X] ! operator
- [ ] Float Variables
- [ ] Const Variables
- [ ] Loops
- [ ] Negative Integers
- [ ] Switch-case statement
- [ ] Increment and Decrement

# GUIDE
Dependencies: Nasm x86-64, gcc Linker

### Building
Dependencies For Building: C++23
```bash
git clone https://github.com/TeMont/X-lang.git
cd X-lang
mkdir build
cd build
cmake ..
cmake --build .
```
# Code Example

```c
int firstNum;
int secondNum;
int result;
char symbol;

firstNum = stdInput("Enter First Num: ");
symbol = stdInput("Enter Action Symbol: ");
secondNum = stdInput("Enter Second Num: ");

if (symbol == '+')
{
    result = firstNum + secondNum;
}
elif (symbol == '-')
{
    result = firstNum - secondNum;
}
elif (symbol == '*')
{
    result = firstNum * secondNum;
}
elif (symbol == '/')
{
    result = firstNum / secondNum;
}
else
{
    stdOut("You Entered Incorrect Symbol");
    return 1;
}
stdOut(result);
return 0;
```

# Code Compile
### Terminal
```bash
./XComp.exe <filename>.xy
./<filename>.exe
```