ZeroPointMapper = ZeroPointMapper or {}
local ZP = ZeroPointMapper

ZP.version = "0.2.1"
ZP.enabled = ZP.enabled or false
ZP.areaName = ZP.areaName or "Zero Point"
ZP.areaID = ZP.areaID
ZP.currentRoom = ZP.currentRoom or nil
ZP.pendingDir = ZP.pendingDir or nil
ZP.buffer = {}
ZP.triggerIDs = ZP.triggerIDs or {}
ZP.aliasIDs = ZP.aliasIDs or {}
ZP.eventIDs = ZP.eventIDs or {}
ZP.msdpSeen = ZP.msdpSeen or false

local DIRS = {
  n="north", north="north", ne="northeast", northeast="northeast",
  e="east", east="east", se="southeast", southeast="southeast",
  s="south", south="south", sw="southwest", southwest="southwest",
  w="west", west="west", nw="northwest", northwest="northwest",
  u="up", up="up", d="down", down="down",
  ["in"]="in", inside="in", out="out", outside="out",
}

local MSDP_DIRS = {
  n="north", ne="northeast", e="east", se="southeast", s="south",
  sw="southwest", w="west", nw="northwest", u="up", d="down",
  ["in"]="in", out="out",
}

local STANDARD_DIRS = {
  "north","northeast","east","southeast","south","southwest",
  "west","northwest","up","down","in","out"
}

local REVERSE = {
  north="south", northeast="southwest", east="west", southeast="northwest",
  south="north", southwest="northeast", west="east", northwest="southeast",
  up="down", down="up", ["in"]="out", out="in",
}

local DELTA = {
  north={0,1,0}, northeast={1,1,0}, east={1,0,0}, southeast={1,-1,0},
  south={0,-1,0}, southwest={-1,-1,0}, west={-1,0,0}, northwest={-1,1,0},
  up={0,0,1}, down={0,0,-1}, ["in"]={0,0,1}, out={0,0,-1},
}

local function zpEcho(msg)
  cecho("\n<cyan>[ZeroPoint Mapper]<reset> " .. msg .. "\n")
end

local function trim(s)
  return (s or ""):gsub("^%s+", ""):gsub("%s+$", "")
end

local function ensureArea(name)
  name = trim(name or ZP.areaName)
  if name == "" then name = "Zero Point" end
  ZP.areaName = name

  local areas = getAreaTable() or {}
  if areas[name] then
    ZP.areaID = areas[name]
    return ZP.areaID
  end

  ZP.areaID = addAreaName(name)
  return ZP.areaID
end

local function roomExists(id)
  id = tonumber(id)
  if not id or id <= 0 then return false end
  local ok, name = pcall(getRoomName, id)
  return ok and name ~= nil
end

local function coordsForNewRoom(fromID, dir)
  if not fromID or not roomExists(fromID) then return 0,0,0 end
  local x,y,z = getRoomCoordinates(fromID)
  x,y,z = x or 0,y or 0,z or 0
  local d = DELTA[dir] or {1,0,0}
  return x+d[1],y+d[2],z+d[3]
end

local function ensureServerRoom(id, name, area, fromID, dir)
  id = tonumber(id)
  if not id or id <= 0 then return nil end

  local areaID = ensureArea(area)
  if not roomExists(id) then
    addRoom(id, areaID)
    local x,y,z = coordsForNewRoom(fromID, dir)
    setRoomCoordinates(id, x, y, z)
  else
    pcall(setRoomArea, id, areaID)
  end

  if name and trim(name) ~= "" then
    setRoomName(id, name)
  end

  return id
end

local function normaliseAuthoritativeExits(exits)
  local result = {}
  if type(exits) ~= "table" then return result end

  for rawDir, rawDest in pairs(exits) do
    local key = tostring(rawDir):lower()
    local dir = MSDP_DIRS[key] or DIRS[key]
    local dest = tonumber(rawDest)
    if dir and dest and dest > 0 then
      result[dir] = dest
    end
  end

  return result
