print("Lua execution started")


-- local primitive = Primitive.new("Cube")
-- local color = primitive.color
-- print("Color: " .. color)

-- local position = primitive.position
-- print("Position: " .. position)

-- print("Primitive name: " .. primitive.name)

print("Version: " .. Version())

local p = Primitive.new()
p:SetPosition(70,5,17)

local x, y, z = p:GetPosition()

print ("Position: ", x, y, z)

print("Lua executed successfully")