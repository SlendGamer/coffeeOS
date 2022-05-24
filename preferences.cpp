#include <Preferences.h>
#include <nvs_flash.h>

String ssid;
String password;

typedef struct {
	char c_ssid[20];
	char c_password[20];
	
} setting_t;

typedef struct {
	char      c_name[10];
	uint8_t   cardUID[4];
	uint8_t   memory[18];
} user_t;

Preferences pref;

/*  Diese Funktion speichert das aktuelle Struct-Array in ein
 *  Preference-Register in Form von reinen Bytes,
 *  die dann beim Auslessen wieder in Form des typedef's gebracht werden müssen.
 *  Es wird immer ein struct-Element mehr gespeichert, das leer ist,
 *  damit das Ende des Arrays erkennbar ist, da hier mit Pointern gearbeitet wird. */
void saveUsers(user_t* user) {
	pref.begin("settings");
	int8_t elements = getElements(user, NULL);
		if(elements == -1) {Serial.println("Saving Users failed!"); pref.end(); return;}
	pref.putBytes("userList", user, (elements + 1) * sizeof(user_t));
	pref.end();
	Serial.println("Saving Users successful!");
}

void saveSettings(setting_t* settings) {
	pref.begin("settings");
		if(settings->c_ssid == NULL || settings->c_password) {Serial.println("Settings Empty!"); pref.end(); return;}
	pref.putBytes("settings", settings, sizeof(setting_t));
	pref.end();
	Serial.println("Saving Settings successful!");
}

void loadUsers(user_t** user) {
	pref.begin("settings");
	size_t len = pref.getBytesLength("userList");
	
	if(len == 0) {Serial.println("Loading Users failed!"); return;}

	char* buffer = (char*) malloc(len);
	pref.getBytes("userList", buffer, len);
	*user = (user_t*) realloc(buffer, len);
	if(*user == NULL) {Serial.println("Loading Users failed!"); pref.end(); return;}

	free(buffer);
	pref.end();
	Serial.println("Loading Users successful!");
}

void loadSettings(setting_t* settings) {
	pref.begin("settings");
	size_t len = pref.getBytesLength("settings");
	if(len == 0) {Serial.println("Loading Settings failed!"); pref.end(); return;} 
	
	pref.getBytes("settings", settings, len);
	pref.end();
	Serial.println("Loading Users successful!");
}

int8_t getElements(user_t* user, uint8_t* UID) {
	int8_t elementCount = 0;
	
		if(UID == NULL) {
			while(strcmp(user[elementCount].c_name, "")) {
				if(elementCount > 126) {
					return -1; 
				}
			elementCount++;
			}
		}
		else { 
			int8_t maxElements = getElements(user, NULL);
			size_t len = sizeof(((user_t){0}).cardUID);
			while(strncmp((const char*) user[elementCount].cardUID, (const char*) UID, len)) {
				if(elementCount > 126 || elementCount == (maxElements - 1)) {
					return -1;
				}
				elementCount++;
			}
		}
		
	return elementCount;
}




// put your setup code here, to run once:
void setup() {

	Serial.begin(115200);
	 
	user_t* user = (user_t*) calloc(1, sizeof(user_t));
	if(user == NULL) { Serial.println("Memory Allocation failed"); while(true); }

	user_t* userTemp  = (user_t*) calloc(1, sizeof(user_t));
	if(userTemp == NULL) { Serial.println("Memory Allocation failed"); while(true); }
	
	setting_t* settings = (setting_t*) calloc(1, sizeof(setting_t));
	if(settings == NULL) { Serial.println("Memory Allocation failed"); while(true); }
	
	pinMode(25, INPUT_PULLUP);
	pinMode(26, INPUT_PULLUP);
	pinMode(27, INPUT_PULLUP);
	pinMode(33, INPUT_PULLUP);

	if(digitalRead(33) == LOW) {
		nvs_flash_erase();
		nvs_flash_init();
	}
	
	
	if(digitalRead(25) == LOW) {
		free(user);
		user = (user_t*) calloc(6, sizeof(user_t));
		if(user == NULL) { Serial.println("Memory Allocation failed"); while(true); }
		user[0] = (user_t) {"Tim"   , {186, 186, 192, 36},  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
		user[1] = (user_t) {"Olaf"  , {209, 190, 78, 2},    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
		user[2] = (user_t) {"Holger", {1, 2, 3, 4},         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
		user[3] = (user_t) {"Markus", {3, 2, 4, 1},         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
		user[4] = (user_t) {"Ronald", {2, 3, 1, 4},         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
		
		saveUsers(user);

		//*settings = (setting_t) {"FRITZ!Box 7490", "+Feuerwehr-dbkt1"};
	}

	loadUsers(&user);
	
	
	/*Element hinzufügen
	if(digitalRead(26) == LOW) {
		int8_t elements = getElements(user, NULL);

		if(elements == 0){ Serial.println("Users Empty!"); while(true);}
		
		int8_t newElements = elements + 3;

		
		userTemp = (user_t*) realloc(user, newElements * sizeof(user_t));
		if(userTemp == NULL) { Serial.println("Memory Allocation failed"); while(true); }
		memmove(userTemp, user, elements * sizeof(user_t));
		free(user);

		user = userTemp;
		//user = (user_t*) realloc(user, 5 * sizeof(user_t));
		
		
		user[elements] = (user_t) {"Stephan", {2, 3, 1, 4}, {10, 5}};
		user[elements + 1] = (user_t) {"Svenja", {2, 3, 1, 4}, {10, 5}};
		//memset(user[elements + 2], 0, sizeof(user_t));
		user[elements + 2] = (user_t) {"", NULL, NULL};
		
		saveUsers(user);
		//free(userTemp);
		//loadUsers(&user);
	}*/
	
	uint8_t uid[] = {186, 186, 192, 36, 0, 0, 0};
	
	int8_t element = getElements(user, uid);

	//Serial.printf("SSID: %s \nPASSWORD: %s\n\n", settings->c_ssid, settings->c_password);
	
	Serial.printf("Anzahl der Einträge: %d\n", element);
	
		for(uint8_t i= 0; i < getElements(user, NULL); i++) {
			Serial.printf("Name: %s \n", user[i].c_name);
			Serial.print("UID: ");
				for(uint8_t c = 0; c < sizeof(user[i].cardUID); c++) {
					Serial.printf(" %d ", user[i].cardUID[c]);
				}
			Serial.println();
		}
 
	while(true);
	
}

void loop() {
	// put your main code here, to run repeatedly:

}
