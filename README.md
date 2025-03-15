# C MARC

MARC parsing library in C.

## About

**Warning:** At the time of writing, this is an experimental learning project. Do not use this project for any production work.

### Basic Usage

Print MARC records to screen:

```bash
$ make clean
$ make
$ ./read_marc /path/to/marc/binary.mrc
```

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
