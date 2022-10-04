#!/bin/bash
EXE_SUFFIX=
case "$(uname -s)" in
	Darwin*)	EXE_SUFFIX=.app;;
	CYGWIN*)	EXE_SUFFIX=.exe;;
	MINGW*)		EXE_SUFFIX=.exe;;
	MSYS*)		EXE_SUFFIX=.exe;;
esac
mkdir -p bin
shopt -s globstar
gcc -o bin/brainfuck$EXE_SUFFIX -Isrc src/**/**/**/**/**.c