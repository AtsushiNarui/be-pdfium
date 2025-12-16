# PDFium WebAssembly ビルド手順（このブランチ用メモ）

このブランチでは `build/config/BUILDCONFIG.gn` に Emscripten 用のデフォルト
ツールチェーン指定を追加し、`target_os = "emscripten"` で GN が通るようにしてある。
以下の手順は Windows/PowerShell 前提。

## 事前準備（Emscripten）
1. 任意の場所に emsdk を展開（推奨はリポジトリ内 `third_party/emsdk`）
   ```ps1
   git clone https://github.com/emscripten-core/emsdk.git third_party/emsdk
   cd third_party/emsdk
   ./emsdk install latest
   ./emsdk activate latest
   ./emsdk_env.ps1   # 以降このシェルで作業
   cd ../..
   ```
   既存インストールがある場合は後述の `emscripten_path` をそのパスに合わせる。

## GN 生成
`gn args out/wasm` で以下をセット（シングルラインでも可）:
```
target_os = "emscripten"
target_cpu = "wasm"
is_debug = false
is_component_build = false
pdf_enable_v8 = false     # V8 + WASM は core/fxcrt/fx_system.h で禁止
pdf_enable_xfa = false    # XFA は V8 依存
pdf_use_skia = false
use_custom_libcxx = false
emscripten_path = "//third_party/emsdk/upstream/emscripten/"
```
上記を設定後:
```ps1
gn gen out/wasm
```

## ビルド
```ps1
ninja -C out/wasm pdfium_test
```
成果物: `out/wasm/pdfium_test.html`, `out/wasm/pdfium_test.wasm`,
`out/wasm/pdfium_test.wasm.debug.wasm`。簡易確認はローカル http サーバーで
`pdfium_test.html` を開いて行う。

ライブラリだけ欲しい場合:
```ps1
ninja -C out/wasm pdfium
```
`out/wasm/obj/` 以下に emar で生成された `.a` が出力される。

## 日常のビルド手順（再掲）
WASM ブランチでのルーチンは「emsdk 環境を読む→gn→ninja」の3ステップ。

1. シェルで Emscripten をロード  
   `third_party/emsdk/emsdk_env.ps1`
2. GN（args は既に `out/wasm/args.gn` に保存済み）  
   `gn gen out/wasm`  
   ※ `checkout_skia=false` の警告は無視可。気になるなら args.gn から行を削除。
3. ビルド  
   `ninja -C out/wasm pdfium_test` （または `ninja -C out/wasm pdfium`）

成果物は `out/wasm/pdfium_test.{html,wasm,wasm.debug.wasm}`。

## メモ
- Emscripten 環境が無いと `gn gen`/`ninja` でツールが見つからず失敗するので、
  必ず `emsdk_env.ps1` をロードしたシェルで実行する。
- さらに機能を有効化する場合（例: Skia）は `pdf_use_skia = true` などで試す。
