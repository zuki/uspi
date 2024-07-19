//
// sysconfig.h
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
#ifndef _uspienv_sysconfig_h
#define _uspienv_sysconfig_h

// memory addresses and sizes
#define MEGABYTE		0x100000

#define MEM_SIZE		(256 * MEGABYTE)		// デフォルトメモリサイズ
#define GPU_MEM_SIZE		(64 * MEGABYTE)			// config.txtで定義した値
#define ARM_MEM_SIZE		(MEM_SIZE - GPU_MEM_SIZE)	// 通常は上書きされる

#define PAGE_SIZE		4096				// ページサイズ ( 4KB )

#define KERNEL_MAX_SIZE		(2 * MEGABYTE)			// サイズは16Kの倍数であること
#define KERNEL_STACK_SIZE	0x20000				// 128 KB
#define EXCEPTION_STACK_SIZE	0x8000				// 32 KB
#define PAGE_TABLE1_SIZE	0x4000				// 16 KB

#define MEM_KERNEL_START	0x8000
#define MEM_KERNEL_END		(MEM_KERNEL_START + KERNEL_MAX_SIZE)
#define MEM_KERNEL_STACK	(MEM_KERNEL_END + KERNEL_STACK_SIZE)		// expands down
#define MEM_ABORT_STACK		(MEM_KERNEL_STACK + EXCEPTION_STACK_SIZE)	// expands down
#define MEM_IRQ_STACK		(MEM_ABORT_STACK + EXCEPTION_STACK_SIZE)	// expands down
#define MEM_PAGE_TABLE1		MEM_IRQ_STACK				// 16Kアラインであること

// Raspberry Pi 3 では適切なメールボック用にコヒーレントな
// メモリ (1 section)が必要
#if RASPPI == 3
#define MEM_COHERENT_REGION	0x400000
#define MEM_HEAP_START		0x500000
#else
#define MEM_HEAP_START		0x400000
#endif

// システムオプション
#if RASPPI == 1			// Raspberry Pi 1 のみ
//#define ARM_DISABLE_MMU
//#define ARM_STRICT_ALIGNMENT
#define GPU_L2_CACHE_ENABLED
#else
#define CORES		4	// 2の倍数であること
#endif

#define KERNEL_TIMERS	20

//	  0x8000 |---------------------------------| 
//               | カーネル (2MB)                  |
//      0x208000 |---------------------------------| 
//               | カーネルスタック (128KB)        |
//      0x228000 |---------------------------------|
//               | 例外ハンドラスタック (32KB)     |
//      0x230000 |---------------------------------|
//               | 割り込みハンドラスタック (32KB) |
//      0x238000 |---------------------------------|
//               | メモリページテーブル (16KB)     |
//      0x23C000 |---------------------------------|
//               | 未使用                          |
//      0x400000 |---------------------------------|
//               | メールボックス用 (1MB)          |
//      0x500000 |---------------------------------|
//               | ヒープメモリ ()                 |

#endif
