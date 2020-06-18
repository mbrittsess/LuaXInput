/*  =======================================
        Best viewed at 140 columns.
   Created by the GMech Project, January 2012.
    =======================================*/

/* THOUGHTS ON IMPROVEMENTS
    I believe it might be worth it to supply the recommended deadzone values in normalized form to the users of the library,
to provide a consistent performance. We could filter it by default ourselves, and provide users the optino to set the deadzone
manually, however it is our job to provide only the raw input as best as we can, and leave it up the users of the library (or a middleman
library) to do any processing of that sort. */

#include <lua.h>
#include <lauxlib.h>
#include <windows.h>
#include <winerror.h>
#include <XInput.h>

static int DenyWrite(lua_State *L) {
    /* This function simply returns nil when it is called. */
    lua_pushnil(L);
    return 1; /* Returns nil */
};

static int GetType(lua_State *L) {
    XINPUT_CAPABILITIES Caps;
    if ( XInputGetCapabilities(
            (DWORD)lua_tointeger(L, lua_upvalueindex(1)),
            0,
            &Caps) != ERROR_SUCCESS ) {
        lua_pushnil(L);
    } else {
        switch(Caps.SubType) {
            case XINPUT_DEVSUBTYPE_GAMEPAD :
                lua_pushliteral(L, "GAMEPAD");          break; /* Seems this stuff is only available in Windows 8? Or something...
            case XINPUT_DEVSUBTYPE_WHEEL :
                lua_pushliteral(L, "WHEEL");            break;
            case XINPUT_DEVSUBTYPE_ARCADE_STICK :
                lua_pushliteral(L, "ARCADE_STICK");     break;
            case XINPUT_DEVSUBTYPE_FLIGHT_STICK :
                lua_pushliteral(L, "FLIGHT_STICK");     break;
            case XINPUT_DEVSUBTYPE_DANCE_PAD :
                lua_pushliteral(L, "DANCE_PAD");        break;
            case XINPUT_DEVSUBTYPE_GUITAR :
                lua_pushliteral(L, "GUITAR");           break;
            case XINPUT_DEVSUBTYPE_GUITAR_ALTERNATE :
                lua_pushliteral(L, "GUITAR_ALTERNATE"); break;
            case XINPUT_DEVSUBTYPE_GUITAR_BASS :
                lua_pushliteral(L, "GUITAR_BASS");      break;
            case XINPUT_DEVSUBTYPE_DRUM_KIT :
                lua_pushliteral(L, "DRUM_KIT");         break;
            case XINPUT_DEVSUBTYPE_ARCADE_PAD :
                lua_pushliteral(L, "ARCADE_PAD");       break; */
            default :
                lua_pushliteral(L, "GAMEPAD");          break; /* This behavior is required by the DirectX SDK */
        };
    };
    
    return 1; /* Returns string or nil */
};

static int IsConnected(lua_State *L) {
    XINPUT_STATE dummy; /* You can't pass NULL to XInputGetState or it returns with ERROR_BAD_ARGUMENTS */
    DWORD result = XInputGetState((DWORD)lua_tointeger(L, lua_upvalueindex(1)), &dummy);
    if (result == ERROR_DEVICE_NOT_CONNECTED) {
        lua_pushboolean(L, 0);
    } else {
        lua_pushboolean(L, 1);
    };
    
    return 1; /* Returns true or false */
};

static int SetVibration(lua_State *L) { /* Receives two Lua Numbers, should both be in the range 0.0..1.0 */
    DWORD result;
    lua_Number InLow; lua_Number InHigh; XINPUT_VIBRATION Vibes;
    WORD OutLow; WORD OutHigh;
    lua_remove(L, 1); /* All methods get a junk "self" parameter, which we will remove. */
    InLow = lua_tonumber(L, 1);
    InHigh = lua_tonumber(L, 2);
    
    if ( InLow < 0.0 ) {
        InLow = 0.0;
    } else if ( InLow > 1.0 ) {
        InLow = 1.0;
    };
    
    if ( InHigh < 0.0 ) {
        InHigh = 0.0;
    } else if ( InHigh > 1.0 ) {
        InHigh = 1.0;
    };
    
    Vibes.wLeftMotorSpeed  = (WORD)(InLow * 65535);
    Vibes.wRightMotorSpeed = (WORD)(InHigh * 65535);
    
    result = XInputSetState((DWORD)lua_tointeger(L, lua_upvalueindex(1)), &Vibes);
    if (result == ERROR_SUCCESS) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    };
    
    return 1; /* Returns true or false */
};

