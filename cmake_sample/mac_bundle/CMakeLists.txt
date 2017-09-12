cmake_minimum_required(VERSION 3.7)

   # the Info.plist template to fill after cmake md Info.plist is ready on bundle dir
   set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
   project(Sax VERSION 0.4.0)
   # Project information.
   set(PRO_MAC_NAME "SaxMac")
   set(CURRENT_VERSION "0.4.0")
   set(PRO_UNIX_NAME "sax")
   # Project Bundle to info plist xml file information.
   set(PROJECT_COMPANY_NAME "Sax Company GMBH")
   string(TIMESTAMP PROJECT_COPYRIGHT "© 1999-%Y User Name" UTC)
   set(PROJECT_DESCRIPTION "A simple documentation to cmake")
   set(PROJECT_URL "https://www.qt.io/")
   # the property added to Info.plist
   set(MACOSX_BUNDLE_ICON_FILE "macicon.icns")

   # Set the OS X Bundle specific CMake variables which will be used to populate the plist for
   # the application bundle
   set(MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}")
   set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.qt.io")
   set(MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} Version ${CURRENT_VERSION}")
   set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
   set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${CURRENT_VERSION})
   set(MACOSX_BUNDLE_BUNDLE_VERSION ${CURRENT_VERSION})
   string(TIMESTAMP PROJECT_COPYRIGHT "Copyright © 1999-%Y User Name All Rights Reserved." UTC)
   set(MACOSX_BUNDLE_COPYRIGHT ${PROJECT_COPYRIGHT})
   set(MACOSX_BUNDLE_EXECUTABLE_NAME ${PRO_MAC_NAME})
   # And this part tells CMake where to find and install the file itself
   set(MAC_APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/resources/macicon.icns)
   set_source_files_properties(${MAC_APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")


   set(CMAKE_CXX_STANDARD 14)
   set(SOURCE_FILES main.cpp)
   find_package( Qt5Core REQUIRED )
   find_package( Qt5Widgets REQUIRED )
   find_package( Qt5Gui REQUIRED )
   #set(PROJECT_LINK_LIBS libxl.dylib)
   #link_directories(${CMAKE_SOURCE_DIR}/LibXL/lib)
   #include_directories(${CMAKE_SOURCE_DIR}/LibXL/include_cpp)


   #Mac Bundle (Built on Mac)
   add_executable(${PRO_MAC_NAME} MACOSX_BUNDLE main.cpp ${MAC_APP_ICON} )
   qt5_use_modules(${PRO_MAC_NAME} Core Widgets Gui )
   target_link_libraries(${PRO_MAC_NAME} Qt5::Widgets)
   #target_link_libraries(project_target_mac ${PROJECT_LINK_LIBS} )
   #set_target_properties(project_target_mac PROPERTIES INSTALL_RPATH "${CMAKE_SOURCE_DIR}/LibXL/lib")

   add_executable(${PRO_UNIX_NAME} ${SOURCE_FILES})
   qt5_use_modules(${PRO_UNIX_NAME} Core Widgets Gui )
   target_link_libraries(${PRO_UNIX_NAME} Qt5::Widgets)
   #target_link_libraries(project_target ${PROJECT_LINK_LIBS
   set(APP_MAC_BUNDLE ${PRO_MAC_NAME}.app )
   # shell script or:
   set(MACDEPLOYQT macdeployqt)
   install(CODE "execute_process(COMMAND ${MACDEPLOYQT} ${APP_MAC_BUNDLE} -dmg) ")





