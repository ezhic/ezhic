#!/usr/bin/env python

import sys
import subprocess
import threading
import json
import web #not standard, run pip install web.py


def singleRun(config):
    print 'starting new run'
    proc = subprocess.Popen(config['cmdargs'], stdout=subprocess.PIPE)
    while config['flag']:
      line = proc.stdout.readline() #blocks
      if not line:
        print "subproc stopped"
        return
      if len(config['lines']) > 64000:
        config['lines'] = []
      config['lines'].append(line.strip())
    print 'config changed'
    proc.kill()
    return

config = {
            'flag' : True,
            'lines' : [],
            'cmdargs' : [sys.argv[1], '30', '10', '200', '2', '4', '2', '4', '2', '2', '0'],
            'count' : 1,
            'thread' : None,
	    'slice' : -32
         }

urls = (
    '/', 'Index'
)

class Index:
    def GET(self):
        global config

        #lazy starting on first get request
        if not config['thread']:
            config['thread'] = threading.Thread(target=singleRun, args=(config,))
            config['thread'].start()
            return '[{"just-starting"}]'

        return '['+(','.join(config['lines'][config['slice']:]))+']'

    def POST(self):
        data = json.loads(web.data())
        config['flag'] = False
        config['thread'].join()
        config['flag'] = True
        config['count'] += 1

        keys = ["timestep", "timecount", "timeout", "kTimeouts", "mTimeouts", "kExceptions", "mExceptions", "kProbe", "kMend", "stickFor" ]
        config['cmdargs'] = [sys.argv[1]]
        config['cmdargs'].extend([data[key] for key in keys])
        print " ".join(config['cmdargs'])
        config['thread'] = threading.Thread(target=singleRun, args=(config,))
        config['thread'].start()

class MyOutputStream(object):
    '''log ignore'''
    def write(self, data):
        '''no-op'''
        pass   # Ignore output

class MyApplication(web.application):
    def run(self, port, *middleware):
        func = self.wsgifunc(*middleware)
        return web.httpserver.runsimple(func, ('127.0.0.1', port))

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "usage:", sys.argv[0],"end2end-path"
    else:
        app = MyApplication(urls, globals())
        web.httpserver.sys.stderr = MyOutputStream()
        PORT_NUMBER = 8083
        app.run(port=PORT_NUMBER)


