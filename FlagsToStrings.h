typedef struct {
    DWORD  Flag;
    LPCSTR String;
} FlagStringPair;

static void FlagsToStrings(lua_State *L, const FlagStringPair Pairs[], DWORD Flags) {
   {int idx;
    for (idx = 0; Pairs[idx].String != NULL; idx++) {
        lua_pushboolean(L, (Flags & Pairs[idx].Flag));
        lua_setfield(L, -2, Pairs[idx].String);
    };};
    
    return;
};