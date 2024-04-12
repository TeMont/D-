# X-lang
Сompiled programming language created with support of basic features.
The compiler is made using NASM & GoLink.

# done
- [X] Variables
- [X] Declaring Without Value
- [X] Return Statement
- [X] String Variables
- [X] Integer Variables
- [X] Bool Variables
- [X] Math operations with precedence
- [X] Logical Operators
- [X] If Statement
- [ ] Elif Statement
- [ ] Else Statement
- [ ] Float Variables
- [ ] Const Variables
- [ ] Loops

# GUIDE
Dependencies: Nasm x86-64, GoLink

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
int x = 2;
int y;
int o;

y = x + 8 * 2;
o = y / x;

return o;
```

# Code Compile
### Terminal
```bash
./XComp.exe input.xy
./result.exe
```

### OUTPUT 
9