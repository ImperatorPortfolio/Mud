ZeroPointRoomUI = ZeroPointRoomUI or {}
local UI = ZeroPointRoomUI

UI.version = "0.4.3"
UI.eventIDs = UI.eventIDs or {}
UI.aliasIDs = UI.aliasIDs or {}
UI.visible = UI.visible ~= false
UI.built = UI.built or false

local function trim(s)
  return tostring(s or ""):gsub("^%s+", ""):gsub("%s+$", "")
end

local function centerMap(roomID)
  roomID = tonumber(roomID)
  if not roomID or roomID <= 0 or type(centerview) ~= "function" then return end
  pcall(centerview, roomID)
  if type(tempTimer) == "function" then
    tempTimer(0, function() pcall(centerview, roomID) end)
  end
end

local function orderedValues(raw)
  local values = {}
  if type(raw) ~= "table" then return values end
  local numbered, other = {}, {}
  for key, value in pairs(raw) do
    local number = tonumber(key)
    if number then
      table.insert(numbered, {number=number, value=tostring(value or "")})
    else
      table.insert(other, {key=tostring(key), value=tostring(value or "")})
    end
  end
  table.sort(numbered, function(a,b) return a.number < b.number end)
  table.sort(other, function(a,b) return a.key < b.key end)
  for _,e in ipairs(numbered) do table.insert(values,e.value) end
  for _,e in ipairs(other) do table.insert(values,e.value) end
  return values
end

local function echoLine(console, text)
  console:echo(tostring(text or "") .. "\n")
end

