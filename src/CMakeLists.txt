target_sources(snc_broker
PRIVATE
  broker.cpp
  commandNames.h
  commandNames.cpp
)

target_sources(snc_client
PRIVATE
    client.cpp
    commandNames.h
    commandNames.cpp
)
