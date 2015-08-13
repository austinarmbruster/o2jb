echo off
rem make_installer.bat

set inFile=o2jb.wxs
set objFile=o2jb.wixobj
set msiFile=o2jb.msi

if exist %msiFile% (
	echo "Removing file %msiFile%"
	del %msiFile%
) else (
	echo "No file to remove"
)

if exist %objFile% (
	echo "Removing file %objFile%"
	del %objFile%
) else (
	echo "No file to remove"
)

candle %inFile%

if %ERRORLEVEL% NEQ 0 (
	echo "Failed to create the object file"
) else (
	light %objFile%
)
