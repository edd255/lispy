include .config/make/config.mk

#==== RULES ====================================================================
define compile_template
$(BUILD_DIR)/%.$(1).o: %.c
	$(Q)echo "====> CC $$@"
	$(Q)mkdir -p $$(dir $$@)
	$(Q)$(CC) $($(2)) $(CFLAGS) -c $$< -o $$@
endef

$(eval $(call compile_template,opt,OPTIMIZED))
$(eval $(call compile_template,dbg,DEBUGGING))
$(eval $(call compile_template,san,SANITIZED))
$(eval $(call compile_template,prof,PROFILING))

#---- OPTIMIZED ----------------------------------------------------------------

$(BIN)_optimized: $(OPT_OBJS)
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(OPTIMIZED) $+ -s -o $@ $(LDFLAGS)

optimized: $(BIN)_optimized

#---- DEBUGGING ----------------------------------------------------------------

$(BIN)_debugging: $(DBG_OBJS) 
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(DEBUGGING) $+ -o $@ $(LDFLAGS)

debugging: $(BIN)_debugging

#---- SANITIZED ----------------------------------------------------------------

$(BIN)_sanitized: $(SAN_OBJS) 
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(SANITIZED) $+ -o $@ $(LDFLAGS)

sanitized: $(BIN)_sanitized

#---- PROFILING ----------------------------------------------------------------

$(BIN)_profiling: $(PROF_OBJS) 
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(PROFILING) $+ -o $@ $(LDFLAGS) -pg

profiling: $(BIN)_profiling

#---- CLEANING -----------------------------------------------------------------

clean:
	$(Q)echo "====> Cleaning..."
	$(Q)$(RM) --recursive --force $(BUILD_DIR)/$(SRC_DIR)
	$(Q)$(RM) cscope.files cscope.in.out cscope.out cscope.po.out tags

#---- DOCUMENTATION ------------------------------------------------------------

docs:
	$(Q)echo "====> Creating the documentation..."
	$(Q)doxygen

#---- STYLE --------------------------------------------------------------------

style:
	$(Q)echo "====> Formatting..."
	$(Q)find $(SRC_DIR) -iname *.h -o -iname *.c | xargs clang-format -i $(CLANG_FMT_CFG)

#---- ANALYSIS -----------------------------------------------------------------

analyze:
	$(Q)$(MKDIR) logs
	$(Q)echo "====> Running scan-build..."
	$(Q)scan-build make all
	$(Q)echo "====> Running cppcheck..."
	$(Q)cppcheck src/ $(CPPCHECK) 2> logs/cppcheck.log
	$(Q)echo "====> Running clang-tidy..."
	$(Q)compiledb make all
	$(Q)clang-tidy.py $(PROJ_SRCS) $(CLANG_TIDY_CFG) || true
	$(Q)yq 'del(.Diagnostics[] | select(.DiagnosticMessage.FilePath | test("deps")))' tidy.log > logs/tidy.log
	$(Q)$(RM) tidy.log
	$(Q)scan-build $(MAKE)

memcheck: debugging
	$(Q)echo "====> Running memcheck..."
	$(Q)valgrind ${VALGRIND} $(BIN)_debugging

callgrind: debugging
	$(Q)echo "====> Running callgrind..."
	$(Q)valgrind ${CALLGRIND} $(BIN)_debugging

massif: debugging
	$(Q)echo "====> Running massif..."
	$(Q)valgrind ${massif} $(BIN)_debugging

dhat: debugging
	$(Q)echo "====> Running massif..."
	$(Q)valgrind ${dhat} $(BIN)_debugging

tags:
	$(Q)echo "====> Generating ctags..."
	$(Q)ctags -R --languages=C --exclude=build --exclude=deps .
	$(Q)echo "====> Generating cscope database..."
	$(Q)find . -name '*.c' -o -name '*.h' > cscope.files
	$(Q)cscope -b -q -k

#---- TESTING ------------------------------------------------------------------

tests: sanitized
	$(Q)for test in $(TESTS); do \
		echo "====> Running $$test..."; \
		$(BIN)_sanitized --filename=$$test; \
	done

#---- INSTALLING ---------------------------------------------------------------

install: optimized
	$(Q)echo "====> Installing the binary..."
	$(Q)cp $(BIN)_optimized $(PREFIX)/bin/$(NAME)
	$(Q)echo "====> Installing the library..."
	$(Q)$(MKDIR) $(PREFIX)/lib/lispy
	$(Q)cp $(LIB)/stdlib.lspy $(PREFIX)/lib/lispy/
	$(Q)echo "====> Finished!"
	
uninstall:
	$(Q)echo "====> Removing the binary..."
	$(Q)$(RM) $(PREFIX)/bin/$(NAME)
	$(Q)echo "====> Removing the library..."
	$(Q)$(RM) $(PREFIX)/lib/lispy/stdlib.lspy
	$(Q)rmdir $(PREFIX)/lib/lispy/
	$(Q)echo "====> Finished!"

#==== EPILOGUE =================================================================

all: style optimized debugging sanitized profiling tests docs tags
	$(Q)echo "====> Finished!"

# Include the .d makefiles
-include $(DEPS)
.PHONY: all optimized debugging memcheck style install uninstall tests profiling docs
