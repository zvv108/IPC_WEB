// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef __NETSERVER_PROXY_H
#define __NETSERVER_PROXY_H

#include <dbus-c++/dbus.h>
#include <cassert>

class netserver_proxy
    : public ::DBus::InterfaceProxy
{
public:

    netserver_proxy(const char *interface)
        : ::DBus::InterfaceProxy(interface) {}

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    void ScanWifi(void)
    {
        ::DBus::CallMessage call;

        call.member("ScanWifi");
        ::DBus::Message ret = invoke_method (call);
    }

    std::string GetService(const std::string& param)
    {
        std::string argout;

        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << param;
        call.member("GetService");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> argout;

        return argout;
    }

    std::string GetConfig(const std::string& param)
    {
        std::string argout;

        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << param;
        call.member("GetConfig");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> argout;

        return argout;
    }

    std::string GetNetworkIP(const std::string& param)
    {
        std::string argout;

        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << param;
        call.member("GetNetworkIP");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        ri >> argout;

        return argout;
    }
};

class DBusNetServer : public netserver_proxy,
    public DBus::IntrospectableProxy,
    public DBus::ObjectProxy
{
public:
    DBusNetServer(DBus::Connection &connection, const char *adaptor_path,
                  const char *adaptor_name, const char *interface)
    : DBus::ObjectProxy( connection, adaptor_path, adaptor_name), netserver_proxy::netserver_proxy(interface) {}
    ~DBusNetServer() {}
};
#endif //__dbusxx__netserver_proxy_h__PROXY_MARSHAL_H
