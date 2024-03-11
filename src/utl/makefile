MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(dir $(MKFILE_PATH))
MODULE_DIR := $(MKFILE_DIR)

BIN = main
SRCS := $(wildcard $(MKFILE_DIR)**/*.cpp)
SRC_DIR := $(MKFILE_DIR)
INCLUDE_DIR := $(realpath $(MKFILE_DIR)../include/)/
OUTPUT_DIR := $(shell pwd)/Build/
INTERMEDIATE_DIR := $(OUTPUT_DIR)Intermediate/
INCLUDES := $(wildcard $(INCLUDE_DIR)**/*.h)

.PHONY = all clean print preprocess compile
CXX := c++
CXX_FLAGS := -std=c++20 -fPIC -I$(INCLUDE_DIR) -I$(SRC_DIR)
LINKER_FLAGS := -lm
OBJECTS := $(addsuffix .o, $(basename $(SRCS:$(MKFILE_DIR)%=%)))
OBJECT_PATHS := $(addprefix $(INTERMEDIATE_DIR),$(OBJECTS))
DEPENDENCIES := $(OBJECTS:.o=.d)
PREPROCESSED := $(OBJECTS:.o=.prep.cpp)

all: $(BIN)

$(BIN): $(OBJECT_PATHS)
	@echo "Linking $(OBJECTS)"
	@$(CXX) $(LINKER_FLAGS) $(OBJECT_PATHS) -o $(OUTPUT_DIR)$(BIN)

compile: $(OBJECTS)
	@

$(OBJECTS):%.o: $(MKFILE_DIR)%.cpp
	@mkdir -p $(@D)
	@echo "Creating object" $@
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF $(INTERMEDIATE_DIR)$(patsubst %.o,%.d,$@) -MT '$(INTERMEDIATE_DIR)$@' -c $< -o '$(INTERMEDIATE_DIR)$@'

-include $(INTERMEDIATE_DIR)$(DEPENDENCIES)

preprocess: $(PREPROCESSED)
	@

$(PREPROCESSED):%.prep.cpp: $(MKFILE_DIR)%.cpp
	@mkdir -p $(@D)
	@echo "Running preprocessor" $@
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF $(patsubst %.prep.cpp,%.d,$@) -MT '$(INTERMEDIATE_DIR)$@' -E $< -o '$(INTERMEDIATE_DIR)$@'

clean:
	@echo "Cleaning up..."
	@rm -rvf $(INTERMEDIATE_DIR)**/*.o $(INTERMEDIATE_DIR)**/*.d $(INTERMEDIATE_DIR)**/*.spec.cpp $(OUTPUT_DIR)$(BIN)

print:
	@echo "Bin: $(BIN)\n"
	@echo "Sources: $(SRCS)\n"
	@echo "Objects: $(OBJECTS)\n"
	@echo "Preprocessed: $(PREPROCESSED)\n"
	@echo "Includes: $(INCLUDES)\n"
	@echo "Dependencies: $(DEPENDENCIES)\n"
	@echo "CXX Flags: $(CXX_FLAGS)\n"
	@echo "Linker Flags: $(LINKER_FLAGS)\n"
	@echo "Output Directory: $(OUTPUT_DIR)\n"
	@echo "Intermediate Directory: $(INTERMEDIATE_DIR)\n"
