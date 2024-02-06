# SIC/XE Assembler

This is a two-pass assembler for the SIC/XE (Simplified Instructional Computer with eXtensions) architecture written in C++. The assembler processes assembly language instructions, generates an intermediate file, and produces the final object code.

## Overview

The assembler is structured into two main passes:

1. **Pass 1:**
   - Reads the assembly language program from the input file ("input.txt").
   - Calculates the location counter (LOCCTR) and builds the symbol table (SYMTAB).
   - Detects errors such as duplicate symbols or invalid operation codes.
   - Writes the intermediate records to "intermediate.txt".

2. **Pass 2:**
   - Reads the intermediate file and generates the final object code.
   - Handles START, END, and other instructions to produce the object code.
   - Writes the final object code to "output.txt" and creates a listing file "listing.txt" with addresses and generated object code.
   - Detects errors such as undefined symbols.

## Usage

1. **Input:**
   - Ensure you have a valid input file named "input.txt" with SIC/XE assembly instructions.

2. **Compile and Run:**
   - Compile the C++ program:
     ```bash
     g++ assembler.cpp -o assembler
     ```
   - Run the assembler:
     ```bash
     ./assembler
     ```

3. **Output:**
   - The program generates the following files:
     - `intermediate.txt`: Intermediate file with addresses and assembly instructions.
     - `output.txt`: Final object code.
     - `listing.txt`: Listing file with addresses, assembly instructions, and generated object code.

## Sample Input

The provided "input.txt" file contains a sample SIC/XE assembly program demonstrating various instructions and addressing modes.

## Notes

- This assembler is designed for educational purposes and may not cover all possible cases.
- Review the generated output and listing files for any errors or warnings.
- The provided "input.txt" is just a sample; feel free to modify it for testing different SIC/XE assembly programs.

For any questions or issues, please contact [your instructor's name/email].

## Author

Vankar Divyesh Kumar

210101108

