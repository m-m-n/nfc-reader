#include "card_list.h"

#include <stdio.h>
#include <string.h>

// カードIDリストを読み込む
int load_card_ids(const char *filename, CardIdList *list)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "許可リストファイルを開けません: %s\n", filename);
        return 0;
    }

    list->count = 0;
    char line[MAX_ID_LENGTH];
    while (fgets(line, sizeof(line), fp) && list->count < MAX_CARD_IDS) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            strncpy(list->ids[list->count], line, MAX_ID_LENGTH - 1);
            list->ids[list->count][MAX_ID_LENGTH - 1] = '\0';
            list->count++;
        }
    }

    fclose(fp);
    return 1;
}

// カードIDが許可リストに含まれているかチェック
int is_card_id_allowed(const char *card_id, const CardIdList *list)
{
    for (int i = 0; i < list->count; i++) {
        if (strcmp(card_id, list->ids[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
