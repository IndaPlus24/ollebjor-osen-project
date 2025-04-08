local LoonarDebug = {}

function LoonarDebug.PrintMetatable(t)
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

return LoonarDebug