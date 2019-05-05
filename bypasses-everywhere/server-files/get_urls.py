import json
import subprocess
import concurrent.futures
import logging

timeout = ["timeout", "--foreground", "--kill-after=1", "2"]

def get_url(url):
    logging.error("Getting: " + url + " ...")
    ret = subprocess.check_output(
        timeout + ["chromium", "--timeout=2500", "--disable-gpu", "--disable-crash-reporter", "--headless", url]
    )
    logging.error(ret)
    return ret

def main():
    try:
        with open("/tmp/urls.json") as f:
            urls = json.load(f)
        with open("/tmp/urls_done", "w") as f:
            f.write("ok")
        with concurrent.futures.ThreadPoolExecutor(max_workers=24) as executor:
            for url in urls:
                executor.submit(get_url, url)
    except Exception as e:
        logging.exception("Error while running URL job:")


if __name__ == '__main__':
    main()
