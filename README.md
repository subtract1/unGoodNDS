# unGoodNDS
Nintend DS rom auditing tool

Project started after noticing that there simply were no good tools out there for Nintendo DS rom auditing.  There's EvoNDS which is just a scene kiddie tool based on CRCs and nothing that tracks versions, hacks, alternates, etc.

At the moment the vision is to have a tool similar to Cowering's GoodTools *but* not include bad dumps or overdumps.  We'll track fixes, hacks, trainers, etc. and possibly offer patches that undo them in order to get a pristine rom.  Bad dumps we'll ignore (maybe offer fixes if it's a common bad dump) and overdumps we'll trim.

We'll use the same flags as the GoodTools and a similar naming convention.  We will replace special characters (I use unix but we'll hold to the windows set with < > : " / \ | ? *) with suitiable replacements.  I'm not sure what to do about foreign names; we MAY allow unicode names and may even do symbols.  We may also allow you to choose a name based on region similar to how GoodMerge operates.  Or we may not.  I'll need someone that speaks japanese and korean to fill those things out.

Ideally the software will be data driven; updates to the DBs will be frequent whereas the actual software will rarely need changing.


Resources:
* http://www.romhacking.net/documents/[469]nds_formats.htm
* http://dsibrew.org/wiki/DSi_Cartridge_Header
* https://gbatemp.net/threads/the-ultimate-nintendo-ds-rom-hacking-guide.291274
* http://nocash.emubase.de/gbatek.htm#dscartridgeheader (through wayback machine)
