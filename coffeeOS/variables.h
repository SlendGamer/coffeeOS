/*============================ ESP32_Coffee_Inator ==========================
  Made by Tim Prüß


 *===========================================================================*/

#define DEBUG
//define HOME

#define OLED_SDA          21
#define OLED_SCL          22

#define PN532_SDA         16
#define PN532_SCL         17
#define PN532_IRQ         5
#define PN532_RST         18

#define ROT_CLK           32
#define ROT_DT            35
#define ROT_BUTTON        34
#define ROT_BUTTON_PRESS  LOW

#define RESET_PIN         -1
#define OLED_ADDR         0x3C
#define FLIP180           0
#define INVERT            0   
#define USE_HW_I2C        0

#define MAX_USERS         20
#define ARTICLES          18

//Variablen deklarieren/ initialisieren
unsigned char menuID = 0;
char sTemp[20];

unsigned long lastMillis = 0;
bool pressConfirm = false, onOff; 

int16_t brightness = 255;
float balance;

static uint8_t ucBackBuffer[1024];

#ifndef HOME
const char* initial_ssid = "FRITZ!Box 7490";
const char* initial_password = "+Feuerwehr-dbkt1";
#else
const char* initial_ssid = "Feldfunkstation 1";
const char* initial_password = "52837754105258160424";
#endif

String header;

typedef struct {
  char      c_name[10];
  uint8_t   cardUID[4];
  uint8_t   memory[18];
} user_t;

user_t* user;
TaskHandle_t Task1;

//Menu-Items
char *menu0[] = {                 //Artikel-IDs
  (char *)"Kaffee",               //0
  (char *)"Ristretto",            //1
  (char *)"Espresso",             //2
  (char *)"Espresso  Macchiato",  //3
  (char *)"Latte     Macchiato",  //4
  (char *)"Cappuccino",           //5
  (char *)"CappuccinoItaliano",   //6
  (char *)"Cafe      Latte",      //7
  (char *)"Cafe      au lait",    //8
  (char *)"Cafe      Americano",  //9
  (char *)"Flat      White",      //10
  (char *)"Long      Black",      //11
  (char *)"Heisse    Milch",      //12
  (char *)"Milch-    schaum",     //13
  (char *)"Schwarzer Tee",        //14
  (char *)"Gruener   Tee",        //15
  (char *)"Kraeuter- tee",        //16
  (char *)"Fruechte- tee",        //17
  (char *)"Settings", NULL};

//Preisliste
float prices[] = {  //Artikel-IDs
  1.000,            //0
  1.500,            //1
  1.200,            //2
  0.500,            //3
  10.10,            //4
  20.00,            //5
  1.750,            //6
  1.230,            //7
  2.500,            //8
  3.500,            //9
  4.200,            //10
  4.200,            //11
  4.200,            //12
  4.200,            //13
  4.200,            //14
  4.200,            //15
  4.200,            //16
  4.200 };          //17

//Settings-Menu
char *menu1[] = { 
  (char *)"<<<<",
  (char *)"User-Info",
  (char *)"User-Reset",
  (char *)"Netzwerk-Info",
  (char *)"Helligkeit", NULL};

//neue Instantzen erzeugen
Adafruit_PN532 nfc(PN532_IRQ, PN532_RST);
Preferences pref;
WiFiServer server(80);
ESP32Encoder encoder;
SIMPLEMENU menu;
OBDISP obd;

void confirm();
void saveUsers(user_t* user);
void loadUsers(user_t** user);
int8_t getElements(user_t* user, uint8_t* UID);
