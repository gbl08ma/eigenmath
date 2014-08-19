# Eigenmath port for the Casio Prizm (fx-CG 10 and 20)

This is a port of the [Eigenmath math engine](http://gweigt.net/eigenmath-index.html) to the Casio Prizm in the form of an add-in, providing numerous math functions not available in the OS, including symbolic computation.

## Features

All of the functionality of the command-line version of Eigenmath is available on this port, with limitations consistent with the technical limits of the Casio Prizm platform. This means certain commands are going to take longer to execute, and certain ones won't complete due to lack of memory (640K ought to be enough for anybody, as if it's 1981 again).

Some features from the graphical versions and ports of Eigenmath are also available, with different UI (adequate to the platform), including:
  - Graph plotting (zoom in and out, move left/right/up/down as with the OS);
  
  - Script execution and creation ("recording");
  
  - Ability to stop command execution;
  
  - Function catalog (with help for each function).
  
In addition to this, there are also some features exclusive to this port, some of which only make sense to include with the Prizm platform:

  - Output pretty-printing: while not as nice as the one done by the OS on built-in math screens, this still is much better than what you get with most Eigenmath ports:
  
  - Bracket color-matching on command input (like the OS does);
  
  - 200 lines of console scrollback and 40 command history entries;
  
  - Session persistence (optional): exit Eigenmath, then open it again, and everything will be there as before leaving, including console scrollback, command history and user symbols (custom variables and functions);
  
  - eActivity strip support; a script can be included in the strip, to be run when it is opened;
  
  - Paste commands from system clipboard and copy commands and results into the system clipboard*;
  
  - Most keys from the keyboard mapped to Eigenmath commands. Pressing [sin] will insert sin( in the command line, pressing [Shift], [cos] will insert arccos(, pressing [ln] will insert log( (since Eigenmath's log is the natural logarithm), and so on;
  
  - Persistent custom functions supported: a certain file will be executed as script whenever Eigenmath's 'clear' command is issued, just put your custom functions and constants there.
  
  - UI customizability: define commands to execute when certain keys are pressed.
  
*OS<->Eigenmath translation not provided, although some expressions may run as-is.
  
## System requirements

  - Casio fx-CG 10 or fx-CG 20, running any OS version (01.03 and up is recommended);
  
  - 300 KiB of storage memory available (for the add-in g3a and data files for session persistence);
  
  - 24 bytes of main memory available (only necessary if session persistence is disabled, to store the preference);
  
## Installation instructions

To install, connect the Prizm calculator to the computer with a miniUSB<->USB cable. On the calculator, press [F1] when a pop-up appears on the screen. Wait for the USB connection to be established. When it's finished, your Prizm will appear on your computer as if it were a pendisk.

Copy "eigenmath.g3a" to the root folder of the "pendisk" (i.e., out of any folders but inside the pendisk; if necessary, overwrite the existing file).

If you also have a "eigensup.txt" file  - this is the "startup script" we'll explain in detail, later - you can copy it too, but to a different folder:

 - On the "pendisk", create the folder "@EIGEN" (must be all-caps);
 - Copy the "eigensup.txt" file to that folder you created.

Safely disconnect the calculator and wait for it to finish "updating the Main Memory". When it does, you should notice a new Main Menu item, called "Eigenmath".

The first time you run Eigenmath you'll be shown a welcome message. See Usage instructions.
  
## Usage instructions
Please see this wiki page: https://github.com/gbl08ma/eigenmath/wiki/Usage-instructions

## Checking for updates
This Eigenmath port, like most software, receives updates from time to time. You should check for updates to this add-in periodically, to ensure you have the greatest feature pack and the most stable version. To check for new versions, you should visit the following page:

[http://gbl08ma.com/casio-prizm-software/](http://gbl08ma.com/casio-prizm-software/)

If the above page isn't available, you can try checking for news at the tny. internet media website:

[http://i.tny.im/](http://i.tny.im/)

Finally, if none of these websites are available, or if you need to contact the authors, this is the email:

gbl08ma@gmail.com


## License

The add-in is available under the GNU GPL version 2. A license should have come with this read-me and the eigenmath.g3a file. If not, the license is available online at [https://www.gnu.org/licenses/gpl-2.0.html](https://www.gnu.org/licenses/gpl-2.0.html)

The license for the current Eigenmath code is unclear, but the code used in this port is doubtlessly from the times when Eigenmath was released under the GNU GPL v2.

See the disclaimer at the end of this file or the messages on the about screen of the add-in (accessible from the Settings screen).

Distributing this Read-Me along with the g3a binary is not mandatory.

Any derivative work based on this software must clearly state it is not the Eigenmath port to the Prizm by gbl08ma at tny. internet media.

## Last words
This Eigenmath port is the result of over one year of research, work and extensive testing. Eigenmath itself has been in development for over ten years. That said, we or the original Eigenmath authors can't guarantee you will have no problems using the add-in or that everything will work as described. You are welcome to report problems to the contacts above, as well as modify the source code to your needs as long as you respect the license.

We hope you enjoy using this Eigenmath port as much as we did developing it. And if this add-in ever becomes useful, you just found a secret feature ;)

---
Copyright (C) 2004-2013 George Weigt (http://gweigt.net/)

Copyright (C) 2013-2014 Gabriel Maia (gbl08ma) and the tny. internet media group

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
