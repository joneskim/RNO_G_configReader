/*
    Written by Joneskim Kimo, 2023
    This script contains functions for reading configuration files for RNO-G data using the Libconfig library. It constains utility functions to convert the settings 
    to strings, retrieve the settings, and handle common aliases.

    Libconfig is a C++ library for reading, manipulating, and writing structured configuration files. IT provides a simple API for reading and writing from/to configuration
    files with support for arrays, lists, groups, etc
*/

#include <iostream>
#include <fstream>
#include <libconfig.h++>
#include <unordered_map>
#include <sstream>

/*
    Converts the value of libconfig Setting to a string representation.

    This function is used to convert various types of setting values(integers, booleans, strings, arrays, lists, etc) to a string representation. It provides a consistent way 
    to convert these values making it easier to handle the different types of settings.

    Parameters
    ----------
    setting : Setting
        The setting to convert to a string representation.
    path :A string
        The path to the setting. This is used to determine the type of the setting.

    Returns
    -------
    string
        The string representation of the setting value.

    Note: 
        The function returns an empty string if the setting type is not supported.
        The function only supports the following types of settings: integers, booleans, strings, arrays, and lists.
*/

std::string settingValueToString(const libconfig::Setting& setting, const std::string& path = "")
{
    if (setting.getType() == libconfig::Setting::TypeInt)
    {
        int intValue = setting;
        return std::to_string(intValue);
    }
    else if (setting.getType() == libconfig::Setting::TypeString)
    {
        std::string stringValue = setting;
        return stringValue;
    }
    else if (setting.getType() == libconfig::Setting::TypeBoolean)
    {
        bool boolValue = setting;
        return std::to_string(boolValue);
    }
    else if (setting.getType() == libconfig::Setting::TypeFloat)
    {
        float floatValue = setting;
        return std::to_string(floatValue);
    }
    else if (setting.getType() == libconfig::Setting::TypeArray)
    {
        std::stringstream ss;
        std::string value = "";
        for (int i = 0; i < setting.getLength(); ++i)
        {
            const libconfig::Setting& subsetting = setting[i];
            if (subsetting.getType() == libconfig::Setting::TypeInt)
            {
                int intValue = subsetting;
                ss << intValue;
            }
            if (subsetting.getType() == libconfig::Setting::TypeFloat)
            {
                float floatValue = subsetting;
                ss << floatValue;
            }
            if (i == 0)
            {
                value += "[" + ss.str();
            }
            else if (i == setting.getLength() - 1)
            {
                value += "," + ss.str() + "]";
            }
            else
            {
                value += "," + ss.str();
            }
            ss.str("");
        }
        return value;
    }
    else if (setting.getType() == libconfig::Setting::TypeList)
    {
        std::stringstream ss;
        std::string value = "";
        for (int i = 0; i < setting.getLength(); ++i)
        {
            const libconfig::Setting& subsetting = setting[i];
            if (subsetting.getType() == libconfig::Setting::TypeInt)
            {
                int intValue = subsetting;
                ss << intValue;
            }
            if (subsetting.getType() == libconfig::Setting::TypeFloat)
            {
                float floatValue = subsetting;
                ss << floatValue;
            }
            if (i == 0)
            {
                value += "(" + ss.str();
            }
            else if (i == setting.getLength() - 1)
            {
                value += "," + ss.str() + ")";
            }
            else
            {
                value += "," + ss.str();
            }
            ss.str("");
        }
        return value;
    }

    return "";
}

/*

    Retrieves the value of a setting from the configuration file.

    This function allows retrieving the value of a setting from the configuration file. It uses the libconfig library to read the configuration file and retrieve the value
    of the setting. It also handles the case where the setting is a group of settings. If the setting is a group it traverses the group and retrieves the value of each setting
    in the group. and returns a string representation of the group.

    Parameters
    ----------
    config : Config
        The configuration object that contains the settings.
    path :A string
        The path to the setting. This is used to determine the type of the setting.

    Returns
    -------
    string
        The string representation of the setting value.

    Note:
        The function returns an empty string if the setting type is not supported.
        The function only supports the following types of settings: integers, booleans, strings, arrays, and lists.
        The function uses the settingValueToString function to convert the setting value to a string representation.

*/

