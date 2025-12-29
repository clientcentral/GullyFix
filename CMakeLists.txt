#//
#// THIS TOOL CODED BY TECNO (YIA) ---
#//

cmake_minimum_required(VERSION 3.20)
project(GullyLauncher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WIN32_EXECUTABLE TRUE)


add_executable(GullyLauncher
        main.cpp
        resource.rc
        Functions/Actions.h
)

target_include_directories(GullyLauncher PRIVATE
        ${CMAKE_SOURCE_DIR}/webview2/include
)

target_link_libraries(GullyLauncher PRIVATE
        user32
        ole32
        shell32
        advapi32
        windowsapp
        ${CMAKE_SOURCE_DIR}/webview2/lib/WebView2LoaderStatic.lib
)
# administrator prms fix ~ crazy
if(MSVC)
    set_target_properties(GullyLauncher PROPERTIES
            LINK_FLAGS "/MANIFEST /MANIFESTUAC:\"level='asInvoker' uiAccess='false'\" /INCREMENTAL:NO"
    )
endif()