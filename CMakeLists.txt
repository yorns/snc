cmake_minimum_required(VERSION 3.1.0)

project(snc VERSION 1.0 LANGUAGES CXX)

set(BROCKER_SRC src/broker.cpp)

set(CLIENTLIB_SRC src/client.cpp)
set(CLIENTLIB_INC src/client.h)

# set(BOOST_ROOT /opt/boost/)
# add boost
find_package(Boost 1.53.0 REQUIRED COMPONENTS system thread)

add_executable(snc_brocker ${BROCKER_SRC})
target_link_libraries(snc_brocker ${Boost_LIBRARIES} pthread)
target_include_directories(snc_brocker PRIVATE ${Boost_INCLUDE_DIRS})
set_target_properties(snc_brocker PROPERTIES
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
        CXX_EXTENSIONS NO
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




