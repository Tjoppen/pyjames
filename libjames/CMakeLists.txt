add_library(libjames
    Exceptions.cpp
    HexBinary.cpp
    XercesString.cpp
    XMLDocument.cpp
    XMLObject.cpp
)

set_target_properties(libjames PROPERTIES OUTPUT_NAME james)
