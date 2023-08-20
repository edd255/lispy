include .config/make/config.mk

#==== RULES ====================================================================
#---- RELEASE ------------------------------------------------------------------

$(BIN)_release: $(patsubst src/%.c, build/%.opt.o, $(SRCS))
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(RELEASE) $+ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.opt.o: src/%.c
	$(Q)echo "====> CC $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(RELEASE) $(CFLAGS) -c $< -o $@

release: $(BIN)_release

#---- DEBUGGING ----------------------------------------------------------------

$(BIN)_debugging: $(patsubst src/%.c, build/%.dbg.o, $(SRCS)) 
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(DEBUGGING) $+ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.dbg.o: src/%.c
	$(Q)echo "====> CC $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(DEBUGGING) $(CFLAGS) -c $< -o $@

debugging: $(BIN)_debugging

#---- SANITIZED ----------------------------------------------------------------

$(BIN)_sanitized: $(patsubst src/%.c, build/%.san.o, $(SRCS)) 
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(SANITIZED) $+ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.san.o: src/%.c
	$(Q)echo "====> CC $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(SANITIZED) $(CFLAGS) -c $< -o $@

sanitized: $(BIN)_sanitized

#---- PROFILING ----------------------------------------------------------------

$(BIN)_profiling: $(patsubst src/%.c, build/%.prof.o, $(SRCS)) 
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(SANITIZED) $+ -o $@ $(LDFLAGS) -pg

$(BUILD_DIR)/%.prof.o: src/%.c
	$(Q)echo "====> CC $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(SANITIZED) $(CFLAGS) -c $< -o $@

profiling: $(BIN)_profiling

#---- CLEANING -----------------------------------------------------------------

clean:
	$(Q)echo "====> Cleaning..."
	$(Q)$(RM) --recursive $(BUILD_DIR) || true

#---- STYLE --------------------------------------------------------------------

style:
	$(Q)echo "====> Formatting..."
	$(Q)find $(SRC_DIR) -iname *.h -o -iname *.c | xargs clang-format -i $(CLANG_FMT_CONFIG)

#---- ANALYSIS -----------------------------------------------------------------

analyze:
	$(Q)echo "====> Running scan-build..."
	$(Q)scan-build make all
	$(Q)echo "====> Running cppcheck..."
	$(Q)cppcheck src/ $(CPPCHECK) 2> cppcheck.log
	$(Q)echo "====> Running clang-tidy..."
	$(Q)compiledb make all
	$(Q)clang-tidy.py $(PROJ_SRCS) $(CLANG_TIDY_CONFIG)

memcheck: debugging
	$(Q)echo "====> Running valgrind..."
	$(Q)valgrind ${VALGRIND_FLAGS} $(BIN)_debugging

#---- TESTING ------------------------------------------------------------------

tests: sanitized
	$(Q)for test in $(TESTS); do \
		echo "====> Running $$test..."; \
		$(BIN)_release --filename=$$test; \
	done

#---- INSTALLING ---------------------------------------------------------------

install: release
	$(Q)echo "====> Installing the binary..."
	$(Q)cp $(BIN)_release $(PREFIX)/bin/$(NAME)
	$(Q)echo "====> Installing the library..."
	$(Q)$(MKDIR) $(PREFIX)/lib/lispy
	$(Q)cp $(ASSETS)/stdlib/stdlib.lspy $(PREFIX)/lib/lispy/
	$(Q)echo "====> Finished!"
	
uninstall:
	$(Q)echo "====> Removing the binary..."
	$(Q)$(RM) $(PREFIX)/bin/$(NAME)
	$(Q)echo "====> Removing the library..."
	$(Q)$(RM) $(PREFIX)/lib/lispy/stdlib.lspy
	$(Q)rmdir $(PREFIX)/lib/lispy/
	$(Q)echo "====> Finished!"

#==== EPILOGUE =================================================================

all: style release debugging sanitized profiling tests
	$(Q)echo "====> Finished!"

# Include the .d makefiles
-include $(DEPS)
.PHONY: all release debugging memcheck style install uninstall tests profiling
