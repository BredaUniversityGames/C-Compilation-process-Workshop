# C++ Compilation process Workshop 2021

by Simon Renger <170636@buas.nl / simon.renger@gmail.com>

----

## Preparations

At first we need to install the right compiler and linker

**Windows**

If we have Visual Studio 19/22 installed with the C++ extensions or VS Code with the C++ tooling we have our linker and compiler installed.

> **Note:** How to use the compiler from the command line follow this guide https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170

**Linux** Debain/Ubuntu

On Linux we need to install the basic build essentials. This includes the default compiler (usually GCC GNU C Compiler) and the linker

> **Note:** if you want to use the WSL follow these instructions https://docs.microsoft.com/en-us/windows/wsl/install and than use the windows store / Microsoft store and download Ubuntu or Debian

Install (gcc):

```
sudo apt update
sudo apt install build-essential
gcc --version
```

Install (clang)

```
sudo apt update
sudo apt install clang
clang --version
```

## Overview

The C/C++ compilation process takes one source file at the time and creates a Translation unit of them. Individually they do not now anything of each other! The linker will combine them at the end.

The compilation process of a translation unit (TU) looks as following:

1. Source files are cleaned
2. We parse the file and run the pre-processor
   1. each introduced include goes into step 1
   2. All pre-processor directives are removed from the source.
3. Literal strings and characters are translated
4. Compilation takes place of tokens: *translation units*
5. Template instanton happens `instantiation units`
6. Linking

> **Note:** Some compilers don't implement instantiation units (also known as [template repositories](http://docs.oracle.com/cd/E18659_01/html/821-1383/bkagr.html#scrolltoc) or [template registries](http://www-01.ibm.com/support/knowledgecenter/SSXVZZ_12.1.0/com.ibm.xlcpp121.linux.doc/compiler_ref/fcat_template.html?lang=en)) and simply compile each template instantiation at Phase 7 (4), storing the code in the object file where it is implicitly or explicitly requested, and then the linker collapses these compiled instantiations into one at Phase 9 (6). (https://en.cppreference.com/w/cpp/language/translation_phases)

## Pre-processor

Did we not forget the pre-processor?

```
#define value 1
// I am a comment
int main(){
int var{value};
}
```

If we now compile this we can store the pre processor output in a file:

**Windows**

```
cl.exe /c /P preprocessor.cpp
```

**Linux**

```
gcc preprocessor.cpp -E>preprocessor.i
clang preprocessor.cpp -E>preprocessor.i
```

We now see a file names: `preprocessor.i` :

```
#line 1 "preprocessor.cpp"

int main(){
int var{1};
}
```

The line `int var{value};` has been changed to `int var{1};` and the `#define value 1` was erased! Also the comment was removed!

### Inclusion

If you include any header file the pre processor will include them recursively.

```
headerA.hpp
struct StructA{};
headerB.hpp
#include "headerA.hpp"
struct StructB{};
source.cpp
#include "headerB.hpp"

int main(){
    StructA struct_a{};
}
```

If we now compile this we can store the pre processor output in a file:

**Windows**

```
cl.exe /c /P source.cpp
```

**Linux**

```c
gcc preprocessor.cpp -E>preprocessor.i
clang preprocessor.cpp -E>preprocessor.i
```

We now see a file names: `source.i` or `linux`:

```cpp
# 1 "preprocessor/source.cpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 383 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "preprocessor/source.cpp" 2
# 1 "preprocessor/headerB.hpp" 1
# 1 "preprocessor/headerA.hpp" 1
struct StructA{};
# 2 "preprocessor/headerB.hpp" 2
struct StructB{};
# 2 "preprocessor/source.cpp" 2

int main(){
    StructA struct_a{};
}
```

### Header Guards

As you could see Includes are recursive this can lead to cycle includes. 

```
cycleA.hpp
#include "cycleB.hpp"
cycleB.hpp
#include "cycleA.hpp"
```

if we now compile this with (clang, gcc, msvc)

