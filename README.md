# C MARC

MARC parsing library in C.

## About

**Warning:** At the time of writing, this is an experimental learning project. Do not use this project for any production work. There is no error handling for malformed MARC records.

### Basic Usage

Print MARC records to screen:

```bash
$ make clean
$ make
$ ./read_marc /path/to/marc/binary.mrc
```

* Read only, no interface to create and modify MARC data
* Works with binary MARC 21 format only

### Data Model

See the `marc.h` file for the data model:

A `Record` object is composed of

* Leader (string)
* Control fields (hash table)
* Data fields (hash table)

Hash tables are indexed by MARC field tag. Hash table values are linked lists because many MARC record fields are repeatable.

A `ControlField` has tag and value strings.

A `DataField` has:

* 2 indicators, `i1` and `i2` of type char
* A count of the number of subfields `sf_count` of type int
* An array of `Subfield`s

A `Subfield` has a code (char) and value (string).

### Interface

The public interface for MARC processing functions have a prefix `MARC_`. See `marc.h`:

* `MARC_get_next_raw`: given a string allocated 100k bytes (99,999 MARC record size limit + 1) and a file pointer, get the next record's bytes
* `MARC_record_create`: given the bytes for a single record, create the `Record` structure
* `MARC_free_record`: given a `Record` structure, free all allocated memory
* `MARC_get_field_tags`: get a sorted list of either `ControlField` tags or `DataField` tags by passing a `Record` structure's corresponding field hash table

The `collections.h` file defines the public interface for the hash table and linked list implementations used by this library.
