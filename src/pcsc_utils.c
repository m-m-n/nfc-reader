#include "pcsc_utils.h"

#include <stdio.h>
#include <string.h>

// エラー処理用の関数
void print_error(const char *message, long error_code)
{
    fprintf(stderr, "%s: %s\n", message, pcsc_stringify_error(error_code));
}

// リソースのクリーンアップ
void cleanup(SCARDCONTEXT hContext, char *readers)
{
    if (readers) {
        SCardFreeMemory(hContext, readers);
    }
    if (hContext) {
        SCardReleaseContext(hContext);
    }
}

// FeliCaカードのIDを取得
int get_felica_id(SCARDHANDLE hCard, unsigned long dwActiveProtocol, char *card_id)
{
    unsigned char recvBuffer[MAX_BUFFER_SIZE];
    unsigned long recvLength = sizeof(recvBuffer);

    // FeliCaのPollingコマンド（システムコード: 0x0003）
    unsigned char polling[] = {
      0xFF, 0x00, 0x00, 0x00, 0x08, 0xD4, 0x60, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};

    long result = SCardTransmit(hCard,
                                dwActiveProtocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1,
                                polling, sizeof(polling),
                                NULL, recvBuffer, &recvLength);

    if (result == SCARD_S_SUCCESS && recvLength >= 18) {
        if (recvBuffer[recvLength - 2] == 0x90 && recvBuffer[recvLength - 1] == 0x00) {
            // IDmは応答の9-16バイト目にあります
            for (unsigned long i = 0; i < FELICA_ID_LENGTH; i++) {
                sprintf(card_id + (i * 2), "%02X", recvBuffer[9 + i]);
            }
            return 1;
        }
    }
    return 0;
}

// ISO/IEC 14443-4カードのIDを取得
int get_iso14443_id(SCARDHANDLE hCard, unsigned long dwActiveProtocol, char *card_id)
{
    unsigned char recvBuffer[MAX_BUFFER_SIZE];
    unsigned long recvLength = sizeof(recvBuffer);

    // ISO/IEC 14443-4のUID取得コマンド
    unsigned char get_uid[] = {
      0xFF, 0xCA, 0x00, 0x00, 0x00};

    long result = SCardTransmit(hCard,
                                dwActiveProtocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1,
                                get_uid, sizeof(get_uid),
                                NULL, recvBuffer, &recvLength);

    if (result == SCARD_S_SUCCESS && recvLength >= 2) {
        if (recvBuffer[recvLength - 2] == 0x90 && recvBuffer[recvLength - 1] == 0x00) {
            // UIDを取得
            for (unsigned long i = 0; i < recvLength - 2; i++) {
                sprintf(card_id + (i * 2), "%02X", recvBuffer[i]);
            }
            return 1;
        }
    }
    return 0;
}

// ISO/IEC 14443-3AカードのIDを取得
int get_iso14443_3a_id(SCARDHANDLE hCard, unsigned long dwActiveProtocol, char *card_id)
{
    unsigned char recvBuffer[MAX_BUFFER_SIZE];
    unsigned long recvLength = sizeof(recvBuffer);

    // ISO/IEC 14443-3AのUID取得コマンド
    unsigned char get_uid[] = {
      0xFF, 0xCA, 0x01, 0x00, 0x00};

    long result = SCardTransmit(hCard,
                                dwActiveProtocol == SCARD_PROTOCOL_T0 ? SCARD_PCI_T0 : SCARD_PCI_T1,
                                get_uid, sizeof(get_uid),
                                NULL, recvBuffer, &recvLength);

    if (result == SCARD_S_SUCCESS && recvLength >= 2) {
        if (recvBuffer[recvLength - 2] == 0x90 && recvBuffer[recvLength - 1] == 0x00) {
            // UIDを取得（応答の最初のバイトは長さ）
            unsigned char uid_length = recvBuffer[0];
            if (uid_length > 0 && uid_length <= 10) { // UIDの最大長は10バイト
                for (unsigned long i = 0; i < uid_length; i++) {
                    sprintf(card_id + (i * 2), "%02X", recvBuffer[i + 1]);
                }
                return 1;
            }
        }
    }
    return 0;
}
