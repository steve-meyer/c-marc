#ifndef __MARC_H__
#define __MARC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "collections.h"


#define LEADER_LENGTH 24
#define DIRECTORY_ENTRY_LENGTH 12
#define TAG_LENGTH 3
#define SUBFIELD_INDICATOR 0x1F
#define END_OF_FIELD 0x1E
#define END_OF_RECORD 0x1D


typedef struct {
  char code;
  char *value;
} Subfield;


typedef struct {
  char *tag;
  char i1;
  char i2;
  int sf_count;
  Subfield *subfields;
} DataField;


typedef struct {
  char *tag;
  char *value;
} ControlField;


typedef struct {
  char *leader;
  HashTable *control_fields;
  HashTable *data_fields;
} Record;


int MARC_get_next_raw(char *raw_record, FILE *fp);
Record* MARC_get_record(char *record_raw);
void MARC_free_record(Record *record);
void MARC_get_field_tags(char **tags, HashTable *fields, size_t count);

int string_cmp(const void *a, const void *b);

#endif
