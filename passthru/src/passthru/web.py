import requests

USER = 'hello'
PSWD = 'world'
HOST = '127.0.0.1'
PORT = 8080
PROXY_URL = f'http://{USER}:{PSWD}@{HOST}:{PORT}'
PROXIES = {
  'http': PROXY_URL,
  'https': PROXY_URL
}
HEADERS = {
    'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:64.0) Gecko/20100101 Firefox/64.0',
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
    'Accept-Language': 'en-US,en;q=0.5',
    'Accept-Encoding': 'gzip, deflate, br',
    'DNT': '1',
    'Upgrade-Insecure-Requests': '1',
    'Pragma': 'no-cache',
    'Cache-Control': 'no-cache'
}

def override_kwargs(kwargs):
    headers = kwargs.get('headers', {})
    headers.update(HEADERS)
    kwargs.update(dict(headers=headers, proxies=PROXIES, verify=False))

def get(*args, **kwargs):
    override_kwargs(kwargs)
    return requests.get(*args, **kwargs)

def put(*args, **kwargs):
    override_kwargs(kwargs)
    return requests.put(*args, **kwargs)

def post(*args, **kwargs):
    override_kwargs(kwargs)
    return requests.post(*args, **kwargs)

def delete(*args, **kwargs):
    override_kwargs(kwargs)
    return requests.delete(*args, **kwargs)

def head(*args, **kwargs):
    override_kwargs(kwargs)
    return requests.head(*args, **kwargs)

def options(*args, **kwargs):
    override_kwargs(kwargs)
    return requests.options(*args, **kwargs)
