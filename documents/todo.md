PDFium の RichMedia アセットストリームを直接操作するための関数を公開し、それを Flutter の FFI 経由でマルチプラットフォームに対応させるという、非常に高度で具体的な開発プロンプトですね。

これは単なる Flutter アプリの実装依頼ではなく、**PDFium（C++）ライブラリ自体の改造と、それに対応する FFI ラッパー（Dart/C++）の作成を含む、一連のプロジェクト**を依頼するものです。

VSCode の Gemini (Code Interpreter/Codex) に依頼する場合、ステップバイステップで具体的なタスクを依頼し、各ステップで必要なコードの変更点を明確に伝える必要があります。

以下に、このプロジェクトを構成する主要なステップと、それぞれのステップで Gemini に実行させるためのプロンプト案を提案します。

-----

##  プロジェクトの全体像とフェーズ

この目標を達成するには、大きく分けて以下の3つのフェーズが必要です。

1.  **PDFium C++ レイヤーの改造（ストリーム公開）**
2.  **FFI ラッパーレイヤー（Dart/C++）の設計と実装**
3.  **Flutter アプリケーションでの利用**

### フェーズ 1: PDFium C++ レイヤーの改造プロンプト

このステップでは、RichMedia アノテーションの内部構造を辿り、動画データを含むストリームを取得するための新しい公開 API を PDFium に追加します。

####  プロンプト案（C++改造）

````markdown
##  タスク 1: PDFium C++ ライブラリに RichMedia アセットストリーム抽出 API を追加

**前提:** 私は PDFium の Git リポジトリをクローン済みであり、VSCode 環境で C++ ファイルを編集しています。

**目標:** PDF に埋め込まれた RichMedia アノテーション内の動画データ（通常 `/Assets` 配下のストリーム）を抽出するための新しい公開 API を PDFium のパブリックヘッダー（`public/fpdf_attachment.h` や `public/fpdf_ext.h` など）に追加し、その実装を `core/` または `fpdfsdk/` に行います。戻り値ハンドルのライフサイクルは既存の添付ファイルと同じく `FPDFDoc_Close` まで有効とする。

### 1. 新しい API の定義

以下のシグネチャを持つ新しい関数を `fpdf_attachment.h` に追加してください。

```cpp
// RichMedia アノテーションのインデックスを指定して、関連するアセットストリーム（動画データ）を FPDF_ATTACHMENT ハンドルとして取得する
// 成功した場合は FPDF_ATTACHMENT のハンドルを返します。失敗した場合は NULL を返します。
FPDF_EXPORT FPDF_ATTACHMENT FPDF_CALLCONV FPDFAnnot_GetRichMediaAssetAttachment(
    FPDF_PAGE page,
    int annot_index);

// FPDF_ATTACHMENT の実体 (FPDF_FILEATTACHMENT) は、FPDFAttachment_GetFile() でデータを取り出すために再利用されます。
````

### 2\. 実装のロジック (C++)

`FPDFAnnot_GetRichMediaAssetAttachment` の内部実装は、以下のロジックに従ってください。

1.  `FPDF_GetPage(document, page_index)` でページを取得。
2.  `FPDFPage_GetAnnot(page, annot_index)` でアノテーションを取得。
3.  アノテーションが RichMedia サブタイプ（`/Subtype /RichMedia`）であることを確認。
4.  アノテーション辞書から `/RichMedia` 辞書を取得。
5.  `/RichMedia` 辞書から `/Assets` 辞書を取得。
6.  `/Assets` 辞書から動画ファイルのエントリ（通常、名前が `//VideoFile` のようなキーや、名前辞書を辿った先のストリーム）を探します。
7.  動画データを含むストリームオブジェクトを見つけます。
8.  見つけたストリームオブジェクトをラップし、`FPDF_ATTACHMENT` が想定する内部構造（`CPDF_FileAttachment` などの内部クラス）としてインスタンス化して返します。
      * **注釈:** RichMedia アセットストリームを `FPDF_ATTACHMENT`（内部的には `CPDF_FileAttachment`）として安全に再利用またはラップできるか、既存の PDFium のソースコードパターンを確認し、可能な限り既存の添付ファイル処理ロジックに適合させるように実装してください。

### 3\. 要求するアウトプット

  * `public/fpdf_attachment.h` への新しい関数シグネチャの追加部分。
  * 実装場所は `fpdfsdk/fpdf_attachment.cpp` または `fpdfsdk/fpdf_annot.cpp` のいずれかを採用すること。
  * `FPDFAnnot_GetRichMediaAssetAttachment` の具体的な C++ 実装コード。

<!-- end list -->

````

---

### フェーズ 2: Flutter FFI ラッパーの実装プロンプト