end

local function reconcileAuthoritativeExits(roomID, serverExits, areaName)
  local localExits = getRoomExits(roomID) or {}

  -- MSDP owns outgoing topology. Delete local standard exits that are absent
  -- from the server or point to a different destination.
  for _, dir in ipairs(STANDARD_DIRS) do
    local existing = tonumber(localExits[dir])
    local wanted = tonumber(serverExits[dir])
    if existing and existing > 0 and existing ~= wanted then
      setExit(roomID, -1, dir)
    end
  end

  -- Add exactly the one-way edges reported by the server. Never invent a
  -- reverse edge: one-way/asymmetric exits are valid MUD topology.
  for dir, destID in pairs(serverExits) do
    local destination = ensureServerRoom(destID, nil, areaName, roomID, dir)
    if destination then
      setExit(roomID, destination, dir)
    end
  end
end

function ZP.onMSDPRoom()
  if not ZP.enabled or not msdp or type(msdp.ROOM) ~= "table" then return end

  local data = msdp.ROOM
  local vnum = tonumber(data.VNUM or data.vnum)
  if not vnum or vnum <= 0 then return end

  local previous = ZP.currentRoom
  local arrivalDir = previous and ZP.pendingDir or nil

  local room = ensureServerRoom(
    vnum,
    data.NAME or data.name,
    data.AREA or data.area,
    previous,
    arrivalDir
  )
  if not room then return end

  ZP.msdpSeen = true
  ZP.currentRoom = room
  ZP.pendingDir = nil
  ZP.buffer = {}

  local serverExits = normaliseAuthoritativeExits(data.EXITS or data.exits or {})
  reconcileAuthoritativeExits(room, serverExits, data.AREA or data.area)

  centerview(room)
end

local function createTextRoom(name, fromID, dir)
  local id = createRoomID()
  addRoom(id, ensureArea())
  setRoomName(id, name)
  local x,y,z = coordsForNewRoom(fromID, dir)
  setRoomCoordinates(id,x,y,z)

  if fromID and dir then
    setExit(fromID,id,dir)
    local rev = REVERSE[dir]
    if rev then setExit(id,fromID,rev) end
  end

  return id
end

local function parseRoomBlock(lines)
  local exitsIndex
  for i,ln in ipairs(lines) do
    if trim(ln):lower() == "obvious exits:" then
      exitsIndex = i
      break
    end
  end
  if not exitsIndex then return nil end

  local title
  for i=1,exitsIndex-1 do
    local t = trim(lines[i])
    if t ~= "" then
      title = t
      break
    end
  end
  if not title then return nil end

  return { title=title }
end

function ZP.finalizeTextRoom()
  if not ZP.enabled or ZP.msdpSeen then
    ZP.buffer = {}
    return
  end

  local room = parseRoomBlock(ZP.buffer)
  ZP.buffer = {}
  if not room then
    ZP.pendingDir = nil
    return
  end

  local previous = ZP.currentRoom
  local nextRoom

  if previous and ZP.pendingDir then
    nextRoom = (getRoomExits(previous) or {})[ZP.pendingDir]
  end

  if not nextRoom then
    nextRoom = createTextRoom(room.title, previous, ZP.pendingDir)
  else
    setRoomName(nextRoom, room.title)
  end

  ZP.currentRoom = nextRoom
  ZP.pendingDir = nil
  centerview(nextRoom)
end

function ZP.onLine(text)
  if ZP.msdpSeen then return end

  local t = text or ""
  if t:match("^Health:%d+/%d+%s+Movement:%d+/%d+%s*>%s*$") then
    ZP.finalizeTextRoom()
    return
  end

  table.insert(ZP.buffer,t)
  if #ZP.buffer > 80 then table.remove(ZP.buffer,1) end
end

