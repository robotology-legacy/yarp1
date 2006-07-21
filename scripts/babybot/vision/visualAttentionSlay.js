var WshShell = WScript.CreateObject("WScript.Shell");

WshShell.Run ("visualAttentionDisconnect.js", 7, true);
WshShell.Run ("visualAttentionToSaccadesDisconnect.js", 7, true);

WshShell.Run ("slay -n scilla visualattention.exe", 7, true);
