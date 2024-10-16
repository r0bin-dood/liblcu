# liblcu (lib lil-c-utils)

liblcu is a flexible C library designed to simplify the development of non-trivial C projects. It provides a collection of utilities and mechanisms, including:

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

### Linking the Library

To use `liblcu` in your C projects, you need to include the header files and link the static library during compilation. Here’s how you can do that with `gcc`:

1. **Include the Header Files**

   In your C source file, include the necessary header files from the `liblcu` library. For example:

   ```c
   #include <lcu/lcu_list.h>   
   #include <lcu/lcu_tpool.h>
   ```
2. **Compile and Link Your Program**

    Use the following gcc command to compile your program, replacing your_program.c with your source file name:

    `gcc -o my_app my_app.c -I/usr/local/include/lcu -L/usr/local/lib -llcu`


## Documentation

You can generate documentation by running:

`make docs`

Alternatively, you can use Doxygen directly:

`doxygen Doxyfile`

## Examples

You can find examples in the examples/ directory. While there are currently only two files, they cover a substantial range of the library's functionalities, and more examples will be added in the future.

---

Feel free to explore the library and contribute! 
