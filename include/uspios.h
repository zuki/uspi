//
// uspios.h
//
// USPiライブラリが使用する外部関数
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014-2018  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _uspios_h
#define _uspios_h

#ifdef __cplusplus
extern "C" {
#endif

//
// システム構成
//
// (USPiライブラリをビルドする前に以下を変更する必要がある!)
//
#define GPU_L2_CACHE_ENABLED		// 通常は有効になっている（config.txtで無効にできる）

#define HZ	100			// タイマーticks / 秒 (タイマー割り込み周波数をセットする）

// デフォルトキーボードマップ（1つだけ有効にする）
//#define USPI_DEFAULT_KEYMAP_DE
//#define USPI_DEFAULT_KEYMAP_ES
//#define USPI_DEFAULT_KEYMAP_FR
//#define USPI_DEFAULT_KEYMAP_IT
#define USPI_DEFAULT_KEYMAP_UK
//#define USPI_DEFAULT_KEYMAP_US

// uspi/util.h の以下の関数の独自実装を使用したい場合は以下をundefineする
#define USPI_PROVIDE_MEM_FUNCTIONS	// mem*()
#define USPI_PROVIDE_STR_FUNCTIONS	// str*()

//
// メモリ割り当て
//
// (割り込みコンテキストから動作しなければならない）
//
void *malloc (unsigned nSize);		// 結果は4バイトアラインされていなければならない
void free (void *pBlock);

//
// タイマー
//
void MsDelay (unsigned nMilliSeconds);
void usDelay (unsigned nMicroSeconds);

#ifndef AARCH64
	typedef unsigned TKernelTimerHandle;
#else
	typedef unsigned long TKernelTimerHandle;
#endif

typedef void TKernelTimerHandler (TKernelTimerHandle hTimer, void *pParam, void *pContext);

// タイマーハンドル (hTimer) を返す
unsigned StartKernelTimer (unsigned	nHzDelay,   // Hz単位（上の"システム構成"を参照)
			   TKernelTimerHandler *pHandler,
			   void *pParam, void *pContext);	// タイマーハンドラに引き渡される

void CancelKernelTimer (unsigned hTimer);

//
// 割り込み処理
//
typedef void TInterruptHandler (void *pParam);

// USPiはUSB IRQ 9を使用
void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pHandler, void *pParam);

//
// プロパティタグ (ARM -> VC)
//
// See: https://github.com/raspberrypi/firmware/wiki/Mailboxes
//

// 失敗の場合は0を返す
int SetPowerStateOn (unsigned nDeviceId);	// "set power state" を "on" にする。完了まで待機

// 失敗の場合は0を返す
int GetMACAddress (unsigned char Buffer[6]);	// "get board MAC address"

//
// ロギング
//

// 深刻度 (異なる値が必要な場合はビルド前に変更すること）
#define LOG_ERROR	1
#define LOG_WARNING	2
#define LOG_NOTICE	3
#define LOG_DEBUG	4

void LogWrite (const char *pSource,		// モジュールのshort name
	       unsigned	   Severity,		// see above
	       const char *pMessage, ...);	// printfフォーマットオプションを使用）

//
// デバッグサポート
//
#ifndef NDEBUG

// "assertion failed"を表示して停止
void uspi_assertion_failed (const char *pExpr, const char *pFile, unsigned nLine);

// hexダンプを表示（pSourceは0でも可)
void DebugHexdump (const void *pBuffer, unsigned nBufLen, const char *pSource /* = 0 */);

#endif

#ifdef __cplusplus
}
#endif

#endif
