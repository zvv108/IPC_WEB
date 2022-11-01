
/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__dbus_ispserver_adaptor_h__ADAPTOR_MARSHAL_H
#define __dbusxx__dbus_ispserver_adaptor_h__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace rockchip {
namespace ispserver {

class server_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

    server_adaptor()
    : ::DBus::InterfaceAdaptor("rockchip.ispserver.server")
    {
        register_method(server_adaptor, GetDumpExposureInfo, _GetDumpExposureInfo_stub);
    }

    ::DBus::IntrospectedInterface *introspect() const 
    {
        static ::DBus::IntrospectedArgument GetDumpExposureInfo_args[] = 
        {
            { "result", "s", false },
            { 0, 0, 0 }
        };
        static ::DBus::IntrospectedMethod server_adaptor_methods[] = 
        {
            { "GetDumpExposureInfo", GetDumpExposureInfo_args },
            { 0, 0 }
        };
        static ::DBus::IntrospectedMethod server_adaptor_signals[] = 
        {
            { 0, 0 }
        };
        static ::DBus::IntrospectedProperty server_adaptor_properties[] = 
        {
            { 0, 0, 0, 0 }
        };
        static ::DBus::IntrospectedInterface server_adaptor_interface = 
        {
            "rockchip.ispserver.server",
            server_adaptor_methods,
            server_adaptor_signals,
            server_adaptor_properties
        };
        return &server_adaptor_interface;
    }

public:

    /* properties exposed by this interface, use
     * property() and property(value) to get and set a particular property
     */

public:

    /* methods exported by this interface,
     * you will have to implement them in your ObjectAdaptor
     */
    virtual std::string GetDumpExposureInfo() = 0;

public:

    /* signal emitters for this interface
     */

private:

    /* unmarshalers (to unpack the DBus message before calling the actual interface method)
     */
    ::DBus::Message _GetDumpExposureInfo_stub(const ::DBus::CallMessage &call)
    {
        ::DBus::MessageIter ri = call.reader();

        std::string argout1 = GetDumpExposureInfo();
        ::DBus::ReturnMessage reply(call);
        ::DBus::MessageIter wi = reply.writer();
        wi << argout1;
        return reply;
    }
};

} } 
#endif //__dbusxx__dbus_ispserver_adaptor_h__ADAPTOR_MARSHAL_H