local function section(console, title, raw)
  local values = orderedValues(raw)
  console:cecho("<cyan>" .. title .. " <grey>(" .. tostring(#values) .. ")<reset>\n")
  if #values == 0 then
    console:cecho("  <grey>None<reset>\n")
  else
    for _,value in ipairs(values) do echoLine(console, "  • " .. value) end
  end
  echoLine(console, "")
end

function UI.build()
  if UI.built then return true end
  if type(Geyser) ~= "table" or not Geyser.UserWindow or not Geyser.VBox
     or not Geyser.Mapper or not Geyser.Label or not Geyser.MiniConsole then
    cecho("\n<red>[ZeroPoint UI] Required Geyser widgets are unavailable.<reset>\n")
    return false
  end

  UI.window = Geyser.UserWindow:new({
    name="ZeroPointRoomInspector",
    titleText="ZeroPoint Room Inspector",
    restoreLayout=true
  })

  UI.layout = Geyser.VBox:new({
    name="ZeroPointRoomInspectorLayout",
    x=0, y=0, width="100%", height="100%"
  }, UI.window)

  UI.map = Geyser.Mapper:new({
    name="ZeroPointRoomInspectorMap",
    v_stretch_factor=5.0
  }, UI.layout)

  UI.header = Geyser.Label:new({
    name="ZeroPointRoomInspectorHeader",
    height=28,
    v_policy=Geyser.Fixed
  }, UI.layout)
  UI.header:setStyleSheet([[
    QLabel {
      background-color: rgb(24,27,31);
      color: rgb(235,235,235);
      padding-left: 8px;
      font-weight: bold;
      border-top: 1px solid rgb(60,65,72);
      border-bottom: 1px solid rgb(60,65,72);
    }
  ]])
  UI.header:echo("ZeroPoint — waiting for ROOM data")

  UI.details = Geyser.MiniConsole:new({
    name="ZeroPointRoomInspectorDetails",
    v_stretch_factor=2.0
  }, UI.layout)
  UI.details:setColor("black")
  UI.details:setFontSize(10)
  UI.details:enableAutoWrap()
  pcall(enableScrollBar, UI.details.name)

  UI.contents = Geyser.MiniConsole:new({
    name="ZeroPointRoomInspectorContents",
    v_stretch_factor=3.0
  }, UI.layout)
  UI.contents:setColor("black")
  UI.contents:setFontSize(10)
  UI.contents:enableAutoWrap()
  pcall(enableScrollBar, UI.contents.name)

  UI.built = true
  if UI.visible then UI.window:show() else UI.window:hide() end

  local current = msdp and msdp.ROOM and tonumber(msdp.ROOM.VNUM or msdp.ROOM.vnum)
  if not current and ZeroPointMapper then current = tonumber(ZeroPointMapper.currentRoom) end
  centerMap(current)
  return true
end

function UI.render(data)
  if type(data) ~= "table" then return end
  if not UI.build() then return end
  local vnum = tonumber(data.VNUM or data.vnum)
  local name = trim(data.NAME or data.name)
  local area = trim(data.AREA or data.area)
  local terrain = trim(data.TERRAIN or data.terrain)
  local description = tostring(data.DESCRIPTION or data.description or "")
  local title = name ~= "" and name or "Unnamed room"
  if vnum then title = title .. "   [#" .. tostring(vnum) .. "]" end
  UI.header:echo(title)
  centerMap(vnum)

  UI.details:clear()
  if area ~= "" or terrain ~= "" then
    UI.details:cecho("<grey>" .. area)
    if area ~= "" and terrain ~= "" then UI.details:cecho("  •  ") end
    UI.details:cecho(terrain .. "<reset>\n\n")
  end
  if trim(description) ~= "" then
    echoLine(UI.details, description:gsub("\r",""))
    echoLine(UI.details, "")
  end
  section(UI.details, "WINDOWS", data.WINDOWS or data.windows)

  UI.contents:clear()
  UI.contents:cecho("<white><b>ROOM CONTENTS</b><reset>\n\n")
  section(UI.contents, "PEOPLE", data.PEOPLE or data.people)
  section(UI.contents, "SHIPS", data.SHIPS or data.ships)
  section(UI.contents, "FURNITURE", data.FURNITURE or data.furniture)
  section(UI.contents, "OBJECTS", data.OBJECTS or data.objects)
end

function UI.onMSDPRoom()
  if msdp and type(msdp.ROOM) == "table" then UI.render(msdp.ROOM) end
end

function UI.show()
  UI.visible=true
  if UI.build() then
    UI.window:show()
    local current = msdp and msdp.ROOM and tonumber(msdp.ROOM.VNUM or msdp.ROOM.vnum)
    if not current and ZeroPointMapper then current = tonumber(ZeroPointMapper.currentRoom) end
    centerMap(current)
  end
end

function UI.hide()
  UI.visible=false
  if UI.built and UI.window then UI.window:hide() end
end

function UI.toggle()
  if UI.visible then UI.hide() else UI.show() end
end

function UI.status()
  cecho("\n<cyan>[ZeroPoint UI]<reset> v" .. UI.version .. " | " ..
        (UI.visible and "<green>visible" or "<red>hidden") .. "<reset>\n")
end

function UI.bootstrap()
  for _,id in ipairs(UI.aliasIDs or {}) do pcall(killAlias,id) end
  for _,id in ipairs(UI.eventIDs or {}) do pcall(killAnonymousEventHandler,id) end
  UI.aliasIDs, UI.eventIDs = {}, {}
  UI.build()
  table.insert(UI.eventIDs, registerAnonymousEventHandler("msdp.ROOM", function() UI.onMSDPRoom() end))
  table.insert(UI.aliasIDs, tempAlias("^zproom(?:\\s+(.*))?$", function()
    local arg=trim(matches[2] or ""):lower()
    if arg=="" or arg=="toggle" then UI.toggle(); return end
    if arg=="show" then UI.show(); return end
    if arg=="hide" then UI.hide(); return end
    if arg=="status" then UI.status(); return end
    cecho("\n<cyan>ZeroPoint Room UI<reset>\n  zproom show\n  zproom hide\n  zproom toggle\n  zproom status\n")
  end))
  if msdp and type(msdp.ROOM)=="table" then UI.render(msdp.ROOM) end
end

if not UI._loadHandler then
  UI._loadHandler=registerAnonymousEventHandler("sysLoadEvent","ZeroPointRoomUI.bootstrap")
end

UI.bootstrap()
