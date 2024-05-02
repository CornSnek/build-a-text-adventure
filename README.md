# build-a-text-adventure

**build-a-text-adventure** is a c++ text adventure project for https://www.codeacademy.com/. I tried what I have learned from C++ to make a text-adventure game.

The story is that you are an adventurer that tries to get the treasures of the Golden Wizard. He has placed many deadly traps in his tower to impede your progress, such as a Dragon, a Worm Monster, and Chemistry. Some items and prompts are randomly-generated, so it's not the same per restart.

This was an interesting project to do. I had little to no trouble in doing this project, because I have learned C++ some years ago. I am not a writer, but I have used chatgpt at https://chat.openai.com to create some prompts, like the Worm Basement Dungeon.

Note: Some things in the `main.cpp` file may not be covered in codeacademy, such as function pointers, unions, void* types, `static_cast`s, lambdas, enum classes, operator overloading, constructors, and the C++ `vector`/`fstream`/`random`/`chrono` libraries.

I have used MSYS2 under Windows to create this project (https://www.msys2.org). [https://www.codecademy.com/article/cpp-compile-execute-locally] was used to run with MSYS2 along with the makefile using `make all`.
However, you can just type `g++ main.cpp` to build it in a MSYS2 shell.