function ZP.move(raw)
  local dir = DIRS[trim(raw):lower()]
  if ZP.enabled and dir then
    ZP.pendingDir = dir
  end
  send(raw)
end

function ZP.start(area)
  if area and trim(area) ~= "" then
    ZP.areaName = trim(area)
  end

  ensureArea(ZP.areaName)
  ZP.enabled = true
  ZP.pendingDir = nil
  ZP.buffer = {}
  ZP.msdpSeen = false

  if sendMSDP then
    pcall(sendMSDP, "REPORT", "ROOM")
  end

  zpEcho("Mapping started. MSDP room topology is authoritative; text parsing is fallback only.")
  send("look")
end

function ZP.stop()
  ZP.enabled = false
  ZP.pendingDir = nil
  ZP.buffer = {}
  zpEcho("Mapping stopped.")
end

function ZP.status()
  local source = ZP.msdpSeen and "<green>MSDP authoritative" or "<yellow>text fallback"
  zpEcho(
    "Mapping: " .. (ZP.enabled and "<green>ON" or "<red>OFF") ..
    "<reset> | source: " .. source ..
    "<reset> | area: <white>" .. ZP.areaName ..
    "<reset> | room: <white>" .. tostring(ZP.currentRoom or "unknown")
  )
end

function ZP.help()
  cecho([[
<cyan>ZeroPoint Mapper v]] .. ZP.version .. [[<reset>
  <white>zpmap start [area]<reset>  Start mapping.
  <white>zpmap stop<reset>          Stop mapping.
  <white>zpmap status<reset>        Show whether MSDP or text fallback is active.
  <white>zpmap here<reset>          Center on the current room.
  <white>zpmap help<reset>          Show this help.

When MSDP is active, VNUMs and outgoing exits are server-authoritative.
The mapper removes stale local links and never invents reverse exits.
]])
end

function ZP.bootstrap()
  for _,id in ipairs(ZP.triggerIDs or {}) do pcall(killTrigger,id) end
  for _,id in ipairs(ZP.aliasIDs or {}) do pcall(killAlias,id) end
  for _,id in ipairs(ZP.eventIDs or {}) do pcall(killAnonymousEventHandler,id) end
  ZP.triggerIDs,ZP.aliasIDs,ZP.eventIDs = {},{},{}

  table.insert(
    ZP.triggerIDs,
    tempRegexTrigger("^.*$", function() ZP.onLine(line) end)
  )

  local movePattern =
    "^(n|north|ne|northeast|e|east|se|southeast|s|south|sw|southwest|" ..
    "w|west|nw|northwest|u|up|d|down|in|inside|out|outside)$"

  table.insert(
    ZP.aliasIDs,
    tempAlias(movePattern,function() ZP.move(matches[1]) end)
  )

  table.insert(
    ZP.aliasIDs,
    tempAlias("^zpmap(?:\\s+(.*))?$",function()
      local arg = trim(matches[2] or "")
      if arg == "" or arg == "help" then ZP.help(); return end
      if arg == "stop" then ZP.stop(); return end
      if arg == "status" then ZP.status(); return end
      if arg == "here" then
        if ZP.currentRoom then centerview(ZP.currentRoom)
        else zpEcho("No current room yet.") end
        return
      end

      local area = arg:match("^start%s+(.+)$")
      if arg == "start" then ZP.start(nil); return end
      if area then ZP.start(area); return end
      ZP.help()
    end)
  )

  table.insert(
    ZP.eventIDs,
    registerAnonymousEventHandler("msdp.ROOM",function() ZP.onMSDPRoom() end)
  )

  zpEcho(
    "Loaded v" .. ZP.version ..
    ". Enable MSDP in Mudlet, reconnect, then type <white>zpmap start<reset>."
  )
end

if not ZP._loadHandler then
  ZP._loadHandler = registerAnonymousEventHandler(
    "sysLoadEvent",
    "ZeroPointMapper.bootstrap"
  )
end

ZP.bootstrap()
