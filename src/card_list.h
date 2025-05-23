#ifndef CARD_LIST_H
#define CARD_LIST_H

#include <crypt.h>

#define MAX_CARD_IDS 1000
#define MAX_ID_HASH_LENGTH 256
#define MAX_USERNAME_LENGTH 32

// カードエントリの構造体
typedef struct {
    char username[MAX_USERNAME_LENGTH];
    char card_id_hash[MAX_ID_HASH_LENGTH];
} CardEntry;

// カードリストの構造体
typedef struct {
    CardEntry entries[MAX_CARD_IDS];
    int count;
} CardList;

// カードリストを読み込む
int load_card_list(const char *filename, CardList *list);

// カードIDが許可リストに含まれているかチェックし、対応するユーザー名を返す
int is_card_id_allowed(const char *card_id, const CardList *list, char *username);

// mkpasswd形式のハッシュを検証する
int verify_mkpasswd_hash(const char *card_id, const char *stored_hash);

#endif // CARD_LIST_H
