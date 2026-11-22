# My simple hash table library

## Description

This is a simple hash table library that I use in my personal projects.

## Instructions

You can then add the header in the necessary files, and add `#define JPR_LIBHASHTABLE_IMPLEMENTATION` in **only one** of them to get access to the function definitions.

This lib uses some functions from [libmem](https://github.com/julien-PR/JPR/tree/main/libmem), [libstr](https://github.com/julien-PR/JPR/tree/main/libstr) and [libllist](https://github.com/julien-PR/JPR/tree/main/libllist). So their implementation definition is required if it hasn't been already added.

It should look something like:

```c
#define JPR_LIBMEM_IMPLEMENTATION
#define JPR_LIBSTR_IMPLEMENTATION
#define JPR_LIBLLIST_IMPLEMENTATION
#define JPR_LIBHASHTABLE_IMPLEMENTATION
#include "libhashtable.h"
```
