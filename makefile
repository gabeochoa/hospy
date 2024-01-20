

RAYLIB_FLAGS := `pkg-config --cflags raylib` 
RAYLIB_LIB := `pkg-config --libs raylib` 

RELEASE_FLAGS = -std=c++2a $(RAYLIB_FLAGS) 

FLAGS = -std=c++2a -Wall -Wextra -Wpedantic -Wuninitialized -Wshadow \
		-Wmost -Wconversion -g $(RAYLIB_FLAGS) -DTRACY_ENABLE 
# LEAKFLAGS = -fsanitize=address
NOFLAGS = -Wno-deprecated-volatile -Wno-missing-field-initializers \
		  -Wno-c99-extensions -Wno-unused-function -Wno-sign-conversion \
		  -Wno-implicit-int-float-conversion
INCLUDES = -Ivendor/ 
LIBS = -L. -Lvendor/ $(RAYLIB_LIB)

SRC_FILES := $(wildcard src/*.cpp src/**/*.cpp src/engine/**/*.cpp)
H_FILES := $(wildcard src/**/*.h src/engine/**/*.h) 
OBJ_DIR := ./output
OBJ_FILES := $(SRC_FILES:%.cpp=$(OBJ_DIR)/%.o)

OUTPUT_EXE := pharmasea.exe

# CXX := g++
CXX := clang++
# CXX := include-what-you-use

OUTPUT_LOG = $(OBJ_DIR)/build.log
GAME_LOG = $(OBJ_DIR)/game.log

.PHONY: all clean


# For tracing you have to run the game, and then connect from Tracy-release


all: $(OUTPUT_EXE)
	./$(OUTPUT_EXE) 2>&1 $(GAME_LOG)

$(OUTPUT_EXE): $(H_FILES) $(OBJ_FILES) 
	$(CXX) $(FLAGS) $(LEAKFLAGS) $(NOFLAGS) $(INCLUDES) $(LIBS) $(OBJ_FILES) -o $(OUTPUT_EXE) 

release: FLAGS=$(RELEASE_FLAGS)
release: NOFLAGS=
release: clean all
	rm -rf release
	mkdir release
	cp $(OUTPUT_EXE) release/
	cp README.md release/ 
	cp -r resources release/
	cp -r vendor release/


$(OBJ_DIR)/%.o: %.cpp makefile
	$(CXX) $(FLAGS) $(NOFLAGS) $(INCLUDES) -c $< -o $@ -MMD -MF $(@:.o=.d) 

%.d: %.cpp
	$(MAKEDEPEND)

clean:
	rm -r $(OBJ_DIR)
	mkdir -p $(OBJ_DIR)/src/

profile: 
	xctrace record --output . --template "Time Profiler" --time-limit 10s --attach `ps -ef | grep "\./$(OUTPUT_EXE)" | grep -v "grep"  | cut -d' ' -f4`

count: 
	git ls-files | grep "src" | grep -v "ui_color.h" | grep -v "vendor" | grep -v "resources" | grep -v "color.h" | xargs wc -l | sort -rn

countall: 
	git ls-files | xargs wc -l | sort -rn

todo: 
	{ git grep -niE '(FIXME|TODO)' src/; cat todo.md | grep -e "- \[\s";} 

todoadded: 
	git log -n 10 --pretty=format:%H | xargs -I {} git diff {} | grep -E "^\+.*TODO"

# for shuf you might need to 
# brew install coreutils
todor:
	{ git grep -niE '(FIXME|TODO)' src/; cat todo.md | grep -e "- \[\s";} | shuf -n1

cppcheck: 
	cppcheck src/ --enable=all --std=c++20 --language=c++ --suppress=noConstructor --suppress=noExplicitConstructor --suppress=useStlAlgorithm --suppress=unusedStructMember --suppress=useInitializationList --suppress=duplicateCondition --suppress=nullPointerRedundantCheck --suppress=cstyleCast
 
gendocs:
	doxygen doxyconfig
	git add .
	git commit -m "update docs" 

prof: 
	rm -rf recording.trace/
	xctrace record --template 'Game Performance' --output 'recording.trace' --launch $(OUTPUT_EXE)

leak: 
	rm -rf recording.trace/
	codesign -s - -f --verbose --entitlements ent_pharmasea.plist $(OUTPUT_EXE)
	xctrace record --template 'Leaks' --output 'recording.trace' --launch $(OUTPUT_EXE)

translate:
	python3 scripts/extract_strings.py
	msgfmt -o resources/translations/en_us.mo resources/translations/en_us.po
	msgfmt -o resources/translations/en_rev.mo resources/translations/en_rev.po

findstr:
	grep -r "\"" src/ | grep -v "preload"  | grep -v "game.cpp" | grep -v "src//strings.h" | grep -v "include" | grep -v "src//test" | grep -v "src//engine" | grep -v "src//dataclass" | grep -v "log" | grep -v "TODO" | grep -v "VALIDATE" 

-include $(OBJ_FILES:.o=.d)
