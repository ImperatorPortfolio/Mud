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

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c attribute_math.c boards.c bounty.c \
	  build.c clans.c color.c comm.c comments.c const.c db.c defense_rules.c dns.c fight.c \
	  handler.c hashstr.c hotboot.c interp.c magic.c makeobjs.c mccp.c \
	  misc.c mssp.c mud_comm.c mud_prog.c player.c reset.c save.c sha256.c shops.c \
	  skills.c space.c special.c swskills.c tables.c track.c training.c traits.c update.c

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
o/handler.o: C_FLAGS += -Dget_defense=legacy_get_defense

#
# Attribute training migration
#
# misc.c still contains the inherited binary do_train implementation. Keep it
# link-safe under a legacy symbol while training.c provides the authoritative
# progress-based command without forcing an unrelated wholesale misc.c rewrite.
#
o/misc.o: C_FLAGS += -Ddo_train=legacy_do_train

# The long-term nutrition drain API takes explicit nutrient values. The old
# misc.c food helper intentionally keeps its local consume_nutrition name, so
# only the translation units using the global value-based API are renamed.
o/attribute_math.o o/act_move.o o/update.o: C_FLAGS += -Dconsume_nutrition=consume_nutrition_values

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
	@rm -f o/*.o ZeroPoint.exe dependencies.d resolver.exe resolver.o *~

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
	@rm -f o/*.o ZeroPoint dependencies.d resolver resolver.o *~
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
	@indent -ts3 -nut -nsaf -nsai -nsaw -npcs -npsl -ncs -nbc -bls -prs -bap -cbi0 -cli3 -bli0 -l125 -lp -i3 -cdb -c1 -cd1 -sc -pmt $(H_FILES)

indentclean:
	@rm *.c~ *.h~

o:
	@mkdir -p $@

o/%.o: %.c | o
	@echo "  Compiling $@...";
	$(CC) -c $(C_FLAGS) $< -o $@