**Windows**

```
cl.exe /c /P cycleA.cpp
```

**Linux**

```
clang cycleA.hpp -E>cycleA.i
gcc cycleA.hpp -E>cycleA.i
```

Result will be:

```bash
user@MININT-DAH9RK3:/mnt/d/cpplecture$ clang cycleA.hpp -E>preprocessor.i
In file included from preprocessor/cycleA.hpp:1:
In file included from preprocessor/cycleB.hpp:1:
In file included from preprocessor/cycleA.hpp:1:
[edit by Simon replaced 200 more of `In file included from preprocessor/cycleA.hpp:1:` with this line]
preprocessor/cycleB.hpp:1:10: error: #include nested too deeply
#include "cycleA.hpp"
```

if we add now a `#pragma once` to `cycleB.hpp`

```c
#pragma once
#include "cycleA.hpp"
```

the compiler (clang, gcc, msvc) will run and spill out:

**Windows**

```
cl.exe /c /P cycleA.cpp
```

**Linux**

```
clang cycleA.hpp -E>cycleA.i
gcc cycleA.hpp -E>cycleA.i
```

The result:

```c
# 1 "preprocessor/cycleA.hpp"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 383 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "preprocessor/cycleA.hpp" 2
# 1 "preprocessor/cycleB.hpp" 1

# 1 "preprocessor/cycleA.hpp" 1
# 2 "preprocessor/cycleB.hpp" 2
# 1 "preprocessor/cycleA.hpp" 2
```

## Compile a source file

The basic source file:

```c++
int main(){}
```

Invoke the compiler:

**Windows**

```
cl.exe /c empty.cpp
```

**Linux**

```
clang -c empty.cpp
gcc -c empty.cpp
```

This generates a object file. `.obj` or `.o` on linux.

## Assembler

This way we skipped one step: The assembler! This is the step in which the compiler creates a assembly representation for your target platform!

To get the output that is used to generate the Object File you must tell the compiler to generate the assemble output for you:

**Windows**

```
cl.exe /FAs /c empty.cpp
```

This will produce as `.asm` file with your assembly

**Linux**

```
clang -S 
```

This will produce a `.s` file with your assembly.

### Inspecting a object file

Normally the compiler does the assembler step automatically and generates the object file. A object file contains all important information about the current translation unit. The most important question is: Which external objects are references? This is important since a TU does not know anything about other TU's! The linker needs this information later to stitch the program together!

> **Note:** A Object file is on windows and Linux Systems different. Since on Windows it is a COFF generated by Microsoft C compiler and on Linux ELF/DWARF.
>
> - https://en.wikipedia.org/wiki/COFF
> - https://en.wikipedia.org/wiki/Executable_and_Linkable_Format

We can view a Object file with:

**Windows**

More infos: https://docs.microsoft.com/en-us/cpp/build/reference/dumpbin-options?view=msvc-170

```bash
DUMPBIN.EXE /ALL empty.obj
```

**Linux**

```bash
objectdump -a empty.o
```

> **Note:** There could be a entire lecture about this! https://www.youtube.com/watch?v=a5L66zguFe4



## Linker

In this stage of the process multiple object files will be combined to a executable!

### Executable

Lets build a game:

```
/game/main.cpp
/lib/engine.hpp
/lib/engine.cpp
main.cpp
#include "engine.hpp"

int main(){
    engine{create()};
}
engine.hpp
struct engine{
// stuff
};

[[nodiscard]] engine create() noexcept;
engine.cpp
#include "engine.hpp"

[[nodiscard]] engine create() noexcept{
    return {};
}
```

Let us compile the game:

**Windows**

```bash
cd /game
cl.exe \c main.cpp
```

**Linux**

```
cd /game
clang -c main.cpp -std=c++17
gcc -c main.cpp -std=c++17
```

This will spill out a compile error!

