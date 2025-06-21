@echo off
echo Building Archimedes Blockchain...
echo.

REM Try different compilers
where gcc >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Using GCC compiler...
    gcc -o archimed.exe main.c block.c pi.c utils.c wallet.c network.c performance.c menu.c -lws2_32 -std=c17 -Wall -Wextra
    if %ERRORLEVEL% == 0 (
        echo Build successful!
        echo.
        echo Running the program:
        echo.
        archimed.exe
    ) else (
        echo Build failed with GCC
    )
    goto :end
)

where cl >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Using Microsoft Visual C++ compiler...
    cl main.c block.c pi.c utils.c wallet.c network.c performance.c menu.c ws2_32.lib /Fe:archimed.exe
    if %ERRORLEVEL% == 0 (
        echo Build successful!
        echo.
        echo Running the program:
        echo.
        archimed.exe
    ) else (
        echo Build failed with MSVC
    )
    goto :end
)

where clang >nul 2>&1
if %ERRORLEVEL% == 0 (
    echo Using Clang compiler...
    clang -o archimed.exe main.c block.c pi.c utils.c wallet.c network.c performance.c menu.c -lws2_32 -std=c17 -Wall -Wextra
    if %ERRORLEVEL% == 0 (
        echo Build successful!
        echo.
        echo Running the program:
        echo.
        archimed.exe
    ) else (
        echo Build failed with Clang
    )
    goto :end
)

echo No compatible C compiler found!
echo Please install one of the following:
echo - GCC (MinGW/MSYS2)
echo - Microsoft Visual C++ (Visual Studio Build Tools)
echo - Clang (LLVM)

:end
pause
