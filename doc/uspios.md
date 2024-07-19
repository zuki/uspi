# USPiライブラリが使用する外部関数

USPiライブラリを使用するには以下のシステム構成と関数の実装が必要である。

## システム構成

USPiライブラリのビルドには以下の定義を変更する必要がある。

```c
// 通常は有効になっている（config.txtで無効にできる）
#define GPU_L2_CACHE_ENABLED

// タイマーticks / 秒 (タイマー割り込み周波数をセットする）
#define HZ    100

// デフォルトキーボードマップ（1つだけ有効にする）
//#define USPI_DEFAULT_KEYMAP_DE
//#define USPI_DEFAULT_KEYMAP_ES
//#define USPI_DEFAULT_KEYMAP_FR
//#define USPI_DEFAULT_KEYMAP_IT
#define USPI_DEFAULT_KEYMAP_UK
//#define USPI_DEFAULT_KEYMAP_US

// uspi/util.h で定義されている以下の関数を独自実装したい場合は以下をundefineする
#define USPI_PROVIDE_MEM_FUNCTIONS    // mem*()
#define USPI_PROVIDE_STR_FUNCTIONS    // str*()
```

## 実装が必要な関数

USPiの実行には以下の関数の実装が必要である。

### メモリ割り当て

割り込みコンテキストから動作しなければならない

```c
// メモリは4バイトアラインされていなければならない
void *malloc (unsigned nSize);
void free (void *pBlock);
```

### タイマー

```c
void MsDelay (unsigned nMilliSeconds);
void usDelay (unsigned nMicroSeconds);

#ifndef AARCH64
    typedef unsigned TKernelTimerHandle;
#else
    typedef unsigned long TKernelTimerHandle;
#endif

typedef void TKernelTimerHandler (TKernelTimerHandle hTimer, void *pParam, void *pContext);

// return: タイマーハンドル (hTimer) を返す
unsigned StartKernelTimer (
        unsigned    nHzDelay,               // Hz単位（上の"システム構成"を参照)
        TKernelTimerHandler *pHandler,
        void *pParam, void *pContext);     	// タイマーハンドラに引き渡される

void CancelKernelTimer (unsigned hTimer);
```

### 割り込み処理

```c
typedef void TInterruptHandler (void *pParam);

// USPiはUSB IRQ 9を使用
void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pHandler, void *pParam);
```

### プロパティタグ (ARM -> VC)

[メールボックス](file:///Users/dspace/xv6_memos/book/raspi/firmware_wiki/index.html)参照

```c
// 失敗の場合は0を返す
int SetPowerStateOn (unsigned nDeviceId);       // "set power state" を "on" にする。完了まで待機

// 失敗の場合は0を返す
int GetMACAddress (unsigned char Buffer[6]);    // "get board MAC address"
```

### ロギング

```c
// 深刻度 (異なる値が必要な場合はビルド前に変更すること）
#define LOG_ERROR       1
#define LOG_WARNING     2
#define LOG_NOTICE      3
#define LOG_DEBUG       4

void LogWrite (const char *pSource,        // モジュールのshort name
           unsigned       Severity,        // see above
           const char *pMessage, ...);    // printfフォーマットオプションを使用）
```

### デバッグサポート

```c
#ifndef NDEBUG

// "assertion failed"を表示して停止
void uspi_assertion_failed (const char *pExpr, const char *pFile, unsigned nLine);

// hexダンプを表示（pSourceは0でも可)
void DebugHexdump (const void *pBuffer, unsigned nBufLen, const char *pSource /* = 0 */);
```