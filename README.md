# safeDialog

**safeDialog** is a PAWN include for SA-MP designed to prevent dialog spam from players. With a cooldown system, automatic spam detection, and temporary blocking, your server will be more secure against dialog abuse.

## Features

- **Dialog cooldown** – Default 2 seconds between dialogs.
- **Automatic spam detection** – Monitors the frequency of dialog openings.
- **Temporary blocking** – If spam is detected, the player is blocked for 30 seconds (configurable).
- **Dialog status tracking** – Know whether a player currently has a dialog open.
- **History analysis** – Stores the last 5 dialogs to detect spam patterns.

## Installation

1. Copy the `safeDialog.inc` file to your `pawno/include` folder.
2. Include it in your main script by adding the following line at the top:

```pawn
#include <safeDialog>
```

## Usage

### Showing a Dialog

Replace the native `ShowPlayerDialog` with `ShowPlayerDialogSafe`:

```pawn
ShowPlayerDialogSafe(playerid, DIALOG_ID, DIALOG_STYLE_INPUT, "Title", "Message", "Button 1", "Button 2");
```

### Closing a Dialog

Replace `ClosePlayerDialog` with `ClosePlayerDialogSafe`:

```pawn
ClosePlayerDialogSafe(playerid);
```

### Handling Dialog Responses

Use the `OnDialogResponseSafe` callback instead of `OnDialogResponse`:

```pawn
public OnDialogResponseSafe(playerid, dialogid, response, listitem, inputtext[])
{
    if (dialogid == DIALOG_LOGIN)
    {
        if (response)
        {
            // Process login input
        }
        else
        {
            // Player pressed cancel
        }
    }
    return 1; // Return 1 if handled
}
```

> **Note:** The original `OnDialogResponse` callback is still called, but it's recommended to use `OnDialogResponseSafe` for full integration with the cooldown system.

### Additional Callbacks

**OnDialogSpamDetected** – Called when a player is detected spamming dialogs.

```pawn
public OnDialogSpamDetected(playerid, dialogid)
{
    SendClientMessage(playerid, -1, "Dialog spam detected. Please wait a moment.");
    return 1;
}
```

**OnDialogBlocked** – Called when a player tries to open a dialog but is currently blocked.

```pawn
public OnDialogBlocked(playerid, remaining_time)
{
    new str[64];
    format(str, sizeof(str), "You must wait %d seconds before opening another dialog.", remaining_time / 1000);
    SendClientMessage(playerid, -1, str);
    return 1;
}
```

## Additional Functions

| Function                          | Description                                          |
|-----------------------------------|------------------------------------------------------|
| `IsPlayerDialogOpenSafe(playerid)`  | Checks if a player currently has a dialog open.      |
| `IsPlayerDialogBlocked(playerid)`   | Checks if a player is currently blocked from dialogs.|
| `GetDialogBlockTime(playerid)`      | Returns the remaining block time (in milliseconds).  |
| `ResetDialogSafe(playerid)`         | Resets the player's cooldown and block status.       |
| `GetPlayerLastDialog(playerid)`     | Gets the last dialog ID the player opened.           |

## Configuration

You can change the default values by defining the following constants before including `safeDialog.inc`:

```pawn
#define SD_COOLDOWN 2000        // Cooldown between dialogs (ms), default 2000
#define SD_HISTORY_SIZE 5       // Number of dialogs stored in history, default 5
#define SD_SPAM_LIMIT 3         // Spam detection threshold, default 3
#define SD_BLOCK_TIME 30000      // Block duration (ms), default 30000
#define SD_FAST_THRESHOLD 500    // Fast attempt threshold for spam detection (ms), default 500

#include <safeDialog>
```

| Constant            | Default | Description                                    |
|---------------------|---------|------------------------------------------------|
| `SD_COOLDOWN`       | 2000    | Minimum wait time between dialogs (ms).        |
| `SD_HISTORY_SIZE`   | 5       | Size of dialog history for spam analysis.      |
| `SD_SPAM_LIMIT`     | 3       | Maximum dialogs in a period before spam is triggered. |
| `SD_BLOCK_TIME`     | 30000   | Block duration after spam is detected (ms).    |
| `SD_FAST_THRESHOLD` | 500     | If a dialog is opened faster than this (ms), it counts as a fast attempt. |
