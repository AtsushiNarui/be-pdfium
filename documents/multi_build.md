ビルドするときにコマンドを変えるだけで、Windows,Macos,ios,android,webのそれぞれ用のライブラリをビルドできるようにしたいけど、可能？


可能です。GN/Ninja でターゲットごとに out ディレクトリと args を分ければ、同じソースから Windows/macOS/iOS/Android/Web(emscripten) を切り替えられます。ポイントと例:

ベース: gn gen out/<target> --args="…", その後 ninja -C out/<target> pdfium.
Windows (MSVC):
target_os="win" target_cpu="x64" is_clang=false（または clangcl）。
macOS:
target_os="mac" target_cpu="x64" または "arm64"、ユニバーサルなら target_cpu="x64" と --args='target_cpu="x64"' で別ビルドを lipo 結合。
iOS:
target_os="ios" target_cpu="arm64" is_component_build=false ios_enable_code_signing=false use_xcode_clang=true など。シミュレータ用は target_cpu="x64" or "arm64"。後で lipo/xcframework まとめ。
Android:
target_os="android" target_cpu="arm64"（他に x86, x86_64, arm）。is_component_build=false is_clang=true。NDK パスは android_ndk_root 等を args で指定。
Web (WASM):
target_os="emscripten" pdf_enable_xfa=false pdf_use_skia=false use_custom_libcxx=true is_component_build=false など。Emscripten 環境で emconfigure gn gen ... や env をセット。ninja -C out/wasm pdfium で .wasm + glue JS が出るようエクスポート設定。
運用例: tools/build_all.py のようなスクリプトで各ターゲットの gn gen + ninja を呼ぶだけにし、引数は環境変数や CLI フラグで切替。
注意点:

iOS/Android/WASM は事前に各 SDK/NDK/Emscripten の環境変数を用意。
iOS/Android/WASM は is_component_build=false (static) 推奨。
Universal/xcframework/AAR など配布形態に合わせて post-step を入れる。
この形で「コマンドを変えるだけ」でビルド切替は実現できます。必要なら具体的な GN args サンプルと簡易スクリプト案を作ります。