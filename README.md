# Assembler in C Language - Final Project

## Introduction

Hello and welcome Tester!

First and foremost, I extend my deepest gratitude for dedicating your time and effort to evaluate my final project. This README is intended to guide you through the process of using and testing my assembler project, developed in C.

## Project Overview

The core of this project is an assembler developed in C language. The aim has been to create a robust, efficient, and user-friendly tool for assembly language programming.

## Getting Started

### Prerequisites

Before you begin, ensure you have a working C compiler and the `make` utility installed on your system. These tools are necessary to compile and run the assembler.

### Compilation

I've invested considerable effort in crafting an effective `makefile` to streamline the compilation process. To compile the project, simply navigate to the project directory in your terminal and execute:

```bash
make
```

### Preparing Test Files

Before running the assembler, you'll need to add your test files. Here's how:

1. Navigate to the `assets` folder.
2. Inside the `assets` folder, locate the `as_files` directory.
3. Place your test files (assembly code files) in the `as_files` directory.

### Configuring the Makefile

Next, you'll need to specify which files the assembler should process:

1. Open the `makefile` in a text editor.
2. Locate the `args` variable.
3. Add the names of your test files (without their file extensions) to this variable.
   - An example has been provided in the makefile (`test1`, `test2`, etc.).

### Running the Assembler

After setting up your test files and updating the makefile, you can run the assembler using:

```bash
make run
```

## Feedback

Your feedback is invaluable in improving this project. Please don't hesitate to share your thoughts, suggestions, or report any issues you encounter. Thank you once again for your involvement and assistance.

---

Wishing you a smooth and enjoyable testing experience!