```
PS D:\cpplecture\linker\game> cl.exe /c main.cpp
Microsoft (R) C/C++ Optimizing Compiler Version 19.29.30136 for x86
Copyright (C) Microsoft Corporation.  All rights reserved.
main.cpp
main.cpp(1): fatal error C1083: Cannot open include file: 'engine.hpp': No such file or directory
```

>  **Note:** On Linux we will have the same result!

We need to tell the compiler where the header file is! `/I[..]` or `-I[..]`

```
PS D:\cpplecture\linker\game> cl.exe /c main.cpp /I ../lib
```

This works! we have a obj file!

Let us investigate what is in the object file: We run `DUMPBIN.EXE /ALL main.obj`

```
PS D:\cpplecture\linker\game> DUMPBIN.EXE /ALL main.obj
Microsoft (R) COFF/PE Dumper Version 14.29.30136.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file main.obj

File Type: COFF OBJECT

FILE HEADER VALUES
             14C machine (x86)
               4 number of sections
        61A8E6C9 time date stamp Thu Dec  2 16:31:21 2021
             193 file pointer to symbol table
               D number of symbols
               0 size of optional header
               0 characteristics

SECTION HEADER #1
.drectve name
       0 physical address
       0 virtual address
      2F size of raw data
      B4 file pointer to raw data (000000B4 to 000000E2)
       0 file pointer to relocation table
       0 file pointer to line numbers
       0 number of relocations
       0 number of line numbers
  100A00 flags
         Info
         Remove
         1 byte align

RAW DATA #1
  00000000: 20 20 20 2F 44 45 46 41 55 4C 54 4C 49 42 3A 22     /DEFAULTLIB:"
  00000010: 4C 49 42 43 4D 54 22 20 2F 44 45 46 41 55 4C 54  LIBCMT" /DEFAULT
  00000020: 4C 49 42 3A 22 4F 4C 44 4E 41 4D 45 53 22 20     LIB:"OLDNAMES"

   Linker Directives
   -----------------
   /DEFAULTLIB:LIBCMT
   /DEFAULTLIB:OLDNAMES

SECTION HEADER #2
.debug$S name
       0 physical address
       0 virtual address
      74 size of raw data
      E3 file pointer to raw data (000000E3 to 00000156)
       0 file pointer to relocation table
       0 file pointer to line numbers
       0 number of relocations
       0 number of line numbers
42100040 flags
         Initialized Data
         Discardable
         1 byte align
         Read Only

RAW DATA #2
  00000000: 04 00 00 00 F1 00 00 00 67 00 00 00 29 00 01 11  ....ñ...g...)...
  00000010: 00 00 00 00 44 3A 5C 63 70 70 6C 65 63 74 75 72  ....D:\cpplectur
  00000020: 65 5C 6C 69 6E 6B 65 72 5C 67 61 6D 65 5C 6D 61  e\linker\game\ma
  00000030: 69 6E 2E 6F 62 6A 00 3A 00 3C 11 01 22 00 00 07  in.obj.:.<.."...
  00000040: 00 13 00 1D 00 B8 75 00 00 13 00 1D 00 B8 75 00  .....¸u......¸u.
  00000050: 00 4D 69 63 72 6F 73 6F 66 74 20 28 52 29 20 4F  .Microsoft (R) O
  00000060: 70 74 69 6D 69 7A 69 6E 67 20 43 6F 6D 70 69 6C  ptimizing Compil
  00000070: 65 72 00 00                                      er..

SECTION HEADER #3
.text$mn name
       0 physical address
       0 virtual address
      12 size of raw data
     157 file pointer to raw data (00000157 to 00000168)
     169 file pointer to relocation table
       0 file pointer to line numbers
       1 number of relocations
       0 number of line numbers
60500020 flags
         Code
         16 byte align
         Execute Read

RAW DATA #3
  00000000: 55 8B EC 51 E8 00 00 00 00 88 45 FF 33 C0 8B E5  U.ìQè.....Eÿ3À.å
  00000010: 5D C3                                            ]Ã

RELOCATIONS #3
                                                Symbol    Symbol
 Offset    Type              Applied To         Index     Name
 --------  ----------------  -----------------  --------  ------
 00000005  REL32                      00000000         9  ?create@@YA?AUengine@@XZ (struct engine __cdecl create(void))

SECTION HEADER #4
 .chks64 name
       0 physical address
       0 virtual address
      20 size of raw data
     173 file pointer to raw data (00000173 to 00000192)
       0 file pointer to relocation table
       0 file pointer to line numbers
       0 number of relocations
       0 number of line numbers
     A00 flags
         Info
         Remove
         (no align specified)

RAW DATA #4
  00000000: 23 07 66 15 27 1A BF 1A FD FD 6A 15 8D E6 A5 E2  #.f.'.¿.ýýj..æ¥â
  00000010: 42 E8 D5 96 AB C2 64 E1 00 00 00 00 00 00 00 00  BèÕ.«Âdá........

COFF SYMBOL TABLE
000 010575B8 ABS    notype       Static       | @comp.id
001 80010191 ABS    notype       Static       | @feat.00
002 00000002 ABS    notype       Static       | @vol.md
003 00000000 SECT1  notype       Static       | .drectve
    Section length   2F, #relocs    0, #linenums    0, checksum        0
005 00000000 SECT2  notype       Static       | .debug$S
    Section length   74, #relocs    0, #linenums    0, checksum        0
007 00000000 SECT3  notype       Static       | .text$mn
    Section length   12, #relocs    1, #linenums    0, checksum 6BED4AA5
009 00000000 UNDEF  notype ()    External     | ?create@@YA?AUengine@@XZ (struct engine __cdecl create(void))
00A 00000000 SECT3  notype ()    External     | _main
00B 00000000 SECT4  notype       Static       | .chks64
    Section length   20, #relocs    0, #linenums    0, checksum        0

String Table Size = 0x1D bytes

  Summary

          20 .chks64
          74 .debug$S
          2F .drectve
          12 .text$mn
```

