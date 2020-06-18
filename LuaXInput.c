#include "LuaXInput.h"
#define lua_print(L, s) lua_getglobal(L, "print"); lua_pushliteral(L, s); lua_call(L, 1, 0)

static void StackDump(lua_State *L, int linnum) {
    lua_print(L, "Entering StackDump in LuaXInput");
    //lua_getglobal(L, "print");
    //lua_pushfstring(L, "Dump of stack at line %d:\n", linnum);
    //lua_call(L, 1, 0);
    
    {int i;
     char* tostring = NULL;
    for (i = -1; i >= -(lua_gettop(L)); i--) {
        lua_pushvalue(L, i); tostring = (char*)lua_tostring(L, -1); lua_pop(L, 1);
        lua_getglobal(L, "print"); lua_pushfstring(L, "[%d] (%s) %s\n", lua_gettop(L)+i+1, lua_typename(L, lua_type(L, i)), tostring);
    };};
    
    return;
};

int luaopen_LuaXInput(lua_State *L) {
    lua_newtable(L);
    lua_pushcclosure(L, lxi_Enable,  0); lua_setfield(L, -2, "Enable");
    lua_pushcclosure(L, lxi_GetInfo, lxi_GetInfoInit(L)); lua_setfield(L, -2, "GetInfo");
    lua_pushcclosure(L, lxi_GetState, lxi_GetStateInit(L)); lua_setfield(L, -2, "GetState");
    
    return 1;
};

static int lxi_Enable(lua_State *L) {
    XInputEnable(lua_isnone(L, 1) ? TRUE : lua_toboolean(L, 1));
    
    return 0;
};

