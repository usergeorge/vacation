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
if %2==GIF goto gif
if %2==JPG goto jpg
if %2==MPG goto mpg
if %2==TXT goto txt
if %2==EXE goto exe
if %2==COM goto com
if %2==BAT goto bat

goto end

:help
echo.
echo Fix_id.Bat - FILE_ID.DIZ extractor, (c)1996 xChaos software
echo 컴컴컴컴컴
echo.
echo Syntax: FIX_ID (full path & filename) (supported extension)
echo For more info, EDIT FIX_ID.BAT :-)
goto end

rem ---------------------- add your own extensions ! -------------------------

:arj
arj e %1 file_id.diz
if exist file_id.diz goto end
echo ** new ARJ archive adopted by ChaosFix **>file_id.diz
goto end

:zip
rem (forced PC-XT mode !)
pkunzip %1 file_id.diz -3 -+ -- -)
if exist file_id.diz goto end
echo ** new ZIP archive adopted by ChaosFix ** >file_id.diz
goto end

:lha
lha e %1 file_id.diz
if exist file_id.diz goto end
echo ** new LHA archive adopted by ChaosFix **>file_id.diz
goto end

:rar
rar e -std %1 file_id.diz
if exist file_id.diz goto end
echo ** new RAR archive adopted by ChaosFix **>file_id.diz
goto end

:gif
echo ** new GIF image adopted by ChaosFix **>file_id.diz
goto end

:jpg
echo ** new JPG image adopted by ChaosFix **>file_id.diz
goto end

:mpg
echo ** new MPG animation adopted by ChaosFix **>file_id.diz
goto end

:txt
echo ** new text file adopted by ChaosFix **>file_id.diz
goto end

:exe
:bat
:com
echo ** new program adopted by ChaosFix **>file_id.diz
goto end

:end
if exist file_id.diz type file_id.diz
