print("Lua execution started [Version " .. Version() .. "]")

-- local function printPos(p)
-- 	local pos = p.Position
-- 	print("Position: ", pos.x, pos.y, pos.z)
-- end

local v = Vector3.new(1, 2, 3)
local u = Vector3.new(2, 4, 6)

local t = getmetatable(v)

for key, value in pairs(t) do
    print(key, value)
end



local p = v:Dot(u)
 print("Dot product: ", p)

-- for i = 1, 10, 1 do
-- 	local p = Primitive.new()
-- 	p.Position = {
-- 		x = i * 2,
-- 		y = i * 2,
-- 		z = i * 2,
-- 	}
-- 	for key, value in pairs(p:GetPosition()) do
-- 		print(key, value)
-- 	end
-- end
-- -- local p = Primitive.new()
-- -- printPos(p)
print("Lua executed successfully")
