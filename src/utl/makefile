MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR := $(patsubst %/,%,$(dir $(MKFILE_PATH)))
PROJECT_ROOT := $(shell git rev-parse --show-toplevel)
MODULE_ROOT := $(MKFILE_DIR)
PRIVATE_DIR := $(MODULE_ROOT)/private
PUBLIC_DIR := $(MODULE_ROOT)/public

BIN = main
OUTPUT_DIR := $(MODULE_ROOT)/build
INTERMEDIATE_DIR := $(OUTPUT_DIR)/obj
MODULE_SRCS := $(shell find $(PRIVATE_DIR) $(PUBLIC_DIR) -name '*.cpp')
MODULE_INCLUDES := $(shell find $(PRIVATE_DIR) $(PUBLIC_DIR) -name '*.h')

.PHONY = all clean print preprocess compile
CXX := c++
CXX_FLAGS := -std=c++17 -fPIC -I$(PUBLIC_DIR) -I$(PRIVATE_DIR) -DUTL_BUILD_TESTS
LINKER_FLAGS := -lm
OBJECTS := $(addsuffix .o, $(basename $(MODULE_SRCS:$(MODULE_ROOT)/%=%)))
OBJECT_PATHS := $(addprefix $(INTERMEDIATE_DIR)/,$(OBJECTS))
DEPENDENCIES := $(OBJECTS:.o=.d)
PREPROCESSED := $(OBJECTS:.o=.prep.cpp)

all: $(BIN)

$(BIN): $(OBJECT_PATHS)
	@echo "Linking $(OBJECTS)"
	@$(CXX) $(LINKER_FLAGS) $(OBJECT_PATHS) -o $(OUTPUT_DIR)/$(BIN)

compile: $(OBJECTS)
	@

$(OBJECTS):%.o: $(MODULE_ROOT)/%.cpp
	@mkdir -p '$(INTERMEDIATE_DIR)/$(@D)'
	@echo "Creating object" $@
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(INTERMEDIATE_DIR)/$(patsubst %.o,%.d,$@)' -MT '$(INTERMEDIATE_DIR)/$@' -c $< -o '$(INTERMEDIATE_DIR)/$@'

-include $(INTERMEDIATE_DIR)/$(DEPENDENCIES)

preprocess: $(PREPROCESSED)
	@

$(PREPROCESSED):%.prep.cpp: $(MODULE_ROOT)/%.cpp
	@mkdir -p '$(INTERMEDIATE_DIR)/$(@D)'
	@echo "Running preprocessor" $@
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(INTERMEDIATE_DIR)/$(patsubst %.prep.cpp,%.d,$@)' -MT '$(INTERMEDIATE_DIR)/$@' -E $< -o '$(INTERMEDIATE_DIR)/$@'

clean:
	@echo "Cleaning up..."
	@rm -rvf $(INTERMEDIATE_DIR)/**/*.o $(INTERMEDIATE_DIR)/**/*.d $(INTERMEDIATE_DIR)/**/*.prep.cpp $(OUTPUT_DIR)/$(BIN)

print:
	@echo "Bin: $(BIN)\n"
	@echo "Makefile Directory: $(MKFILE_DIR)\n"
	@echo "Sources: $(MODULE_SRCS)\n"
	@echo "Objects: $(OBJECTS)\n"
	@echo "Preprocessed: $(PREPROCESSED)\n"
	@echo "Includes: $(MODULE_INCLUDES)\n"
	@echo "Dependencies: $(DEPENDENCIES)\n"
	@echo "CXX Flags: $(CXX_FLAGS)\n"
	@echo "Linker Flags: $(LINKER_FLAGS)\n"
	@echo "Output Directory: $(OUTPUT_DIR)\n"
	@echo "Intermediate Directory: $(INTERMEDIATE_DIR)\n"