Under `RELOCATIONS #3` we find something intersting:

```
                                      Symbol    Symbol
 Offset    Type  Applied   To         Index     Name

00000005  REL32 00000000               9       ?create@@YA?AUengine@@XZ (struct engine __cdecl create(void))
```

This somehow looks like: `[[nodiscard ]] engine create() noexcept`. That is correct! In C++ we mangle names we (the compiler) renames the name after they implementation defined scheme (yes clang uses a different method than msvc! GREAT!)

> The reason for this is that we can have overloads (in short!) more infos here https://www.geeksforgeeks.org/extern-c-in-c/

This means the compiler stores in the object file all sorts of interesting information about this TU (translation unit). In this case the compiler stores that at a offset of 5 the symbol `?create@@YA?AUengine@@XZ (struct engine __cdecl create(void))` needs to be relocated! Relocated means that it can be found external!

Enough of this inspection lets us create our executable!

```
PS D:\cpplecture\linker\game> link.exe  main.obj
```

This also leads to a error!

```cpp
Microsoft (R) Incremental Linker Version 14.29.30136.0
Copyright (C) Microsoft Corporation.  All rights reserved.

main.obj : error LNK2019: unresolved external symbol "struct engine __cdecl create(void)" (?create@@YA?AUengine@@XZ) referenced in function _main
main.exe : fatal error LNK1120: 1 unresolved externals
```

Do we remember:

> This means the compiler stores in the object file all sorts of interesting information about this TU (translation unit). In this case the compiler stores that at a offset of 5 the symbol `?create@@YA?AUengine@@XZ (struct engine __cdecl create(void))` needs to be relocated! Relocated means that it can be found external!

This is what it means in practice! Oh right we did not compile `lib/engine.cpp` Let us fix this! 

```
PS D:\cpplecture\linker\game> cd ..
PS D:\cpplecture\linker> cd lib
PS D:\cpplecture\linker\lib> cl.exe /c engine.cpp
```

Now we have a `lib/engine.obj`

cool lets go back to our game and try again!

