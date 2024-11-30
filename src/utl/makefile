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

.PHONY = clean print preprocess compile tests
CXX := c++
CXX_FLAGS := -std=c++14 -fPIC -O1 -I$(PUBLIC_DIR) -I$(PRIVATE_DIR) -DUTL_BUILD_TESTS -DUTL_BUILDING_LIBRARY=1 -Wall -Wpedantic -Wno-gnu-zero-variadic-macro-arguments
LINKER_FLAGS := -lm
OBJECTS := $(addsuffix .o, $(MODULE_SRCS:$(MODULE_ROOT)/%=%))
OBJECT_PATHS := $(addprefix $(INTERMEDIATE_DIR)/,$(OBJECTS))
DEPENDENCIES := $(OBJECTS:.o=.d)
PREPROCESSED := $(OBJECTS:.o=.i)
TEST_EXE_OBJECTS := $(filter %.pass.cpp.o,$(OBJECTS))
TEST_EXES := $(patsubst %.pass.cpp.o,%,$(TEST_EXE_OBJECTS))
TEST_EXE_PATHS := $(addprefix $(OUTPUT_DIR)/,$(TEST_EXES))

compile: $(OBJECTS)
	@

tests: $(TEST_EXES)
	@

$(TEST_EXES):%: $(OUTPUT_DIR)/%
	@echo "Running test" $@
	@$< && echo $@ "succeeded" || echo $@ "failed"

$(OUTPUT_DIR)/%: $(INTERMEDIATE_DIR)/%.pass.cpp.o $(MKFILE_PATH)
	@mkdir -p '$(@D)'
	@echo "Building test" $(patsubst $(OUTPUT_DIR)/%,%,$@)
	@$(CXX) $(LINKER_FLAGS) $< -o $@

$(OBJECTS):%.cpp.o: $(INTERMEDIATE_DIR)/%.cpp.o
	@

$(INTERMEDIATE_DIR)/%.cpp.o: $(MODULE_ROOT)/%.cpp $(MKFILE_PATH)
	@mkdir -p '$(@D)'
	@echo "Creating object" $(patsubst $(INTERMEDIATE_DIR)/%,%,$@)
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(@:.o=.d)' -MT '$@' -c $< -o '$@'

-include $(INTERMEDIATE_DIR)/$(DEPENDENCIES)

preprocess: $(INTERMEDIATE_DIR)/%.cpp.i
	@

$(INTERMEDIATE_DIR)/%.cpp.i: $(MODULE_ROOT)/%.cpp
	@mkdir -p '$(@D)'
	@echo "Running preprocessor" $@
	@$(CXX) $(CXX_FLAGS) -MMD -MP -MF '$(@:.i=.d)' -MT '$@' -E $< -o '$@'

clean:
	@echo "Cleaning up..."
	@rm -rvf $(INTERMEDIATE_DIR)/**/*.i $(INTERMEDIATE_DIR)/**/*.o $(INTERMEDIATE_DIR)/**/*.d $(INTERMEDIATE_DIR)/**/*.prep.cpp $(OUTPUT_DIR)/

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
	@echo "Test Objects: $(TEST_EXE_OBJECTS)\n"
	@echo "Tests: $(TEST_EXES)\n"

