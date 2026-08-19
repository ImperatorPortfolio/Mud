ZeroPointMapper = ZeroPointMapper or {}
local ZP = ZeroPointMapper

ZP.version = "0.3.0"
ZP.enabled = ZP.enabled or false
ZP.currentRoom = nil
ZP.lastRoom = nil
ZP.lastPacket = nil
ZP.roomExits = {}
ZP.aliasIDs = ZP.aliasIDs or {}
ZP.eventIDs = ZP.eventIDs or {}
ZP.msdpSeen = false

local MSDP_DIRS = {
  n="north", north="north", ne="northeast", northeast="northeast",
  e="east", east="east", se="southeast", southeast="southeast",
  s="south", south="south", sw="southwest", southwest="southwest",
  w="west", west="west", nw="northwest", northwest="northwest",
  u="up", up="up", d="down", down="down",
  ["in"]="in", inside="in", out="out", outside="out"
}

local STANDARD_DIRS = {
  "north","northeast","east","southeast","south","southwest",
  "west","northwest","up","down","in","out"
}

local DELTA = {
  north={0,1,0}, northeast={1,1,0}, east={1,0,0}, southeast={1,-1,0},
  south={0,-1,0}, southwest={-1,-1,0}, west={-1,0,0}, northwest={-1,1,0},
  up={0,0,1}, down={0,0,-1}, ["in"]={0,0,1}, out={0,0,-1}
}

local function zpEcho(msg)
  cecho("\n<cyan>[ZeroPoint Mapper]<reset> " .. msg .. "\n")
end

local function trim(s)
  return tostring(s or ""):gsub("^%s+", ""):gsub("%s+$", "")
end

local function roomExists(id)
  id = tonumber(id)
  if not id or id <= 0 then return false end
  local ok, name = pcall(getRoomName, id)
  return ok and name ~= nil
end

local function ensureArea(name)
  name = trim(name)
  if name == "" then name = "Zero Point" end
  local areas = getAreaTable() or {}
  if areas[name] then return areas[name] end
  return addAreaName(name)
end

local function findArrivalDirection(fromRoom, toRoom)
  if not fromRoom or not toRoom then return nil end
  local exits = ZP.roomExits[tonumber(fromRoom)]
  if type(exits) ~= "table" then return nil end
  local found
  for dir, dest in pairs(exits) do
    if tonumber(dest) == tonumber(toRoom) then
      if found then return nil end
      found = dir
    end
  end
  return found
end

local function nextCoordinates(fromRoom, dir)
  if not fromRoom or not dir or not roomExists(fromRoom) then return 0,0,0 end
  local x,y,z = getRoomCoordinates(fromRoom)
  x,y,z = tonumber(x) or 0, tonumber(y) or 0, tonumber(z) or 0
  local d = DELTA[dir]
  if not d then return x,y,z end
  return x+d[1], y+d[2], z+d[3]
end

local function ensureRoom(id, name, areaName, fromRoom, dir)
  id = tonumber(id)
  if not id or id <= 0 then return nil end
  local areaID = ensureArea(areaName)
  if not roomExists(id) then
    addRoom(id, areaID)
    local x,y,z = nextCoordinates(fromRoom, dir)
    setRoomCoordinates(id, x, y, z)
  else
    pcall(setRoomArea, id, areaID)
  end
  if name and trim(name) ~= "" then setRoomName(id, trim(name)) end
  return id
end

local function normaliseExits(raw)
  local result = {}
  if type(raw) ~= "table" then return result end
  for rawDir, rawDest in pairs(raw) do
    local dir = MSDP_DIRS[trim(rawDir):lower()]
    local dest = tonumber(rawDest)
    if dir and dest and dest > 0 then result[dir] = dest end
  end
  return result
end

local function reconcileRoom(roomID, exits, areaName)
  local localExits = getRoomExits(roomID) or {}
  for _, dir in ipairs(STANDARD_DIRS) do
    local existing = tonumber(localExits[dir])
    local wanted = tonumber(exits[dir])
    if existing and existing > 0 and existing ~= wanted then
      setExit(roomID, -1, dir)
    end
  end
  for dir, destID in pairs(exits) do
    local destination = ensureRoom(destID, nil, areaName, roomID, dir)
    if destination then
      local current = tonumber((getRoomExits(roomID) or {})[dir])
      if current ~= destination then
        if current and current > 0 then setExit(roomID, -1, dir) end
        setExit(roomID, destination, dir)
      end
    end
  end
  updateMap()
end

function ZP.onMSDPRoom()
  if not ZP.enabled or not msdp or type(msdp.ROOM) ~= "table" then return end
  local data = msdp.ROOM
  local vnum = tonumber(data.VNUM or data.vnum)
  if not vnum or vnum <= 0 then
    zpEcho("<red>Ignored malformed MSDP ROOM packet: missing valid VNUM.<reset>")
    return
  end
  local areaName = trim(data.AREA or data.area)
  local exits = normaliseExits(data.EXITS or data.exits)
  local previous = ZP.currentRoom
  local arrivalDir = findArrivalDirection(previous, vnum)
  local room = ensureRoom(vnum, data.NAME or data.name, areaName, previous, arrivalDir)
  if not room then return end
  ZP.msdpSeen = true
  ZP.lastRoom = previous
  ZP.currentRoom = room
  ZP.roomExits[room] = exits
  ZP.lastPacket = {
    VNUM=room, NAME=data.NAME or data.name, AREA=areaName,
    TERRAIN=data.TERRAIN or data.terrain, EXITS=exits
  }
  reconcileRoom(room, exits, areaName)
  centerview(room)
