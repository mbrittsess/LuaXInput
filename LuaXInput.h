#define UNICODE
#include <windows.h>
#include <xinput.h>
#include <lua.h>
#include <luaW.h>

#include "PushLastErrMsg.h"
#include "FlagsToStrings.h"

static const FlagStringPair ButtonFlags[] = {
    {XINPUT_GAMEPAD_DPAD_UP,        "D_Up"},
    {XINPUT_GAMEPAD_DPAD_DOWN,      "D_Down"},
    {XINPUT_GAMEPAD_DPAD_LEFT,      "D_Left"},
    {XINPUT_GAMEPAD_DPAD_RIGHT,     "D_Right"},
    {XINPUT_GAMEPAD_START,          "Start"},
    {XINPUT_GAMEPAD_BACK,           "Back"},
    {XINPUT_GAMEPAD_LEFT_THUMB,     "L3"},
    {XINPUT_GAMEPAD_RIGHT_THUMB,    "R3"},
    {XINPUT_GAMEPAD_LEFT_SHOULDER,  "LB"},
    {XINPUT_GAMEPAD_RIGHT_SHOULDER, "RB"},
    {XINPUT_GAMEPAD_A,              "A"},
    {XINPUT_GAMEPAD_B,              "B"},
    {XINPUT_GAMEPAD_X,              "X"},
    {XINPUT_GAMEPAD_Y,              "Y"},
    {0,                             NULL}};

static const FlagStringPair BatteryTypes[] = {
    {BATTERY_TYPE_DISCONNECTED,     "Disconnected"},
    {BATTERY_TYPE_WIRED,            "Wired"},
    {BATTERY_TYPE_ALKALINE,         "Alkaline"},
    {BATTERY_TYPE_NIMH,             "NickelMetalHydride"},
    {BATTERY_TYPE_UNKNOWN,          "Unknown"},
    {0,                             NULL}};
static const FlagStringPair BatteryLevels[] = {
    {BATTERY_LEVEL_EMPTY,           "Empty"},
    {BATTERY_LEVEL_LOW,             "Low"},
    {BATTERY_LEVEL_MEDIUM,          "Medium"},
    {BATTERY_LEVEL_FULL,            "Full"},
    {0,                             NULL}};

int luaopen_LuaXInput(lua_State *L);
static int lxi_Enable(lua_State *L);
static int lxi_GetInfoInit(lua_State *L);
static int lxi_GetInfo(lua_State *L);
static int lxi_GetStateInit(lua_State *L);
static int lxi_GetState(lua_State *L);