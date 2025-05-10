#ifndef CARD_LIST_H
#define CARD_LIST_H

#define MAX_CARD_IDS 1000
#define MAX_ID_LENGTH 32

// カードIDリストの構造体
typedef struct {
    char ids[MAX_CARD_IDS][MAX_ID_LENGTH];
    int count;
} CardIdList;

// カードIDリストを読み込む
int load_card_ids(const char *filename, CardIdList *list);

// カードIDが許可リストに含まれているかチェック
int is_card_id_allowed(const char *card_id, const CardIdList *list);

#endif // CARD_LIST_H
