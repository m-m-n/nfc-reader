#ifndef PCSC_UTILS_H
#define PCSC_UTILS_H

#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>

#define MAX_ATR_SIZE 33
#define MAX_BUFFER_SIZE 264
#define FELICA_ID_LENGTH 8

// エラー処理用の関数
void print_error(const char *message, long error_code);

// リソースのクリーンアップ
void cleanup(SCARDCONTEXT hContext, char *readers);

// FeliCaカードのIDを取得
int get_felica_id(SCARDHANDLE hCard, unsigned long dwActiveProtocol, char *card_id);

// ISO/IEC 14443-4カードのIDを取得
int get_iso14443_id(SCARDHANDLE hCard, unsigned long dwActiveProtocol, char *card_id);

// ISO/IEC 14443-3AカードのIDを取得
int get_iso14443_3a_id(SCARDHANDLE hCard, unsigned long dwActiveProtocol, char *card_id);

#endif // PCSC_UTILS_H
