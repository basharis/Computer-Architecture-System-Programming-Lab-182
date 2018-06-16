# Computer Architecture System & Programming Lab 182 (CASPL)

## Assignments

### Assignment 1: Post-fix Calculator for Bignums in C and x86/64-bit Assembly
In this assignment we have written a postfix-based calculator for integer arithmetic, somewhat similar to the dc program on Unix/Linux.

### Assignment 3: SIC (Single Instruction Computer) VM in x86/64-bit Assembly and SIC
We have implemented a VM for running SIC in x86 ASM, that runs a VM for SIC in SIC. The goal was to practice writing self-modifying code in a safe environment.

## Labs

### Lab 1: Introduction to C programming
We have created a small word count program with several argument options similar to wc in Linux.

*Lab goals: C primer, on simple stream IO library functions and parsing command-line arguments.*

### Lab 2: Program Memory and Pointers, Debugging and Simulating Object Oriented Programming
We have implemented our own version of printf: toy_printf.

*Lab goals: C primer, pointers to data, structures, and pointers to functions.*

### Lab 3: Continuation of Lab 2
In this lab we have practiced function pointers with our implementation of toy_printf

*Lab goals: C primer, pointers to data, structures, and pointers to functions.*

### Lab 4: System Calls
For this lab, we were not allowed to use any C standard libraries, and created a program patcher using direct system calls.

*Lab goals: getting acquainted with the low-level interface to system calls, understand how programs can work without the use of standard library and learning about the structure of a directory, and how to traverse it.*

### Lab 5: Implementation of a Linux Terminal
We created a simple shell and implemented input/output redirection and pipelines. Your shell is able to execute non-trivial commands such as "tail -n 2 in.txt| cat > out.txt".

*Lab goals: learning how to use fork, pipe, execute.*

### Lab 6: Job control
We extended our terminal implementation to support multi-pipelines and added job control functionality.

*Lab goals: learning how to use signals.*

### Lab 7: ELF Introduction
We implemented a version of hexedit, and used it to patch an ELF-64 file.

*Lab goals: Extracting useful information from ELF files (under 64-bit architecture), and fixing files using this information - reverse engineering.*

### Lab 8: ELF-Introduction with Code
We have parsed an ELF file and extract useful information from it. In particular, we accessed the data in the section header table, and in the symbol table. We used the mmap system call.

*Lab goals: Learning how to use mmap to create our own version of readelf.*

### Lab 9: TCP Client / Server
We have implemented a small FTP client and server. The system supports connecting, listing directories and downloading files.

*Lab goals: Learning the basics of network programming in C*

---
## Programming Languages

* C (Built with makefile)
* x86/64 Assembly

## Author

Shahar Bashari

---
## Disclaimer
The assignment and lab specifications do not belong to me, but to Ben Gurion University of the Negev.
