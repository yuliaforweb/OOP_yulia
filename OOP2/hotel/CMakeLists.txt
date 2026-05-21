cmake_minimum_required(VERSION 3.20)
project(hotel CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Автоматичне викачування та збірка GLFW з гітхабу
include(FetchContent)
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
        GIT_SHALLOW    TRUE
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# ====================================================================
# КРОСПЛАТФОРМНИЙ РОЗУМНИЙ ПОШУК POSTGRESQL
# ====================================================================
if(WIN32)
    find_package(PostgreSQL QUIET)
    if(NOT PostgreSQL_FOUND)
        message(STATUS "Автоматичний пошук PostgreSQL не спрацював. Включаємо ручний режим для Windows...")
        set(PostgreSQL_INCLUDE_DIRS "C:/Program Files/PostgreSQL/18/include")
        set(PostgreSQL_LIBRARIES "C:/Program Files/PostgreSQL/18/lib/libpq.lib")
        set(PostgreSQL_FOUND TRUE)
    endif()
else()
    find_package(PostgreSQL REQUIRED)
endif()

# Шляхи до заголовків бібліотек
include_directories(${PostgreSQL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/imgui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

# ====================================================================
# РОЗДІЛЕННЯ СИСТЕМНИХ БІБЛІОТЕК ДЛЯ WINDOWS ТА LINUX
# ====================================================================
if(WIN32)
    set(SYS_NET_LIBS wsock32 ws2_32)
    set(SYS_GFX_LIBS opengl32)
else()
    set(SYS_NET_LIBS "")
    find_package(OpenGL REQUIRED)
    set(SYS_GFX_LIBS ${OpenGL_LIBRARIES})
endif()

# Спільні сирцеві файли готелю
set(CORE_SOURCES
        hotel.cpp
        hotel.h
        logger.cpp
        logger.h
        auth_manager.cpp
        auth_manager.h
)

# Складові файли для графічного клієнта
set(CLIENT_SOURCES
        main_client.cpp
        network_client.cpp
        network_client.h
        ui_views.cpp
        ui_views.h
        ui_shared.h
        ui_tabs_ops.cpp
        ui_tabs_admin.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
)

# Складові файли для серверної частини (БЕЗ застарілого imgui_listener)
set(SERVER_SOURCES
        main_server.cpp
        http_handler.cpp
        http_handler.h
        db_initializer.cpp
        db_initializer.h
        web_page.h
)

# Опис збірки сервера
add_executable(hotel_server ${SERVER_SOURCES} ${CORE_SOURCES})
target_link_libraries(hotel_server ${PostgreSQL_LIBRARIES} ${SYS_NET_LIBS})

# Опис збірки клієнта
add_executable(hotel_client ${CLIENT_SOURCES} ${CORE_SOURCES})
target_link_libraries(hotel_client glfw ${PostgreSQL_LIBRARIES} ${SYS_GFX_LIBS} ${SYS_NET_LIBS})

# Автоматичне копіювання dll на Windows
if(WIN32)
    file(GLOB PROJECT_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/bin/*.dll")
    if(PROJECT_DLLS)
        file(COPY ${PROJECT_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endif()

# Юніт-тести
add_executable(hotel_tests
        tests.cpp
        ui_views.cpp
        ui_tabs_ops.cpp
        ui_tabs_admin.cpp
        network_client.cpp
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_impl_glfw.cpp
        imgui/imgui_impl_opengl3.cpp
        ${CORE_SOURCES}
)

target_include_directories(hotel_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/external
        ${PostgreSQL_INCLUDE_DIRS}
)

target_link_libraries(hotel_tests PRIVATE
        ${PostgreSQL_LIBRARIES}
        glfw
        ${SYS_GFX_LIBS}
        ${SYS_NET_LIBS}
)