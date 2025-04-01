print("Lua execution started")


-- local primitive = Primitive.new("Cube")
-- local color = primitive.color
-- print("Color: " .. color)

-- local position = primitive.position
-- print("Position: " .. position)

-- print("Primitive name: " .. primitive.name)

print("Version: " .. Version())

local p = Primitive.new()
Primitive.SetPosition(p, 1, 2, 3)

print (Primitive.GetPosition(p))

print("Lua executed successfully")