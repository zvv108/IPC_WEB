cmake_minimum_required(VERSION 3.8)

project(ispserver)

include(FindPkgConfig)
message("=================================")
message("$PROJECT_SOURCE_DIR")

include_directories(
	${PROJECT_SOURCE_DIR}/./
        ${PROJECT_SOURCE_DIR}/../interface/
        ${PROJECT_SOURCE_DIR}/../common/
        ${CMAKE_SYSROOT}/usr/include/rkaiq/uAPI/
        ${CMAKE_SYSROOT}/usr/include/rkaiq/algos
        ${CMAKE_SYSROOT}/usr/include/rkaiq/common
        ${CMAKE_SYSROOT}/usr/include/rkaiq/xcore
        ${CMAKE_SYSROOT}/usr/include/rkaiq/iq_parser
        ${PROJECT_SOURCE_DIR}/../utils/
        ${PROJECT_SOURCE_DIR}/../../CallFunIpc
if(DEFINED CONFIG_DBUS)
        ${PROJECT_SOURCE_DIR}/../../CallFunIpc

endif()
	)


aux_source_directory(./ SRCS)
aux_source_directory(./impl SRCS)

add_executable(${PROJECT_NAME}
	${SRCS})

if(DEFINED CONFIG_OEM)
   add_definitions(-DCONFIG_OEM=${CONFIG_OEM})
endif()

if(DEFINED CONFIG_DBUS)
   pkg_check_modules (GLIB REQUIRED IMPORTED_TARGET glib-2.0)
   pkg_check_modules (DBUS REQUIRED IMPORTED_TARGET dbus-1)
   add_definitions(-DCONFIG_DBUS=${CONFIG_DBUS})
endif()

if(DEFINED CONFIG_DBSERVER)
   pkg_check_modules (JSON-C REQUIRED IMPORTED_TARGET json-c)
   add_definitions(-DCONFIG_DBSERVER=${CONFIG_DBSERVER})
endif()

option(ENABLE_MINILOGGER  "enbale minilogger" ON)
if (${ENABLE_MINILOGGER})
    find_package(MiniLogger REQUIRED)
    add_definitions(-DENABLE_MINILOGGER)
    target_link_libraries(${PROJECT_NAME} MiniLogger::MiniLogger)
endif()

option(ENABLE_MEDIASERVER  "enbale mediaserver" ON)
if (${ENABLE_MEDIASERVER})
    add_definitions(-DENABLE_MEDIASERVER)
endif()

#find_package(RkAiq REQUIRED)
#include_directories(${RKAIQ_INCLUDE_DIRS})
#add_definitions(-DRKAIQ)


link_directories(
	${PROJECT_SOURCE_DIR}/./lib
	${PROJECT_SOURCE_DIR}/../libs
	)


target_link_libraries(${PROJECT_NAME}
        pthread
        rkaiq
        )
#if(CONFIG_DBUS)
   target_link_libraries(${PROJECT_NAME} gdbus CallFunIpc) #PkgConfig::GLIB PkgConfig::DBUS 
#endif()
if(CONFIG_DBSERVER)
   target_link_libraries(${PROJECT_NAME} IPCProtocol PkgConfig::JSON-C)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} dl)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin)
install(FILES dbusconfig/ispserver.conf
	DESTINATION share/dbus-1/system.d)
