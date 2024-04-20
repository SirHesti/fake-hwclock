
## __Liesmich__

macht dasselbe wie das gleichnamige Bash-Skript, aber schneller

Dies ist eine vollständige Neufassung in c. absolut nahe am Originalskript

Geschrieben 2022-2024 von Heiko Stoevesandt <hstools "ta" t-online.de>
Ursprünglich wurde dies als Bash-Skript von Steve McIntyre geschrieben

ZUSAMMENFASSUNG
  fake-hwclock [Befehl] [Force]
Befehl: Speichern oder laden (oder leer, um den Befehl zum Speichern festzulegen)

Weitere Informationen finden Sie unter man(8) fake-hwclock

Das Original hat keinen wirklich brauchbaren Rückgabewert
fake-hwclock hat nun folgende Rückkehrcodes

---

## __ReadMe__

does the same as the bash script of the same name, but faster

this is a complete rewrite in c. absolutly near by the orginal script

this is written 2022-2024 by Heiko Stoevesandt <hstools "ta" t-online.de>
orginal this was written as a bash-script by Steve McIntyre

SYNOPSIS
 fake-hwclock [ command ] [ force ]
command : save or load (or empty to set command to save)

more can be found at man(8) fake-hwclock

The Orginal have not really a usable return value
fake-hwclock has now following return codes

---

## __Install__

Das fertige Programm ist für den Raspberry bestimmt. DH. ist eine echte "Windows-Version" sinnfrei. 
Alte Windowsoptionen im Source können so ignoriert werden.

The finished program is intended for the Raspberry. DH. a real "Windows version" makes no sense.
Old Windows options in the source can be ignored.



Nach dem Entpacken / After unpacking
```ts
$ cmake .
$ make
$ sudo make install
password: ******
```
done/fertig

---

# fake-hwclock errorcodes

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
