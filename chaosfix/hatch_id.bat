@echo off
rem (c)1996 xChaos software - file_id.diz extractor / maker

if (%1)==() goto help
if (%2)==() goto help

if exist file_id.diz del file_id.diz

rem ---------------------- add your own extensions ! ------------------------

if %2==ARJ goto arj
if %2==ZIP goto zip
if %2==LZH goto lha
if %2==RAR goto rar

edit FILE_ID.DIZ

goto end2

:help
echo.
echo Hatch_id.Bat - FILE_ID.DIZ extractor, (c)1996 xChaos software
echo 컴컴컴컴컴컴
echo.
echo Syntax: HATCH_ID (full path & filename) (supported extension)
echo INTERACTIVE VERSION - For more info, edit HATCH_ID.BAT :-)
goto end2

rem ---------------------- add your own extensions ! -------------------------

:arj
arj e %1 file_id.diz
if exist file_id.diz goto end
edit FILE_ID.DIZ
arj a %1 file_id.diz
goto end2

:zip
rem (forced PC-XT mode !)
pkunzip %1 file_id.diz -3 -+ -- -)
if exist file_id.diz goto end
edit FILE_ID.DIZ
pkzip %1 file_id.diz
goto end2

:lha
lha e %1 file_id.diz
if exist file_id.diz goto end
edit FILE_ID.DIZ
lha a %1 file_id.diz
goto end2

:rar
rar e -std %1 file_id.diz
if exist file_id.diz goto end
edit FILE_ID.DIZ
rar a -std %1 file_id.diz
goto end2

:end
edit file_id.diz
:end2