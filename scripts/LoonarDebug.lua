print("Lua execution started [Version " .. Version .. "]")
--Do something

local p = Primitive.new()
Window:SetTitle("Loonar")

Window.Minimized:Connect(function()
    print("Window Minimized!")
    math.sqrt(-1)
    error("Please error")
    print("Print mee")
end)

print("Lua executed successfully")
