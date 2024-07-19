USPi
====

> Raspberry Pi is a trademark of the Raspberry Pi Foundation.

> If you read this file in an editor you should switch line wrapping on.

概要
--------

USPiは、Cで書かれたRaspberry Pi用のベアメタルUSBドライバです。Circle USB libraryを
移植したものです。C言語を使用したことで、Raspberry PiのベアメタルCコードから使用
することができます。Circle USBライブラリと同様にコントロール（同期）、バルク、
インタラプト（同期、非同期）転送をサポートしています。USBキーボード、マウス、MIDI楽器、
ゲームパッド、大容量記憶装置（USBメモリなど）とオンボードEthernetコントローラ用の
関数ドライバが利用できます。USPiは既存のすべてのRaspberry Piモデルで動作するはずです。

USPiには環境ライブラリ（*env/*サブディレクトリ）が付属しており、USPiを動作させる
ために必要なすべての関数が提供されています。さらに、環境ライブラリを使用するUSPiの
使用方法を示すサンプルプログラム（*sample/*サブディレクトリ）も用意されています。
これらは自分でアプリケーションと環境を用意すれば必要ありません。

このUSBライブラリは趣味のプロジェクトで開発されたものであることに注意してください。
ダイナミックアタッチメントがない、エラーリカバリーがない、スプリットサポートが
限定的であるなどの既知の問題があります。私の環境では問題なく動作していますが、
必ずしもすべてのデバイス、すべての状況でうまくいくとは限りません。また、開発は
パフォーマンスではなく機能に焦点を当てています。

USPiは、C++で書かれたCircle USB libraryを「機械的に」移植したものです。そのため
ソースコードが少し変に見えるかもしれません。でも、そのほうが早かったんです。

ディレクトリ
----------------

* include: USPiヘッダーファイル
* lib: USPiソースコード
* sample: サンプルプログラム
* env: サンプルプログラムで使用される環境（USPiライブラリ自体には不要）
* doc: ドキュメントファイル

インタフェース
---------------

USPiライブラリはベアメタル環境からUSBデバイスにアクセスするための関数を提供します。
8つの関数グループがあり、*include/uspi.h*で宣言されています。

* USPiの初期化
* キーボード
* マウス
* ゲームパッド/ジョイスティック
* USB大容量格納装置
* Ethernetコントローラー
* MIDI (入力のみ)
* USBデバイス情報

ベアメタル環境では*include/uspios.h*で宣言されているいくつかの関数をUSPiライブラリに
提供する必要があります。この関数は6つのグループに分かれています。

**訳注**: `env/lib/uspibind.c`で実装されている。envが使用できないaarch64ではこれらの
関数を独自に実装する必要がある。

* メモリアロケーション
* タイマー
* 割り込み処理
* プロパティタグ（MBOX処理）
* ログ出力
* デバッグサポート

構成
-------------

USPiライブラリをビルドする前に *include/uspios.h* ファイル（のトップセクション）で
システム構成に合うように設定する必要があります。

もう一つのオプション(NDEBUG)はRules.mkで定義して、リリースバージョンをビルドすることが
できます。テスト段階では多くのチェックを含むデバッグバージョンを使用する方がよいでしょう。

ビルド
--------

> AArch64のサポートについてはこのファイルの最後を参照してください.

ビルドは通常PC Linux上で行います。Raspberry Pi 1用にビルドする場合は、ARM1176JZFコア用の[ツールチェーン](http://elinux.org/Rpi_Software#ARM)が必要です。Raspberry Pi 2/3の場合は、Cortex-A7/-A53をサポートするツールチェーンが必要です。これら全てに対応するツールチェーンは[ここ](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)からダウンロードできます。USPiはこのウェブサイトのバージョン*7-2018-q2-update* でテストされています。Raspbian上のRaspberry Pi自体でもUSPiをビルドすることができます。

まず *Rules.mk* ファイルを編集し、Raspberry Pi のバージョン (*RASPPI*、1、2、3) とツールチェーンコマンドの *PREFIX* を設定します。あるいは、*Config.mk*ファイル（gitでは無視されます）を作成し、Raspberry Piのバージョンと*PREFIX*変数にコンパイラの接頭辞を次のように設定します（最後のダッシュを忘れないでください）。

	RASPPI = 1
	PREFIX = arm-none-eabi-

次に、USPiのlib/ディレクトリで以下を実行してください。

	make clean
	make

*libuspi.a* ファイルがビルドされて lib/ ディレクトリにあるはずです。

使用法
-------

Makefile のincludeパスに USPi include/ ディレクトリを追加し、 *libuspi.a* ライブラリを *ld* に
指定してください。USPi関数にアクセスしたいところでファイル*uspi.h*をインクルードしてください。

サンプル
--------

*sample/* サブディレクトリにあるサンプルプログラムと必要なライブラリは、USPiルートで次を実行する
ことでビルドできます。


	./makeall clean
	./makeall


*kernel.img*イメージファイルはソースコードと同じディレクトリにあります。これをファームウェアファイル
*bootcode.bin*, *fixup.dat*, *start.elf* （これらは[ここ](https://github.com/raspberrypi/firmware/tree/master/boot)から取得できます）と共にSD(HC)カードにコピーしてください。そして、SD(HC)カードをRaspberry 
Piに入れて起動してください。

AArch64
-------

USPiライブラリは、AArch64（64ビット）環境で使用できるようにビルドすることもできます。
そのためにはビルド前に*Rules.mk*か*Config.mk*で次の定義をしておく必要があります。

	AARCH64 = 1

AArch64のサポートには環境ライブラリは対応していません。そのため、自身で用意した
OS環境と組み合わせて使用することしかできません。*sample/* サブディレクトリにある
サンプルプログラムはそのままではビルドできません。

USPiライブラリをビルドするには次のようにします。

	cd lib
	make clean
	make

USPiライブラリをAArch64モードで使用する場合は使用するOS環境のコンパイラオプションに `-DAARCH64=1` を追加する必要があります。
