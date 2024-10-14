# Huffman

_Huffman_ compression and decompression program.

## About

This project is a formal implementation of the Huffman algorithm. The project is written in C and is meant to be used as an executable.

> **Note:** This project is meant to be educational. It can be used in production. However, please make sure to read the code and understand what it does before using it in production.

## Features

Please refer to the documentation for a list of features. See [Usage](#documentation).

## Installation

## Usage

### Compiling

To compile the library, run the following command:

```bash
make
```

### Documentation

The documentation can be created using Doxygen. To create the documentation, run the following command:

```bash
make docs
```

Then, open `docs/html/index.html` in your browser.

### Tests

The tests can be compiled and run using the following command:

```bash
make tests
```

### Debugging

The library can be debugged (using Valgrind) using the following command:

```bash
make debug
```

> **Note:** If you need to use `debug` in a workflow, you can use `make debug/headless` instead.

### Cleaning

To clean the project, run the following command:

```bash
make clean
```

Alternatively, you can run the following commands:

```bash
make clean/objects clean/lib clean/exec clean/docs clean/debug # ...
```

### Installing

To install the dependencies, run the following command:

```bash
make install/debian
```

> **Note:** This will install the dependencies for Debian-based systems. If you are using a different system, please install the dependencies manually or update the script. You amy also open an issue or a pull request.

## License

This work is licensed under the MIT License. See `LICENSE` for more information.

## Contributors

- 2024: [Josselin Dulongcourty](https://github.com/josselinonduty)
