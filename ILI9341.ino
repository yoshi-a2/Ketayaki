#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

#define TFT_WIDTH 320    //x方向ビット数
#define TFT_HEIGHT 240   //y方向ビット数
int page = 2;  //表示ページ
unsigned long time_start = 0;   //桁焼き開始時刻
unsigned long time_now = 0;
unsigned long time_start_to_now = 0;
#define sound 0 //タッチ音

#define TFT_CS 22   // CS
#define TFT_RST 21   // Reset 
#define TFT_DC 20   // D/ C
#define TOUCH_MISO 16
#define TOUCH_CS 17
#define TFT_TOUCH_MOSI 19   // SDI(MOSI)
#define TFT_TOUCH_SCK 18   // SCK

//グラフィックのインスタンス
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);

//タッチパネルのインスタンス
XPT2046_Touchscreen ts(TOUCH_CS);

//スプライト（16bit）
GFXcanvas16 canvas(TFT_WIDTH, TFT_HEIGHT);

//変数宣言
bool lampSignal = false;   // ランプ点灯状態格納用
bool change_page = false;   //ページを変えるスイッチ用


/******************** テキスト描画関数 ********************/
void drawText(int16_t x, int16_t y, const char* text, const GFXfont* font, uint16_t color) {
  canvas.setFont(font);       // フォント
  canvas.setTextColor(color); // 文字色
  canvas.setCursor(x, y);     // 表示座標
  canvas.println(text);       // 表示内容
}

/***************** ランプ点灯状態更新関数 *****************/
void updateLamp_on() {
  if (lampSignal == true) { // ランプ状態点灯なら
    canvas.fillCircle(70, 22, 17, ILI9341_RED);         // ランプ部点灯
  }
  else { // ランプ状態消灯なら
    canvas.fillCircle(70, 22, 17, ILI9341_DARKGREY); // ランプ部消灯
  }
}
void updateLamp_off() {
  if (lampSignal == false) { // ランプ状態点灯なら
    canvas.fillCircle(210, 22, 17, ILI9341_BLUE);         // ランプ部点灯
  }
  else { // ランプ状態消灯なら
    canvas.fillCircle(210, 22, 17, ILI9341_DARKGREY); // ランプ部消灯
  }
}

/******************** ボタン描画関数 ********************/
void drawButton(int x, int y, int w, int h, const char* label, const GFXfont* font, uint16_t bgColor, uint16_t labelColor) {
  canvas.fillRect(x, y, w, h, ILI9341_DARKGREY);      // 外枠
  canvas.fillRect(x + 3, y + 3, w-6, h-6, ILI9341_WHITE); // 境界線
  canvas.fillRect(x + 6, y + 6, w-12, h-12, bgColor);       // 操作部
  canvas.setFont(font); // 表示ラベル

  // テキストの幅と高さを取得
  int16_t textX, textY; // テキスト位置取得用
  uint16_t textWidth, textHeight; // テキストサイズ取得用
  canvas.getTextBounds(label, x, y, &textX, &textY, &textWidth, &textHeight);
  
  // 中央揃えのための新しいx, y座標の計算
  int16_t centeredX = x + (w - textWidth) / 2;               // xを中央へ
  int16_t centeredY = y + (h - textHeight) / 2 + textHeight; // yを下げて中央へ
  
  canvas.setTextColor(labelColor);        // 文字色
  canvas.setCursor(centeredX, centeredY); // 新しいカーソル位置を設定
  canvas.print(label);                    // テキストを描画
}


