#include "card_list.h"

#include <crypt.h>
#include <stdio.h>
#include <string.h>

// カードリストを読み込む
int load_card_list(const char *filename, CardList *list)
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "許可リストファイルを開けません: %s\n", filename);
        return 0;
    }

    list->count = 0;
    char line[MAX_ID_HASH_LENGTH + MAX_USERNAME_LENGTH + 2]; // +2 for tab and null terminator
    while (fgets(line, sizeof(line), fp) && list->count < MAX_CARD_IDS) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            char *tab = strchr(line, '\t');
            if (tab) {
                *tab = '\0'; // タブ文字を終端文字に置換
                strncpy(list->entries[list->count].username, line, MAX_USERNAME_LENGTH - 1);
                list->entries[list->count].username[MAX_USERNAME_LENGTH - 1] = '\0';
                strncpy(list->entries[list->count].card_id_hash, tab + 1, MAX_ID_HASH_LENGTH - 1);
                list->entries[list->count].card_id_hash[MAX_ID_HASH_LENGTH - 1] = '\0';
                list->count++;
            }
        }
    }

    fclose(fp);
    return 1;
}

// mkpasswd形式のハッシュを検証する
int verify_mkpasswd_hash(const char *card_id, const char *stored_hash)
{
    char *result = crypt(card_id, stored_hash);
    if (result == NULL) {
        return 0;
    }
    return strcmp(result, stored_hash) == 0;
}

// カードIDが許可リストに含まれているかチェックし、対応するユーザー名を返す
int is_card_id_allowed(const char *card_id, const CardList *list, char *username)
{
    for (int i = 0; i < list->count; i++) {
        if (verify_mkpasswd_hash(card_id, list->entries[i].card_id_hash)) {
            if (username) {
                strncpy(username, list->entries[i].username, MAX_USERNAME_LENGTH - 1);
                username[MAX_USERNAME_LENGTH - 1] = '\0';
            }
            return 1;
        }
    }
    return 0;
}