```
PS D:\cpplecture\linker\game> link.exe  main.obj
Microsoft (R) Incremental Linker Version 14.29.30136.0
Copyright (C) Microsoft Corporation.  All rights reserved.

main.obj : error LNK2019: unresolved external symbol "struct engine __cdecl create(void)" (?create@@YA?AUengine@@XZ) referenced in function _main
main.exe : fatal error LNK1120: 1 unresolved externals
```

We get the same error! But we compiled the other thing...

This time we have to remmeber:

> Translation Units are looked at individually and main.obj does not know anything of engine.obj!

This means we need to tell the linker **where** our external symbols are living!

```
PS D:\cpplecture\linker\game> link.exe  main.obj ../lib/engine.obj
```

This works we get out executable!



### Library

In reality projects are really big and sometimes we split them into libraries. In general a library is nothing else than a collection of object files the linker can use statically or dynamically!

> **Note:** This overview will not touch the dynamic approach since this is a huge topic on its own!



Lets upgrade our "engine" example a bit:

```
engine.hpp
#pragma once
using cstring = const char*;

template<typename T>
struct container_t{
    T* items;
    unsigned int num_items;
    unsigned int capacity;
};


struct entity_t {
    unsigned int id;
};


struct world_t{
    container_t<entity_t> entities;
};


struct engine_t{
    void update(double dt);
    entity_t spawn();
    world_t world;    
};

[[nodiscard ]] engine_t create() noexcept;
#include "engine.hpp"

[[nodiscard]] engine_t create() noexcept{
    engine_t e{};
    e.world.entities.items = new entity_t[100];
    e.world.entities.capacity = 100;
    return e;
}

entity_t engine_t::spawn(){
    if(world.entities.num_items == world.entities.capacity){
        // magic
    }else{
        world.entities.items[world.entities.num_items] = {world.entities.num_items};
    }
    return {world.entities.num_items};
}

void engine_t::update(double dt){
    //...
}
```

Let us add a json parser ... every good C++ project needs one!

```
json.hpp
#pragma once

using cstring = const char*;

struct string_view{
    cstring data_ptr;
    unsigned int len;
};

struct json{
//..
};

struct json_object{
//..
};

struct json_array{
    json_object* objects;
    unsigned int len;
};

[[nodiscard]] json parse(cstring path) noexcept;

[[nodiscard]] json_object get_object(const json& root,cstring key) noexcept;

[[nodiscard]] json_array get_array(const json& root,cstring key) noexcept;

[[nodiscard]] string_view get_string(const json& root,cstring key) noexcept;

[[nodiscard]] double get_number(const json& root,cstring key) noexcept;
#include "json.hpp"

[[nodiscard]] json parse(cstring path) noexcept{
    return {};
}

[[nodiscard]] json_object get_object(const json& root,cstring key) noexcept{
    return {};
}

[[nodiscard]] json_array get_array(const json& root,cstring key) noexcept{
    return {};
}

[[nodiscard]] string_view get_string(const json& root,cstring key) noexcept{
    return {};
}

[[nodiscard]] double get_number(const json& root,cstring key) noexcept{
    return {};
}
```

**The game:**

```
level.hpp
#include "engine.hpp"

struct level{
    world_t world;
};


 void load(engine_t& engine, cstring level) noexcept;
level.cpp
#include "engine.hpp"
#include "json.hpp"

 void load(engine_t& engine, cstring level) noexcept{
    json l{parse(level)};
}
main.cpp
#include "engine.hpp"
#include "level.hpp"

int main(){
    engine_t core{create()};
    load(core,"Test_level.json");
}
```

Let us build a `.lib` or `.a` file.

> **Note**: again a static lib is nothing else than a collection (or archive) or object files. This is why the name on Linux is .a which stands for *archive* and the tool on Linux for creating one is called `ar` https://linux.die.net/man/1/ar

At first we need to compile both of our translation units (source files):

**Windows**

