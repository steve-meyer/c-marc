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
    while (status != EOF && record_count < 10)
    {
        record_count++;

        Record *record = MARC_get_record(record_raw);
        printf("LEADER %s\n", record->leader);

        ControlField *field = (ControlField *)HT_get(record->control_fields, "001");
        printf("%s %s\n", field->tag, field->value);

        HashTableIterator cf_iter = HT_iterator(record->control_fields);
        while (HT_next(&cf_iter))
        {
            ControlField *cf = (ControlField *)cf_iter.value;
            printf("%s %s\n", cf->tag, cf->value);
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
