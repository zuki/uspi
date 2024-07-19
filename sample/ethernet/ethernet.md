# Ethernetサンプル

このサンプルはEthernetコントローラを搭載したRaspberry Piだけで動作する
Ethernet ARPレスポンダです。ビルドする前に`main.c`の`OWN_IP_ADDRESS`に
ローカルネットワークで有効なIPアドレスを設定する必要があります。

ビルドして実行した後、ネットワーク上の他のコンピュータから"arping <ipaddress>"を
実行します。<ipaddress> は選択したアドレスです。arpingコマンドは毎秒毎にARP (
Address Resolution Protocol)リクエストを送信します。すべてがうまくいけば、
Raspberry Piはこのリクエストに応答し、arpingがその応答を表示します。使用するには
root権限が必要な場合があります。

arpingが使えない場合は"ping <ipaddress>"を使うこともできます。この場合は
ARPリクエストは一度だけ送信され応答されます。その後、pingはICMPエコー
パケットを送信しますがこれは応答されません。（もしあれば）Wiresharkなどの
ネットワークモニタでネットワークトラフィックを見ることができます。
