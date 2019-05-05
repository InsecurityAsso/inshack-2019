# Bypasses Everywhere V2 - writeup

Please read the writeup of bypasses-everywhere first, it is almost the same solve.

During the CTF a participant solved the first part in an unintended way, by bypasses the referer checks using javascript, which I didn't test.

So we decided to release this challenge as a "v2" of the first challenge, basically it does check the referer proprely this time. So you do need to use the intended solution from the writeup of this repository (see `../bypasses-everywhere/writeup.md`).

Another Change from the previous version is that the endpoint which validates the flag is no longer at POST `/article` but at post `/admin` which means that we need to change our javascript payload to POST to `/admin` instead of the previous `/article`.

Why is this problematic? Because we can't simply change the `action=".."` of the `form` from the previous payload because this is blocked by the chromium XSS auditor, so we need to do this in our javascript payload.

To do this I needed a better CSP bypass endpoint than the previous one, which is not enough permissive, this one is perfect! https://twitter.com/brutelogic/status/955911040721674241

All put together here is our final payload:

```
http(s)://www.google.com/complete/search?client=chrome%26jsonp=a.action=`/admin`;a.submit();
```

Using the same `<form>` as before :)

Please see exploit/exploit.py for the full exploit.

