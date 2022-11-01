#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <inttypes.h>
//#include <iostream>
#include <glib.h>

#include <pthread.h>
#include <gdbus.h>

#include "json-c/json.h"
#include "rkdb.h"
#include "common.h"

#define TABLE_NETWORK_IP            "NetworkIP"
#define TABLE_NETWORK_SERVICE       "NetworkService"
#define TABLE_NETWORK_POWER         "NetworkPower"
#define TABLE_NETWORK_NTP           "ntp"
#define TABLE_NETWORK_ZONE          "zone"
#define TABLE_NETWORK_PORT          "port"
#define TABLE_NETWORK_VERSION       "NetworkVersion"

#define NETWORK_VERSION             "1.0.3"

int network_dbus_register(DBusConnection *dbus_conn)
{
   bool ret = g_dbus_register_interface(dbus_conn, "/",
                              DB_NET_INTERFACE,
                              methods,
                              signals, NULL, DB_NET_INTERFACE, NULL);
   printf("network_dbus_register ret is :%d,\n",ret);
    return 0;
}

void network_init(void)
{
    char *col_para;
//    printf("TABLE_NETWORK_VERSION %s\n",TABLE_NETWORK_VERSION);
//     printf("NETWORK_VERSION %s\n",NETWORK_VERSION);
//     printf("equal_version(TABLE_NETWORK_VERSION, NETWORK_VERSION) %d\n",equal_version(TABLE_NETWORK_VERSION, NETWORK_VERSION));
     
     
     
     
//     printf("rkdb_drop(TABLE_NETWORK_IP):  %s\n",rkdb_drop(TABLE_NETWORK_IP));
//     printf("rkdb_drop(TABLE_NETWORK_SERVICE):  %s\n",rkdb_drop(TABLE_NETWORK_SERVICE));
//     printf(" rkdb_drop(TABLE_NETWORK_POWER)):  %s \n",rkdb_drop(TABLE_NETWORK_POWER));
//     printf("rkdb_drop(TABLE_NETWORK_NTP)) :  %s \n",rkdb_drop(TABLE_NETWORK_NTP));
//     printf("  :  %s\n ",rkdb_drop(TABLE_NETWORK_ZONE));
//     printf("  :  %s\n ",rkdb_drop(TABLE_NETWORK_PORT));
//     printf("  :  %s\n ",rkdb_drop(TABLE_NETWORK_VERSION));

   // if (equal_version(TABLE_NETWORK_VERSION, NETWORK_VERSION))
    //    return;

    printf("rkdb_drop0 %s\n",rkdb_drop(TABLE_NETWORK_IP));
    printf("rkdb_drop1 %s\n",rkdb_drop(TABLE_NETWORK_SERVICE));
    printf("rkdb_drop2 %s\n",rkdb_drop(TABLE_NETWORK_POWER));
    printf("rkdb_drop3 %s\n",rkdb_drop(TABLE_NETWORK_NTP));
    printf("rkdb_drop4 %s\n",rkdb_drop(TABLE_NETWORK_ZONE));
    printf("rkdb_drop5 %s\n",rkdb_drop(TABLE_NETWORK_PORT));
    printf("rkdb_drop6 %s\n",rkdb_drop(TABLE_NETWORK_VERSION));

    creat_version_table(TABLE_NETWORK_VERSION, NETWORK_VERSION);

    col_para = "id INTEGER PRIMARY KEY AUTOINCREMENT," \
               "sService TEXT NOT NULL UNIQUE," \
               "sPassword TEXT DEFAULT ''," \
               "iFavorite INT DEFAULT 0," \
               "iAutoconnect INT DEFAULT 0";

    printf("rkdb_create0 %s\n",rkdb_create(TABLE_NETWORK_SERVICE, col_para));

    col_para = "id INTEGER PRIMARY KEY AUTOINCREMENT," \
               "sInterface TEXT NOT NULL UNIQUE," \
               "sType TEXT NOT NULL," \
               "sV4Method TEXT DEFAULT ''," \
               "sV4Address TEXT DEFAULT ''," \
               "sV4Netmask TEXT DEFAULT ''," \
               "sV4Gateway TEXT DEFAULT ''," \
               "sV6Method TEXT DEFAULT ''," \
               "sV6Address TEXT DEFAULT ''," \
               "sV6Netmask TEXT DEFAULT ''," \
               "sV6Gateway TEXT DEFAULT ''," \
               "sNicSpeed TEXT DEFAULT ''," \
               "sDNS1 TEXT DEFAULT ''," \
               "sDNS2 TEXT DEFAULT ''";
    /* sNicSpeed:Auto,10baseT/Half,10baseT/Full,100baseT/Half,100baseT/Full,1000baseT/Half 1000baseT/Full */
    printf("rkdb_create1 %s\n",rkdb_create(TABLE_NETWORK_IP, col_para));
    printf("rkdb_insert0 %s\n",rkdb_insert(TABLE_NETWORK_IP, "sInterface,sType,sV4Method", "'wlan0','wifi', 'dhcp'"));
    printf("rkdb_insert 1%s\n",rkdb_insert(TABLE_NETWORK_IP, "sInterface,sType,sV4Method,sNicSpeed", "'eth0','ethernet', 'dhcp','Auto'"));

    col_para = "id INTEGER PRIMARY KEY AUTOINCREMENT," \
               "sType TEXT NOT NULL UNIQUE," \
               "iPower INT DEFAULT 0";

    printf("rkdb_create 2%s\n",rkdb_create(TABLE_NETWORK_POWER, col_para));
    printf("rkdb_insert 2%s\n",rkdb_insert(TABLE_NETWORK_POWER, "sType,iPower", "'wifi',0"));
    printf("rkdb_insert3%s\n",rkdb_insert(TABLE_NETWORK_POWER, "sType,iPower", "'ethernet',1"));


    col_para = "id INTEGER PRIMARY KEY," \
               "sNtpServers TEXT NOT NULL," \
               "sTimeZone TEXT NOT NULL," \
               "sTimeZoneFile TEXT NOT NULL," \
               "sTimeZoneFileDst TEXT NOT NULL," \
               "iAutoMode INT DEFAULT 0," \
               "iAutoDst INT DEFAULT 0," \
               "iRefreshTime INT DEFAULT 120";
    g_free(rkdb_create(TABLE_NETWORK_NTP, col_para));
    g_free(rkdb_insert(TABLE_NETWORK_NTP, "id,sNtpServers,sTimeZone,sTimeZoneFile,sTimeZoneFileDst,iAutoDst,iAutoMode,iRefreshTime", "0,'122.224.9.29 94.130.49.186','ChinaStandardTime-8','posix/Etc/GMT-8','posix/Asia/Shanghai',0,1,60"));

    col_para = "id INTEGER PRIMARY KEY AUTOINCREMENT," \
               "sTimeZone TEXT NOT NULL," \
               "sTimeZoneFile TEXT NOT NULL," \
               "sTimeZoneFileDst TEXT NOT NULL";
    g_free(rkdb_create(TABLE_NETWORK_ZONE, col_para));
    g_free(rkdb_insert(TABLE_NETWORK_ZONE, "sTimeZone,sTimeZoneFile,sTimeZoneFileDst", "'HawaiianStandardTime10','posix/Etc/GMT+10','posix/US/Hawaii'"));//(UTC-10:00) 夏威夷
    g_free(rkdb_insert(TABLE_NETWORK_ZONE, "sTimeZone,sTimeZoneFile,sTimeZoneFileDst", "'PacificStandardTime8DaylightTime,M3.2.0,M11.1.0','posix/Etc/GMT+8','posix/US/Pacific'"));//(UTC-08:00) 太平洋时间(美国和加拿大)
    g_free(rkdb_insert(TABLE_NETWORK_ZONE, "sTimeZone,sTimeZoneFile,sTimeZoneFileDst", "'GMTStandardTime0DaylightTime,M3.5.0/1,M10.5.0','posix/Etc/GMT-0','posix/Europe/London'"));//(UTC+00:00) 都柏林,爱丁堡,里斯本,伦敦
    g_free(rkdb_insert(TABLE_NETWORK_ZONE, "sTimeZone,sTimeZoneFile,sTimeZoneFileDst", "'ChinaStandardTime-8','posix/Etc/GMT-8','posix/Asia/Shanghai'"));//(UTC+08:00) 北京,重庆,香港特别行政区,乌鲁木齐
    g_free(rkdb_insert(TABLE_NETWORK_ZONE, "sTimeZone,sTimeZoneFile,sTimeZoneFileDst", "'TokyoStandardTime-9','posix/Etc/GMT-9','posix/Asia/Tokyo'"));//(UTC+09:00) 大阪,札幌,东京

    col_para = "id INTEGER PRIMARY KEY," \
               "sProtocol TEXT NOT NULL," \
               "iPortNo INT DEFAULT 0";
    g_free(rkdb_create(TABLE_NETWORK_PORT, col_para));
    g_free(rkdb_insert(TABLE_NETWORK_PORT, "id,sProtocol,iPortNo", "0,'HTTP',80"));
    g_free(rkdb_insert(TABLE_NETWORK_PORT, "id,sProtocol,iPortNo", "1,'HTTPS',443"));
    g_free(rkdb_insert(TABLE_NETWORK_PORT, "id,sProtocol,iPortNo", "2,'DEV_MANAGE',8080"));
    g_free(rkdb_insert(TABLE_NETWORK_PORT, "id,sProtocol,iPortNo", "3,'RTSP',554"));
    g_free(rkdb_insert(TABLE_NETWORK_PORT, "id,sProtocol,iPortNo", "4,'RTMP',1935"));
    
        return;
}
