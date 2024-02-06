@echo off

set TOOL_PATH=c:\toolchain\armv6-m_thumb_le\bin
set TOOL_PREF=arm-none-eabi-
set ARG=-a

set TOOL=%TOOL_PATH%\%TOOL_PREF%readelf.exe
echo Calling readelf: %TOOL%

"%TOOL%" %ARG% "%1" > "%1.readelf"