std::string getSettingValue(const libconfig::Config& config, const std::string& path)
{
    try
    {
        const libconfig::Setting& setting = config.lookup(path);
        if (setting.getType() != libconfig::Setting::TypeGroup)
        {
            return settingValueToString(setting, path);
        }
        else if (setting.getType() == libconfig::Setting::TypeGroup)
        {
            std::stringstream ss;
            std::string value = "";
            for (int i = 0; i < setting.getLength(); ++i)
            {
                const libconfig::Setting& subsetting = setting[i];
                std::string subsettingValue = settingValueToString(subsetting, path + "." + subsetting.getName());
                std::string subsettingName = subsetting.getName();
                if (i == 0)
                {
                    value += "{\n" + subsettingName + " = " + subsettingValue;
                }
                else if (i == setting.getLength() - 1)
                {
                    value += ", \n" + subsettingName + " = " + subsettingValue + "\n}";
                }
                else
                {
                    value += ", \n" + subsettingName + " = " + subsettingValue;
                }
            }
            return value;
        }
        return "";
    }
    catch (const libconfig::SettingException& ex)
    {
        std::cout << "Error: " << ex.what() << ": " << path << std::endl;
        return "";
    }
}


/*
    Retrieves the value of a common setting from the configuration file.

    This function allows retrieving the value of a common setting from the configuration file. It has a list of common settings and their aliases. 
    It uses the libconfig library to read the configuration file and retrieve the value of the setting.

    Parameters
    ----------
    config : Config
        The configuration object that contains the settings.
    alias :A string
        The alias of the common setting. The alias is used to determine the path to the setting.

    Returns
    -------
    string
        The string representation of the setting value.

    Note:
        The function returns an empty string if the setting type is not supported.
        The function only supports the following types of settings: integers, booleans, strings, arrays, and lists.
        The function uses the settingValueToString function to convert the setting value to a string representation.
*/


std::string getCommonSettingValue(const libconfig::Config& config, const std::string& alias)
{
    if (alias.find(".") != std::string::npos)
    {
        return getSettingValue(config, alias);
    }

    static std::unordered_map<std::string, std::string> commonSettings = {
        {"rf0_enabled", "radiant.trigger.RF0.enabled"},
        {"rf1_enabled", "radiant.trigger.RF1.enabled"},
        {"scalers_use_pps", "radiant.scalers.use_pps"}
        // add more common settings here
    };

    auto it = commonSettings.find(alias);
    if (it != commonSettings.end())
    {
        return getSettingValue(config, it->second);
    }
    else
    {
        std::cout << "Error: Unknown common setting alias: " << alias << std::endl;
        // Return empty string if alias is not found
        return "";
    }
}

/*
    
    Reads the configuration file for a run.

    This function reads the configuration file for a run. 

    Parameters
    ----------
    station : int
        The station number.
    run : int
        The run number.
    directory : string
        The directory where the run data is stored.
    configSettingPath : string
        The path to the common setting in the configuration file.

    Returns
    -------
    string
        The string representation of the setting value.

    Note:
        The function returns an empty string if the setting type is not supported.
        The function only supports the following types of settings: integers, booleans, strings, arrays, and lists.
        The function uses the settingValueToString function to convert the setting value to a string representation.
*/

void readConfigFile(int station, int run, const std::string& directory = "data/handcarry22/rootified", const std::string& configSettingPath = "radiant.scalers.use_pps")
{
    std::string configFilepath = directory + "/station" + std::to_string(station) + "/run" + std::to_string(run) + "/cfg/" + "acq.cfg";
    libconfig::Config cfg;

    try
    {
        chdir("/");
        cfg.readFile(configFilepath.c_str());
    }
    catch (const libconfig::FileIOException& fioex)
    {
        std::cout << "Error: I/O error while reading file." << std::endl;
        return;
    }
    catch (const libconfig::ParseException& pex)
    {
        std::cout << "Error: Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
        return;
    }
    std::string value_f = getCommonSettingValue(cfg, configSettingPath);
    std::cout << configSettingPath << " : " << value_f << std::endl;
}

/*
    
    Example of reading the configuration file for a run.

    Parameters
    ----------
    station : int
        The station number.
    run : int
        The run number.
    directory : string
        The directory where the run data is stored.
    configSettingPath : string
        The path to the common setting in the configuration file.

    Returns
    -------
    string
        The string representation of the setting value.

    Note:
        The function returns an empty string if the setting type is not supported.
        The function only supports the following types of settings: integers, booleans, strings, arrays, and lists.
        The function uses the settingValueToString function to convert the setting value to a string representation.

*/

void configReader(int station=23, int run=327, std::string directory="data/handcarry22/rootified", std::string setting_path_alias="radiant.scalers.use_pps")
{
    return readConfigFile(station, run, directory, "radiant.scalers.period");
}