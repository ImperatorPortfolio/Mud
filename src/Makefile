#This is the ZeroPoint Makefile.
#This should be usable on pretty much any Linux system.
#Refer to your 'man gcc' for explanations of these options
#To compile in FreeBSD or OpenBSD, use the gmake compiler.

#Change this to reflect whichever compiler you want to use.
CC      = g++
#CC      = clang++

# Automatically detect if we are running in a Cygwin environment
ifeq ($(findstring CYGWIN, $(shell uname -s)), CYGWIN)
    IS_CYGWIN = 1
endif

# Check if libdl is available on the system
HAS_DL := $(shell echo 'int main(){}' | $(CC) -x c - -ldl -o /dev/null 2>/dev/null && echo yes)

ifeq ($(HAS_DL),yes)
    NEED_DL = -ldl
else
    NEED_DL =
endif

#OS Detection. Used to check for MacOS below.
UNAME_S := $(shell uname -s)

# Set EXPORT_SYMBOLS only if not on Darwin (MacOS)
ifneq ($(UNAME_S), Darwin)
    EXPORT_SYMBOLS = -export-dynamic
endif

#GCC 4.8+ memory checking option. https://github.com/google/sanitizers Does not work in Cygwin.
#These should only be enabled for testing environments. Disable for a live environment.
ifndef IS_CYGWIN
   M_FLAGS = -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer
endif

# These protect against stack smashing and buffer overflows in production.
# We add -D_FORTIFY_SOURCE=2 to perform runtime checks on string/memory functions.
HARDENING_FLAGS = -fstack-protector-strong -D_FORTIFY_SOURCE=2

W_FLAGS = -std=c++20 -Wall -Wshadow -Wformat-security -Wpointer-arith -Wcast-align -Wredundant-decls

C_FLAGS = -g2 $(W_FLAGS) $(M_FLAGS) $(HARDENING_FLAGS) $(EXPORT_SYMBOLS)
L_FLAGS = $(M_FLAGS) $(HARDENING_FLAGS) -lz $(NEED_DL)

#D_FLAGS : For the DNS Slave process. No need in linking all the extra libs for this.
D_FLAGS = -g2 -O $(M_FLAGS) $(HARDENING_FLAGS)

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c area_builder_commands.c attribute_math.c boards.c bounty.c \
	  build.c clans.c color.c comm.c comments.c const.c db.c defense_rules.c dns.c elevator.c exercise.c fight.c geography.c geography_commands.c geography_ui_commands.c \
	  handler.c hashstr.c hotboot.c interp.c magic.c makeobjs.c mccp.c msdp_mapper.c msdp_telnet.c \
	  misc.c mssp.c mud_comm.c mud_prog.c natural_progress.c player.c reset.c room_metadata.c save.c sha256.c shops.c \
	  skills.c space.c special.c swskills.c tables.c track.c training.c training_guard.c traits.c update.c

O_FILES := $(patsubst %.c,o/%.o,$(C_FILES))

H_FILES = $(wildcard *.h)

#
# D20-P4 — Legacy Attribute Math Removal
#
ATTRIBUTE_MATH_DEFINES = \
	-Dstr_app=ability_str_app \
	-Dint_app=ability_int_app \
	-Dwis_app=ability_wis_app \
	-Ddex_app=ability_dex_app \
	-Dcon_app=ability_con_app \
	-Dcha_app=ability_cha_app \
	-Dlck_app=ability_lck_app \
	-Dfrc_app=ability_frc_app

ATTRIBUTE_MATH_OBJECTS := $(filter-out o/const.o o/attribute_math.o,$(O_FILES))
$(ATTRIBUTE_MATH_OBJECTS): C_FLAGS += $(ATTRIBUTE_MATH_DEFINES)

#
# D20-P5 — Proper Armor & Defense
#
# handler.c still contains the old Stage-2 get_defense implementation.
# Rename that one translation-unit-local exported symbol at compile time so
# defense_rules.c can provide the authoritative get_defense() without forcing
# a giant unrelated handler.c replacement into this copy-over package.
#
# This is intentionally bounded compatibility scaffolding.  The old function
# can be physically deleted from handler.c in a later cleanup pass once this
# source slice has been integrated upstream.
#
o/handler.o: C_FLAGS += -Dget_defense=legacy_get_defense -Dchar_to_room=legacy_char_to_room

#
# Hierarchical geography bootstrap
#
# Keep the inherited planet loader intact behind a compatibility symbol.
# geography.c owns the exported load_planets() boundary so sectors and zones
# load immediately after the existing planets have resolved systems and areas.
o/clans.o: C_FLAGS += -Dload_planets=legacy_load_planets

# Keep the inherited data-driven command loader intact. geography_commands.c
# layers the Sector/Zone builder commands over it, then geography_ui_commands.c
# owns the final load_commands() boundary for friendly show selectors and
# showplanet parity.
o/tables.o: C_FLAGS += -Dload_commands=legacy_load_commands
o/geography_commands.o: C_FLAGS += -Dload_commands=geography_load_commands

