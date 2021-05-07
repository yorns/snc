target_sources(snc_broker
PRIVATE
  broker.cpp
  commandNames.h
  commandNames.cpp

PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/config.h>
  $<INSTALL_INTERFACE:include/snc/config.h>
)

target_sources(snc_client
PRIVATE
    client.cpp
    commandNames.h
    commandNames.cpp

PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/client.h>
$<INSTALL_INTERFACE:include/snc/client.h>
)
