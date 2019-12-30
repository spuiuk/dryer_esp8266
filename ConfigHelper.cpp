#include <EEPROM.h>
#include <string.h>

#include "globals.h"
#include "ConfigHelper.h"

ConfigHelper::ConfigHelper()
{
    memset(&conf, 0, sizeof(struct ConfigStruct));
    memcpy(&conf.version, CONFIG_VERSION, 4);
    return;
}

bool ConfigHelper::readConfig()
{
    struct ConfigStruct c;

    EEPROM.get(CONFIG_START_FROM, c);
    if(strcmp(conf.version, CONFIG_VERSION)) {
        dbg_print("Invalid configuration present");
        return false;
    }

    memcpy(&conf, &c, sizeof(struct ConfigStruct));
    return true;
}

bool ConfigHelper::saveConfig()
{
    /* Can store max of 512 bytes */
    if (sizeof(struct ConfigStruct) > 512) {
        dbg_print("Configuration size too large");
        return false;
    }

    EEPROM.put(CONFIG_START_FROM, conf);

    return true;
}