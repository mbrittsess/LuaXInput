print "Checkpoint 1"
package.cpath = [[C:\Projects\?\?52.dll;C:\Projects\?\?.dll]]
print(package.cpath)
print "Checkpoint 2"
local xinput = require("LuaXInput")
print "Checkpoint 3"
local wincon = require("LuaWinCon")
print "Checkpoint 4"

local Title = [[XInput Test #2]]

wincon.SetConsoleSize({25,11})
wincon.SetTitle(Title)

--Blank out the screen
wincon.FillRegion({1,1},{25,1},{1,16},' ') --Titlebar
wincon.FillRegion({1,2},{25,10},{16,1},' ') --Body

--Write the title to the in-console titlebar, centered
--Note: wincon.WriteRun() had not been written when this test script was written.
wincon.WriteRegion({1+math.floor((25 - #Title:sub(1,25))/2),1}, {25,1}, Title:sub(1,25))

--Write the label
wincon.WriteRegion({2,10},{13,2},"XInput       " ..
                                 "Controller #1")

local base, highlight = {16,1}, {1,16}
--None of the Unicode or UTF-8 libraries had been written when this test script was written.
local left_arrow  = "\226\151\132" --U+25C4, BLACK LEFT-POINTING POINTER
local right_arrow = "\226\150\186" --U+25BA, BLACK RIGHT-POINTING POINTER
local up_arrow    = "\226\150\178" --U+25B2, BLACK UP-POINTING TRIANGLE
local down_arrow  = "\226\150\188" --U+25BC, BLACK DOWN-POINTING TRIANGLE

local prev_stamp  = nil
while true do
    local curr_state = xinput[1]:GetState()
    if curr_state.stamp ~= prev_stamp then
        --Didn't bother to optimize how many writes we make or times we call. Not for this test.
        
        --l_trigger
         wincon.WriteRegion({3 ,3},{4,1},string.format("%.3u%%", math.floor(curr_state.l_trigger * 100)))
        --r_trigger
         wincon.WriteRegion({11,3},{4,1},string.format("%.3u%%", math.floor(curr_state.r_trigger * 100)))
        --lb
         wincon.WriteRegion({4 ,4},{2,1},curr_state.lb and highlight or base, "LB")
        --rb
         wincon.WriteRegion({12,4},{2,1},curr_state.rb and highlight or base, "RB")
        --l3
         wincon.WriteRegion({4 ,6},{2,1},curr_state.l3 and highlight or base, "L3")
        --r3
         wincon.WriteRegion({11,8},{2,1},curr_state.r3 and highlight or base, "R3")
        --select
         wincon.WriteRegion({8 ,6},{1,1},curr_state.select and highlight or base, left_arrow)
        --start
         wincon.WriteRegion({10,6},{1,1},curr_state.start  and highlight or base, right_arrow)
        --y
         wincon.WriteRegion({13,5},{1,1},curr_state.y  and highlight or base, "Y")
        --b
         wincon.WriteRegion({14,6},{1,1},curr_state.b  and highlight or base, "B")
        --x
         wincon.WriteRegion({12,6},{1,1},curr_state.x  and highlight or base, "X")
        --a
         wincon.WriteRegion({13,7},{1,1},curr_state.a  and highlight or base, "A")
        --d_up
         wincon.WriteRegion({6 ,7},{1,1},curr_state.d_up    and highlight or base, up_arrow)
        --d_right
         wincon.WriteRegion({7 ,8},{1,1},curr_state.d_right and highlight or base, right_arrow)
        --d_left
         wincon.WriteRegion({5 ,8},{1,1},curr_state.d_left  and highlight or base, left_arrow)
        --d_down
         wincon.WriteRegion({6 ,9},{1,1},curr_state.d_down  and highlight or base, down_arrow)
        --lx
         wincon.WriteRegion({16,4},{8,1},string.format("LX:%4.3d%%", math.floor(curr_state.lx * 100)))
        --ly
         wincon.WriteRegion({16,5},{8,1},string.format("LY:%4.3d%%", math.floor(curr_state.ly * 100)))
        --rx
         wincon.WriteRegion({16,7},{8,1},string.format("RX:%4.3d%%", math.floor(curr_state.rx * 100)))
        --ry
         wincon.WriteRegion({16,8},{8,1},string.format("RY:%4.3d%%", math.floor(curr_state.ry * 100)))
        
        prev_stamp = curr_state.stamp
    end
    
    if curr_state.select and curr_state.start then break end
end

wincon.FillRegion({1,1},{25,11},{16,1}," ")
wincon.SetConsoleSize({140,50})

os.exit()