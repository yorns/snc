cmake_minimum_required(VERSION 3.1.0)

project(snc VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(BROKER_SRC src/broker.cpp)

set(CLIENTLIB_SRC src/client.cpp)
set(CLIENTLIB_INC src/client.h)

# in case we have a valid installation under /opt/boost, use that (ubuntu useses strange boost installation)
if(EXISTS /opt/boost/include/boost/function.hpp)
        set(BOOST_ROOT /opt/boost)
endif()

# add boost
find_package(Boost 1.53.0 REQUIRED COMPONENTS system thread)

add_executable(snc_broker ${BROKER_SRC})
target_link_libraries(snc_broker ${Boost_LIBRARIES} pthread)
target_include_directories(snc_broker PRIVATE ${Boost_INCLUDE_DIRS})
set_target_properties(snc_broker PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )
#target

add_library(snc_client ${CLIENTLIB_SRC})
target_link_libraries(snc_client ${Boost_LIBRARIES} pthread)
target_include_directories(snc_client PRIVATE ${Boost_INCLUDE_DIRS}
        PUBLIC ${CMAKE_SOURCE_DIR}/src)
set_target_properties(snc_client PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO )
        set_target_properties(snc_client
        PROPERTIES PUBLIC_HEADER src/client.h
        )

SET(TESTCLIENT_SRC commandline/CommandLine.cpp commandline/KeyHit.cpp commandline/cmd_iface.cpp)

add_executable(client ${TESTCLIENT_SRC})
target_link_libraries(client ${Boost_LIBRARIES} snc_client pthread)
target_include_directories(snc_client PRIVATE ${Boost_INCLUDE_DIRS})
set_target_properties(client PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
        )

# -DCMAKE_INSTALL_PREFIX=/usr
# cmake -DCMAKE_INSTALL_PREFIX=$OECORE_TARGET_SYSROOT/usr/ ..
install(TARGETS client snc_client snc_broker
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/snc
       )

