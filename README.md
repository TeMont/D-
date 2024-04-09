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
- [ ] Float Variables
- [ ] Const Variables
- [ ] Logical Operators
- [ ] If Statement
- [ ] Loops

# GUIDE
Dependencies: Nasm x86-64, GoLink

### Building
Dependencies For Building: C++23
git clone https://github.com/TeMont/X-lang.git
cd dust-lang
mkdir build
cd build
cmake ..
cmake --build .

# Code Example

int x = 2;
int y;
int o;

y = x + 8 * 2;
o = y / x;

return o;

# Code Compile
### Terminal
./XComp.exe input.xy
./result.exe

### OUTPUT 
9