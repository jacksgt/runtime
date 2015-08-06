# RUNTIME - A tool to track the total uptime of a system
`runtime` monitors and manages the total uptime of your system. This is achieved by adding up all 
individual uptimes of the system, thus resulting in a total 'runtime'.

To do this, runtime should be launched after every boot of the system in 'track' mode (see 
Command line Options). It will then wait until the system is shutdown, read the uptime from 
/proc/uptime (default), read /var/cache/runtime, add the two values together and save the result 
to /var/cache/runtime.

## Installation
*Note: runtime currently requires systemd*
To install `runtime`, you first need to compile it.
```bash
$ cd /tmp;
$ git clone https://github.com/jacksgt/runtime;
$ cd runtime;
$ make # or alternatively: $ gcc -lm main.c print.c io.c -o runtime
$ sudo cp runtime.service /etc/systemd/system/
$ sudo systemctl enable runtime # runtime will now automatically start after boot
$ sudo cp runtime /usr/local/bin
```
## How do I use runtime?
After you have set up runtime to run in the background, runtime can report the runtime and uptime 
of the system:
```bash 
$ runtime --seconds # shows the runtime in seconds
$ runtime --uptime # shows the uptime
$ runtime # or alternatively: runtime --runtime
```
*For more options please have a look at Command line Arguments*

## But what happens when my computer crashes?
No worries, runtime got your back: it keeps a cache of the uptime. This means after booting the 
program sleeps for a while (1 hour by default), then write this value (3600 seconds) to 
/var/cache/runtime_cache and sleep again (and so and and so forth).
This way when the computer crashes after 6 hours and 30 minutes of uptime, `runtime` will read 
the value of /var/cache/runtime_cache and at least know there were 6 hours worth of uptime. 
`runtime` will then add this value to /var/cache/runtime and move on.

## Command line Options
Use `runtime [option]` where valid options are:
```
    --verbose (not yet functional)
    --version
    --help
    --save
    --track
    --seconds
    --uptime
    --runtime (the default option)
```

## Bugs & Features
If you find a bug, please don't keep, but share it! I'd love to fix it.
Also, if you have an suggestions or feature request, just speak up.


