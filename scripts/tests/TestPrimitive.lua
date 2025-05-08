local material = Material.new("assets/test/Brick_Wall_014_COLOR.jpg", "assets/test/Brick_Wall_014_NORM.jpg")


-- Test constructors
local p = Primitive.new(PrimitiveType.Cube)
local p2 = Primitive.new(PrimitiveType.Sphere, material)
local p3 = Primitive.new(PrimitiveType.Cube, material, RigidBodyType.Static)

-- Test properties
local pos = Vector3.new(1, 2, 3)
p.Position = pos
assert(p.Position == pos, "Position mismatch: expected " .. tostring(pos) .. ", got " .. tostring(p.Position))

local type = PrimitiveType.Sphere
p.Type = type
assert(p.Type == type, "Type mismatch: expected " .. tostring(type) .. ", got " .. tostring(p.Type))


print("All Primitive tests passed!")