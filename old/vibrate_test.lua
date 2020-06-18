local controller1 = require("LuaXInput")[1]
while true do
    local state = controller1:GetState()
    controller1:SetVibration(state.l_trigger, state.r_trigger)
end
os.exit()