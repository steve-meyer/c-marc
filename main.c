#include "main.h"

void validate_params(int argc, char *argv[]);


int main(int argc, char *argv[]) {
    validate_params(argc, argv);

    FILE *fp;
    fp = fopen(argv[1], "r");

    int status;
    char *record_raw;
    record_raw = malloc(100000);
    status = MARC_get_next_raw(record_raw, fp);

    int record_count = 0;
    while (status != EOF && record_count < 100)
    {
        record_count++;

        Record *record = MARC_get_record(record_raw);
        printf("LEADER %s\n", record->leader);

        HashTableIterator cf_iter = HT_iterator(record->control_fields);
        while (HT_next(&cf_iter))
        {
            Node *control_field = (Node *)cf_iter.value;
            while (control_field != NULL)
            {
                ControlField *field = (ControlField *)control_field->data;
                printf("%s %s\n", field->tag, field->value);
                control_field = control_field->next;
            }
        }

        HashTableIterator df_iter = HT_iterator(record->data_fields);
        while (HT_next(&df_iter))
        {
            Node *data_field = (Node *)df_iter.value;
            while (data_field != NULL)
            {
                DataField *field = (DataField *)data_field->data;
                printf("%s %c%c", field->tag, field->i1, field->i2);
                for (int i = 0; i < field->sf_count; i++)
                    printf(" $%c %s", field->subfields[i].code, field->subfields[i].value);
                puts("");
                data_field = data_field->next;
            }

        }

        puts("");

        MARC_free_record(record);
        status = MARC_get_next_raw(record_raw, fp);
    }

    fclose(fp);
    return EXIT_SUCCESS;
}


void validate_params(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No filepath passed\n");
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Unable to open filepath: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    fclose(fp);
}