end

function ZP.start()
  ZP.enabled = true
  ZP.msdpSeen = false
  ZP.currentRoom = nil
  ZP.lastRoom = nil
  ZP.lastPacket = nil
  ZP.roomExits = {}
  if type(sendMSDP) == "function" then pcall(sendMSDP, "REPORT", "ROOM") end
  zpEcho("MSDP-only mapping enabled. Waiting for authoritative <white>ROOM<reset> data.")
  send("look")
  tempTimer(3, function()
    if ZP.enabled and not ZP.msdpSeen then
      zpEcho("<red>No MSDP ROOM data received.<reset> Nothing has been mapped. Check Mudlet MSDP settings and reconnect.")
    end
  end)
end

function ZP.stop()
  ZP.enabled = false
  zpEcho("Mapping stopped.")
end

function ZP.status()
  local state = ZP.enabled and "<green>ON" or "<red>OFF"
  local source = ZP.msdpSeen and "<green>MSDP authoritative" or "<red>no MSDP ROOM data"
  zpEcho("Mapping: " .. state .. "<reset> | source: " .. source ..
         "<reset> | room: <white>" .. tostring(ZP.currentRoom or "unknown"))
end

function ZP.debug()
  if not ZP.lastPacket then zpEcho("No authoritative ROOM packet has been accepted yet."); return end
  local p = ZP.lastPacket
  cecho("\n<cyan>[ZeroPoint Mapper Debug]<reset>\n")
  cecho("  VNUM: <white>" .. tostring(p.VNUM) .. "<reset>\n")
  cecho("  NAME: <white>" .. tostring(p.NAME or "") .. "<reset>\n")
  cecho("  AREA: <white>" .. tostring(p.AREA or "") .. "<reset>\n")
  cecho("  TERRAIN: <white>" .. tostring(p.TERRAIN or "") .. "<reset>\n")
  cecho("  EXITS:\n")
  local count = 0
  for _, dir in ipairs(STANDARD_DIRS) do
    local dest = p.EXITS and p.EXITS[dir]
    if dest then
      cecho("    <white>" .. dir .. "<reset> -> <white>" .. tostring(dest) .. "<reset>\n")
      count = count + 1
    end
  end
  if count == 0 then cecho("    <grey>(none)<reset>\n") end
end

function ZP.clear(confirm)
  if confirm ~= true then
    zpEcho("<red>This deletes the ENTIRE Mudlet map database.<reset> Type <white>zpmap clear confirm<reset> to continue.")
    return
  end
  if type(deleteMap) ~= "function" then
    zpEcho("<red>Your Mudlet version does not provide deleteMap(). Update Mudlet before clearing.<reset>")
    return
  end
  local ok, err = deleteMap()
  if not ok then zpEcho("<red>Map clear failed:<reset> " .. tostring(err or "unknown error")); return end
  ZP.currentRoom = nil
  ZP.lastRoom = nil
  ZP.lastPacket = nil
  ZP.roomExits = {}
  ZP.msdpSeen = false
  zpEcho("<green>Map cleared.<reset> Type <white>zpmap start<reset> to rebuild from authoritative MSDP.")
end

function ZP.help()
  cecho([[
<cyan>ZeroPoint Mapper v]] .. ZP.version .. [[<reset>
  <white>zpmap start<reset>          Start strict MSDP-only mapping.
  <white>zpmap stop<reset>           Stop mapping.
  <white>zpmap status<reset>         Show MSDP/map state.
  <white>zpmap here<reset>           Center on the authoritative current room.
  <white>zpmap debug<reset>          Show the last authoritative ROOM packet.
  <white>zpmap clear<reset>          Show the destructive clear warning.
  <white>zpmap clear confirm<reset>  Delete the entire Mudlet map database.
  <white>zpmap help<reset>           Show this help.

ZeroPoint Mapper never parses room text and never infers topology from movement commands.
ROOM.VNUM is room identity. ROOM.EXITS is the only source of map links.
]])
end

function ZP.bootstrap()
  for _,id in ipairs(ZP.aliasIDs or {}) do pcall(killAlias,id) end
  for _,id in ipairs(ZP.eventIDs or {}) do pcall(killAnonymousEventHandler,id) end
  ZP.aliasIDs,ZP.eventIDs = {},{}
  table.insert(ZP.aliasIDs, tempAlias("^zpmap(?:\\s+(.*))?$", function()
    local arg = trim(matches[2] or ""):lower()
    if arg == "" or arg == "help" then ZP.help(); return end
    if arg == "start" then ZP.start(); return end
    if arg == "stop" then ZP.stop(); return end
    if arg == "status" then ZP.status(); return end
    if arg == "debug" then ZP.debug(); return end
    if arg == "clear" then ZP.clear(false); return end
    if arg == "clear confirm" then ZP.clear(true); return end
    if arg == "here" then
      if ZP.currentRoom then centerview(ZP.currentRoom) else zpEcho("No authoritative current room yet.") end
      return
    end
    ZP.help()
  end))
  table.insert(ZP.eventIDs, registerAnonymousEventHandler("msdp.ROOM", function() ZP.onMSDPRoom() end))
  zpEcho("Loaded v" .. ZP.version .. ". Enable MSDP, reconnect, then type <white>zpmap start<reset>.")
end

if not ZP._loadHandler then ZP._loadHandler = registerAnonymousEventHandler("sysLoadEvent", "ZeroPointMapper.bootstrap") end
ZP.bootstrap()
