local controller1 = require("LuaXInput")[1]
while true do
    os.execute("cls") --Clear the screen
    for k,v in pairs(controller1:GetState()) do
        print(k, v)
    end
end