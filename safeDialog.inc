#if defined _safeDialog_included
    #endinput
#endif
#define _safeDialog_included

#include <a_samp>

#define SD_COOLDOWN 2000
#define SD_HISTORY_SIZE 5
#define SD_SPAM_LIMIT 3
#define SD_BLOCK_TIME 30000
#define SD_FAST_THRESHOLD 500

enum E_SD_DATA
{
    bool:E_SD_OPEN,
    E_SD_LAST_SHOW,
    E_SD_LAST_CLOSE,
    E_SD_HISTORY[SD_HISTORY_SIZE],
    E_SD_HISTORY_IDX,
    E_SD_SPAM_COUNT,
    E_SD_BLOCKED_UNTIL,
    E_SD_LAST_ID
}

static gSD[MAX_PLAYERS][E_SD_DATA];

forward OnDialogResponseSafe(playerid, dialogid, response, listitem, inputtext[]);
forward OnDialogSpamDetected(playerid, dialogid);
forward OnDialogBlocked(playerid, remaining_time);

public OnPlayerConnect(playerid)
{
    for(new i = 0; i < SD_HISTORY_SIZE; i++) 
    {
        gSD[playerid][E_SD_HISTORY][i] = 0;
    }
    
    gSD[playerid][E_SD_HISTORY_IDX] = 0;
    gSD[playerid][E_SD_SPAM_COUNT] = 0;
    gSD[playerid][E_SD_BLOCKED_UNTIL] = 0;
    gSD[playerid][E_SD_OPEN] = false;
    gSD[playerid][E_SD_LAST_SHOW] = 0;
    gSD[playerid][E_SD_LAST_CLOSE] = 0;
    gSD[playerid][E_SD_LAST_ID] = -1;
    
    #if defined SD_OnPlayerConnect
        return SD_OnPlayerConnect(playerid);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif
#define OnPlayerConnect SD_OnPlayerConnect

#if defined SD_OnPlayerConnect
    forward SD_OnPlayerConnect(playerid);
#endif

public OnPlayerDisconnect(playerid, reason)
{
    #if defined SD_OnPlayerDisconnect
        return SD_OnPlayerDisconnect(playerid, reason);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect SD_OnPlayerDisconnect

#if defined SD_OnPlayerDisconnect
    forward SD_OnPlayerDisconnect(playerid, reason);
#endif

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    gSD[playerid][E_SD_OPEN] = false;
    gSD[playerid][E_SD_LAST_CLOSE] = GetTickCount();
    gSD[playerid][E_SD_SPAM_COUNT] = 0;
    
    return CallLocalFunction("OnDialogResponseSafe", "iiiis", playerid, dialogid, response, listitem, inputtext);
}

#if defined _ALS_OnDialogResponse
    #undef OnDialogResponse
#else
    #define _ALS_OnDialogResponse
#endif
#define OnDialogResponse SD_OnDialogResponse

forward SD_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);

public OnPlayerUpdate(playerid)
{
    if(gSD[playerid][E_SD_BLOCKED_UNTIL] > 0 && GetTickCount() > gSD[playerid][E_SD_BLOCKED_UNTIL])
    {
        gSD[playerid][E_SD_BLOCKED_UNTIL] = 0;
        gSD[playerid][E_SD_SPAM_COUNT] = 0;
        
        for(new i = 0; i < SD_HISTORY_SIZE; i++) 
        {
            gSD[playerid][E_SD_HISTORY][i] = 0;
        }
    }
    
    #if defined SD_OnPlayerUpdate
        return SD_OnPlayerUpdate(playerid);
    #else
        return 1;
    #endif
}

#if defined _ALS_OnPlayerUpdate
    #undef OnPlayerUpdate
#else
    #define _ALS_OnPlayerUpdate
#endif
#define OnPlayerUpdate SD_OnPlayerUpdate

#if defined SD_OnPlayerUpdate
    forward SD_OnPlayerUpdate(playerid);
#endif

