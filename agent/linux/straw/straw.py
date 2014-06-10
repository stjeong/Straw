#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
    Created on 2014. 6. 9.

    @author: geeksaga
    :copyright: (c) 2014 by geeksaga.
    :license: MIT LICENSE 2.0, see license for more details.
'''

import os, sys
import logging
from geeksaga.straw.cpu import *


def print_settings():
    '''
    logging.basicConfig(filename='straw.log',level=logging.DEBUG)
    '''
    logging.basicConfig(level=logging.INFO)
    logging.info('========================================================')
    logging.info('Settings for Straw Agent')
    logging.info('========================================================')
    '''for key, value in config:
        print('%s=%s' % (key, value))'''
    logging.info('========================================================')

def doMain():
    try:
        '''
        while True:
        '''
        print_settings()
    except:
        print("Error: %s" % sys.exc_info()[0])
    finally:
        pass

if __name__ == '__main__':
    doMain()
    
    load()
    
    '''from geeksaga.straw.logger import Log'''
    '''log_filepath = os.path.join(geeks_app.root_path, geeks_app.config['LOG_FILE_PATH'])
    Log.init(log_filepath=log_filepath)
    Log.info("straw")'''
    
    pass

