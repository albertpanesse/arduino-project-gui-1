#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define MAIN_PAGE 1
#define NET_SETUP_PAGE 2

#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF
#define LIGHTGREY 0xDDDD

#define BTN_BACK          1
#define BTN_NET_SETUP     2

const int XP = 6, XM = A2, YP = A1, YM = 7;
const int TS_LEFT = 154, TS_RT = 814, TS_TOP = 91, TS_BOT = 883;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button btnBack, btnNetSetup;
bool btnEnabled[2] = {BTN_BACK, BTN_NET_SETUP};

int pixel_x, pixel_y, scrWidth, scrHeight, prevPage, currentPage, pageChanged;

void setup(void)
{
  Serial.begin(9600);
  
  uint16_t ID = tft.readID();
  Serial.print("TFT ID = 0x");
  Serial.println(ID, HEX);
  Serial.println("Calibrate for your Touch Panel");
  
  if (ID == 0xD3D3) ID = 0x9486;
  
  tft.begin(ID);
  tft.setRotation(0);
  tft.fillScreen(BLACK);  

  scrWidth = tft.width();
  scrHeight = tft.height();

  Serial.print("Screen width = ");
  Serial.println(scrWidth);
  Serial.print("Screen height = ");
  Serial.println(scrHeight);

  prevPage = 0;
  currentPage = MAIN_PAGE;
  pageChanged = 1;
}

bool TouchGetXY(void)
{
  TSPoint p = ts.getPoint();
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);
  digitalWrite(YP, HIGH);
  digitalWrite(XM, HIGH);
  bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
  if (pressed) {
    pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
    pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
  }
  return pressed;
}

void disableAllBtn() {
  
}

void centerPrint(const char *s, int y)
{
  int len = strlen(s) * 6 * 2;
  tft.setCursor((scrWidth - len) / 2, y);
  tft.setTextColor(WHITE);
  tft.print(s);
}

void printTitle(const char *s)
{
  tft.setTextSize(2);
  centerPrint(s, 10);
}

void toast(const char *s, int delay) {
  tft.fillRoundRect(40, 80, 160, 80, 5, LIGHTGREY);
}

void drawIpAddrInput() {
  tft.fillRect(0, 0, 240, 320, BLACK);

  drawBackBtn();
}

void drawBackBtn() {
  tft.setTextSize(1);

  btnBack.initButton(&tft,  50, 280, 60, 30, WHITE, BLUE, BLACK, "Back", 1);
  btnBack.drawButton(false);  
}

void drawMainPage() {
  tft.fillRect(0, 0, 240, 320, BLACK);

  printTitle("Network Scanner");

  btnNetSetup.initButton(&tft,  120, 70, 160, 30, WHITE, CYAN, BLACK, "Net Setup", 2);
  btnNetSetup.drawButton(false);
}

void drawNetSetupPage() {
  tft.fillRect(0, 0, 240, 320, BLACK);

  printTitle("Network Scanner");

  tft.setTextColor(WHITE);
  tft.setTextSize(1);

  // IP Address Input
  tft.setCursor(10, 55);
  tft.print("IP Address");

  tft.drawRect(100, 50, 120, 20, WHITE);

  tft.setCursor(115, 55);
  tft.print("xxx.xxx.xxx.xxx");

  // Netmask Input
  tft.setCursor(10, 85);
  tft.print("Netmask");

  tft.drawRect(100, 80, 120, 20, WHITE);

  tft.setCursor(115, 85);
  tft.print("xxx.xxx.xxx.xxx");

  // Gateway Input
  tft.setCursor(10, 115);
  tft.print("Gateway");

  tft.drawRect(100, 110, 120, 20, WHITE);

  tft.setCursor(115, 115);
  tft.print("xxx.xxx.xxx.xxx");

  // DNS Input
  tft.setCursor(10, 145);
  tft.print("DNS Server");

  tft.drawRect(100, 140, 120, 20, WHITE);

  tft.setCursor(115, 145);
  tft.print("xxx.xxx.xxx.xxx");

  drawBackBtn();
}

void loop(void)
{
  if (pageChanged > 0) {
    switch(currentPage) {
      case MAIN_PAGE:
        drawMainPage();
        break;
      case NET_SETUP_PAGE:
        drawNetSetupPage();
        break;
    }

    pageChanged = 0;
  }

  bool down = TouchGetXY();

  btnBack.press(down && btnBack.contains(pixel_x, pixel_y));
  btnNetSetup.press(down && btnNetSetup.contains(pixel_x, pixel_y));
  
  if (btnBack.justPressed()) {
    currentPage = prevPage;
    prevPage = 0;
    pageChanged = 1;
  }

  if (btnNetSetup.justPressed()) {
    prevPage = currentPage;
    currentPage = NET_SETUP_PAGE;
    pageChanged = 1;
  }
}
