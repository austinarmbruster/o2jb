# How To

Until the installer is written, the driver needs to be manually added to Windows registry.  Add the HKLM\Software\ODBC\ODBCINST.INI\O2JB Driver key. Then add the following values:

* ConnectFunctions / REG_SZ / YYN
* Driver / REG_SZ / <path to dll>\o2jb.dll
* Setup / REG_SZ / <path to dll>\o2jb.dll
* SQLLevel / REG_SZ / 0

