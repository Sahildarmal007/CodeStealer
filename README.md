Overview :

Lucifer Panel Cracker is a DLL-based tool designed for advanced memory manipulation, logging, and detection evasion. It hooks into WriteProcessMemory using MinHook to monitor memory changes and log them. Additionally, it employs anti-debugging techniques and bypasses DLL detection.

Features :

1. Memory Manipulation Detection: Logs memory modifications made using WriteProcessMemory.

2. Anti-Debugging Protection: Prevents execution in debugging environments.

3. DLL Detection Bypass: Erases PE headers to evade detection.

4. Remote Control: Checks external Pastebin content for activation status.

5. Stealth Logging: Logs data to Desktop with encrypted messages using skCrypter.

Prerequisites :

1. Windows OS

2. Visual Studio or any C++ compatible IDE

3. MinHook Library

4. WinINet Library

Installation :

1. Open the project in Visual Studio.

2. Ensure MinHook is properly set up.

3. Build the DLL.

Usage :

1. Inject the compiled DLL into the target process using your preferred injector.

2. Logs will be saved to C:\Windows\System32\Lucifer_Logs.txt.

3. Ensure administrative privileges are granted for file access.

Important Notice :

This tool is intended strictly for educational purposes, testing, and research. Unauthorized usage, especially for malicious purposes, is illegal and unethical.

Disclaimer :

The author takes no responsibility for any misuse or damages caused by this software. Use it responsibly within legal and ethical guidelines.

Contact :

Join the community or report issues on Discord: Celestial Mods

License :

This project is licensed under the MIT License. See LICENSE for details.
