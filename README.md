# liblcu (lib lil-c-utils)

liblcu is a versatile C library designed to simplify the development of non-trivial C projects. It provides a collection of utilities and mechanisms, including:

- A flexible doubly linked-list implementation
- A POSIX thread pool
- Safe synchronization
- A multi-threaded logger

More features are coming.

## Building and Installing

To build the library, simply run:

`make`

To build the examples, run:

`make examples`

To build both the library and the examples, run:

`make all`

To clean the project, run:

`make clean`

### Installation

To install the library, use:

`sudo make install`

And to uninstall:

`sudo make uninstall`

## Using the Library

After building and installing the library with:
```
make
sudo make install
```

A static library will be created and copied to /usr/local/lib, while the header files will be placed in /usr/local/include/lcu.

## Documentation

You can generate documentation by running:

`make docs`

Alternatively, you can use Doxygen directly:

`doxygen Doxyfile`

## Examples

You can find examples in the examples/ directory. While there are currently only two files, they cover a substantial range of the library's functionalities, and more examples will be added in the future.

---

Feel free to explore the library and contribute! 
