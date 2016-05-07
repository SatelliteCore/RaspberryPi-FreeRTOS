
# extension .s is just assembled by as assembler
# extension .S is tradtionally assembled via cc and can see C macros

$(BUILD_DIR)%.o: $(BASE)%.s
	$(Q)$(PRETTY) --dbuild "AS" $(MODULE_NAME) $(notdir $@)
	@mkdir -p $(dir $@)
	$(Q)$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)%.o: $(BASE)%.S
	$(Q)$(PRETTY) --dbuild "AS" $(MODULE_NAME) $(notdir $@)
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) $< -o $@


