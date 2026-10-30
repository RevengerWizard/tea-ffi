# `ffi` API

This reference covers the `ffi` module.

- `ffi.cdef(declarations)`
Parse C declarations and register types, typedefs, and function prototypes. Declarations end with semicolons.

- `ffi.load(path, global = false)`
Load the shared library at `path` and return a `CLib`. When `global` is true, request global symbol visibility. Access declared functions as library attributes, such as `lib.printf`.

- `ffi.cnew(ctype [, initializer])`
Allocate a `CData` value of the given C type. An optional list initializes an array; an optional map initializes record fields. Without an initializer, storage is zero-initialized.

-`ffi.cast(ctype, value)`
Convert `value` to the given C type and return it as `CData`. For function-pointer types, a Teascript callable can be wrapped as a C callback.

- `ffi.typeof(value)`
Return the `CType` for a C type declaration, `CType`, or `CData`.

- `ffi.addressof(cdata)`
Return a pointer `CData` to the storage occupied by `cdata`.

- `ffi.gc(cdata [, finalizer])`
Set the finalizer called with `cdata` when it is collected. Omitting the finalizer clears the current one. Returns no value.

- `ffi.sizeof(ctype)`
Return the size in bytes of a C type.

- `ffi.offsetof(ctype, field)`
Return a record field's byte offset, or `nil` if the named field is not found. For non-record types, returns no value.

- `ffi.alignof(ctype)`
Return the C type's alignment in bytes.

- `ffi.istype(ctype, cdata)`
Return whether `cdata` has exactly the same interned `CType` as `ctype`.

- `ffi.tonumber(cdata)`
Convert scalar numeric C data to a Teascript number; returns `nil` for non-scalar types.

- `ffi.string(cdata [, length])`
Read a NUL-terminated string from a `char*`, `void*`, or char array. With `length`, read exactly that many bytes from a pointer, array, or record's storage.

- `ffi.copy(dst, src [, length])`
Copy bytes to a C pointer, array, or record. Without `length`, `src` must be a string: its bytes and a trailing NUL are copied. With `length`, `src` may be a string or `CData`. Returns the copied byte count.

- `ffi.fill(dst, length [, byte])`
Fill `length` bytes at a C pointer, array, or record with `byte` (default `0`).

- `ffi.errno([value])`
Return the current C `errno`; if `value` is supplied, set `errno` first. The returned value is the previous value.

- `ffi.abi(name)`
Return whether the target matches an ABI name: `"32bit"`, `"64bit"`, `"le"` (little-endian), or `"be"` (big-endian).

## C data (`CData`)

`CData` is returned by `cnew`, `cast`, `addressof`, and library function lookup. Scalar values convert to Teascript numbers or booleans as appropriate. C pointers can be compared with other C pointers and with `nil` (a null pointer compares equal to `nil`).

- Index pointer and array values with integer indices: `ptr[i]`, `array[i]`. Indices are zero-based and pointer access is unchecked.
- Access record fields with attributes: `record.x`; pointer-to-record values also support `ptr.x`. Assignments write through to C memory. Const-qualified data cannot be assigned to.
- Call function-pointer data directly. Fixed parameters are converted according to the declared prototype; variadic functions accept additional arguments with default C promotions. The call must supply all fixed arguments, and non-variadic calls require exactly the declared number.
- `tostring(cdata)` produces a type-and-address representation such as `cdata<int*>: 0x...`.

## C types (`CType`)

`CType` values describe C types and are accepted wherever a type declaration can be used. `tostring(ffi.typeof("int"))` produces `ctype<int>`. The parser supports scalar and pointer types, arrays, records (`struct` and `union`), typedefs, function declarations, and variadic function declarations, subject to the types supported by this module and libffi.

## C libraries (`CLib`)

`ffi.load` returns a `CLib`. Reading a declared function name from it looks up that symbol in the shared library and returns callable function-pointer `CData`; the result is cached on the library object. Looking up a name without a prior declaration made by `ffi.cdef` raises an error. The module also exposes the process C library as `ffi.C`.

## Notes

- The module includes common C scalar types and fixed-width integer types such as `int32_t`, `uint64_t`, and `size_t`.
- C declarations must match the platform ABI and the actual library signatures. Pointer indexing and memory operations do not check buffer bounds.
- A `CData` finalizer installed by `ffi.gc` is invoked during garbage collection. Use it to release externally allocated resources, for example a pointer returned by `malloc`.