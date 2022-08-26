#include "coffeeOS.h"

void saveUsers(user_t *user)
{
    pref.begin("settings");
    int8_t elements = getElements(user, NULL);
    if (elements == -1)
    {
        Serial.println("Saving Users failed!");
        pref.end();
        return;
    }
    pref.putBytes("userList", user, (elements + 1) * sizeof(user_t));
    pref.end();
    Serial.println("Saving Users successful!");
}

void loadUsers(user_t **user)
{
    pref.begin("settings");
    size_t len = pref.getBytesLength("userList");

    if (len == 0)
    {
        Serial.println("Loading Users failed!");
        return;
    }

    char *buffer = (char *)malloc(len);
    pref.getBytes("userList", buffer, len);
    *user = (user_t *)realloc(buffer, len);
    if (*user == NULL)
    {
        Serial.println("Loading Users failed!");
        pref.end();
        return;
    }

    free(buffer);
    pref.end();
    Serial.println("Loading Users successful!");
}

int8_t getElements(user_t *user, uint8_t *UID)
{
    int8_t elementCount = 0;

    if (UID == NULL)
    {
        while (strcmp(user[elementCount].c_name, ""))
        {
            if (elementCount > 126)
            {
                return -1;
            }
            elementCount++;
        }
    }
    else
    {
        int8_t maxElements = getElements(user, NULL);
        size_t len = sizeof(((user_t){0}).cardUID);
        while (strncmp((const char *)user[elementCount].cardUID, (const char *)UID, len))
        {
            if (elementCount > 126 || elementCount == (maxElements - 1))
            {
                return -1;
            }
            elementCount++;
        }
    }

    return elementCount;
}