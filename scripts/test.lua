print("Lua execution started [Version " .. Version() .. "]")

local function printPos(p)
	local pos = p.Position
	print("Position: ", pos.x, pos.y, pos.z)
end

for i = 1, 10, 1 do
	local p = Primitive.new()
	p.Position = {
		x = i * 2,
		y = i * 2,
		z = i * 2,
	}
	for key, value in pairs(p:GetPosition()) do
		print(key, value)
	end
end
-- local p = Primitive.new()
-- printPos(p)
print("Lua executed successfully")
