#include "marc.h"
#include "collections.h"


#define LEADER_LENGTH 24
#define DIRECTORY_ENTRY_LENGTH 12
#define TAG_LENGTH 3
#define FIELD_LENGTH 4
#define ADDRESS_LENGTH 5
#define SUBFIELD_INDICATOR 0x1F
#define END_OF_FIELD 0x1E
#define END_OF_RECORD 0x1D


typedef struct {
    char tag[TAG_LENGTH + 1];
    int data_length;
    int data_addr;
} DirectoryEntry;


// Non-public interface helpers not in marc.h
char* get_leader(char *record_raw);
int get_data_address(char *leader);
void get_directory(char *directory, int directory_length, char *record);
DirectoryEntry* parse_directory_entry(char *directory, int offset);
void append_field(Node *field_node, HashTable *fields, char *tag);
void get_control_fields(Record *record, size_t directory_length, char *directory, char *record_raw);
void get_data_fields(Record *record, size_t directory_length, char *directory, char *record_raw);
int get_subfield_count(size_t data_len, char *data);
ControlField* control_field_create(char *tag, char *data);
DataField *data_field_create(char *tag, size_t subfield_count, char *data);
Subfield *subfield_create(size_t token_length, char *token);

void marc_chomp(char *s);
int string_cmp(const void *a, const void *b);


int MARC_get_next_raw(char *raw_record, FILE *fp) {
    int i = 0;
    char c;

    // TODO: throw an error if the byte count exceeds the 99,999 set in the spec.
    while ((c = fgetc(fp)) != END_OF_RECORD)
    {
        if (c == EOF)
            return EOF;

        raw_record[i] = c;
        i++;
    }
    raw_record[i] = '\0';

    return i + 1;
}


Record* MARC_get_record(char *record_raw) {
    Record *record = malloc(sizeof(Record));
    record->leader = malloc(LEADER_LENGTH + 1);
    record->leader = get_leader(record_raw);

    int data_addr = get_data_address(record->leader);
    int directory_length = data_addr - 24;
    char directory[directory_length + 1];
    get_directory(directory, directory_length, record_raw);

    record->control_fields = HT_create();
    get_control_fields(record, directory_length, directory, record_raw);

    record->data_fields = HT_create();
    get_data_fields(record, directory_length, directory, record_raw);

    return record;
}


void MARC_free_record(Record *record) {
    // Each field list is (will be) a hash table of linked lists. First iterate over the hash
    // table entries and then and then traverse each node in each entry's list to free. Linked
    // list nodes will be either ControlField or DataField structs.

    if (record->control_fields != NULL) {
        HashTableIterator cf_itr = HT_iterator(record->control_fields);
        Node *tmp;
        while (HT_next(&cf_itr))
        {
            Node *cf_node = (Node *)cf_itr.value;
            tmp = cf_node;
            cf_node = cf_node->next;

            ControlField *node_data = tmp->data;
            free(node_data->tag);
            free(node_data->value);
            free(node_data);

            free(tmp);
        }
        HT_destroy(record->control_fields);
    }

    if (record->data_fields != NULL) {
        HashTableIterator df_itr = HT_iterator(record->data_fields);
        Node *tmp;
        while (HT_next(&df_itr))
        {
            Node *df_node = (Node *)df_itr.value;
            tmp = df_node;
            df_node = df_node->next;

            DataField *node_data_field = tmp->data;
            if (node_data_field->tag)
                free(node_data_field->tag);

            for (int i = 0; i < node_data_field->sf_count; i++) {
                if (node_data_field->subfields[i].value)
                    free(node_data_field->subfields[i].value);
            }
            free(node_data_field);

            free(tmp);
        }
        HT_destroy(record->data_fields);
    }

    if (record->leader)
        free(record->leader);

    free(record);
}


/**
 * Return a sorted list of either control or data field tags.
 */
void MARC_get_field_tags(char **tags, HashTable *fields, size_t count) {
    HashTableIterator cf_itr = HT_iterator(fields);
    int i = 0;
    while (HT_next(&cf_itr))
    {
        tags[i] = (char *)malloc((TAG_LENGTH + 1) * sizeof(char));
        strcpy(tags[i], cf_itr.key);
        tags[i][TAG_LENGTH] = '\0';
        i++;
    }
    qsort(tags, count, sizeof(tags[0]), string_cmp);
}


char* get_leader(char *record_raw) {
    char *leader = malloc(LEADER_LENGTH + 1);

    strncpy(leader, record_raw, LEADER_LENGTH + 1);
    leader[24] = '\0';

    return leader;
}


int get_data_address(char *leader) {
    char base_data_addr[6];
    strncpy(base_data_addr, leader+12, 6);
    base_data_addr[5] = '\0';

    return atoi(base_data_addr);
}


void get_directory(char *directory, int directory_length, char *record_raw) {
    strncpy(directory, record_raw + LEADER_LENGTH, directory_length);
    directory[directory_length] = '\0';
}


