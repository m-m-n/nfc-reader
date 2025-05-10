#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "card_list.h"
#include "pcsc_utils.h"

// 定数定義
#define MAX_ATR_SIZE 33
#define MAX_READERS 16
#define MAX_BUFFER_SIZE 264
#define MAX_CARD_IDS 1000
#define MAX_ID_LENGTH 32
#define FELICA_ID_LENGTH 8
#define POLLING_INTERVAL 100000 // 100ミリ秒

// 使用方法を表示
static void print_usage(const char *program_name)
{
    fprintf(stderr, "使用方法: %s [オプション]\n", program_name);
    fprintf(stderr, "オプション:\n");
    fprintf(stderr, "  --allow-list-file=ファイル名  許可するカードIDのリストファイル\n");
    fprintf(stderr, "  --timeout=秒数               タイムアウト時間（秒）\n");
    fprintf(stderr, "  --help                       このヘルプを表示\n");
}

// カードの接続とID取得を試みる
static int try_connect_and_get_id(SCARDCONTEXT hContext, const char *reader_name,
                                  CardIdList *allowed_cards, int use_allow_list, int *return_value)
{
    SCARDHANDLE hCard = 0;
    unsigned long dwActiveProtocol;
    char card_id[MAX_ID_LENGTH] = {0};
    int id_obtained = 0;

    // カードの接続を試みる
    long result = SCardConnect(hContext, reader_name,
                               SCARD_SHARE_SHARED,
                               SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                               &hCard, &dwActiveProtocol);

    if (result == SCARD_S_SUCCESS) {
        // カードの状態を取得
        unsigned long atrLen = MAX_ATR_SIZE;
        unsigned char atr[MAX_ATR_SIZE];
        result = SCardStatus(hCard, NULL, NULL, NULL, NULL, atr, &atrLen);

        if (result == SCARD_S_SUCCESS) {
            // 各カードタイプでID取得を試みる（FeliCaを優先）
            if (!id_obtained) {
                id_obtained = get_felica_id(hCard, dwActiveProtocol, card_id);
            }
            if (!id_obtained) {
                id_obtained = get_iso14443_3a_id(hCard, dwActiveProtocol, card_id);
            }
            if (!id_obtained) {
                id_obtained = get_iso14443_id(hCard, dwActiveProtocol, card_id);
            }

            if (id_obtained) {
                // 許可リストのチェック
                if (use_allow_list) {
                    if (is_card_id_allowed(card_id, allowed_cards)) {
                        *return_value = 0;
                    } else {
                        *return_value = 1;
                    }
                } else {
                    printf("%s\n", card_id);
                    *return_value = 0;
                }
            } else {
                fprintf(stderr, "カードIDの取得に失敗しました\n");
            }
        }
    }

    // カードの切断
    if (hCard) {
        SCardDisconnect(hCard, SCARD_LEAVE_CARD);
    }

    return id_obtained;
}

int main(int argc, char *argv[])
{
    SCARDCONTEXT hContext = 0;
    char *readers = NULL;
    unsigned long cch = SCARD_AUTOALLOCATE;
    CardIdList allowed_cards = {0};
    int use_allow_list = 0;
    int timeout_seconds = 0;
    time_t start_time;
    int return_value = 1;

    // コマンドライン引数の処理
    static struct option long_options[] = {
      {"allow-list-file", required_argument, 0, 'a'},
      {"timeout", required_argument, 0, 't'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "a:t:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a':
                if (!load_card_ids(optarg, &allowed_cards)) {
                    return 1;
                }
                use_allow_list = 1;
                break;
            case 't':
                timeout_seconds = atoi(optarg);
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // PCSCコンテキストの初期化
    long result = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
    if (result != SCARD_S_SUCCESS) {
        print_error("コンテキストの初期化に失敗しました", result);
        return 1;
    }

    // 利用可能なリーダーの取得
    result = SCardListReaders(hContext, NULL, (char *)&readers, &cch);
    if (result != SCARD_S_SUCCESS) {
        print_error("リーダーの取得に失敗しました", result);
        cleanup(hContext, readers);
        return 1;
    }

    // リーダーが接続されているか確認
    if (readers == NULL) {
        fprintf(stderr, "NFCリーダーが見つかりません。\n");
        cleanup(hContext, readers);
        return 1;
    }

    start_time = time(NULL);

    while (1) {
        // タイムアウトチェック
        if (timeout_seconds > 0 && (time(NULL) - start_time) >= timeout_seconds) {
            fprintf(stderr, "タイムアウトしました\n");
            cleanup(hContext, readers);
            return 255;
        }

        // カードの接続とID取得を試みる
        if (try_connect_and_get_id(hContext, readers, &allowed_cards, use_allow_list, &return_value)) {
            break;
        }

        // 少し待機
        usleep(POLLING_INTERVAL);
    }

    // クリーンアップ
    cleanup(hContext, readers);

    return return_value;
}
