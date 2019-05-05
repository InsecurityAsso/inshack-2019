# You Shall Not Pass Write-Up

## Finding the sesame

You've got a NTFS filesystem with a movie and some crap about bacon and meat.

The movie catches your eye because it's a famous extract of LOTR. In fact you're looking almost in the right place. 

What you're looking for is hidden in the slack space of the movie and it's encoded in `utf16-le`. 

You might also be able to find it using `strings -el` on the filesystem image.

It's a base64 encoded blob surrounded by markers: `-BGZ-` and `-EGZ-`. 

`GZ` stands for `gzip` which is usual and should not be hard to guess.

You've got something now. It's a JSON configuration file or something called sesame.

## Knocking at the door

In fact, this is a configuration for a custom port knocking service. The URL can be found in the challenge description.

Just knock at all the doors using given protocol and port number and finally ask politely for the password at the final door.
