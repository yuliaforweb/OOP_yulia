# ==============================================================================
# НАЗВА ПРОЄКТУ: Інформаційна система Готель
# Опис: Естетично оформлений та структурований макіфайл для MinGW
# ==============================================================================

# Налаштування компілятора та прапорців попереджень
CXX            = g++
CXXFLAGS       = -std=gnu++20 -g -Wall -I"C:/Program Files/PostgreSQL/18/include" -I./imgui -I. -Iexternal

# Прапорці лінкувальника для бази даних та графіки
LDFLAGS_SERVER = -L"C:/Program Files/PostgreSQL/18/lib" -lpq -lwsock32 -lws2_32
LDFLAGS_CLIENT = -L"C:/Program Files/PostgreSQL/18/lib" -lpq -lglfw3 -lopengl32 -lwsock32 -lws2_32

# Списки об єктних файлів компонентів системи
CORE_OBJS      = hotel.o logger.o auth_manager.o
SERVER_OBJS    = main_server.o http_handler.o imgui_listener.o db_initializer.o
CLIENT_OBJS    = main_client.o network_client.o ui_views.o ui_tabs_ops.o ui_tabs_admin.o \
                 imgui/imgui.o imgui/imgui_draw.o imgui/imgui_tables.o imgui/imgui_widgets.o \
                 imgui/imgui_impl_glfw.o imgui/imgui_impl_opengl3.o

# ------------------------------------------------------------------------------
# ГОЛОВНІ МЕТИ ЗБІРКИ ПРОЄКТУ
# ------------------------------------------------------------------------------

all: copy_dlls hotel_server.exe hotel_client.exe

# Перевірка наявності та копіювання бібліотек dll
copy_dlls:
	@if not exist hotel_server.exe (xcopy /y /d "bin\\*.dll" ".\\")

# Генерація виконуваного файлу сервера готелю
hotel_server.exe: $(SERVER_OBJS) $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS_SERVER)

# Генерація виконуваного файлу графічного клієнта готелю
hotel_client.exe: $(CLIENT_OBJS) $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS_CLIENT)

# ------------------------------------------------------------------------------
# ШАБЛОННІ ПРАВИЛА ДЛЯ ОБ ЄКТНИХ ФАЙЛІВ
# ------------------------------------------------------------------------------

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Повне очищення робочої директорії від сміття компіляції
clean:
	del /q *.o *.exe imgui\*.o 2>nul