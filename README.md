# fake-hwclock
does the same as the bash script of the same name, but faster

this is a complete rewrite in c. absolutly near by the orginal script

this is written 10'2022 by Heiko Stoevesandt <hstools "ta" t-online.de>
orginal this was written as a bash-script by Steve McIntyre

SYNOPSIS
 fake-hwclock [ command ] [ force ]
command : save or load (or empty to set command to save)

more can be found at man(8) fake-hwclock

The Orginal have not really a usable return value
fake-hwclock has now following return codes

RT | Description
---|----------------
 0 | no error (save or load)
 1 | save error
 2 | don't save fake-hwclock with time before HWCLOCK_EPOCH_SEC ; use force to overwrite
 3 | load error ( file not found )
 4 | load error ( file can't read )
 5 | load error ( size error )
 6 | don't load - clock is in the past
 7 | --
 8 | --
 9 | command not found
10 | settimeofday error

