target_sources(client
    PRIVATE
    CommandLine.h
    CommandLine.cpp
    KeyHit.h
    KeyHit.cpp
    cmd_iface.cpp
)

target_sources(sender
    PRIVATE
    genericSender.cpp
)

target_sources(sender2
    PRIVATE
    genericSender2.cpp
)
