# Unchained writeup

We have an application which signs a value that you send to it and stores it in the `username` cookie.

From the homepage we can guess that we need to have a cookie containing the "admin" value, but sadly the server rejects our request when we ask it to sign username="admin".

We also received some files with the challenge, from them we can tell that the server is running a django behind a nginx proxy (which also serves static files).

Here are our first vulnerability in the `nginx.conf` file:
```
# static files
location /static {
    alias /srv/app/static/;
}
```

the `location` value does not end with a `/` but the `alias` value does. Which means that if we query: https://unchained.ctf.insecurity-insa.fr/static../thing
then the `location` will match and will concatenate `../thing` to the alias, which gives us this local file access: /srv/app/static/../thing

As we know the server is running a django application and we know where it is located (Dockerfile, uwsgi file etc..) We can download the settings.py file \o/

`https://unchained.ctf.insecurity-insa.fr/static../unchained/settings.py`

Which gives us the `SECRET_KEY` which is used during the signing process in django.

Next, we just have to generate a valid cookie for `admin` and our job is done.
Please see `exploit/exploit` for the full exploit and signing process

## Resources
* https://docs.djangoproject.com/en/2.2/topics/signing/#protecting-the-secret-key
* https://i.blackhat.com/us-18/Wed-August-8/us-18-Orange-Tsai-Breaking-Parser-Logic-Take-Your-Path-Normalization-Off-And-Pop-0days-Out-2.pdf#page=19

