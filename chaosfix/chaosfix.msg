! xchMSG message system (c)1996 xChaos software - first line MUST be comment !
WARNING "WARNING:"
! Between keyword and quote (character' " ') MUST be EXACTLY ONE space
! After the closing quote there MUST be end of the line character. ('\n')
! Messages can have more than 1 line. Maximum size is 1024 characters.
HELP "
Syntax: %s Fix           ... process inbound *.TIC files
        %s Mgr           ... process netmail *.MSG files
        %s Event         ... process inbound + netmail (1.Mgr 2.Fix)
        %s Hatch [file]  ... send file to fileecho (interactive)

"
CTL "Reading 'Chaosfix.Ctl'..."
ERRCTL "Error reading 'Chaosfix.Ctl' !"
SELFCHECK "Testing integrity of 'Chaosfix.Exe'..."
ERRCHECK "Error in file 'Chaosfix.Exe': was the program installed correctly ?"
ERRCTLLONG "Error: file 'Chaosfix.Ctl' is too long ! (maximum=32k)"
ERRCTLSETUP "Error in 'Chaosfix.Ctl' SETUP section: %s
"
ERRCTLAREAS "Error in 'Chaosfix.Ctl' AREAS section: %s
"
ERRKEYWORD "This keyword is not allowed in this section !"
ERRADR "Error: adress must be defined !"
READING "Reading '%s'...
"
ERRREAD "Error reading '%s' !"
ERRPWDLONG "Error: password file is too long ! (maximum=32k)"
ERRDIRLONG "Error: directories file is too long ! (maximum=32k)"
EVENT "þ Command: Event (Mgr+Fix)"
MGR "þ Command: Mgr"
NETMAIL "
Processing netmail messages..."
REQUEST "
Processing Allfix request %s:
From: %s (%s)
  To: %s (%s)"
QUERY "
Query report (list of connected areas) for %s (%s):

* ... active areas
+ ... passive areas
! ... you will not receive comments in this area

 Area            Status     Description
%s
"
CONNECTED "
You are connected to %d area(s) (%d area(s) are passive).
"
LIST "
List of fileecho areas available for %s (%s):

* ... connected areas (active or passive - use %%QUERY)
+ ... not yet connected areas
! ... you will not receive comments in this area

 Area            Type     Description
%s
"
ACCESS "
You have access to %d area(s).
"
FRIEND "
You have 'Friend' status (unlimited access to 'Private' areas).
"
GUEST "
You have 'Guest' status in %d 'Private' area(s).
"
UPLINK "
You have 'Uplink' status.
New areas exported from your node will be auto-added at %s.
"
ACTIVE "
You are now ACTIVE in all %d connected area(s).
"
PASSIVE "
You are now PASSIVE in all %d connected area(s).
"
MAIL+ALL "
You will receive netmails with comments in all %d areas.
"
MAIL-ALL "
You will not receive netmails with comments in any %d areas.
"
MAIL+ "
You will receive netmails with comments in area %s.
"
MAIL- "
You will not receive netmails with comments in area %s.
"
ALL "
You are now connected to all %d available area(s) (%d read/write).
"
NONE "
You are now NOT connected to any area.
"
RDWR "
%s connected for reading and writing files.
"
RDONLY "
%s connected for reading files.
"
NOTCON "
You are NOT allowed to access this area.
"
DISCON "
%s disconnected.
"
PASS1 "
%s is PASSIVE.
"
ERRNETMAIL "%s: message is not a valid Fido Netmail !"
JINEJFIX "Detected mail to another Allfix robot (%d/%d.%d) !"
FIX "þ Command: Fix"
INBOUND "
Processing TIC files in inbound directory..."
NOTALLOWED "%s is not allowed to write files to %s"
CRC? "%s Crc of %s is different from value in TIC."
CRC!! "%s Invalid Crc of %s in ChaosFix generated TIC !"
RCVD "ÄÄ> Received %s in area %s (%s..%s->%s)"
DSTR "Distributing %s in area %s ÄÄ> %s"
EXEC "Executing %s %s%s"
CANNOT "%s file %s cannot be replaced."
REPLACES>1 "%s more than one file match TIC command 'Replaces %s'."
SAVING "
Saving TIC as "
NOTUPLINK "%s only uplinks can use 'Replaces' keyword in TICs.
Node %s attempted to replace %s."
NOTFOUND "%s attached file %s not found: skipping %s"
BADAKA "%s is adressed to %s - unknown AKA"
BADAREA "Unknown area '%s' in %s"
CREATING ": creating new area."
A_UZEL " and node "
NENI.. " is not an uplink"
BADTIC ":
moving to BADTICS\"
ERRTIC "%s: error reading TIC file."
TRANSIT "
Processing transit files..."
DELETING "Deleting successfully distributed file %s. (no unsent TICs...)"
HATCH "þ Command: Hatch
"
NAME "    Filename: "
AREA "        Area: "
DESC " Description: "
RPLC "    Replaces: "
FNOTFOUND "%s: file not found !"
ANOTFOUND "%s: area not found !"
APATH "   Area path= %s
"
ADESC "Area comment= %s
"
CRC "
Calculating CRC..."
EXDIZ? "
Extract FILE_ID.DIZ and use as LDesc ? [Y/n] "
FIXNOW? "
Fix the file (send it to fileecho) NOW ? [Y/n] "
NONODES "
No nodes connected to %s - cannot send file.\n"
HATCHTIC "Saving HATCH.TIC. To send file to fileecho, move HATCH.TIC and %s
to directory %s and run Chaosfix Fix."
HATCH>1! "%s more than one file match %s !"
DELETE? "
Do you want to DELETE %s ? [y/N] "
MOVEDEST? "
Move %s to destination directory %s ? [Y/n] "
COPYDEST? "
Copy %s to destination directory %s ? [Y/n] "
NOTDEST! "
Files can be sent only from destination directory."
HATCHING "
Hatching file %s, described as '%s':"
FILESBBS? "
Modify BBS description file %s%s ? [Y/n] "
MEMERR "Out of memory !!!"
WRITINGCTL "Writing 'Chaosfix.Ctl'..."
CTLWROK "'Chaosfix.Ctl' succesfuly written."
CTLWRERR "Error writing 'Chaosfix.Ctl' !"
ILLEGALPWD "
Illegal password for %s (%s): %s
"
SORRYNOTKNOWN "
Sorry, node %s (%s) not known for %s
"
SECVIOLATION "%s %s security violation"
MAILTEXT "
File: %s
Area: %s
Desc: %s
%s
"
ADDINGDESC "Adding description to: %s"
NOTIFYSYSOP "Notification message from %s %s to sysop"
NEWAREA "New area"
AUTOAREA "Area automaticaly created by ChaosFix"
!
! following line is version dependent - this message is for version 1.10
!
SELFTEST "17155"
!
! end of file CHAOSFIX.MSG