```
cd lib
PS D:\cpplecture\linker\lib> cl.exe /c engine.cpp json.cpp
--> engine.obj
--> json.obj
```

**Linux**

```
clang -c engine.cpp json.cpp -std=c++17
gcc -c engine.cpp json.cpp -std=c++17
--> json.o
--> engine.o
```

Now we need to stich those object files together this can be done via the `lib.exe` or the `ar` tool on linux:

**Windows**

```
lib.exe .\json.obj .\engine.obj /out:engine.lib
```

> https://docs.microsoft.com/en-us/cpp/build/reference/overview-of-lib?view=msvc-170

**Linux**

```
ar rc engine.a json.o engine.o
```

> https://llvm.org/docs/CommandGuide/llvm-ar.html or https://linux.die.net/man/1/ar



Now we have a lib: `engine.lib` or `engine.a` and we can now link against this with `ld` (on Linux) or `link.exe` on windows just like we were to link object files:

**Windows**

```
 cl.exe main.cpp level.cpp /c /I ../lib
 link.exe .\main.obj .\level.obj ../lib/engine.lib /out:game.exe
 --> game.exe
```

**Linux**

```
clang main.cpp level.cpp -c -I ../lib
ld .\main.o .\level.o ../lib/engine.a -o game
 --> game
```



## Extra

If we now inspect the `engine.obj`

```
PS D:\cpplecture\linker\game> DUMPBIN.EXE /RELOCATIONS engine.lib
Microsoft (R) COFF/PE Dumper Version 14.29.30136.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file engine.lib

File Type: LIBRARY

RELOCATIONS #3
                                                Symbol    Symbol
 Offset    Type              Applied To         Index     Name
 --------  ----------------  -----------------  --------  ------
 00000077  REL32             00000000         9  ??_U@YAPAXI@Z (void * __cdecl operator new[](unsigned int))
```

and for `main.obj`

```
PS D:\cpplecture\linker\game> DUMPBIN.EXE /RELOCATIONS main.obj
Microsoft (R) COFF/PE Dumper Version 14.29.30136.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file main.obj

File Type: COFF OBJECT

RELOCATIONS #4
                                                Symbol    Symbol
 Offset    Type              Applied To         Index     Name
 --------  ----------------  -----------------  --------  ------
 0000000B  REL32             00000000         C  ?create@@YA?AUengine_t@@XZ (struct engine_t __cdecl create(void))
 00000036  DIR32             00000000         9  $SG2851
 0000003F  REL32             00000000         D  ?load@@YAXAAUengine_t@@PBD@Z (void __cdecl load(struct engine_t &,char const *))
```

We have 3:

| Symbol                                                       |                                                       |
| ------------------------------------------------------------ | ----------------------------------------------------- |
| `?create@@YA?AUengine_t@@XZ (struct engine_t __cdecl create(void))` | `[[nodiscard]] engine_t create() noexcept`            |
| $SG2851                                                      | dunno                                                 |
| `?load@@YAXAAUengine_t@@PBD@Z (void __cdecl load(struct engine_t &,char const *))` | `void load(engine_t& engine, cstring level) noexcept` |

If we now look at what symbols we can find in `engine.lib`:

