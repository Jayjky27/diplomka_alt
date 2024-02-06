@echo off

set TOOL_PATH=c:\toolchain\armv6-m_thumb_le\bin
set TOOL_PREF=arm-none-eabi-
set ARG=-d -S

set TOOL=%TOOL_PATH%\%TOOL_PREF%objdump.exe
echo Calling objdump: %TOOL%

"%TOOL%" %ARG% "%1" > "%1.lst"
