print("Lua execution started [Version " .. Version() .. "]")
--local d = require("./LoonarDebug")
require("os")
local function PrintMetatable(t)
    local mt = getmetatable(t)
    if mt then
        print("Metatable:")
        for key, value in pairs(mt) do
            print(key, value)
        end
    else
        print("No metatable found.")
    end
end
PrintMetatable(game)
print("game: ", game)
local Window = game:Get("Window")
print("Window: ", Window)
print("WindowType: ", type(Window))

local v = Vector3.new(1, 2, 3)
local u = Vector3.new(2, 4, 6)

PrintMetatable(v)

-- d.PrintMetatable(v)

local dot = v:Dot(u)
print("Dot product: ", dot)

local cross = v:Cross(u)
print("Cross product: ", cross)

print("u: ", u.X, u.Y, u.Z)
print("v: ", v.X, v.Y, v.Z)

local p = Primitive.new()
print("Primitive created: ")
p:SetPosition(Vector3.new(0,100,0))
print("Primitive position: ")
local pos = p:GetPosition()
print("Primitive position: ", pos.X, pos.Y, pos.Z)

-- for i = 1, 200 do
--     local p = Primitive.new()
--     p:SetPosition(Vector3.new(0, i, 0))
--     p:SetType(0)
-- end

print("Lua executed successfully")