#ifndef ConfigHelper_h
#define ConfigHelper_h

/* Max of three characters */
#define CONFIG_VERSION "dr1"
#define CONFIG_START_FROM 0

struct ConfigStruct
{
    char version[4];
};

class ConfigHelper {
    public:
        ConfigHelper();
        bool readConfig();
        bool saveConfig();
        struct ConfigStruct conf;
};

#endif