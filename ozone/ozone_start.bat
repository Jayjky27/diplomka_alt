@echo off
cd /D %~dp0
start "" "c:\program files\segger\ozone\ozone.exe" -project .\lorawan_stage.jdebug
