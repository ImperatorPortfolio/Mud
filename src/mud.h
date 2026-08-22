/***************************************************************************
 * Zero Point compatibility umbrella header.
 *
 * Module declarations are organized under core/, character/, world/,
 * space/, and game/. Existing translation units may include only mud.h
 * while they migrate to focused headers.
 ***************************************************************************/
#ifndef ZEROPOINT_MUD_H
#define ZEROPOINT_MUD_H

#include "core/base.h"
#include "core/types.h"
#include "core/flags.h"
#include "core/limits.h"
#include "core/runtime.h"

#include "character/race.h"
#include "world/services.h"
#include "space/space.h"
#include "game/social.h"
#include "game/quest.h"
#include "game/dialogue.h"
#include "world/affect.h"
#include "world/definitions.h"
#include "character/character.h"
#include "character/player.h"
#include "character/progression.h"
#include "world/world.h"
#include "space/geography.h"
#include "world/room_metadata.h"
#include "game/skills.h"
#include "game/api.h"

#endif /* ZEROPOINT_MUD_H */