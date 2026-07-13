@echo off
cd %~dp0
set make_cmd=make
set make_option=-s
set make_logfile=log.txt
set make_logout=..\..\logoutput.exe %make_logfile%

echo Cleaning ...

set make_target=clean
%make_cmd% %make_option% %make_target%
if exist %make_logfile% del /f %make_logfile%

echo Building ...

set make_target=target
%make_cmd% %make_option% %make_target% 2>>&1 | %make_logout%

echo %0
echo complete !

