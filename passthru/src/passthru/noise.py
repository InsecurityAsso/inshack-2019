import os
import re
import signal
from random import randint
from asyncio import get_event_loop, sleep, gather
from argparse import ArgumentParser
from functools import partial
from passthru import web

# global termination flag
TERMINATION_REQUESTED = False

def cprint(message=None):
    '''[summary]
    '''
    if message is None:
        print()
    else:
        print(f"\x1b[34m{message}\x1b[0m")

class Crawler:
    '''[summary]
    '''
    def __init__(self, name, prefix, doc_re, start_doc, min_wait=4, max_wait=8):
        '''[summary]
        '''
        self.name = name
        self.prefix = prefix
        self.doc_re = re.compile(doc_re)
        self.docs_seen = []
        self.start_doc = start_doc
        self.current_doc = None
        self.min_wait = min_wait
        self.max_wait = max_wait

    def _info(self, message):
        '''[summary]
        '''
        cprint(f"{self.name}: {message}")

    async def _wait(self):
        '''[summary]
        '''
        delay = randint(self.min_wait, self.max_wait)
        self._info(f"going to sleep for {delay} seconds...")
        await sleep(delay)

    async def _get_web_page(self, doc):
        '''[summary]
        '''
        url = self.prefix + doc
        await self._wait()
        self._info(f"requesting web page at {url}...")
        resp = web.get(url)
        self._info(f"got a code {resp.status_code}...")
        return resp

    def _find_available_docs(self, resp):
        '''[summary]
        '''
        matches = list(self.doc_re.finditer(resp.text))
        return set([match.group('doc') for match in matches])

    def _select_unseen_doc(self, docs):
        '''[summary]
        '''
        unseen_docs = list(filter(lambda x: x not in self.docs_seen, docs))
        if unseen_docs:
            return unseen_docs[randint(0, len(unseen_docs)-1)]
        return None

    async def crawl(self, doc=None, max_depth=5):
        '''[summary]
        '''
        if TERMINATION_REQUESTED:
            return
        # select start doc if first call
        doc = doc or self.start_doc
        # trace doc as seen to avoid loops
        self._info(f"next doc will be: {doc}")
        self.docs_seen.append(doc)
        # request web page for given doc on the same website
        resp = await self._get_web_page(doc)
        # parse page to find available docs on the same site
        docs = self._find_available_docs(resp)
        # keep unseen docs only
        unseen_doc = self._select_unseen_doc(docs)
        # recurse
        if unseen_doc and max_depth > 0:
            await self.crawl(unseen_doc, max_depth-1)
        else:
            self._info("crawling limits reached.")
        self.docs_seen = []

async def run_crawler(crawler):
    '''[summary]
    '''
    while not TERMINATION_REQUESTED:
        await crawler.crawl()

async def main():
    '''[summary]
    '''
    # define several crawlers
    crawlers = [
        Crawler('Wikipedia',
                'https://en.wikipedia.org/',
                r'(?P<doc>wiki/[^"]+)',
                'wiki/Hacker'),
        Crawler('Python 3 Documentation',
                'https://docs.python.org/3/library/',
                r'(?P<doc>(\.\./)?[a-z\-_]+\.html#[^"]+)',
                'asyncio-task.html'),
        Crawler('ForensicsWiki',
                'https://forensicswiki.org/',
                r'(?P<doc>wiki/[^"]+)',
                'wiki/Whireshark'),
        Crawler('BBC',
                'https://www.bbc.com/',
                r'(?P<doc>news/[a-z\-]+\d+[^"])',
                ''),
        Crawler('CPPReference',
                'https://en.cppreference.com/',
                r'(?P<doc>w/cpp/[^"]+)',
                'w/cpp/regex/basic_regex'),
    ]
    # start crawlers
    await gather(*[run_crawler(crawler) for crawler in crawlers])

def exit_sighdlr(signame):
    '''[summary]
    '''
    global TERMINATION_REQUESTED
    cprint()
    cprint(f"{signame} caught, exiting gracefully.")
    TERMINATION_REQUESTED = True

def app():
    '''[summary]
    '''
    p = ArgumentParser(description="Multi-threaded network traffic generator.")
    args = p.parse_args()
    # retrieve asyncio loop
    loop = get_event_loop()
    # install interrupt handler
    for signame in {'SIGINT', 'SIGTERM'}:
        loop.add_signal_handler(getattr(signal, signame), partial(exit_sighdlr, signame))
    # start crawling
    cprint(f"Running with pid: {os.getpid()}")
    cprint("Crawler will run until you press Ctrl+C to interrupt.")
    loop.run_until_complete(main())
