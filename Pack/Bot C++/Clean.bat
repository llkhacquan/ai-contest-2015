@echo off

echo Cleaning 2013_Debug
if exist 2013_Debug rmdir /s /q 2013_Debug

echo Cleaning 2013_Release
if exist 2013_Release rmdir /s /q 2013_Release

echo Cleaning 2013_Debug
if exist AI_Template\2013_Debug rmdir /s /q AI_Template\2013_Debug

echo Cleaning 2013_Release
if exist AI_Template\2013_Release rmdir /s /q AI_Template\2013_Release



echo Cleaning *.suo
if exist *.suo del "*.suo"

echo Cleaning *.sdf
if exist *.sdf del "*.sdf"

