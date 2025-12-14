PDFium ビルドから DLL 作成までのコマンド例（Windows, PowerShell）

1) ビルド用ディレクトリ作成と引数設定  
```
cd C:\WorkSpace\pdfium
mkdir out\Release -Force
cmd /c "echo is_component_build = false>> out\Release\args.gn"
cmd /c "echo pdf_is_standalone = true>> out\Release\args.gn"
cmd /c "echo pdf_enable_v8 = true>> out\Release\args.gn"
cmd /c "echo v8_static_library = true>> out\Release\args.gn"
cmd /c "echo v8_monolithic = true>> out\Release\args.gn"
cmd /c "echo v8_use_external_startup_data = false>> out\Release\args.gn"
cmd /c "echo is_debug = false>> out\Release\args.gn"
cmd /c "echo is_clang = true>> out\Release\args.gn"
cmd /c "echo use_lld = true>> out\Release\args.gn"
cmd /c "echo symbol_level = 1>> out\Release\args.gn"
```

2) GN 生成  
```
gn gen out/Release
```

3) ビルド（DLL は `out\Release\be-pdfium.dll` として出力）  
```
autoninja -C out/Release be_pdfium_dll
```

4) 生成物の確認  
```
dir out\Release\be-pdfium.dll
```

備考
- 上記は DLL 名を `be-pdfium.dll` に変更済みの `BUILD.gn` を前提とします。
- `args.gn` は既存内容を残したい場合、`gn args out/Release --list` で現在値を確認し、必要分だけ追加してください。
