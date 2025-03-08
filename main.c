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

    while (status != EOF)
    {
        Record *record = MARC_get_record(record_raw);
        printf("LEADER %s\n", record->leader);

        for (int i = 0; i < record->cf_count; i++)
            printf("%s %s\n", record->control_fields[i].tag, record->control_fields[i].value);

        for (int i = 0; i < record->df_count; i++) {
            printf("%s %c%c", record->data_fields[i].tag, record->data_fields[i].i1, record->data_fields[i].i2);
            for (int j = 0; j < record->data_fields[i].sf_count; j++)
                printf(" $%c %s", record->data_fields[i].subfields[j].code, record->data_fields[i].subfields[j].value);
            puts("");
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
