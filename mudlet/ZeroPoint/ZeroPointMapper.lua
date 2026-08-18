ZeroPointMapper = ZeroPointMapper or {}
local ZP = ZeroPointMapper

ZP.version = "0.1.0"
ZP.enabled = ZP.enabled or false
ZP.areaName = ZP.areaName or "Zero Point"
ZP.areaID = ZP.areaID
ZP.currentRoom = ZP.currentRoom or nil
ZP.pendingDir = ZP.pendingDir or nil
ZP.buffer = {}
ZP.triggerIDs = ZP.triggerIDs or {}
ZP.aliasIDs = ZP.aliasIDs or {}

local DIRS = {
  n="north", north="north",
  ne="northeast", northeast="northeast",
  e="east", east="east",
  se="southeast", southeast="southeast",
  s="south", south="south",
  sw="southwest", southwest="southwest",
  w="west", west="west",
  nw="northwest", northwest="northwest",
  u="up", up="up",
  d="down", down="down",
  ["in"]="in", inside="in",
  out="out", outside="out",
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

local function ensureArea()
  local areas = getAreaTable() or {}
  if areas[ZP.areaName] then
    ZP.areaID = areas[ZP.areaName]
    return ZP.areaID
  end
  ZP.areaID = addAreaName(ZP.areaName)
  return ZP.areaID
end

local function coordsForNewRoom(fromID, dir)
  if not fromID then return 0,0,0 end
  local x,y,z = getRoomCoordinates(fromID)
  x, y, z = x or 0, y or 0, z or 0
  local d = DELTA[dir] or {1,0,0}
  return x+d[1], y+d[2], z+d[3]
end

local function createRoom(name, fromID, dir)
  ensureArea()
  local id = createRoomID()
  addRoom(id, ZP.areaID)
  setRoomName(id, name)
  local x,y,z = coordsForNewRoom(fromID, dir)
  setRoomCoordinates(id, x, y, z)
  if fromID and dir then
    setExit(fromID, id, dir)
    local rev = REVERSE[dir]
    if rev then setExit(id, fromID, rev) end
  end
  return id
end

local function addExitStubs(roomID, exits)
  if not roomID then return end
  local linked = getRoomExits(roomID) or {}
  for dir,_ in pairs(exits) do
    if not linked[dir] then
      setExitStub(roomID, dir, true)
    end
  end
end

local function parseRoomBlock(lines)
  local exitsIndex = nil
  for i,ln in ipairs(lines) do
    if trim(ln):lower() == "obvious exits:" then
      exitsIndex = i
      break
    end
  end
  if not exitsIndex then return nil end

  local title = nil
  for i=1,exitsIndex-1 do
    local t = trim(lines[i])
    if t ~= "" then
      title = t
      break
    end
  end
  if not title then return nil end

  local exits = {}
  for i=exitsIndex+1,#lines do
    local t = trim(lines[i])
    local dirText = t:match("^([%a]+)%s*%-%s*.+$")
    if dirText then
      local dir = DIRS[dirText:lower()]
      if dir then exits[dir] = true end
    end
  end

  return { title=title, exits=exits }
end

function ZP.finalizeRoom()
  if not ZP.enabled then
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
  local nextRoom = nil

  if previous and ZP.pendingDir then
    local known = getRoomExits(previous) or {}
    nextRoom = known[ZP.pendingDir]
  end

  if not nextRoom then
    nextRoom = createRoom(room.title, previous, ZP.pendingDir)
  else
    setRoomName(nextRoom, room.title)
  end

  ZP.currentRoom = nextRoom
  centerview(nextRoom)
  addExitStubs(nextRoom, room.exits)
  ZP.pendingDir = nil
end

function ZP.onLine(text)
  local t = text or ""
  if t:match("^Health:%d+/%d+%s+Movement:%d+/%d+%s*>%s*$") then
    ZP.finalizeRoom()
    return
  end
  table.insert(ZP.buffer, t)
  if #ZP.buffer > 80 then table.remove(ZP.buffer, 1) end
end

function ZP.move(raw)
  local cmd = trim(raw):lower()
  local dir = DIRS[cmd]
  if ZP.enabled and dir then
    ZP.pendingDir = dir
  end
  send(raw)
end

function ZP.start(area)
  if area and trim(area) ~= "" then ZP.areaName = trim(area) end
  ensureArea()
  ZP.enabled = true

  local playerRoom = getPlayerRoom()
  if playerRoom and playerRoom > 0 and getRoomName(playerRoom) ~= "" then
    ZP.currentRoom = playerRoom
  end

  ZP.pendingDir = nil
  ZP.buffer = {}
  zpEcho("Mapping started in area <white>" .. ZP.areaName .. "<reset>. Walk normally; rooms will be created and linked.")
  send("look")
end

function ZP.stop()
  ZP.enabled = false
  ZP.pendingDir = nil
  ZP.buffer = {}
  zpEcho("Mapping stopped.")
end

function ZP.status()
  local state = ZP.enabled and "<green>ON" or "<red>OFF"
  local rid = ZP.currentRoom and tostring(ZP.currentRoom) or "unknown"
  zpEcho("Mapping: " .. state .. "<reset> | area: <white>" .. ZP.areaName ..
         "<reset> | room id: <white>" .. rid)
end

function ZP.help()
  cecho([[
<cyan>ZeroPoint Mapper v]] .. ZP.version .. [[<reset>
  <white>zpmap start [area]<reset>  Start mapping; defaults to the current area name.
  <white>zpmap stop<reset>          Stop automatic mapping.
  <white>zpmap status<reset>        Show mapper state and current room ID.
  <white>zpmap here<reset>          Center Mudlet's mapper on the current room.
  <white>zpmap help<reset>          Show this help.

While mapping, use normal movement commands: n, ne, e, se, s, sw, w, nw, u, d, in, out.
Zero Point room blocks are detected from "Obvious exits:" through the Health/Movement prompt.
]])
end

function ZP.bootstrap()
  for _,id in ipairs(ZP.triggerIDs or {}) do pcall(killTrigger, id) end
  for _,id in ipairs(ZP.aliasIDs or {}) do pcall(killAlias, id) end
  ZP.triggerIDs, ZP.aliasIDs = {}, {}

  table.insert(ZP.triggerIDs,
    tempRegexTrigger("^.*$", function() ZP.onLine(line) end))

  local movePattern = "^(n|north|ne|northeast|e|east|se|southeast|s|south|sw|southwest|w|west|nw|northwest|u|up|d|down|in|inside|out|outside)$"
  table.insert(ZP.aliasIDs,
    tempAlias(movePattern, function() ZP.move(matches[1]) end))

  table.insert(ZP.aliasIDs,
    tempAlias("^zpmap(?:\\s+(.*))?$", function()
      local arg = trim(matches[2] or "")
      if arg == "" or arg == "help" then ZP.help(); return end
      if arg == "stop" then ZP.stop(); return end
      if arg == "status" then ZP.status(); return end
      if arg == "here" then
        if ZP.currentRoom then centerview(ZP.currentRoom) else zpEcho("No current mapped room yet.") end
        return
      end
      local area = arg:match("^start%s+(.+)$")
      if arg == "start" then ZP.start(nil); return end
      if area then ZP.start(area); return end
      ZP.help()
    end))

  zpEcho("Loaded v" .. ZP.version .. ". Type <white>zpmap start The Hague<reset> to begin.")
end

if not ZP._loadHandler then
  ZP._loadHandler = registerAnonymousEventHandler("sysLoadEvent", "ZeroPointMapper.bootstrap")
end

ZP.bootstrap()
