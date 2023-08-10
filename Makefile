include config.mk

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

#---- MEMCHECK -----------------------------------------------------------------

$(BIN)_sanitized: $(patsubst src/%.c, build/%.san.o, $(SRCS)) 
	$(Q)$(MKDIR) $(BIN_DIR)
	$(Q)echo -e "====> LD $@"
	$(Q)$(CC) $(MEMCHECK) $+ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.san.o: src/%.c
	$(Q)echo "====> CC $@"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(MEMCHECK) $(CFLAGS) -c $< -o $@

memcheck: $(BIN)_sanitized

#---- CLEANING -----------------------------------------------------------------

clean:
	$(Q)echo "====> Cleaning..."
	$(Q)$(RM) --recursive $(BUILD_DIR)

#---- STYLE --------------------------------------------------------------------

style:
	$(Q)echo "====> Formatting..."
	$(Q)find $(SRC_DIR) -iname *.h -o -iname *.c | xargs clang-format -i

#---- INSTALLING ---------------------------------------------------------------

install: release
	$(Q)echo "====> Installing the binary..."
	$(Q)cp $(BIN)_release $(PREFIX)/bin/$(NAME)
	$(Q)echo "====> Finished!"
	
uninstall:
	$(Q)echo "====> Removing the binary..."
	$(Q)$(RM) $(INSTALL_BIN_DIR)/$(NAME)
	$(Q)echo "====> Finished!"

#==== EPILOGUE =================================================================

all: style release debugging memcheck

# Include the .d makefiles
-include $(DEPS)
.PHONY: all release debugging memcheck style install uninstall
