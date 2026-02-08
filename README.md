# Ceylonicus

[NOTE THIS UNDER RAPID DEVELOPMENT, HENCE CHANGES ARE VERY UNSTABLE]

It's the first sinhala programming language, and was written in python for prototyping, this is the C implementation of the language. It was originally interpreted and use the python interpreter for the execution, which you can see in [https://github.com/RezSat/Ceylonicus](https://github.com/RezSat/Ceylonicus)

Key changes in this implementation is that, the language is no longer interpreted instead it is fully compiled (choosen because I want to learn a few bit about compilers and LLVM)

So basically this is C/C++ Frontend -> LLVM Backend

Implement lexer + parser + AST in C/C++.

Generate LLVM IR using LLVM C++ APIs (or C API, but C++ is the “main” path).

List of things to do (for my note):
[] Write the main file
[] Test the lexer
[] parser
[] -- I have to figure out what next haha....