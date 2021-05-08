cmake_minimum_required(VERSION 3.15)


project(snc VERSION 1.0 LANGUAGES CXX)

include(GNUInstallDirs)

# in case we have a valid installation under /opt/boost, use that one (to prefent pre-installed one
# e.g. ubuntu useses strange boost installation)
if(EXISTS /opt/boost/include/boost/function.hpp) # <- "specified" place should be set via cmake parameter -DBOOST_ROOT=...
        set(BOOST_ROOT /opt/boost)
endif()
if (BOOST_DIR) # Not sure if we need this. It can be set via cmake parameter like -DBOOST_ROOT=....
  set(BOOST_ROOT ${BOOST_DIR})
endif (BOOST_DIR)

# add boost
find_package(Threads)
find_package(Boost 1.70.0 REQUIRED CONFIG)

add_executable(snc_broker)
add_library(snc_client)

add_subdirectory(src)
add_subdirectory(system)

target_link_libraries(snc_broker PUBLIC Boost::headers Threads::Threads)
#EXTRA_OECMAKE = "-DWITH_SYSTEMD=1"
if(WITH_SYSTEMD)
  message(STATUS "compiling with systemd")
  target_link_libraries(snc_broker PUBLIC ${systemd_lib}) # <- Where is this set?
  target_compile_definitions(snc_broker PUBLIC WITH_SYSTEMD)
endif()
target_compile_features(snc_broker PUBLIC cxx_std_14)
#target

target_link_libraries(snc_client PUBLIC Boost::headers Threads::Threads)
target_include_directories(snc_client
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>)
target_compile_features(snc_client PUBLIC cxx_std_14)
install(FILES src/config.h src/client.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/snc)
add_library(snc::snc_client ALIAS snc_client)


add_executable(client)
add_executable(sender)
add_executable(sender2)

add_subdirectory(commandline)

target_link_libraries(client PUBLIC snc::snc_client)
target_link_libraries(sender PUBLIC snc::snc_client)
target_link_libraries(sender2 PUBLIC snc::snc_client)


# cmake -DCMAKE_INSTALL_PREFIX=$OECORE_TARGET_SYSROOT/usr/ ..
install(TARGETS client snc_client snc_broker sender sender2
        EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/snc
       )

set(CONFIG_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake")

include(CMakePackageConfigHelpers)

# Make package findable
configure_package_config_file(cmake/find-config.cmake.in ${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CONFIG_INSTALL_CMAKE_DIR}
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  NO_SET_AND_CHECK_MACRO
)

# Enable version checks in find_package
write_basic_package_version_file(${PROJECT_NAME}-config-version.cmake COMPATIBILITY SameMajorVersion)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
  DESTINATION ${CONFIG_INSTALL_CMAKE_DIR}
)
install(EXPORT ${PROJECT_NAME}-targets
  NAMESPACE snc::
  DESTINATION ${CONFIG_INSTALL_CMAKE_DIR}
)
