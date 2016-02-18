
This program is a server for smart home system.
The architecture of smarthome system has 3 level:
     	----------------------
     	|					  |
     	|		app			  |
     	|					  |
     	----------------------
     	|					  |
     	|   system server	  |
     	|					  |
     	----------------------	
     	|					  |
     	|		device		  |
     	|					  |
     	----------------------	
This program is the second level, it provides all kinds of
services for app and process the lowest level request and 
control command from app or system.

This level also provides API for app(we call it 'local-app') 
which is run at the same device with system server program, 
maybe some app(we call 'remote-app')) is run at other device, 
like your phone, pad, or your pda, etc. If develop the local-app, 
you'd better to known these APIs. We try the best ability to 
make these APIs modularization. Now, the system server was 
divided five parts.
	1. communication management
		a)wifi    b)serial    c)bluetooth   and so on.
	2. transmission data process
		a)init    b)packaging    c)un-pakaging    d)send    e)receive
	3. device management
		a)classify    b)checking    c)get status   d)register   and so on
	4. monitor management
		a)video    b)record
	5. environmental management
More details about the five parts in file < SMART HOME SYSTEM >.


Prerequisite Experience

Before we get started, we need to take a moment here to discuss the level 
of this tutorial. It’s flagged as advanced. The reason for this is that we, 
the authors, are going to assume that you would agree with the following 
statements:
	1.You are experienced with Java and C.
	2.You are comfortable using the command line.
	3.You know how to figure out what version of Cygwin, awk, and other 
	  tools you have.
	4.You are comfortable with Android Development.
	5.You have a working Android development environment (as if this writing, 
	  the authors are using Android 4.2)
	6.You use Eclipse or can translate Eclipse instructions to your own 
	  IDE with ease.

Copyright © 2015 x Electronics. All rights reserved. 

Contact us
Tel: +86-592-6666666
E-mail: x@x.com.cn
Address: Room ..., No.x, Road, Soft Park , Xiamen, China 361004
Website: www.xxx.com.cn 
         www.elsonic.com.cn



