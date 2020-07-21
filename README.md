README
======

Libtb is a small and flexible library for controlling VISCA PTZ cameras, primarily focusing on Tandberg cameras.
Protocols can be swapped out (for example, you could write a serial driver for a microcontroller or VISCA over IP, and you only need to supply 2 function pointers).

Hardware notes:
---------------

The PrecisionHD 1080p12x ignores any movement speed arguments that are given to it.
The movement speed is solely based on the current zoom level of the camera.

The PrecisionHD 720p seems to be intentionally limited in terms of movement, as standard diagonal movement commands are not allowed, but diagonal movement is possible with direct movement commands

Most Tandberg/Cisco VISCA cameras do not use ACKs like other VISCA cameras.  Since I do not use any PTZ cameras that use ACKs, the demo program ignores them, but the functionality is there for you to be able to handle these packets in your code.  Everything is kept intentionally modular, for scalability and flexibility.
	
On cameras without ACKs, commands are often noticeably staggered when cameras are daisy-chained. Running 1 camera per serial interface is recommended if you are planning to drive these cameras simultaneously.  For individual control, it is fine to daisy-chain the cameras.
