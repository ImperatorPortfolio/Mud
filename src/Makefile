#This is the ZeroPoint Makefile.
#This should be usable on pretty much any Linux system.
#Refer to your 'man gcc' for explanations of these options
#To compile in FreeBSD or OpenBSD, use the gmake compiler.

#Change this to reflect whichever compiler you want to use.
CC      = g++
#CC      = clang++

ifeq ($(findstring CYGWIN, $(shell uname -s)), CYGWIN)
    IS_CYGWIN = 1
endif

HAS_DL := $(shell echo 'int main(){}' | $(CC) -x c - -ldl -o /dev/null 2>/dev/null && echo yes)

ifeq ($(HAS_DL),yes)
    NEED_DL = -ldl
else
    NEED_DL =
endif

UNAME_S := $(shell uname -s)

ifneq ($(UNAME_S), Darwin)
    EXPORT_SYMBOLS = -export-dynamic
endif

ifndef IS_CYGWIN
   M_FLAGS = -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer
endif

HARDENING_FLAGS = -fstack-protector-strong -D_FORTIFY_SOURCE=2
W_FLAGS = -std=c++20 -Wall -Wshadow -Wformat-security -Wpointer-arith -Wcast-align -Wredundant-decls
C_FLAGS = -g2 $(W_FLAGS) $(M_FLAGS) $(HARDENING_FLAGS) $(EXPORT_SYMBOLS)
L_FLAGS = $(M_FLAGS) $(HARDENING_FLAGS) -lz $(NEED_DL)
D_FLAGS = -g2 -O $(M_FLAGS) $(HARDENING_FLAGS)

C_FILES = act_comm.c act_info.c act_move.c act_obj.c act_wiz.c attribute_math.c boards.c bounty.c \
	  build.c clans.c color.c comm.c comments.c const.c db.c defense_rules.c dns.c exercise.c fight.c \
	  handler.c hashstr.c hotboot.c interp.c magic.c makeobjs.c mccp.c msdp_mapper.c \
	  misc.c mssp.c mud_comm.c mud_prog.c natural_progress.c player.c reset.c save.c sha256.c shops.c \
	  skills.c space.c special.c swskills.c tables.c track.c training.c training_guard.c traits.c update.c

O_FILES := $(patsubst %.c,o/%.o,$(C_FILES))
H_FILES = $(wildcard *.h)

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

o/handler.o: C_FLAGS += -Dget_defense=legacy_get_defense -Dchar_to_room=legacy_char_to_room
o/misc.o: C_FLAGS += -Ddo_train=legacy_do_train

o/attribute_math.o: C_FLAGS += \
	-Dconsume_nutrition=consume_nutrition_values \
	-Dgain_ability_training_progress=legacy_gain_ability_training_progress
o/act_move.o o/update.o: C_FLAGS += -Dconsume_nutrition=consume_nutrition_values
o/traits.o: C_FLAGS += -Dget_trait_training_chance=legacy_get_trait_training_chance
o/update.o: C_FLAGS += -Dupdate_handler=legacy_update_handler

all:
	$(info    Using flags: $(C_FLAGS))
	$(MAKE) -s ZeroPoint
	$(MAKE) -s dns

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
