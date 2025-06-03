# Ketayaki
桁焼き用のプログラムやアイデアをメモしておく場所


# タッチパネル & SDカードスロット付ディスプレイ　[ILI9341](https://akizukidenshi.com/catalog/g/g116265/)
## <ライブラリ>
### グラフィック
- https://github.com/adafruit/Adafruit-GFX-Library
- https://github.com/adafruit/Adafruit_ILI9341
  
### タッチパネル
- https://github.com/PaulStoffregen/XPT2046_Touchscreen

### jpeg表示
- https://github.com/Bodmer/TJpg_Decoder


## <サイト>
- https://tamanegi-digick.com/it/ili9341/ ←こいつを見ればすべてが分かる
  - arduino環境
  - 画面表示，タッチパネル，SD  

- https://qiita.com/hcuymitoh/items/a7a0ae1a8b61478eac9d
  - arduino環境
  - タッチパネル特化型
  
## **リファレンス**
### 文字の表示
tft.printf("文字\n");  
現状，アルファベットと数字，多少の記号しか表示できない

### 色  
tft.setTextColor(ILI9341_カラー名);
- 定義場所：Adafruit_ILI9341.h　L106~
- デフォのカラバリ
  - BLACK
  - NAVY
  - DARKGREEN
  - DARKCYAN
  - MAROON
  - PURPLE
  - OLIVE
  - LIGHTGREY
  - DARKGREY
  - BLUE
  - GREEN
  - CYAN
  - RED
  - MAGENTA
  - YELLOW
  - WHITE
  - ORANGE
  - GREENYELLOW
  - PINK

### 背景色
tft.fillScreen(ILI9341_カラー名);

### 字体
#include ".hあり字体名";  
tft.setFont(.hなし字体名);

### テキストサイズ
tft.setTextSize(1~?の数字);

### カーソル
tft.setCursor(xドット数, yドット数);  
xは0～340, yは0～240までの値　　  
横文字列の先頭文字の位置を指定するコード


