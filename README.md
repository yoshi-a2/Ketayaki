# Ketayaki
桁焼き用のプログラムやアイデアをメモしておく場所


## タッチパネル & SDカードスロット付ディスプレイ　[ILI9341](https://akizukidenshi.com/catalog/g/g116265/)
### <ライブラリ>
#### グラフィック
- https://github.com/adafruit/Adafruit-GFX-Library
- https://github.com/adafruit/Adafruit_ILI9341
  
#### タッチパネル
- https://github.com/PaulStoffregen/XPT2046_Touchscreen

#### jpeg表示
- https://github.com/Bodmer/TJpg_Decoder


### <サイト>
- https://tamanegi-digick.com/it/ili9341/ ←こいつを見ればすべてが分かる
  - arduino環境
  - 画面表示，タッチパネル，SD  

- https://qiita.com/hcuymitoh/items/a7a0ae1a8b61478eac9d
  - arduino環境
  - タッチパネル特化型
 
### コマンド達
#### 色  
Adafruit_ILI9341.h　L106~  
#define ILI9341_BLACK 0x0000       ///<   0,   0,   0
#define ILI9341_NAVY 0x000F        ///<   0,   0, 123
#define ILI9341_DARKGREEN 0x03E0   ///<   0, 125,   0
#define ILI9341_DARKCYAN 0x03EF    ///<   0, 125, 123
#define ILI9341_MAROON 0x7800      ///< 123,   0,   0
#define ILI9341_PURPLE 0x780F      ///< 123,   0, 123
#define ILI9341_OLIVE 0x7BE0       ///< 123, 125,   0
#define ILI9341_LIGHTGREY 0xC618   ///< 198, 195, 198
#define ILI9341_DARKGREY 0x7BEF    ///< 123, 125, 123
#define ILI9341_BLUE 0x001F        ///<   0,   0, 255
#define ILI9341_GREEN 0x07E0       ///<   0, 255,   0
#define ILI9341_CYAN 0x07FF        ///<   0, 255, 255
#define ILI9341_RED 0xF800         ///< 255,   0,   0
#define ILI9341_MAGENTA 0xF81F     ///< 255,   0, 255
#define ILI9341_YELLOW 0xFFE0      ///< 255, 255,   0
#define ILI9341_WHITE 0xFFFF       ///< 255, 255, 255
#define ILI9341_ORANGE 0xFD20      ///< 255, 165,   0
#define ILI9341_GREENYELLOW 0xAFE5 ///< 173, 255,  41
#define ILI9341_PINK 0xFC18        ///< 255, 130, 198
