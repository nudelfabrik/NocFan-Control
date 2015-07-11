NoctFan Control
===============

Control a 4-pin PWM Fan with an Arduino.   

Uses mainly [this Snippet][Speed] to set the Counter 1 frequency to 
the 25KHz specified for 4-pin Fans. Currently has the control backwards. (lower <code>fanSpeed</code> equals higher speed).

Cycle through different Speeds with a Button at Pin 2.

Reads the Fanspeed via Pin 4 and prints to Serial, see [here][RPM].

Very first iteration, code unifished and ugly.

[RPM]: http://www.beefrankly.org/blog/2011/12/21/read-out-4-pin-cpu-fan-speed/
[Speed]: http://forum.arduino.cc/index.php?topic=109403.msg821808#msg821808
