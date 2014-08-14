# Eigenmath port for the Casio Prizm (fx-CG 10 and 20)

This is a port of the [Eigenmath math engine](http://gweigt.net/eigenmath-index.html) to the Casio Prizm in the form of an add-in, providing numerous math functions not available in the OS, including symbolic computation.

## Features

All of the functionality of the command-line version of Eigenmath is available on this port, with limitations consistent with the technical limits of the Casio Prizm platform. This means certain commands are going to take longer to execute, and certain ones won't complete due to lack of memory (640K ought to be enough for anybody, as if it's 1981 again).

Some features from the graphical versions and ports of Eigenmath are also available, with different UI (adequate to the platform), including:
  - Graphic plotting (zoom in and out, move left/right/up/down as with the OS);
  
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

To install, connect the Prizm calculator to the computer with a miniUSB<->USB cable. On the calculator, press F1 when a pop-up appears on the screen. Wait for the USB connection to be established. When it's finished, your Prizm will appear on your computer as if it were a pendisk.

Copy "eigenmath.g3a" to the root folder of the "pendisk" (i.e., out of any folders but inside the pendisk; if necessary, overwrite the existing file).

If you also have a "eigensup.txt" file  - this is the "startup script" we'll explain in detail, later - you can copy it too, but to a different folder:

 - On the "pendisk", create the folder "@EIGEN" (must be all-caps);
 - Copy the "eigensup.txt" file to that folder you created.

Safely disconnect the calculator and wait for it to finish "updating the Main Memory". When it does, you should notice a new Main Menu item, called "Eigenmath".

The first time you run Eigenmath you'll be shown a welcome message. See Usage instructions.
  
## Usage instructions

The first time you select Eigenmath from the calculator's Main Menu, you'll see a screen with the text "Welcome to Eigenmath", "To see more options, press Shift then Menu", with a blue arrow below, a vertical line after it and a otherwise empty line.

![Initial screen](/docs/usage1.png?raw=true)

The blue arrow is meant to indicate a command entry line (where you'll type what you want Eigenmath to do), and the vertical black line is the cursor (it doesn't blink). To type, do as you normally would: press the keys on the keyboard, use [Shift] and [Alpha] to modify the key meaning, and use [F5] to switch between upper and lower-case alpha mode.

### Basic Operation