#
# Telnet/MSDP input filtering and canonical command capacity
#
# comm.c owns both the game-loop call site and the canonical parser, so patch
# that translation unit directly at compile time rather than relying on symbol
# interposition. The source guard must match exactly once or the build fails.
o/comm.o: C_FLAGS += -DZEROPOINT_TELNET_INPUT

#
# Room metadata and area-builder command layering
#
# Keep the inherited command implementations intact behind compatibility
# symbols. room_metadata.c owns the final look/redit/rstat boundary, while
# area_builder_commands.c intercepts only aset filename renames so geography
# subdirectories and Planet/Zone persistence stay consistent.
o/act_info.o: C_FLAGS += -Ddo_look=legacy_do_look
o/elevator.o: C_FLAGS += -Ddo_look=elevator_do_look
o/build.o: C_FLAGS += -Ddo_redit=legacy_do_redit -Ddo_aset=legacy_do_aset
o/act_wiz.o: C_FLAGS += -Ddo_rstat=legacy_do_rstat

#
# Attribute training migration
#
# misc.c still contains the inherited binary do_train implementation. Keep it
# link-safe under a legacy symbol while training.c provides the authoritative
# progress-based command. do_push is similarly retained behind elevator.c.
#
o/misc.o: C_FLAGS += -Ddo_train=legacy_do_train -Ddo_push=legacy_do_push

# The long-term nutrition drain API takes explicit nutrient values. The old
# misc.c food helper intentionally keeps its local consume_nutrition name, so
# only the translation units using the global value-based API are renamed.
# The old progress function is also retained under a legacy name while
# training_guard.c provides the hard natural-potential boundary.
o/attribute_math.o: C_FLAGS += \
	-Dconsume_nutrition=consume_nutrition_values \
	-Dgain_ability_training_progress=legacy_gain_ability_training_progress
o/act_move.o o/update.o: C_FLAGS += -Dconsume_nutrition=consume_nutrition_values

# get_trait_training_chance() previously clamped to a minimum of 1 even when
# the character was already at or above natural potential. Keep that inherited
# implementation link-safe while training_guard.c supplies the guarded version.
o/traits.o: C_FLAGS += -Dget_trait_training_chance=legacy_get_trait_training_chance

# update.c keeps the inherited scheduler under a legacy symbol while
# natural_progress.c layers bounded passive attribute development after it.
o/update.o: C_FLAGS += -Dupdate_handler=legacy_update_handler

# elevator.c becomes the final update-handler layer. It calls the existing
# natural-progress wrapper first, then advances timed elevator state.
o/natural_progress.o: C_FLAGS += -Dupdate_handler=natural_update_handler

all:
	$(info    Using flags: $(C_FLAGS))
	$(MAKE) -s ZeroPoint
	$(MAKE) -s dns

# pull in dependency info for *existing* .o files
-include dependencies.d

ifdef IS_CYGWIN
ZeroPoint: $(O_FILES)
	@rm -f ZeroPoint.exe
	@dlltool --export-all --output-def ZeroPoint.def $(O_FILES)
	@dlltool --dllname ZeroPoint.exe --output-exp ZeroPoint.exp --def ZeroPoint.def
	$(CC) -o ZeroPoint.exe $(O_FILES) ZeroPoint.exp $(L_FLAGS)
	@echo "Generating dependency file ...";
	@$(CC) -MM $(C_FLAGS) $(C_FILES) > dependencies.d
	@perl -pi -e 's.^([a-z]).o/$$1.g' dependencies.d
	@echo "Done compiling mud.";
	@chmod g+w ZeroPoint.exe
	@chmod a+x ZeroPoint.exe
	@chmod g+w $(O_FILES)

clean:
	@rm -f o/*.o ZeroPoint.exe dependencies.d resolver.exe resolver.o *~ .comm_extended.c

else
ZeroPoint: $(O_FILES)
	@rm -f ZeroPoint
	$(CC) -export-dynamic -o ZeroPoint $(O_FILES) $(L_FLAGS)
	@echo "Generating dependency file ...";
	@$(CC) -MM $(C_FLAGS) $(C_FILES) > dependencies.d
	@perl -pi -e 's.^([a-z]).o/$$1.g' dependencies.d
	@echo "Done compiling mud.";
	@chmod g+w ZeroPoint
	@chmod a+x ZeroPoint
	@chmod g+w $(O_FILES)

clean:
	@rm -f o/*.o ZeroPoint dependencies.d resolver resolver.o *~ .comm_extended.c
endif

dns: resolver.o
	@rm -f resolver
	$(CC) $(D_FLAGS) -o resolver resolver.o
	@echo "Done compiling DNS resolver.";
	@chmod g+w resolver
	@chmod a+x resolver
	@chmod g+w resolver.o

indent:
	@indent -ts3 -nut -nsaf -nsai -nsaw -npcs -npsl -ncs -nbc -bls -prs -bap -cbi0 -cli3 -bli0 -l125 -lp -i3 -cdb -c1 -cd1 -sc -pmt $(C_FILES)

indentclean:
	@rm *.c~ *.h~

o:
	@mkdir -p $@

o/%.o: %.c Makefile | o
	@echo "  Compiling $@...";
	$(CC) -c $(C_FLAGS) $< -o $@