stock ShowPlayerDialogSafe(playerid, dialogid, style, caption[], info[], button1[], button2[])
{
    if(!IsPlayerConnected(playerid)) 
    {
        return 0;
    }
    
    new ct = GetTickCount();
    
    if(gSD[playerid][E_SD_BLOCKED_UNTIL] > ct)
    {
        CallLocalFunction("OnDialogBlocked", "ii", playerid, gSD[playerid][E_SD_BLOCKED_UNTIL] - ct);
        return 0;
    }
    
    if(gSD[playerid][E_SD_OPEN])
    {
        if(ct - gSD[playerid][E_SD_LAST_SHOW] < 100)
        {
            gSD[playerid][E_SD_SPAM_COUNT]++;
            
            if(gSD[playerid][E_SD_SPAM_COUNT] >= SD_SPAM_LIMIT)
            {
                gSD[playerid][E_SD_BLOCKED_UNTIL] = ct + SD_BLOCK_TIME;
                CallLocalFunction("OnDialogSpamDetected", "ii", playerid, dialogid);
                return 0;
            }
        }
        else 
        {
            gSD[playerid][E_SD_SPAM_COUNT] = 0;
        }
        
        ClosePlayerDialog(playerid);
    }
    
    if(ct - gSD[playerid][E_SD_LAST_CLOSE] < SD_COOLDOWN) 
    {
        return 0;
    }
    
    new diff = ct - gSD[playerid][E_SD_LAST_SHOW];
    
    gSD[playerid][E_SD_HISTORY][gSD[playerid][E_SD_HISTORY_IDX]] = diff;
    gSD[playerid][E_SD_HISTORY_IDX] = (gSD[playerid][E_SD_HISTORY_IDX] + 1) % SD_HISTORY_SIZE;
    
    new fast = 0;
    
    for(new i = 0; i < SD_HISTORY_SIZE; i++)
    {
        if(gSD[playerid][E_SD_HISTORY][i] > 0 && gSD[playerid][E_SD_HISTORY][i] < SD_FAST_THRESHOLD) 
        {
            fast++;
        }
    }
    
    if(fast >= SD_SPAM_LIMIT)
    {
        gSD[playerid][E_SD_BLOCKED_UNTIL] = ct + SD_BLOCK_TIME;
        
        for(new i = 0; i < SD_HISTORY_SIZE; i++) 
        {
            gSD[playerid][E_SD_HISTORY][i] = 0;
        }
        
        CallLocalFunction("OnDialogSpamDetected", "ii", playerid, dialogid);
        return 0;
    }
    
    gSD[playerid][E_SD_OPEN] = true;
    gSD[playerid][E_SD_LAST_SHOW] = ct;
    gSD[playerid][E_SD_LAST_ID] = dialogid;
    
    return ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
}

stock ClosePlayerDialogSafe(playerid)
{
    if(!IsPlayerConnected(playerid) || !gSD[playerid][E_SD_OPEN]) 
    {
        return 0;
    }
    
    gSD[playerid][E_SD_OPEN] = false;
    gSD[playerid][E_SD_LAST_CLOSE] = GetTickCount();
    gSD[playerid][E_SD_SPAM_COUNT] = 0;
    
    return ClosePlayerDialog(playerid);
}

stock IsPlayerDialogOpenSafe(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return 0;
    }
    
    return gSD[playerid][E_SD_OPEN] ? 1 : 0;
}

stock IsPlayerDialogBlocked(playerid)
{
    if(!IsPlayerConnected(playerid)) 
    {
        return 0;
    }
    
    new ct = GetTickCount();
    
    return gSD[playerid][E_SD_BLOCKED_UNTIL] > ct ? 1 : 0;
}

stock GetDialogBlockTime(playerid)
{
    if(!IsPlayerConnected(playerid)) 
    {
        return 0;
    }
    
    new ct = GetTickCount();
    
    if(gSD[playerid][E_SD_BLOCKED_UNTIL] > ct)
    {
        return gSD[playerid][E_SD_BLOCKED_UNTIL] - ct;
    }
    
    return 0;
}

stock ResetDialogSafe(playerid)
{
    if(!IsPlayerConnected(playerid)) 
    {
        return 0;
    }
    
    gSD[playerid][E_SD_BLOCKED_UNTIL] = 0;
    gSD[playerid][E_SD_SPAM_COUNT] = 0;
    gSD[playerid][E_SD_LAST_SHOW] = 0;
    gSD[playerid][E_SD_LAST_CLOSE] = 0;
    
    for(new i = 0; i < SD_HISTORY_SIZE; i++) 
    {
        gSD[playerid][E_SD_HISTORY][i] = 0;
    }
    
    return 1;
}

stock GetPlayerLastDialog(playerid)
{
    if(!IsPlayerConnected(playerid))
    {
        return -1;
    }
    
    return gSD[playerid][E_SD_LAST_ID];
}