void get_control_fields(Record *record, size_t directory_length, char *directory, char *record_raw) {
    int field_count = (directory_length - 1) / 12;

    for (int i = 0, offset = 0; i < field_count; i++, offset += DIRECTORY_ENTRY_LENGTH) {
        DirectoryEntry *directory_entry = parse_directory_entry(directory, offset);
        if (atoi(directory_entry->tag) >= 10)
            return;

        char data[directory_entry->data_length];
        strncpy(data, record_raw + LEADER_LENGTH + directory_length + directory_entry->data_addr, directory_entry->data_length);

        ControlField *control_field = control_field_create(directory_entry->tag, data);
        Node *node = Node_create(control_field);
        append_field(node, record->control_fields, directory_entry->tag);

        free(directory_entry);
    }
}


/**
 * Append the current field to the supplied field list.
 *
 * Field should be a linked list node whose data is either a ControlField or DataField.
 * Hash table will be either the control or data fields hash tables.
 * Tag is the MARC field tag (e.g., 001 or 245).
 */
void append_field(Node *field_node, HashTable *fields, char *tag) {
    Node *tag_list = (Node *)HT_get(fields, tag);

    if (tag_list == NULL) {
        HT_set(fields, tag, field_node);
    } else {
        while (tag_list->next != NULL)
            tag_list = tag_list->next;

        tag_list->next = field_node;
    }
}


ControlField* control_field_create(char *tag, char *data) {
    ControlField *control_field = malloc(sizeof(ControlField));

    control_field->tag = malloc(4);
    control_field->value = malloc(strlen(data));
    strcpy(control_field->tag, tag);
    control_field->tag[3] = '\0';
    strcpy(control_field->value, data);
    marc_chomp(control_field->value);

    return control_field;
}


void get_data_fields(Record *record, size_t directory_length, char *directory, char *record_raw) {
    int field_count = (directory_length - 1) / 12;

    for (int i = 0, offset = 0; i < field_count; i++, offset += DIRECTORY_ENTRY_LENGTH) {
        DirectoryEntry *directory_entry = parse_directory_entry(directory, offset);
        if (atoi(directory_entry->tag) < 10)
            continue;

        char data[directory_entry->data_length + 1];
        strncpy(data, record_raw + LEADER_LENGTH + directory_length + directory_entry->data_addr, directory_entry->data_length);
        data[directory_entry->data_length] = '\0';

        int subfield_count = get_subfield_count(directory_entry->data_length, data);
        DataField *data_field = data_field_create(directory_entry->tag, subfield_count, data);
        Node *node = Node_create(data_field);
        append_field(node, record->data_fields, directory_entry->tag);

        free(directory_entry);
    }
}


DirectoryEntry* parse_directory_entry(char *directory, int offset) {
    char entry[DIRECTORY_ENTRY_LENGTH + 1];
    strncpy(entry, directory + offset, DIRECTORY_ENTRY_LENGTH);
    entry[DIRECTORY_ENTRY_LENGTH] = '\0';

    DirectoryEntry *directory_entry = malloc(sizeof(DirectoryEntry));

    strncpy(directory_entry->tag, entry, TAG_LENGTH + 1);
    directory_entry->tag[3] = '\0';

    char field_length_s[FIELD_LENGTH + 1];
    strncpy(field_length_s, entry + TAG_LENGTH, FIELD_LENGTH + 1);
    field_length_s[FIELD_LENGTH] = '\0';
    directory_entry->data_length = atoi(field_length_s);

    char field_data_addr_s[ADDRESS_LENGTH + 1];
    strncpy(field_data_addr_s, entry + TAG_LENGTH + FIELD_LENGTH, ADDRESS_LENGTH + 1);
    field_data_addr_s[ADDRESS_LENGTH] = '\0';
    directory_entry->data_addr = atoi(field_data_addr_s);

    return directory_entry;
}


DataField* data_field_create(char *tag, size_t subfield_count, char *data) {
    DataField *data_field = malloc(sizeof(DataField));
    data_field->tag = malloc(4);
    strcpy(data_field->tag, tag);
    data_field->tag[3] = '\0';

    data_field->sf_count = subfield_count;
    data_field->subfields = malloc(subfield_count * sizeof(Subfield));

    int token_index = 0;
    char *token = strtok(data, "\x1F");
    while (token)
    {
        // Get the string length without the MARC SUBFIELD_DELIMITER
        marc_chomp(token);

        if (token_index == 0) {
            data_field->i1 = token[0];
            data_field->i2 = token[1];
        } else {
            int token_length = strlen(token);
            data_field->subfields[token_index - 1] = *subfield_create(token_length, token);
        }

        token = strtok(NULL, "\x1F");
        token_index++;
    }

    return data_field;
}


Subfield* subfield_create(size_t token_length, char *token) {
    Subfield *sf = malloc(sizeof(Subfield));

    sf->code = token[0];
    sf->value = malloc(token_length);
    strncpy(sf->value, token + 1, token_length - 1);
    marc_chomp(sf->value);

    return sf;
}


int get_subfield_count(size_t data_len, char *data) {
    int subfield_count = 0, i;

    for (i = 0; i < data_len; i++)
        if (data[i] == SUBFIELD_INDICATOR)
            subfield_count++;

    return subfield_count;
}


/**
 * Remove MARC control characters from a string.
 */
void marc_chomp(char *s) {
    s[strcspn(s, "\x1D\x1E\x1F")] = '\0';
}


/**
 * Helper function for comparing strings (e.g., MARC field tags) for qsort.
 */
int string_cmp(const void *a, const void *b) {
    return strcmp( *(const char**)a, *(const char**)b );
}