static int lxi_GetInfoInit(lua_State *L) {
    static const FlagStringPair TypePairs[] = {
        {XINPUT_DEVTYPE_GAMEPAD,        "Gamepad"},
        {0,                             NULL}};
    static const FlagStringPair SubTypePairs[] = {
        {XINPUT_DEVSUBTYPE_GAMEPAD,     "Gamepad"},
        {XINPUT_DEVSUBTYPE_WHEEL,       "Wheel"},
        {XINPUT_DEVSUBTYPE_ARCADE_STICK,"ArcadeStick"},
        {XINPUT_DEVSUBTYPE_FLIGHT_SICK, "FlightStick"}, /* It really does need to be "FLIGHT_SICK", that's what it's called in XInput.h */
        {XINPUT_DEVSUBTYPE_DANCE_PAD,   "DancePad"},
        {XINPUT_DEVSUBTYPE_GUITAR,      "Guitar"},
        {XINPUT_DEVSUBTYPE_DRUM_KIT,    "DrumKit"},
        {0,                             NULL}};
    
    /* Create the Type String Table */
    lua_newtable(L);
   {int i;
    for (i = 0; TypePairs[i].String != NULL; i++) {
        lua_pushstring(L, TypePairs[i].String);
        lua_rawseti(L, -2, TypePairs[i].Flag);
    };};
    lua_newtable(L);
    luaL_loadstring(L, "return [[Unknown]]");
    lua_setfield(L, -2, "__index");
    lua_pushvalue(L, -1); /* Make a copy, we'll re-use it for the SubType table */
    lua_setmetatable(L, -3);
    
    /* Create the SubType String Table */
    lua_newtable(L);
   {int i;
    for (i = 0; SubTypePairs[i].String != NULL; i++) {
        lua_pushstring(L, SubTypePairs[i].String);
        lua_rawseti(L, -2, SubTypePairs[i].Flag);
    };};
    lua_insert(L, -2);
    lua_setmetatable(L, -2);
    
    /* Create the BatteryTypes String Table */
    lua_newtable(L);
   {int i;
    for (i = 0; BatteryTypes[i].String != NULL; i++) {
        lua_pushstring(L, BatteryTypes[i].String);
        lua_rawseti(L, -2, BatteryTypes[i].Flag);
    };};
    
    /* Create the BatteryLevels String Table */
    lua_newtable(L);
   {int i;
    for (i = 0; BatteryLevels[i].String != NULL; i++) {
        lua_pushstring(L, BatteryLevels[i].String);
        lua_rawseti(L, -2, BatteryLevels[i].Flag);
    };};
    
    return 4; /* The number of upvalues created... */
};
static int lxi_GetInfo(lua_State *L) {
    static const int TypeStringTbl         = lua_upvalueindex(1);
    static const int SubTypeStringTbl      = lua_upvalueindex(2);
    static const int BatteryTypeStringTbl  = lua_upvalueindex(3);
    static const int BatteryLevelStringTbl = lua_upvalueindex(4);
    int ControllerIdx;
    
    /* Check arguments */
    lua_settop(L, 1);
    if (!lua_isnumber(L, 1)) {
        return luaL_error("bad argument #1 to 'LuaXInput.GetInfo' (number expected, got %s)", lua_typename(L, lua_type(L, 1)));
    } else {
        ControllerIdx = lua_tointeger(L, 1);
    };
    
    if ((ControllerIdx < 1) || (ControllerIdx > XUSER_MAX_COUNT)) {
        return luaL_error("bad argument #1 to 'LuaXInput.GetInfo' (invalid controller index)");
    };
    
    /* Check status, push error message if necessary. */
   {XINPUT_CAPABILITIES XICaps;
    {int Status = XInputGetCapabilities(ControllerIdx-1, 0, &XICaps);
     switch (Status) {
        case ERROR_SUCCESS :
            break;
        case ERROR_DEVICE_NOT_CONNECTED :
            lua_pushnil(L);
            lua_pushfstring(L, "Controller #%d not connected", ControllerIdx);
            return 2;
        default:
            lua_pushnil(L);
            PushLastErrorMsg(L, Status, TRUE);
            return 2;
    };};
    
    /* Start pushing capability info */
    lua_newtable(L);
    
    /* First, type and subtype */
    lua_pushinteger(L, (lua_Integer)XICaps.Type);
    lua_gettable(L, TypeStringTbl);
    lua_setfield(L, -2, "Type");
    
    lua_pushinteger(L, (lua_Integer)XICaps.SubType);
    lua_gettable(L, SubTypeStringTbl);
    lua_setfield(L, -2, "SubType");
    
    /* Then various miscellaneous capabilities */
   {static FlagStringPair CapsFlags[] = {
        {XINPUT_CAPS_VOICE_SUPPORTED, "SupportsVoice"},          /* My headers do not currently list these other ones. :S
        {XINPUT_CAPS_FFB_SUPPORTED,   "SupportsRumble"},         **
        {XINPUT_CAPS_WIRELESS,        "IsWireless"},             **
        {XINPUT_CAPS_PMD_SUPPORTED,   "SupportsPlugInModules"},  **
        {XINPUT_CAPS_NO_NAVIGATION,   "LacksNavigationButtons"}, */
        {0,                           NULL}};
    FlagsToStrings(L, CapsFlags, XICaps.Flags);
    };
    
    /* Now for the buttons, and axis & vibration precision */
    lua_newtable(L);
    FlagsToStrings(L, ButtonFlags, XICaps.Gamepad.wButtons);
    lua_setfield(L, -2, "Supported"); /* Button support */
    
    lua_newtable(L); /* NumValues */
    lua_newtable(L); /* ValueDelta */
    
   {static const struct {lua_Number Range; LPCSTR String;} AxisCaps[8] = {
        {1.0, "L_Trigger"},
        {1.0, "R_Trigger"},
        {2.0, "LX"},
        {2.0, "LY"},
        {2.0, "RX"},
        {2.0, "RY"},
        {2.0, "LoFreq"},
        {2.0, "HiFreq"}};
    unsigned int BaseVals[8] = {
        (unsigned int)XICaps.Gamepad.bLeftTrigger,
        (unsigned int)XICaps.Gamepad.bRightTrigger,
        (unsigned int)((unsigned short)XICaps.Gamepad.sThumbLX),
        (unsigned int)((unsigned short)XICaps.Gamepad.sThumbLY),
        (unsigned int)((unsigned short)XICaps.Gamepad.sThumbRX),
        (unsigned int)((unsigned short)XICaps.Gamepad.sThumbRY),
        (unsigned int)XICaps.Vibration.wLeftMotorSpeed,
        (unsigned int)XICaps.Vibration.wRightMotorSpeed};
    int i;
    for (i = 0; i < 8; i++) {
        unsigned int ThisVal = BaseVals[i];
        while ((ThisVal & 1) != 1) {
            ThisVal >>= 1;
        };
        lua_pushinteger(L, ThisVal + 1);
        lua_setfield(L, -3, AxisCaps[i].String);
        lua_pushnumber(L, AxisCaps[i].Range / (lua_Number)ThisVal);
        lua_setfield(L, -2, AxisCaps[i].String);
    };
    
    lua_setfield(L, -3, "ValueDelta");
    lua_setfield(L, -2, "NumValues");
    };};
    
    /* Now for the battery info. */
   {XINPUT_BATTERY_INFORMATION GamepadBattery;
    XINPUT_BATTERY_INFORMATION HeadsetBattery;
    lua_newtable(L);
    
    /* Start with the gamepad. */
    lua_newtable(L);
    if (XInputGetBatteryInformation(ControllerIdx-1, BATTERY_DEVTYPE_GAMEPAD, &GamepadBattery) == ERROR_SUCCESS) {
        lua_pushinteger(L, (lua_Integer)GamepadBattery.BatteryType);
        lua_gettable(L, BatteryTypeStringTbl);
        lua_setfield(L, -2, "Type");
        switch (GamepadBattery.BatteryType) {
            case BATTERY_TYPE_ALKALINE :
            case BATTERY_TYPE_NIMH :
                lua_rawgeti(L, BatteryLevelStringTbl, GamepadBattery.BatteryLevel);
                lua_setfield(L, -2, "Level");
                break;
            default :
                lua_pushnil(L);
                lua_setfield(L, -2, "Level");
        };
    };
    lua_setfield(L, -2, "Gamepad");
    
    /* Now for the headset. */
    lua_newtable(L);
    if (XInputGetBatteryInformation(ControllerIdx-1, BATTERY_DEVTYPE_HEADSET, &HeadsetBattery) == ERROR_SUCCESS) {
        lua_rawgeti(L, BatteryTypeStringTbl, HeadsetBattery.BatteryType);
        lua_setfield(L, -2, "Type");
        switch (HeadsetBattery.BatteryType) {
            case BATTERY_TYPE_ALKALINE :
            case BATTERY_TYPE_NIMH :
                lua_rawgeti(L, BatteryLevelStringTbl, GamepadBattery.BatteryLevel);
                lua_setfield(L, -2, "Level");
                break;
            default :
                lua_pushnil(L);
                lua_setfield(L, -2, "Level");
        };
    };
    lua_setfield(L, -2, "Headset");
    
    lua_setfield(L, -2, "Battery");
    };
    
    /* And now we're done. */
    return 1;
};