```
PS D:\cpplecture\linker\game> DUMPBIN.EXE /SYMBOLS engine.lib
Microsoft (R) COFF/PE Dumper Version 14.29.30136.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file engine.lib

File Type: LIBRARY

COFF SYMBOL TABLE
000 010575B8 ABS    notype       Static       | @comp.id
001 80010191 ABS    notype       Static       | @feat.00
002 00000002 ABS    notype       Static       | @vol.md
003 00000000 SECT1  notype       Static       | .drectve
    Section length   2F, #relocs    0, #linenums    0, checksum        0
005 00000000 SECT2  notype       Static       | .debug$S
    Section length   78, #relocs    0, #linenums    0, checksum        0
007 00000000 SECT3  notype       Static       | .text$mn
    Section length   A9, #relocs    1, #linenums    0, checksum  47C05A6
009 00000000 UNDEF  notype ()    External     | ??_U@YAPAXI@Z (void * __cdecl operator new[](unsigned int))
00A 00000000 SECT3  notype ()    External     | ?update@engine_t@@QAEXN@Z (public: void __thiscall engine_t::update(double))
00B 00000010 SECT3  notype ()    External     | ?spawn@engine_t@@QAE?AUentity_t@@XZ (public: struct entity_t __thiscall engine_t::spawn(void))
00C 00000060 SECT3  notype ()    External     | ?create@@YA?AUengine_t@@XZ (struct engine_t __cdecl create(void))
00D 00000000 SECT4  notype       Static       | .chks64
    Section length   20, #relocs    0, #linenums    0, checksum        0

String Table Size = 0x6B bytes

COFF SYMBOL TABLE
000 010575B8 ABS    notype       Static       | @comp.id
001 80010191 ABS    notype       Static       | @feat.00
002 00000002 ABS    notype       Static       | @vol.md
003 00000000 SECT1  notype       Static       | .drectve
    Section length   2F, #relocs    0, #linenums    0, checksum        0
005 00000000 SECT2  notype       Static       | .debug$S
    Section length   74, #relocs    0, #linenums    0, checksum        0
007 00000000 SECT3  notype       Static       | .text$mn
    Section length   67, #relocs    0, #linenums    0, checksum 789BEE7A
009 00000000 SECT3  notype ()    External     | ?parse@@YA?AUjson@@PBD@Z (struct json __cdecl parse(char const *))
00A 00000010 SECT3  notype ()    External     | ?get_object@@YA?AUjson_object@@ABUjson@@PBD@Z (struct json_object __cdecl get_object(struct json const &,char const *))
00B 00000020 SECT3  notype ()    External     | ?get_array@@YA?AUjson_array@@ABUjson@@PBD@Z (struct json_array __cdecl get_array(struct json const &,char const *))
00C 00000040 SECT3  notype ()    External     | ?get_string@@YA?AUstring_view@@ABUjson@@PBD@Z (struct string_view __cdecl get_string(struct json const &,char const *))
00D 00000060 SECT3  notype ()    External     | ?get_number@@YANABUjson@@PBD@Z (double __cdecl get_number(struct json const &,char const *))
00E 00000000 UNDEF  notype       External     | __fltused
00F 00000000 SECT4  notype       Static       | .rdata
    Section length    8, #relocs    0, #linenums    0, checksum        0, selection    2 (pick any)
011 00000000 SECT4  notype       External     | __real@0000000000000000
012 00000000 SECT5  notype       Static       | .chks64
    Section length   28, #relocs    0, #linenums    0, checksum        0

String Table Size = 0xE6 bytes
```

We get the list of all symbols in both object files: `engine.obj` and `json.obj`


## References:

- https://docs.microsoft.com/en-us/cpp/build/reference/overview-of-lib?view=msvc-170
- https://llvm.org/docs/CommandGuide/llvm-ar.html
- https://mottosso.gitbooks.io/clang/content/building_a_static_library.html
- https://gist.github.com/jsanchezuy/23b1fc8c592455f1bb84
- https://man7.org/linux/man-pages/man1/objdump.1.html
- https://www.computerhope.com/unix/uld.htm
- https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options?view=msvc-170
- https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170
- https://linux.die.net/man/1/ar
- https://en.cppreference.com/w/cpp/language/translation_phases
- https://en.wikipedia.org/wiki/COFF
- https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
- https://docs.microsoft.com/en-us/cpp/build/reference/dumpbin-options?view=msvc-170
- https://www.youtube.com/watch?v=a5L66zguFe4 (CppCon 2017: Michael Spencer “My Little Object File: How Linkers Implement C++”)
- https://www.geeksforgeeks.org/extern-c-in-c/
- https://www.ibm.com/docs/en/i/7.2?topic=linkage-name-mangling-c-only
- http://web.mit.edu/tibbetts/Public/inside-c/www/mangling.html
