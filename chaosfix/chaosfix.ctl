; ChaosFix SAMPLE configuration file, (c)1996 xChaos software
;-----------------------------------------------------------------------------
; * SETUP section * maintained ONLY by USER * '�' ... value MUST be defined !
; single keywords: Address,Sysop,Directories,Passwords,Inbound,Netmail,New
; listed keywords: AKA,Uplink
;-----------------------------------------------------------------------------
Address 2:420/85.0               ;�Your main adress - must be defined !
AKA 128:888/0.0                  ;�Your AKA's (unlimited number, optional)
Sysop Ferda Mravenec             ;�Your (syspop's) full name (optional)
Directories C:\T-MAIL\DIR.FRQ    ;�Full name of DIR.FRQ file (optional)
Passwords C:\T-MAIL\PASSWORD.LST ;�Full name of PASSWORD.LST - must exist !
Inbound C:\T-MAIL\IN             ;�Inbound files & *.TIC directory
Netmail C:\T-MAIL\NETMAIL        ;�Fido *.MSG netmail directory
New C:\T-MAIL\FILES\NEWAREAS     ;�Directory for auto-added areas (optional)
Uplink 2:420/100.0               ;�*.TIC files from uplinks can create new
Uplink 128:666/100.0             ; 'Echo' areas (unlimited umber, optional)
;-----------------------------------------------------------------------------
; * Extended SETUP * maintained only by user * only OPTIONAL variables
; single keywords: CreatePath,NewAreaType,CommentFiles,UpdateFrq,LogFile
; listed keywords: Friend,FreeString,PrivateString,EchoString,TransitString
;-----------------------------------------------------------------------------
;Friend 2:420/85.13              ;�Friends can connect & %LIST 'Private' areas
;Friend 128:888/13               ; as 'Free' (unlimited number, optional)
;CreatePath ChaosFix             ;�'No'(default)/'ChaosFix'/'T-Fix'/'Allfix'
;UpdateFrq C:\T-MAIL\DIR.FRQ     ;�Update freq list when using 'CreatePath'
;NewAreaType Private             ;�'Echo'(default)/'Free'/'Transit'/'Private'
;FreeString PECH.                ;�If new area name contains 'FreeString',it
;FreeString FREE.                ; will be 'Free' (unlimited number, optional)
;PrivateString PVT.              ;�for new 'Private' areas - see 'FreeString'
;EchoString .UF                  ;�for new 'Echo' areas - see 'FreeString'
;EchoString .42
;TransitString GAMES.            ;�for new 'Transit' areas - see 'FreeString'
;CommentFiles LDFN               ;�L-TIC LDesc,D-TIC Desc,F-FILE_ID.DIZ,N-None
;LogFile C:\T-MAIL\FIDO.LOG      ;�use this file as log (default CHAOSFIX.LOG)
;-----------------------------------------------------------------------------
; * AREAS section * maintained either by user OR by ChaosFix * keywords:
; single: Echo,Free,Transit,Private,Desc,Path,UseAKA,Exec,FilesBBS,AddComment
; listed: Rw,Ro,Sleeping,Inactive,Guest,NoMail
;--------------------------------------------------------------------------->>
Transit REGION42
Desc FidoNet - regionalni nodelist
Exec NODECOMP.BAT
Rw 2:420/100.0
Ro 2:420/85.13
Ro 2:420/85.99
NoMail 2:420/85.13
;-----------------------------------------------------------------------------
Transit POINT42
Desc FidoNet - regionalni pointlist
Exec NODECOMP.BAT
Rw 2:420/100.0
Ro 2:420/85.13
Ro 2:420/85.99
NoMail 2:420/85.13
;-----------------------------------------------------------------------------
Echo BBS.CSK
Desc Seznam ceskych a slovenskych BBS - TXT a DBF
Path 8
AddComment N
Rw 2:420/100.0
Ro 2:420/85.13
Ro 2:420/85.99
NoMail 2:420/85.13
;-----------------------------------------------------------------------------
Echo IKARIE
Desc Povidky z casopisu Ikarie
Path *LITERA
AddComment DF
Rw 2:420/100.0
;-----------------------------------------------------------------------------
Free PECH.S_MOD
Desc Pechnet.zvuky.mody (formaty MOD/XM/S3M...)
Path *SOUND
UseAKA 128:888/0.0
Rw 128:666/100.0
Rw 128:888/13
Rw 128:888/99
;-----------------------------------------------------------------------------
Free PECH.R_PIC_F
Desc PechNet.obrazky.fantasy
Path *OBRAZKY
UseAKA 128:888/0.0
Rw 128:666/100.0
Rw 128:888/13
Rw 128:888/99
;-----------------------------------------------------------------------------
Private PVT.TEST
Desc Testovaci oblast
Path C:\T-MAIL\FILES\TEST\
FilesBBS FILEZ.BBS
Guest 2:420/85.99
Rw 2:420/85.13
;-----------------------------------------------------------------------------
;12.3.1996
