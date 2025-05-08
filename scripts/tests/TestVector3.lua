local v = Vector3.new(1, 2, 3)
local u = Vector3.new(2, 4, 6)

--Test getting and setting values
assert(v.X == 1, "X value mismatch: expected 1, got " .. v.X)
assert(v.Y == 2, "Y value mismatch: expected 2, got " .. v.Y)
assert(v.Z == 3, "Z value mismatch: expected 3, got " .. v.Z)
v.X = 5
v.Y = 6
v.Z = 7
assert(v.X == 5, "X value mismatch after set: expected 5, got " .. v.X)
assert(v.Y == 6, "Y value mismatch after set: expected 6, got " .. v.Y)
assert(v.Z == 7, "Z value mismatch after set: expected 7, got " .. v.Z)

--Test addition and subtraction
local sum = v + u
assert(sum.X == 7 and sum.Y == 10 and sum.Z == 13,
    "Addition failed: expected (7, 10, 13), got (" .. sum.X .. ", " .. sum.Y .. ", " .. sum.Z .. ")")

local diff = v - u
assert(diff.X == 3 and diff.Y == 2 and diff.Z == 1,
    "Subtraction failed: expected (3, 2, 1), got (" .. diff.X .. ", " .. diff.Y .. ", " .. diff.Z .. ")")

--Test multiplication and division
local scalar = 2
local product = v * scalar
assert(product.X == 10 and product.Y == 12 and product.Z == 14,
    "Multiplication failed: expected (10, 12, 14), got (" .. product.X .. ", " .. product.Y .. ", " .. product.Z .. ")")

local quotient = v / scalar
assert(quotient.X == 2.5 and quotient.Y == 3 and quotient.Z == 3.5,
    "Division failed: expected (2.5, 3, 3.5), got (" .. quotient.X .. ", " .. quotient.Y .. ", " .. quotient.Z .. ")")

--Test length and normalization
local length = v:Length()
local expectedLength = math.sqrt(5 ^ 2 + 6 ^ 2 + 7 ^ 2)
assert(math.abs(length - expectedLength) < 0.001,
    "Length calculation failed: expected approximately " .. expectedLength .. ", got " .. length)

local normalized = v:Normalize()
local epsilon = 0.0001
local expectedX, expectedY, expectedZ = 5 / length, 6 / length, 7 / length
assert(
    math.abs(normalized.X - expectedX) < epsilon and
    math.abs(normalized.Y - expectedY) < epsilon and
    math.abs(normalized.Z - expectedZ) < epsilon,
    string.format(
        "Normalization failed: expected (%.6f, %.6f, %.6f), got (%.6f, %.6f, %.6f)",
        expectedX, expectedY, expectedZ, normalized.X, normalized.Y, normalized.Z
    )
)

-- Reset u, v
local w = Vector3.new(1, 2, 3)
local q = Vector3.new(-1, 5, 9)

local dot = w:Dot(q)
assert(dot == 1 * -1 + 2 * 5 + 3 * 9,
    "Dot product failed: expected " .. (1 * -1 + 2 * 5 + 3 * 9) .. ", got " .. dot)

local cross = w:Cross(q)
assert(cross.X == 2 * 9 - 3 * 5 and cross.Y == 3 * -1 - 1 * 9 and cross.Z == 1 * 5 - 2 * -1,
    "Cross product failed: expected (" ..
    (2 * 9 - 3 * 5) ..
    ", " ..
    (3 * -1 - 1 * 9) .. ", " .. (1 * 5 - 2 * -1) .. "), got (" .. cross.X .. ", " .. cross.Y .. ", " .. cross.Z .. ")")

print("All Vector3 tests passed!")