static int lxi_GetStateInit(lua_State *L) {
    return 0;
};
static int lxi_GetState(lua_State *L) {
    XINPUT_STATE State;
    int ControllerIdx;
    static BOOLEAN FirstTime = TRUE;
    
    /* Check arguments */
    lua_settop(L, 1);
    if (!lua_isnumber(L, 1)) {
        return luaL_error("bad argument #1 to 'LuaXInput.GetState' (number expected, got %s)", lua_typename(L, lua_type(L, 1)));
    } else {
        ControllerIdx = lua_tointeger(L, 1);
    };
    
    if ((ControllerIdx < 1) || (ControllerIdx > XUSER_MAX_COUNT)) {
        return luaL_error("bad argument #1 to 'LuaXInput.GetState' (invalid controller index)");
    };
    
    {int Status = XInputGetState(ControllerIdx-1, &State);
     switch (Status) {
        case ERROR_SUCCESS :
            break;
        case ERROR_DEVICE_NOT_CONNECTED :
            lua_pushnil(L);
            lua_pushfstring(L, "Controller #%d not connected", ControllerIdx);
            return 2;
        default:
            lua_pushnil(L);
            PushLastErrorMsg(L, Status, TRUE);
            return 2;
    };};
    
    { /* Start filling in the table. */
        lua_newtable(L);
        
        FlagsToStrings(L, ButtonFlags, State.Gamepad.wButtons);
        
        lua_pushnumber(L, ((double)State.Gamepad.bLeftTrigger) / 255.0);  lua_setfield(L, -2, "L_Trigger");
        lua_pushnumber(L, ((double)State.Gamepad.bRightTrigger) / 255.0); lua_setfield(L, -2, "R_Trigger");
        lua_pushnumber(L, (State.Gamepad.sThumbLX < 0) ? 
            (((double)State.Gamepad.sThumbLX) / 32768.0) :
            (((double)State.Gamepad.sThumbLX) / 32767.0));                lua_setfield(L, -2, "LX");
        lua_pushnumber(L, (State.Gamepad.sThumbLY < 0) ? 
            (((double)State.Gamepad.sThumbLY) / 32768.0) :
            (((double)State.Gamepad.sThumbLY) / 32767.0));                lua_setfield(L, -2, "LY");
        lua_pushnumber(L, (State.Gamepad.sThumbRX < 0) ? 
            (((double)State.Gamepad.sThumbRX) / 32768.0) :
            (((double)State.Gamepad.sThumbRX) / 32767.0));                lua_setfield(L, -2, "RX");
        lua_pushnumber(L, (State.Gamepad.sThumbRY < 0) ? 
            (((double)State.Gamepad.sThumbRY) / 32768.0) :
            (((double)State.Gamepad.sThumbRY) / 32767.0));                lua_setfield(L, -2, "RY");
        
        return 1;
    };
};