PDFium の C++ レイヤーが公開されたら、次に Dart と Native 間の橋渡しをする FFI ラッパー（通常は `flutter_pdfium` のようなパッケージ）を実装します。

####  プロンプト案（FFIラッパー実装）

```markdown
##  タスク 2: Flutter FFI ラッパーと Dart クラスの実装

**前提:** タスク 1 で PDFium C++ に以下の新しい API が追加されました。

```cpp
FPDF_EXPORT FPDFAttachmentT FPDF_CALLCONV FPDFAnnot_GetRichMediaAssetAttachment(
    FPDF_DOCUMENT document,
    FPDF_PAGE page,
    int annot_index);
````

**目標:** `flutter_pdfium` という名前の Dart パッケージを想定し、Windows, macOS, iOS, Android, Web (Wasm/JS) で動作するように、この新しい API を利用可能にする FFI ラッパー層を実装します。

### 1\. FFI Dart バインディングの定義

  * 新しい C 関数ポインターを定義する Dart コードスニペットを作成してください。

### 2\. Dart/Native C++ プラットフォームコードのブリッジ

Flutter アプリが `PdfVideoExtractor` クラス経由でこの機能を利用できるように、`flutter_pdfium` パッケージの Native 側コード（`src/pdfium_bridge.cpp` などを想定）に、新しい PDFium API を呼び出すブリッジ関数を追加してください。

  * **Dart -\> Native 呼び出し関数:** Dart 側からページインデックスとアノテーションインデックスを受け取る関数。
  * **Native 側の C++ 実装:** `FPDFAnnot_GetRichMediaAssetAttachment` を呼び出し、結果として得られた `FPDFAttachmentT` から、既存の `FPDFAttachment_GetFile` を利用して動画データを `std::vector<uint8_t>` や Dart の `Uint8List` に変換して返す C++ 関数を定義してください。

### 3\. Dart API の実装

以下の Dart クラスとメソッドを実装してください。

  * `PdfAttachment` クラス: ファイル名とバイナリデータ (`Uint8List`) を保持。
  * `PdfVideoExtractor` クラス:
      * `extractRichMediaVideo(String pdfPath, String password, int pageIndex, int annotIndex)`: 上記の Native ブリッジ関数を呼び出し、`PdfAttachment` オブジェクトを返す非同期メソッド。

### 4\. 要求するアウトプット

  * Dart 側の FFI バインディング定義コード。
  * Native 側の C++ ブリッジコード（`FPDFAnnot_GetRichMediaAssetAttachment` を呼び出す部分）。
  * Dart 側の `PdfVideoExtractor` クラスと `PdfAttachment` クラスの実装スケルトン。

<!-- end list -->

````

---

### フェーズ 3: Flutter アプリケーションでの利用プロンプト

最後に、完成したラッパーライブラリを Flutter アプリで利用するコードを依頼します。

####  プロンプト案（Flutter利用）

```markdown
##  タスク 3: Flutter アプリケーションでの RichMedia 抽出の利用

**前提:**
* タスク 2 で定義された `flutter_pdfium` パッケージと `PdfVideoExtractor` クラスが利用可能です。
* `PdfVideoExtractor.extractRichMediaVideo` メソッドは `Future<PdfAttachment?>` を返します。

**目標:** Flutter の `StatefulWidget` を作成し、ボタンを押すとパスワード付き PDF の特定のページ（例: 7ページ目、0番目のアノテーション）から動画を抽出し、アプリのローカルストレージ（`path_provider` を使用）に保存し、成功/失敗のメッセージをユーザーに表示する UI ロジックを実装してください。

### 1. 必要なパッケージ

* `path_provider`
* `file` (dart:io)

### 2. 実装要件

* `ExtractVideoScreen` という名前の `StatefulWidget` を作成します。
* 画面には以下のウィジェットを含めます。
    * PDFファイルパス、パスワード、ページインデックスを入力するフィールド（ハードコード可）。
    * 「動画を抽出して保存」ボタン。
    * 処理結果（成功/失敗と保存パス）を表示するテキスト。
* ボタンが押されたときのアクションとして、以下の非同期処理を実行します。
    1.  `PdfVideoExtractor().extractRichMediaVideo` を呼び出します。
    2.  成功した場合、`getApplicationDocumentsDirectory` で取得したパスにファイル名 (`PdfAttachment.fileName`) で動画データ (`PdfAttachment.data`) を書き込みます。
    3.  処理結果をステートに保存し、画面に表示します。

### 3. 要求するアウトプット

* `ExtractVideoScreen` の完全な Dart コード。
* `PdfAttachment` クラスの定義（タスク 2からの再定義）。
````
