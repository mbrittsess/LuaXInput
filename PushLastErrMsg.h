static void PushLastErrorMsg(lua_State *L, DWORD ErrCode, BOOLEAN UsePassedCode) {
    DWORD  ErrorCode = UsePassedCode ? ErrCode : GetLastError();
    LPWSTR ErrorMsg  = NULL;
    DWORD  MsgLen    = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        ErrorCode,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPWSTR)&ErrorMsg,
        0,
        NULL);
    
    if (MsgLen != 0) {
        luaW_pushlstring(L, ErrorMsg, MsgLen);
        LocalFree((HLOCAL)ErrorMsg);
    } else {
        lua_pushliteral(L, "Encountered an error while attempting to retrieve system error message.");
    };
    
    return;
};