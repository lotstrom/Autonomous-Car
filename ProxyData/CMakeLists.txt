PROJECT (2014-DIT-168-project-template-proxy-Arduino)

# Include directories from core.
INCLUDE_DIRECTORIES (${libopendavinci_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES (../msv-data/include)
INCLUDE_DIRECTORIES (include)

#Protobuf dependency
#SET_PROPERTY(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/apps/MainModule.cpp" APPEND PROPERTY OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/ProxyData.c")

# Recipe for building "proxy-Arduino".
FILE(GLOB_RECURSE proxy-Arduino-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
ADD_LIBRARY (proxy-Arduinolib STATIC ${proxy-Arduino-sources})
ADD_EXECUTABLE (proxy-Arduino "${CMAKE_CURRENT_SOURCE_DIR}/apps/MainModule.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/ProxyData.c")
#ADD_EXECUTABLE (STM32F4Test2 "${CMAKE_CURRENT_SOURCE_DIR}/apps/STM32F4Test2.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/ProxyData.c")
TARGET_LINK_LIBRARIES (proxy-Arduino proxy-Arduinolib ${MSVDATA_LIBS} ${OPENDAVINCI_LIBS} ${LIBS}) 
#TARGET_LINK_LIBRARIES (STM32F4Test2 proxy-Arduinolib ${MSVDATA_LIBS} ${OPENDAVINCI_LIBS} ${LIBS}) 

# Recipe for installing "proxy-STM32F4".
INSTALL(TARGETS proxy-Arduino RUNTIME DESTINATION bin/2014/DIT-168/project-template) 
#INSTALL(TARGETS STM32F4Test2 RUNTIME DESTINATION bin/2014/DIT-168/project-template) 