If this is your first time using Eigenmath, you should see the [Eigenmath manual](http://gweigt.net/Eigenmath.html). Everything except instructions related to the user interface (which is considerably different in the desktop versions) will apply to this port. Because of this, you should give it a quick read.

You can use Eigenmath as a simple calculator - type 2+3 then press [EXE], the result will appear along with another line for command entry:

![Basic operation](/docs/usage2.png?raw=true)

The output will be pretty-printed whenever applicable, however, command input takes place in a single line, not pretty-printed. This means that if you want to calculate the square root of 7, you can either type sqrt(7) character by character, or use the square root key as a shortcut for typing sqrt(, after which you'll only need to press [7], [)] and [EXE]:

![Basic operation](/docs/usage3.png?raw=true)

What's different in Eigenmath, is that you can also use variables to do symbolic computation with them, unlike with Casio's non-symbolic system where variables always correspond to a number:

![Basic operation](/docs/usage4.png?raw=true)

As you probably understood from reading the Eigenmath manual, you can use a set of commands to manipulate the symbolic expressions. Most commands will treat x as the main variable, unless otherwise specified. Variables and commands are case-sensitive, which means typing "simplify" is different from "SIMPLIFY", and "x" means a different variable than "X".

So, to calculate the derivative of the last expression with regards to x, use `d(last)`. `last` is a special variable that always holds the result of the last successful computation.

![Basic operation](/docs/usage5.png?raw=true)

(as you see, when expressions do not fit on screen they will not be pretty-printed, but rather shown linearly)

When certain keys, like [+], [-], [*], [^] and [/], are pressed right after a result is displayed, `last` will be automatically inserted before them, because the software understands you are trying to operate on the last result.

Therefore, a key sequence like this...

[1], [2], [+], [4], [EXE], [+], [5], [EXE], [*], [7], [EXE], [x^2], [EXE]

...results in this:

![Basic operation](/docs/usage6.png?raw=true)

(you can insert `last` at any time by pressing the same keys you'd press to type Ans on Run-Matrix)

If you don't want to insert "last" automatically, start by pressing some key other than the "magic" ones, such as [DEL] or [AC/ON], before typing.

Also note that, by default, Eigenmath does not operate with scientific notation or decimal numbers; converting a number to scientific notation or decimal form results in an internal loss of precision for Eigenmath (the conversion is one-way and lossy). Similarly, numbers entered in decimal form will not and can not be converted to a fraction representation, so for better results, use fractions if possible.

The `float` command (insert it with the [F<>D] key) can be used to convert a number into its decimal/scientific notation form.

![A number in decimal form will never be turned into a fraction, but you can turn a fraction into its decimal form](/docs/usage7.png?raw=true)

![Big numbers are displayed as big numbers, conversion to scientific notation is done on user request](/docs/usage8.png?raw=true)

The ! symbol is used to express the factorial (n! = n*(n-1)*(n-2)*..., 5!=5*4*3*2*1) of an integer; you can insert this symbol with the [->] key above [AC/ON] - this key was chosen because it had no utility in Eigenmath, otherwise.

### Operating with variables and custom functions (user symbols)

Variables and custom functions are generally called user symbols. They don't need to be single-letter: most combinations of letters and numbers that start with a number, are interpreted as a single symbol. Be careful: if you type "ab", it doesn't mean a*b, but a variable (or function) called "ab"! Additionally, as explained before, symbols (user-defined and built-in) are case-sensitive.

Before, we said that variables are interpreted symbolically, but you can still assign a value to a variable, and reset the variable to its empty meaning later:

![Assigning values to variables and resetting variables](/docs/usage9.png?raw=true)

In the example above, we start by assigning a value of 34 to the variable a, then calculate using that value. The "quote" command returns the unevaluated meaning of a variable, and can be used for more than clearing symbols:

![Assigning values to variables and resetting variables](/docs/usage10.png?raw=true)

As you see, in this case the quote command makes the value of C be retrieved whenever CplusFive is evaluated. Had we just said that `CplusFive = C + 5`, CplusFive wouldn't have updated when C changed.

You can also define custom functions, that return values and take a number of arguments. Custom functions can be chained and mixed with built-in functions.

![Assigning values to variables and resetting variables](/docs/usage11.png?raw=true)

Of course, after some usage, variables and functions start to get into strange states, because you'll certainly forget to reset them to the "empty" state when you no longer need them. Furthermore, after using a handful of custom symbols it's a pain to clear them all, one by one. The "clear" command clears all user symbols, plus console scrollback, in one go.

### Using the console

The console is the black-text-on-white-background screen you see when you open Eigenmath, and where commands are typed and their result is displayed. Now you ask: but that's what I've been seeing all this time?! Indeed you have been using the console, but there's more to it than you know.

By now, we have already done a lot of things on the console, haven't we? (If not, execute something like `235!` or `5123^342` just to fill the console so we can proceed.)

The text on the console scrolls up and goes out of sight as more things are displayed. But a big part of it can be displayed again if you wish. First press the [Shift] key, then press the Up "replay" key. You should see a screen similar to the following:

![Console scrolling mode](/docs/usage12.png?raw=true)

See that message at the top? Press the Up or Down "replay" keys (and no other keys) to scroll up and down. The little rectangle to the right is the scroll bar, indicating what part of the console you are seeing relative to the current bottom.

![Console scrolling mode](/docs/usage13.png?raw=true)

To exit scrolling mode, press any key other than the Up or Down keys, like [EXIT] or the Left or Right key. The console will jump to the current command input.

There's more past to bring back! Remember that long command you just typed, but was a bit off and had a syntax error? Or that long command that took ages to type? There's a good chance you can bring it back to edit and execute it again! Just press the Up key (this time, without [Shift]), to recall the last command. Keep pressing for older commands. You can also press the Down key to recall a more recent command, until you are in the newest (not yet executed) line.

![Command history](/docs/usage14.png?raw=true)

Also, you probably already figured this by now, but you can move the cursor to edit the command at any position, using the Left and Right keys. When the command is too long to fit on the screen, pink arrows will be shown in the direction of the text that's hidden.

![Command editing](/docs/usage15.png?raw=true)

There's also an advanced console feature, which allows for entering long commands in multiple steps:

![Partial commands](/docs/usage15a.png?raw=true)

To use it:
  - Enter part of the command you want to execute;
  - Press [Shift] then [EXE] - a new line with a different blue arrow will appear;
  - Type the part of the command that follows;
  - Repeat the two previous steps as needed;
  - When done, press [EXE], without [Shift], to execute the concatenation of all the partial commands.
  
### Using the clipboard

You can paste from the calculator clipboard into the console, by pressing [Shift] then [9] as usual.

To copy the last result into the clipboard, press [Shift] then [8], select "Last result", and a confirmation message will appear on the status area.

To copy or cut things in the current command entry, press [Shift] then [8] and select "Current command". A traditional text entry will appear, containing the current command - just use [Shift] then [8] again to copy or cut, as usual. Any changes you make to this entry, including text deletion and cutting, will be reflected on the console entry. When you're done copying, cutting or editing what you want, press [EXIT] to return to the console.

### Function catalog

This port features a function catalog organized by categories, with a short description for each command. You can access it by pressing [Shift] then [Menu] and selecting the appropriate option, or directly from the console, as you'd normally access the OS function catalog: by pressing [Shift] then [4]. A list of categories will appear:

![Function catalog](/docs/usage16.png?raw=true)

You can choose to see all commands, or a specific category. After selecting an option, press [EXE] or [F1] for it to be inserted at the current cursor position, or press [F6] to see its help text:

![Example of a help text for a function](/docs/usage17.png?raw=true)

Press [EXIT] to close this screen and go back to the command list. To switch categories, press [EXIT] once, and then again to exit without inserting a function.

Note: menus on Eigenmath, including the catalog menus, don't support jumping to a certain letter by pressing the respective keyboard key. There's, however, other way to quickly navigate menus: the keys [1] through [0] select options 1 to 10, and the keys above, from [xOt] to [->], select a handful of other options.
If you want to quickly insert, for example, `rationalize(`, press, from the console, the following key sequence:

[Shift], [4], [9], [a b/c]

...in the end, you should be back in the console with `rationalize(` inserted at the cursor position. Pressing just four keys, you inserted text that would otherwise require over a dozen of presses!

### Graph plotting

If you have used Eigenmath before, telling you that the `draw` command is supported should be enough to get you going. If not, then you should read the [relevant Eigenmath manual section](http://gweigt.net/Eigenmath.html#x1-90001.7).

To demonstrate the feature, start by executing `draw(x^2-2)`. After a waiting period, the graph for the expression y=x^2-2 should appear:

![Graph screen](/docs/usage17a.png?raw=true)

You can use the "replay" directional keys to move the graph in any direction, as you would do on the built-in "Graph" mode. Use the + and - keys to zoom in and out (again, as on the "Graph" mode).

As explained in the Eigenmath manual, the view window can be adjusted numerically by setting the symbols xrange, yrange and trange, but you can also choose from three presets by pressing, on the graph screen, F3 followed by a key from F1 to F3.

To exit the graph screen, just press EXIT, and you should be back in the console.

As with most commands, executing `draw` with no arguments draws the previous result. If the previous result is not drawable in the real plane (for example, if it contains complex numbers or is otherwise hard to map to a real plot), the graph screen will still display, but nothing will be drawn on it.

Feel free to try the more complex `draw` examples from the Eigenmath manual!

You can draw multiple graphs at once, using the `do` command, like this:
`do(draw(x),draw(exp(x)),draw(sin(x)))`

![Multiple graphs](/docs/usage17a1.png?raw=true)

Each expression will be drawn in a different color.

### Aborting execution

Certain expressions and scripts (which we'll explain later) can take a long time to execute. If a computation is taking more time than what we're willing to wait, press AC/On to break execution (as with most calculations throughout the OS).

Stopping may not be instant, but after some time you'll see something like this:

![Stopping execution](/docs/usage17b.png?raw=true)

`Stop: esc key` and `Stop: escape key stop` are two messages that mean the same thing: execution was aborted by the user. Which one appears depends on what Eigenmath is doing at the time.

The [AC/ON] key can also be used to stop the drawing of graphs, with the advantage that the graph still gets displayed, in an incomplete form (usually a low-density points cloud):

![Stopping graph plotting](/docs/usage17c.png?raw=true)

This can be useful for quickly changing the view window, without having to wait for each change to be completely drawn.

### Script running and recording

Because it's easier to explain the scripting feature when we actually have some script to execute, let's start by explaining how to create a script.

On the console, start by pressing Shift+Menu and selecting "Record Script", or type `record` (in lower-case) and press [EXE].

![Script recording](/docs/usage18.png?raw=true)

Now every command typed, including ones with syntax errors, will be recorded, up to a maximum of 200 commands. Commands are executed as usually.

To demonstrate the feature, let's type the following three commands:

  - `print("This is a script.")`
  - `a=2+34`
  - `a*3`
  
Finally, execute `record` again or press [Shift] then [Menu] and select "Stop Recording". You will be asked for a name for the script, let's just call it "test":

![Specifying a script name](/docs/usage19.png?raw=true)

Don't forget to press EXE. The script will be created in the storage memory with the name "test.txt".

To better demonstrate the scripting feature, you should execute `clear` now. Press Shift+Menu and select "Load Script". A file browser for the storage memory will appear, select test.txt - this is the script we just created. This is what you'll see:

![Script executed](/docs/usage20.png?raw=true)

See how every command we typed when recording got executed? Also note how the commands we entered don't get displayed. This allows for fine control of what is to be displayed. Any symbols set by the script are still available. Our script set the variable `a`, so let's check it:

![Symbol set by the script](/docs/usage21.png?raw=true)

You can rename and organize your scripts in the Storage Memory by connecting the calculator to the computer via USB, or by using an add-in with a file manager, such as Utilities. Scripts can also be written and edited on a computer, and some of the Eigenmath scripts you find on the Internet will run just fine (the problems with those that don't, will be slowness or lack of memory).

### Startup script and key customization

You can specify a script to be run whenever Eigenmath is open and there's no session saved, or when "clear" is executed. This is good for specifying custom functions and constants.

Let's say you want to have a custom function always available, that returns the logarithm of a number in a certain base. You'd normally define such a function by executing something along these lines:

  - `logab(a,b)=log(b)/log(a)`
  
Now, how can we make logab always available, even after `clear`ing? It's simple, just put it in the startup script:

  - Connect the calculator to the computer (or use an add-in like Utilities) and create the folder "@EIGEN" (without the quotes), if it doesn't exist already.
  - Create, or copy into that folder, a file called "eigensup.txt" (without the quotes). For the purposes of this demonstration, that text file should contain "logab(a,b)=log(b)/log(a)" (without the quotes).
  - Save the file, safely disconnect the calculator (or leave the add-in you used to create the folder) and open Eigenmath.
  - Execute "logab(20,4)". Your logab function was defined already!
  
Add more things to the startup script, by putting the expressions you'd like to execute automatically each in their line, as with a normal script.

You can also set actions for keys which usually do nothing, so that they directly execute some Eigenmath expression. Look at the [suggested eigensup.txt script](https://github.com/gbl08ma/eigenmath/blob/master/eigensup.txt) to see how it's done. Basically, the following must be done (you can execute this step by step on the console, and see things change):
  - The variable `prizmUIhandleKeys` must be set to 1;
  - The function `prizmUIkeyHandler(k,s)` must be defined. k will receive a code for the key pressed, and s will receive the keyboard modifier code (Shift, Alpha, etc.). You can see the codes by defining this function to print the codes, or by not defining it (and whenever the UI executes it, it will be printed on the screen together with the codes).
  - Optionally, you can set labels for the function keys (except [F5]), by setting the variables `prizmUIfkey1label`, `prizmUIfkey2label`, `prizmUIfkey3label`, `prizmUIfkey4label` and `prizmUIfkey6label` to the code of the label you want to show (a positive integer). You can find the codes by trial-and-error, or by using an add-in like INSIGHT.
  
The suggested eigensup.txt assigns, for example, the [F3] key to the "clear" command and the F6 key to the "draw" command.

### Usage as a eActivity strip

Eigenmath can be launched from the Main Menu, or as an eActivity strip. Strips are things you can insert into eActivity documents and which allow for launching functions external to eActivity, including some add-ins.

You can insert strips into a eActivity document by pressing [F2] - STRIP - when editing the document. To open a strip, press [EXE] while it is selected. To switch between the eActivity document and the open strip, press [Shift] and then the [->] key above [AC/ON].

![Examples of strips](/docs/usage22.png?raw=true)

When you open a Eigenmath strip, you'll be presented the same screen as when running the add-in for the first time, or when session persistence is disabled. This is because session persistence doesn't work on strips.

You can use Eigenmath as usual, but once you leave the add-in (by selecting another strip, or by closing the eActivity) everything you do will be forgotten. The only persistent thing in the Eigenmath strip is the "strip script", which is a script that can be linked with the strip and will run every time it is opened.

Let's say we want to run the script we created in the previous section, test.txt, every time the strip is open. Open the Eigenmath strip, press Shift then Menu, select "Set Strip Script", read the message and select the script we created.

![Setting a script for the strip](/docs/usage23.png?raw=true)

If all went well, the message "Script set successfully" will be displayed.

Now, let's test our setup. Press [Shift] and the arrow key above [AC/ON], select our Eigenmath strip, and press [EXE] - this will restart the strip:

![Success!](/docs/usage24.png?raw=true)

And there it is, our script ran. You can now use Eigenmath as usual and play around with the script result through any symbols it may have set, as if you had run the script manually.

As explained on the screen that appears when selecting a strip script, the script is stored inside the eActivity (more precisely, inside the strip), which means that it will work on any calculator, even if the script is not present in the storage memory. You should keep this in mind, because if you edit the script in the storage memory, changes will not apply to the scripts inside the strips! You'll need to update them individually on a per-strip basis, doing as you normally would to select a strip script.

### Disabling session persistence

Press [Shift] then [Menu], and uncheck the "Save Session" option. You can enable it back at any point in the future.

Session persistence (or its setting) is not available when running as a eActivity strip.

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