static int GetState(lua_State *L) {
    XINPUT_STATE State;
    if (XInputGetState((DWORD)lua_tointeger(L, lua_upvalueindex(1)), &State) != ERROR_SUCCESS) {
        State.dwPacketNumber = 0;
        State.Gamepad.wButtons = 0;
        State.Gamepad.bLeftTrigger = 0.0;
        State.Gamepad.bRightTrigger = 0.0;
        State.Gamepad.sThumbLX = 0.0;
        State.Gamepad.sThumbLY = 0.0;
        State.Gamepad.sThumbRX = 0.0;
        State.Gamepad.sThumbRY = 0.0;
    };
    
    lua_remove(L, 1); /* Delete our dummy parameter */
    
    lua_createtable(L, 0, 15); /* Stack Index 1 */
    
    lua_pushinteger(L, (lua_Integer)State.dwPacketNumber);
    lua_setfield(L, 1, "stamp");
    
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP));
    lua_setfield(L, 1, "d_up");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN));
    lua_setfield(L, 1, "d_down");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT));
    lua_setfield(L, 1, "d_left");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT));
    lua_setfield(L, 1, "d_right");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_START));
    lua_setfield(L, 1, "start");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_BACK));
    lua_setfield(L, 1, "select");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB));
    lua_setfield(L, 1, "l3");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB));
    lua_setfield(L, 1, "r3");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER));
    lua_setfield(L, 1, "lb");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER));
    lua_setfield(L, 1, "rb");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_A));
    lua_setfield(L, 1, "a");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_B));
    lua_setfield(L, 1, "b");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_X));
    lua_setfield(L, 1, "x");
    lua_pushboolean(L, (int)(State.Gamepad.wButtons & XINPUT_GAMEPAD_Y));
    lua_setfield(L, 1, "y");
    
    lua_pushnumber(L, (lua_Number)((double)State.Gamepad.bLeftTrigger / 255.0)); /* Convert the 0-255 BYTE value of bLeftTrigger to a
        double, divide it by 255, then convert it to a lua_Number */
    lua_setfield(L, 1, "l_trigger");
    lua_pushnumber(L, (lua_Number)((double)State.Gamepad.bRightTrigger / 255.0));
    lua_setfield(L, 1, "r_trigger");
    
    lua_pushnumber(L, (lua_Number)((double)State.Gamepad.sThumbLX / ((State.Gamepad.sThumbLX < 0) ? 32768 : 32767))); /* It should be noted, the range of values on a thumbstick
        is from -32768 to +3276_7_...so all-the-way-right (or -up) will not register as 1.0 for us. We could redo our calculations to be
        a bit more complex, however I feel this is not worth it since XBox 360 controllers have a circular range-of-motion, not a square
        one, and so the edge cases essentially don't exist. */
    lua_setfield(L, 1, "lx");
    lua_pushnumber(L, (lua_Number)((double)State.Gamepad.sThumbLY / ((State.Gamepad.sThumbLY < 0) ? 32768 : 32767)));
    lua_setfield(L, 1, "ly");
    lua_pushnumber(L, (lua_Number)((double)State.Gamepad.sThumbRX / ((State.Gamepad.sThumbRX < 0) ? 32768 : 32767)));
    lua_setfield(L, 1, "rx");
    lua_pushnumber(L, (lua_Number)((double)State.Gamepad.sThumbRY / ((State.Gamepad.sThumbRY < 0) ? 32768 : 32767)));
    lua_setfield(L, 1, "ry");
    
    return 1; /* Returns a table */
};

/* KEEP THESE TWO TOGETHER */
static const char* MethodNames[] = {
    "GetType",
    "IsConnected",
    "SetVibration",
    "GetState"
};
static const lua_CFunction MethodFuncs[] = {
    GetType,
    IsConnected,
    SetVibration,
    GetState
};

int luaopen_LuaXInput(lua_State *L) {
    /* First up, we'll create our metatable which makes shit read-only. */
    lua_settop(L, 0);
    lua_createtable(L, 0, 1);
    lua_pushcfunction(L, DenyWrite);
    lua_setfield(L, 1, "__newindex"); /* Metatable created, Stack Index 1 */
    
    /* Now we'll make our export table. */
    lua_createtable(L, 4, 0);         /* Export table created, Stack Index 2 */
    /* And now, to fill it... */
    { lua_Integer i;
    for (i = 1; i < 5; i++) {
        lua_pushinteger(L, i);        /* Will be our index into the export table, Stack Index 3 */
        lua_createtable(L, 0, 4);     /* XInput object created, Stack Index 4 */
        { int j;
        for (j = 0; j < 4; j++) {
            lua_pushstring(L, MethodNames[j]);
            lua_pushinteger(L, i-1);  /* Our functions will use this upvalue as their "Identity" */
            lua_pushcclosure(L, MethodFuncs[j], 1);
            lua_settable(L, 4);
        };};
        lua_pushvalue(L, 1);         /* Copy the metatable to the top of the stack */
        lua_setmetatable(L, 4);      /* And set the XInput object's metatable */
        lua_settable(L, 2);          /* Now set export[i] = XInputObject */
    };};
    lua_pushvalue(L, 1);
    lua_setmetatable(L, 2);          /* Now the export table's metatable is set */
    lua_settop(L, 2);                /* Now set the export table as the one to be returned */
    
    return 1; /* Returns a table */
};