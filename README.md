# Kos
UEFIを利用した64bit対応のOS（作りかけ）

特徴としてEDKIIなどの外部ライブラリに頼ることなくブートローダーが実装されています。  
UEFIからブートローダ、kernelの読み込み、起動までできます。

## Requirements
* x86_64-w64-mingw32-gcc
* qemu-system-x86_64

Ubuntuでなら
`sudo apt install gcc-mingw-w64 qemu-system-x86`
でインストールできる
