# This file is in the public domain.
add_library(libjames
    Exceptions.cpp
    HexBinary.cpp
    XercesString.cpp
    XMLObject.cpp
)

set_target_properties(libjames PROPERTIES OUTPUT_NAME james)
