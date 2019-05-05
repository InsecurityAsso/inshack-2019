# Tower of Babel

## Build procedure

```
make
```

## Memory dump creation procedure

Remote host must have

```
/home/user/
    bootstrap
    tob/
        tob.ko
        lime-4.18.0-16-generic.ko
```

and a VirtualBox shared folder

```
/media/sf_vms-share/
```

which must not contain a file named `mem.dmp`.

In a terminal

```
$ ssh user@192.168.56.101
$ ./bootstrap
```
