print("Lua execution started [Version " .. Version() .. "]")
-- local d = require("./LoonarDebug")

local function PrintTable(t)
	for key, value in pairs(t) do
		print(key, value)
		if type(value) == "table" then
			print("Subtable: ")
			PrintTable(value)
			print("End")
		end
	end
end

local function PrintMetatable(t)
	local mt = getmetatable(t)
	if mt then
		print("Metatable:")
		PrintTable(mt)
	else
		print("No metatable found.")
	end
end

Window:SetTitle("Loonar Test")

local v = Vector3.new(1, 2, 3)
local u = Vector3.new(2, 4, 6)

local signal = Signal.new()
-- PrintMetatable(signal)

local disconnect = signal:OnReceive(function(...)
	print("Signal received: ", ...)
end)
local disconnect2 = Window.Minimized:OnReceive(function(...)
	print("Window Minimized", ...)
end)

signal:Send(1, 2, 3, "fyra")

print("Disconnecting signal...")
disconnect()
signal:Send(4, 5, 6, "sju")


-- d.PrintMetatable(v)

local dot = v:Dot(u)
print("Dot product: ", dot)

local cross = v:Cross(u)
print("Cross product: ", cross)

print("u: ", u.X, u.Y, u.Z)
print("v: ", v.X, v.Y, v.Z)


local mat = Material.new("assets/test/Brick_Wall_014_COLOR.jpg", "assets/test/Brick_Wall_014_NORM.jpg")
print("Material created: ")

local p = Primitive.new(0, mat)
print("Primitive created: ")
p:SetPosition(Vector3.new(0, 100, 0))
print("Primitive position: ")
local pos = p:GetPosition()
print("Primitive position: ", pos.X, pos.Y, pos.Z)

-- for i = 1, 200 do
--     local p = Primitive.new()
--     p:SetPosition(Vector3.new(0, i, 0))
--     p:SetType(0)
-- end

-- Window.Minimized:Connect(function()
-- 	print("Window minimized")
-- end)

print("Lua executed successfully")
