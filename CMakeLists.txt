cmake_minimum_required(VERSION 3.20)
project(gai4 C)

set(CMAKE_C_STANDARD 11)

add_executable(gai4 main.c semantic.c semantic.h symbol.h symbol.c syntax.tab.h syntax.tab.c log.h log.c lex.yy.c tree.h tree.c hashTable.h hashTable.c)
