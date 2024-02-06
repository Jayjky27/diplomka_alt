@echo off

set TOOL_PATH=c:\toolchain\armv6-m_thumb_le\bin
set TOOL_PREF=arm-none-eabi-
set ARG=-A -d --common

set TOOL=%TOOL_PATH%\%TOOL_PREF%size.exe
echo Calling size: %TOOL%

"%TOOL%" %ARG% "%1"
