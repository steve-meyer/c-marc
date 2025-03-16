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
        Record *record = MARC_record_create(record_raw);
        printf("LEADER %s\n", record->leader);

        char **cf_tags = (char **)malloc(record->control_fields->length * sizeof(char *));
        MARC_get_field_tags(cf_tags, record->control_fields, (size_t)record->control_fields->length);
        for (int i = 0; i < record->control_fields->length; i++) {
            Node *cf_node = (Node *)HT_get(record->control_fields, cf_tags[i]);
            while (cf_node != NULL)
            {
                ControlField *field = (ControlField *)cf_node->data;
                printf("%s %s\n", field->tag, field->value);
                cf_node = cf_node->next;
            }
        }

        char **df_tags = (char **)malloc(record->data_fields->length * sizeof(char *));
        MARC_get_field_tags(df_tags, record->data_fields, (size_t)record->data_fields->length);
        for (int i = 0; i < record->data_fields->length; i++) {
            Node *df_node = (Node *)HT_get(record->data_fields, df_tags[i]);
            while (df_node != NULL)
            {
                DataField *field = (DataField *)df_node->data;
                printf("%s %c%c", field->tag, field->i1, field->i2);
                for (int i = 0; i < field->sf_count; i++)
                    printf(" $%c %s", field->subfields[i].code, field->subfields[i].value);
                puts("");
                df_node = df_node->next;
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
