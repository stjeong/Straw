#!/usr/bin/python
# -*- coding: utf-8 -*-
'''
    Created on 2014. 6. 9.

    @author: geeksaga
    :copyright: (c) 2014 by geeksaga.
    :license: MIT LICENSE 2.0, see license for more details.
'''

def load():
    f = open("./resources/stat_1.txt", 'r')

    while 1: 
        line = f.readline()
        if not line: break
        
        if line.startswith('cpu'):
            print(parse(line))
            break
     
    f.close()
    

def parse(data):
    values = token(data.strip().split(' '))[1:]
    
    return values

def token(src):
    values = []
    
    for s in src:
        if s.strip():
            values.append(s)
            
    return values