/******************** 2ページ目 ********************/
int page_2(){
  lampSignal = false;
  canvas.fillScreen(ILI9341_BLACK);   //背景色リセット
  drawText(92, 30, "Heating", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(232, 30, "Waiting", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(60, 210, "Temp:", &FreeSans12pt7b, ILI9341_WHITE);
  drawText(190, 210, "[degC]", &FreeSans12pt7b, ILI9341_WHITE);

  // 平行線(x始点，y始点，長さ)
  canvas.drawFastHLine(0, 46, 320, ILI9341_WHITE);

  // Heating_Waitingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(70, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(70, 22, 18, ILI9341_WHITE);    // 境界線
  canvas.fillCircle(210, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(210, 22, 18, ILI9341_WHITE);    // 境界線
  canvas.fillCircle(70, 22, 17, ILI9341_DARKGREY); // Heatingランプ部消灯
  canvas.fillCircle(210, 22, 17, ILI9341_BLUE); // Waitingランプ部点灯

  // ボタン描画（左上x, 左上y, wide, high, ラベル, フォント, ボタン色, ラベル色）
  drawButton(25, 80, 140, 100, "Manual", &FreeSansBold18pt7b, ILI9341_ORANGE, ILI9341_WHITE); // ONボタン
  drawButton(170, 80, 140, 100, "Auto", &FreeSansBold18pt7b, ILI9341_GREEN, ILI9341_WHITE); // OFFボタン

  //スプライトをディスプレイ表示
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);


  if (ts.touched() == true) {  // タッチされていれば
    TS_Point tPoint = ts.getPoint();  // タッチ座標を取得

    // タッチx座標をTFT画面の座標に換算
    int16_t x = (tPoint.x-400) * TFT_WIDTH / (4095-550);  // タッチx座標をTFT画面の座標に換算
    int16_t y = (tPoint.y-230) * TFT_HEIGHT / (4095-420); // タッチy座標をTFT画面の座標に換算

    // ボタンタッチエリア検出
    if (x >= 25 && x <= 165 && y >= 80 && y <= 180){
      tone(0,3000,100);
      page = 3;
    }    // 範囲内ならmanualに
    if (x >= 170 && x <= 310 && y >= 80 && y <= 180){
      tone(0,3000,100);
      page = 4;
    } // 範囲内ならautoに

  }
  else{
    page_2();
  }
  
  return page;
}

/******************** 3ページ目 ********************/
int page_3(){
  canvas.fillScreen(ILI9341_BLACK);   //背景色リセット
  drawText(92, 30, "Heating", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(232, 30, "Waiting", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(8, 85, "Temp:", &FreeSans18pt7b, ILI9341_WHITE);
  drawText(245, 88, "[degC]", &FreeSans12pt7b, ILI9341_WHITE);

  // 平行線(x始点，y始点，長さ)
  canvas.drawFastHLine(0, 46, 320, ILI9341_WHITE);
  canvas.drawFastHLine(0, 100, 320, ILI9341_WHITE);

  // Heatingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(70, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(70, 22, 18, ILI9341_WHITE);    // 境界線
  updateLamp_on(); // ボタン点灯状態更新関数呼び出し
  updateLamp_off(); // ボタン点灯状態更新関数呼び出し

  // Waitingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(210, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(210, 22, 18, ILI9341_WHITE);    // 境界線
  updateLamp_off(); // ボタン点灯状態更新関数呼び出し
  updateLamp_on(); // ボタン点灯状態更新関数呼び出し

  // ボタン描画（左上x, 左上y, wide, high, ラベル, フォント, ボタン色, ラベル色）
  drawButton(25, 102, 140, 85, "ON", &FreeSansBold18pt7b, ILI9341_RED, ILI9341_WHITE); // OFFボタン
  drawButton(170, 102, 140, 85, "OFF", &FreeSansBold18pt7b, ILI9341_BLUE, ILI9341_WHITE); // ONボタン
  drawButton(60, 188, 200, 45, "Main Menu", &FreeSans18pt7b, ILI9341_ORANGE, ILI9341_WHITE); // cancelボタン

  //スプライトをディスプレイ表示
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);


  if (ts.touched() == true) {  // タッチされていれば
    TS_Point tPoint = ts.getPoint();  // タッチ座標を取得

    // タッチx座標をTFT画面の座標に換算
    int16_t x = (tPoint.x-400) * TFT_WIDTH / (4095-550);  // タッチx座標をTFT画面の座標に換算
    int16_t y = (tPoint.y-230) * TFT_HEIGHT / (4095-420); // タッチy座標をTFT画面の座標に換算

    // ボタンタッチエリア検出
    if (x >= 25 && x <= 165 && y >= 102 && y <= 187){
      lampSignal = true;   // 範囲内ならランプ状態を点灯へ
      tone(0,3000,100);
    }
    if (x >= 170 && x <= 310 && y >= 102 && y <= 187) {
      lampSignal = false; // 範囲内ならランプ状態を消灯へ
      tone(0,3000,100);
    }
    if (x >= 60 && x <= 260 && y >= 188 && y <= 233){
      page = 2; // 範囲内ならpage2へ
      tone(0,3000,100);
    }

  }
  else{
    page_3();
  }

  return page;
}

/******************** 4ページ目 ********************/
int page_4(){
  lampSignal = false;
  canvas.fillScreen(ILI9341_BLACK);   //背景色リセット
  drawText(92, 30, "Heating", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(232, 30, "Waiting", &FreeSans9pt7b, ILI9341_WHITE);

  drawText(20, 80, "Adjust to 90 degC", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(230, 80, "60 min", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(20, 100, "Keep 90 degC", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(230, 100, "360 min", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(20, 120, "Adjust to 130 degC", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(230, 120, "40 min", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(20, 140, "Keep 130 degC", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(230, 140, "120 min", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(20, 160, "Adjust to 30 degC", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(230, 160, "100 min", &FreeSans9pt7b, ILI9341_WHITE);




  // 平行線(x始点，y始点，長さ)
  canvas.drawFastHLine(0, 46, 320, ILI9341_WHITE);

  // Heating_Waitingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(70, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(70, 22, 18, ILI9341_WHITE);    // 境界線
  canvas.fillCircle(210, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(210, 22, 18, ILI9341_WHITE);    // 境界線
  canvas.fillCircle(70, 22, 17, ILI9341_DARKGREY); // Heatingランプ部消灯
  canvas.fillCircle(210, 22, 17, ILI9341_BLUE); // Waitingランプ部点灯

  // ボタン描画（左上x, 左上y, wide, high, ラベル, フォント, ボタン色, ラベル色）
  drawButton(25, 170, 140, 55, "OK", &FreeSansBold18pt7b, ILI9341_CYAN, ILI9341_WHITE); // Cancelボタン
  drawButton(170, 170, 140, 55, "Cancel", &FreeSansBold18pt7b, ILI9341_CYAN, ILI9341_WHITE); // OKボタン

  //スプライトをディスプレイ表示
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);


  if (ts.touched() == true) {  // タッチされていれば
    TS_Point tPoint = ts.getPoint();  // タッチ座標を取得

    // タッチx座標をTFT画面の座標に換算
    int16_t x = (tPoint.x-400) * TFT_WIDTH / (4095-550);  // タッチx座標をTFT画面の座標に換算
    int16_t y = (tPoint.y-230) * TFT_HEIGHT / (4095-420); // タッチy座標をTFT画面の座標に換算

    // ボタンタッチエリア検出
    if (x >= 25 && x <= 165 && y >= 170 && y <= 225){
      page = 6;   // 範囲内ならpage6
      tone(0,3000,100);
    }
    if (x >= 170 && x <= 310 && y >= 170 && y <= 225){
      page = 2; // 範囲内ならpage2
      tone(0,3000,100);
    }
 

  }
  else{
    page_4();
  }

  return page;
}

/******************** 5ページ目 ********************/
void page_5(){
  lampSignal = false;
  canvas.fillScreen(ILI9341_BLACK);   //背景色リセット
 
    
}

/******************** 6ページ目 ********************/
int page_6(){
  lampSignal = false;
  canvas.fillScreen(ILI9341_BLACK);   //背景色リセット
  drawText(92, 30, "Heating", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(232, 30, "Waiting", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(60, 230, "Temp:", &FreeSans12pt7b, ILI9341_WHITE);
  drawText(190, 230, "[degC]", &FreeSans12pt7b, ILI9341_WHITE);
  drawText(10, 80, "Start ", &FreeSans18pt7b, ILI9341_WHITE);
  drawText(45, 110, "Auto-Ketayaki?", &FreeSans18pt7b, ILI9341_WHITE);

  
  // 平行線(x始点，y始点，長さ)
  canvas.drawFastHLine(0, 46, 320, ILI9341_WHITE);

  // Heating_Waitingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(70, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(70, 22, 18, ILI9341_WHITE);    // 境界線
  canvas.fillCircle(210, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(210, 22, 18, ILI9341_WHITE);    // 境界線
  canvas.fillCircle(70, 22, 17, ILI9341_DARKGREY); // Heatingランプ部消灯
  canvas.fillCircle(210, 22, 17, ILI9341_BLUE); // Waitingランプ部点灯


  // ボタン描画（左上x, 左上y, wide, high, ラベル, フォント, ボタン色, ラベル色）
  drawButton(25, 120, 140, 85, "Start", &FreeSansBold18pt7b, ILI9341_ORANGE, ILI9341_WHITE); // ONボタン
  drawButton(170, 120, 140, 85, "Cancel", &FreeSansBold18pt7b, ILI9341_GREEN, ILI9341_WHITE); // OFFボタン

  //スプライトをディスプレイ表示
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);


  if (ts.touched() == true) {  // タッチされていれば
    TS_Point tPoint = ts.getPoint();  // タッチ座標を取得

    // タッチx座標をTFT画面の座標に換算
    int16_t x = (tPoint.x-400) * TFT_WIDTH / (4095-550);  // タッチx座標をTFT画面の座標に換算
    int16_t y = (tPoint.y-230) * TFT_HEIGHT / (4095-420); // タッチy座標をTFT画面の座標に換算

    // ボタンタッチエリア検出
    if (x >= 25 && x <= 165 && y >= 120 && y <= 205){
      tone(0,3000,100);
      page = 7;   // 範囲内ならpage7
    }
    if (x >= 170 && x <= 310 && y >= 120 && y <= 205){
      page = 4; // 範囲内ならpage4
      tone(0,3000,100);
    }

  }
  else{
    page_6();
  }
  
  return page;
}

/******************** 7ページ目 桁焼き中********************/
int page_7(){
  canvas.fillScreen(ILI9341_BLACK);   //背景色リセット
  drawText(92, 30, "Heating", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(232, 30, "Waiting", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(8, 85, "Temp:", &FreeSans18pt7b, ILI9341_WHITE);
  drawText(245, 88, "[degC]", &FreeSans12pt7b, ILI9341_WHITE);
  drawText(8, 130, "Phase:", &FreeSans18pt7b, ILI9341_WHITE);

  time_now = millis();

  time_start_to_now = (time_now - time_start);

  /*
  if(0 <= time_start_to_now < ){
    drawText(150, 130, "Adjust to 90", &FreeSans12pt7b, ILI9341_WHITE);
  }
  */
  drawText(150, 130, "Adjust to 90", &FreeSans12pt7b, ILI9341_WHITE);

  canvas.setFont(&FreeSans9pt7b);  // フォント指定
  canvas.setCursor(50, 155);          // 表示座標指定
  canvas.print(time_start_to_now);           // 経過時間をms単位で表示


  // 平行線(x始点，y始点，長さ)
  canvas.drawFastHLine(0, 46, 320, ILI9341_WHITE);
  canvas.drawFastHLine(0, 100, 320, ILI9341_WHITE);
  canvas.drawFastHLine(0, 180, 320, ILI9341_WHITE);

  // スライドボリューム描画
  canvas.fillRect(10, 162, 300, 11, ILI9341_WHITE);    // 縦ライン枠
  canvas.fillRect(12, 164, 296, 7, ILI9341_BLACK);     // 縦ライン
  //canvas.fillRect(270, slideYValue, 45, 20, 0x0904);   // ボリュームつまみ
  //canvas.fillRect(278, slideYValue + 8, 29, 3, ILI9341_RED);  // ボリュームつまみライン




  // Heatingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(70, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(70, 22, 18, ILI9341_WHITE);    // 境界線
  updateLamp_on(); // ボタン点灯状態更新関数呼び出し
  updateLamp_off(); // ボタン点灯状態更新関数呼び出し

  // Waitingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(210, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(210, 22, 18, ILI9341_WHITE);    // 境界線
  updateLamp_off(); // ボタン点灯状態更新関数呼び出し
  updateLamp_on(); // ボタン点灯状態更新関数呼び出し

  // ボタン描画（左上x, 左上y, wide, high, ラベル, フォント, ボタン色, ラベル色）
  drawButton(25, 185, 140, 50, "Graph", &FreeSans18pt7b, ILI9341_ORANGE, ILI9341_WHITE); // OFFボタン
  drawButton(170, 185, 140, 50, "Emergency", &FreeSansBold9pt7b, ILI9341_RED, ILI9341_YELLOW); // ONボタン

  //スプライトをディスプレイ表示
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);


  if (ts.touched() == true) {  // タッチされていれば
    TS_Point tPoint = ts.getPoint();  // タッチ座標を取得

    // タッチx座標をTFT画面の座標に換算
    int16_t x = (tPoint.x-400) * TFT_WIDTH / (4095-550);  // タッチx座標をTFT画面の座標に換算
    int16_t y = (tPoint.y-230) * TFT_HEIGHT / (4095-420); // タッチy座標をTFT画面の座標に換算

    // ボタンタッチエリア検出
    if (x >= 25 && x <= 165 && y >= 185 && y <= 235){
      page = 10;   // 範囲内ならpage10
      tone(0,3000,100);
    }
    if (x >= 170 && x <= 310 && y >= 185 && y <= 235){
      page = 8; // 範囲内ならpage8
      tone(0,3000,100);
    }

  }
  else{
    page_7();

  }

  return page;
}


/******************** 10ページ目 グラフ********************/
int page_10(){
  canvas.fillScreen(ILI9341_BLACK);   //背景色リセット
  drawText(92, 30, "Heating", &FreeSans9pt7b, ILI9341_WHITE);
  drawText(232, 30, "Waiting", &FreeSans9pt7b, ILI9341_WHITE);


  // 平行線，垂直(x始点，y始点，長さ)
  canvas.drawFastHLine(0, 46, 320, ILI9341_WHITE);
  canvas.drawFastHLine(15, 225, 300, ILI9341_WHITE);
  canvas.drawFastVLine(15, 50, 175, ILI9341_WHITE);


  // Heatingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(70, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(70, 22, 18, ILI9341_WHITE);    // 境界線
  updateLamp_on(); // ボタン点灯状態更新関数呼び出し
  updateLamp_off(); // ボタン点灯状態更新関数呼び出し

  // Waitingランプ描画(x, y, 半径, 色)
  canvas.fillCircle(210, 22, 20, ILI9341_DARKGREY); // 外枠
  canvas.fillCircle(210, 22, 18, ILI9341_WHITE);    // 境界線
  updateLamp_off(); // ボタン点灯状態更新関数呼び出し
  updateLamp_on(); // ボタン点灯状態更新関数呼び出し

  // ボタン描画（左上x, 左上y, wide, high, ラベル, フォント, ボタン色, ラベル色）
  drawButton(25, 185, 140, 50, "Back", &FreeSans18pt7b, ILI9341_ORANGE, ILI9341_WHITE); // page7に戻る


  //スプライトをディスプレイ表示
  tft.drawRGBBitmap(0, 0, canvas.getBuffer(), TFT_WIDTH, TFT_HEIGHT);


  if (ts.touched() == true) {  // タッチされていれば
    TS_Point tPoint = ts.getPoint();  // タッチ座標を取得

    // タッチx座標をTFT画面の座標に換算
    int16_t x = (tPoint.x-400) * TFT_WIDTH / (4095-550);  // タッチx座標をTFT画面の座標に換算
    int16_t y = (tPoint.y-230) * TFT_HEIGHT / (4095-420); // タッチy座標をTFT画面の座標に換算

    // ボタンタッチエリア検出
    if (x >= 25 && x <= 165 && y >= 185 && y <= 235) {
      tone(0,3000,100);
      page = 7;
      }   // 範囲内ならpage7


  }
  else{
    page_10();
  }

  return page;
}


void setup(void){
  Serial.begin(115200); 


  //SPI0
  SPI.setTX(TFT_TOUCH_MOSI);
  SPI.setSCK(TFT_TOUCH_SCK);

  //グラフィック設定
  tft.begin();
  tft.setRotation(3);                         //画面回転（0~3）
  tft.fillScreen(ILI9341_BLACK);   //背景色リセット
  canvas.setTextSize(1);                      //テキストサイズ

  //タッチパネル設定
  ts.begin();                   // タッチパネル初期化
  ts.setRotation(1); // タッチパネルの回転(画面回転が3ならここは1)
}

void loop(){
  switch(page){
    case 1:
      page_2();
      break;
    case 2:
      page_2();
      break;
    case 3:
      delay(200);
      page_3();
      break;
    case 4:
      delay(200);
      page_4();
      break;
    case 5:
      delay(200);
      page_5();
      break;
    case 6:
      delay(200);
      page_6();
      break;
    case 7:
      time_start = millis();
      delay(200);
      page_7();
      break;
    case 8:
      delay(200);
      page_2();
      break;
    case 9:
      delay(200);
      page_2();
      break;
    case 10:
      delay(200);
      page_10();
      break;
    case 11:
      delay(200);
      page_2();
      break;

    default:
      Serial.print("Page number error!!");

  }






  




}