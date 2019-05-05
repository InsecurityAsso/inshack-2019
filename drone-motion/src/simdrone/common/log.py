'''
file: log.py
date: 2018-07-31
author: koromodako
purpose:

'''
# =============================================================================
#  IMPORTS
# =============================================================================
from logging import (
    StreamHandler,
    FileHandler,
    Formatter,
    getLogger,
    DEBUG,
    INFO
)
from termcolor import colored
# =============================================================================
#  CLASSES
# =============================================================================
class ColoredFormatter(Formatter):
    '''[summary]
    '''
    COLORS = {
        'DEBUG': 'green',
        'INFO': 'blue',
        'WARNING': 'yellow',
        'ERROR': 'red',
        'CRITICAL': 'magenta'
    }

    def __init__(self, fmt=None, datefmt=None, style='%'):
        '''[summary]
        '''
        super().__init__(fmt, datefmt, style)

    def format(self, record):
        '''[summary]
        '''
        os = super().format(record)
        return colored(os, ColoredFormatter.COLORS[record.levelname])
# =============================================================================
#  GLOBALS
# =============================================================================
FMT = '[drone](%(levelname)s)> %(message)s'
_fmtr = ColoredFormatter(FMT)
_hdlr = StreamHandler()
_hdlr.setFormatter(_fmtr)
app_log = getLogger('application')
app_log.setLevel(INFO)
app_log.addHandler(_hdlr)
# =============================================================================
#  FUNCTIONS
# =============================================================================
def log_to_file(fp):
    '''[summary]
    '''
    hdlr = FileHandler(fp)
    hdlr.setFormatter(Formatter(FMT))
    app_log.addHandler(hdlr)

def disable_color():
    '''[summary]
    '''
    _fmtr.disable_color()

def enable_debug(debug=True):
    '''[summary]
    '''
    app_log.setLevel(DEBUG if debug else INFO)

def disable_logging():
    '''[summary]
    '''
    app_log.removeHandler(hdlr)
