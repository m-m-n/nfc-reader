# NFC Reader

NFCカードリーダーを使用してカードの読み取りを行うシンプルなコマンドラインツールです

## 必要条件

- Linux OS
- PCSC-Lite
- libpcsclite-dev
- NFCカードリーダー

## インストール方法

### 依存パッケージのインストール

```bash
sudo apt update
sudo apt install -y libpcsclite-dev
```

### ビルドとインストール

```bash
make
sudo make install
```

## 使用方法

```bash
nfc-reader [オプション]
```

### オプション

- `--allow-list-file=ファイル名`: 許可するカードIDのリストファイルを指定（指定時はIDの照合を行います）
- `--timeout=秒数`: タイムアウト時間を秒単位で指定
- `--help`: ヘルプを表示

### 動作モード

1. 引数なしで実行した場合：
   - NFCカードをスキャンし、カードIDを表示して終了します
   - カードIDの照合は行いません

2. `--allow-list-file`を指定した場合：
   - NFCカードをスキャンし、指定された許可リストと照合します
   - 許可リストに登録されているカードIDの場合のみ成功（0）を返します
   - 登録されていないカードIDの場合は失敗（1）を返します

### 例

```bash
# カードIDを表示するモード
nfc-reader

# 許可リストを使用して照合するモード
nfc-reader --allow-list-file=allowed-list.txt

# タイムアウトを設定する場合
nfc-reader --timeout=30
```

## 戻り値

- `0`: カードの読み取りに成功（許可リストが指定されている場合は、許可されたカードの場合）
- `1`: カードの読み取りに失敗、または許可されていないカード
- `255`: タイムアウト

## 許可リストファイル

`allowed-list.txt`は、アクセスを許可するNFCカードのIDを管理するファイルです  
このファイルは`/usr/local/etc/allowed-list.txt`にインストールされます

### ファイル形式

1行にユーザー名と1つのカードIDをTAB区切りで記述します  
カードIDは16進法で記述します

例：

```
USERNAME1[\t]0123456789ABCDEF
USERNAME2[\t]FEDCBA9876543210
```
