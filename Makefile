##
## EPITECH PROJECT, 2025
## Makefile
## File description:
## Zappy
##

NAME_AI = zappy_ai
NAME_SERVER = zappy_server
NAME_GUI = zappy_gui

PYTHON = python3
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -Werror -I./server/include -O2
CXXFLAGS = -Wall -Wextra -Werror -I./server/include -std=c++17
LDFLAGS = -lm

SRCS_SERVER = server/src/core/server.c \
              server/src/core/config.c \
              server/src/core/config_parsing.c \
              server/src/core/config_validation.c \
              server/src/core/server_init.c \
              server/src/core/server_utils.c \
              server/src/core/server_client.c \
              server/src/core/server_handler.c \
              server/src/core/server_starvation.c \
              server/src/core/server_cleanup.c \
              server/src/network/socket.c \
              server/src/network/client.c \
              server/src/network/client_utils.c \
              server/src/network/client_init.c \
              server/src/network/buffer.c \
              server/src/network/buffer_utils.c \
              server/src/world/map_spawn.c \
              server/src/world/map.c \
              server/src/world/map_utils.c \
              server/src/game/player.c \
              server/src/game/player_death.c \
              server/src/game/player_elevation.c \
              server/src/game/player_vision.c \
              server/src/game/player_vision_utils.c \
              server/src/game/player_movement.c \
              server/src/game/team.c \
              server/src/game/team_utils.c \
              server/src/game/egg.c \
              server/src/engine/time.c \
              server/src/engine/timestamp.c \
              server/src/engine/timeout.c \
              server/src/commands/ai_commands.c \
              server/src/commands/ai_movement_commands.c \
              server/src/commands/ai_commands_execute.c \
              server/src/commands/ai_player_commands.c \
              server/src/commands/ai_player_commands2.c \
              server/src/commands/ai_player_commands3.c \
              server/src/commands/ai_resource_commands.c \
              server/src/commands/gui_commands.c \
              server/src/commands/gui_commands_utils.c \
              server/src/commands/gui_send.c \
              server/src/commands/gui_send2.c \
              server/src/commands/gui_send3.c \
              server/src/commands/gui_broadcast.c \
              server/src/commands/gui_broadcast2.c \
              server/src/commands/gui_death.c

OBJ_SERVER = $(SRCS_SERVER:server/%.c=obj/server/%.o)
DEPS_SERVER = $(OBJ_SERVER:.o=.d)

SRC_GUI = $(wildcard gui/*.cpp gui/src/*/*.cpp)
OBJ_GUI = $(SRC_GUI:gui/%.cpp=obj/gui/%.o)
DEPS_GUI = $(OBJ_GUI:.o=.d)

RAYLIB_AVAILABLE := $(shell pkg-config --exists raylib && echo yes || echo no)
ifeq ($(RAYLIB_AVAILABLE),yes)
    RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    CXXFLAGS += -DRAYLIB_AVAILABLE
endif

SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network \
            -lsfml-audio

SRCS_AI = $(wildcard new_ia.py)

all: zappy_ai zappy_server zappy_gui
	@echo "Build Successful !!"

zappy_ai:
	@$(PYTHON) -c "import py_compile; import os; \
	[py_compile.compile(os.path.join('ai', f)) for f in os.listdir('ai') \
	if f.endswith('.py')]"
	@echo '#!/bin/bash' > $(NAME_AI)
	@echo 'SCRIPT_DIR="$$(cd "$$(dirname "$${BASH_SOURCE[0]}")" && pwd)"' \
		>> $(NAME_AI)
	@echo 'cd "$$SCRIPT_DIR" && python3 -m ai.main "$$@"' >> $(NAME_AI)
	@chmod +x $(NAME_AI)

zappy_server: $(OBJ_SERVER)
	@$(CC) $(CFLAGS) -o $(NAME_SERVER) $(OBJ_SERVER) $(LDFLAGS)

zappy_gui: $(OBJ_GUI)
	@$(CXX) $(CXXFLAGS) -o $(NAME_GUI) $(OBJ_GUI) $(SFML_LIBS) \
	$(RAYLIB_LIBS)

obj/server/%.o: server/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

obj/gui/%.o: gui/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -MMD -Igui/include -c $< -o $@

clean:
	@rm -rf obj
	@find . -name "*.pyc" -delete
	@find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true

fclean: clean
	@rm -f $(NAME_SERVER) $(NAME_GUI) $(NAME_AI)
	@find server -name "*.o" -type f -delete

re: fclean all

.PHONY: all clean fclean re zappy_ai zappy_server zappy_gui

-include $(DEPS_SERVER)
-include $(DEPS_GUI)
