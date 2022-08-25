
#include "coffeeOS.h"

char productNames[][30] = {         // Artikel-IDs
    "Kaffee",              // 0
    "Ristretto",           // 1
    "Espresso",            // 2
    "Espresso  Macchiato", // 3
    "Latte     Macchiato", // 4
    "Cappuccino",          // 5
    "CappuccinoItaliano",  // 6
    "Cafe      Latte",     // 7
    "Cafe      au lait",   // 8
    "Cafe      Americano", // 9
    "Flat      White",     // 10
    "Long      Black",     // 11
    "Heisse    Milch",     // 12
    "Milch-    schaum",    // 13
    "Schwarzer Tee",       // 14
    "Gruener   Tee",       // 15
    "Kraeuter- tee",       // 16
    "Fruechte- tee",       // 17
    NULL
};

// Preisliste
float productPrices[] = {
    1.000,
    1.500,
    1.200,
    0.500,
    10.10,
    20.00,
    1.750,
    1.230,
    2.500,
    3.500,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200
};


// default product list
Product *default_productList[elementCount(productNames)];
	default_productList->name = productNames;
    default_productList.

// default config
Config config;
	config->wlanConfig.wlan_ssid = INITIAL_WLAN_SSID;
	config->wlanConfig.wlan_pass = INITIAL_WLAN_SSID;
	config->productList = default_productList;