print("Lua execution started [Version " .. Version() .. "]")
--local d = require("./LoonarDebug")

function PrintMetatable(t)
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
p:SetPosition(u)
local pos = p:GetPosition()
print("Primitive position: ", pos.X, pos.Y, pos.Z)

print("Lua executed successfully")