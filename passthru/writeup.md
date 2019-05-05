# Passthru Write-Up

This challenge consists in analyzing a network capture of a man-in-the-middle proxy.

You will find that someone is using google image search to bypass the proxy restrictions and exfiltrate some data.

If you look carefully at the `requestbin` URLs you'll find that there is a GET parameter called 
`kcahsni` followed by hexadecimal characters.

Oh man... if you do a `echo 'kcahsni' | rev` you see `inshack` ;)

Now you concatenate all the hexadecimal parts. 

You decode the hexadecimal string and you find an inversed flag with some garbage around. You reverse the order of the bytes and you've got your flag.
