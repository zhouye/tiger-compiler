from os.path import basename
from urlparse import urlsplit
import urllib2

def url2name(url):
    return basename(urlsplit(url)[2])

def download(url, localFileName = None):
    localName = url2name(url)
    req = urllib2.Request(url)
    r = urllib2.urlopen(req)
    if r.info().has_key('Content-Disposition'):
        # If the response has Content-Disposition, we take file name from it
        localName = r.info()['Content-Disposition'].split('filename=')[1]
        if localName[0] == '"' or localName[0] == "'":
            localName = localName[1:-1]
    elif r.url != url:
        # if we were redirected, the real file name we take from the final URL
        localName = url2name(r.url)
    if localFileName:
        # we can force to save the file as specified name
        localName = localFileName
    f = open(localName, 'wb')
    f.write(r.read())
    f.close()

for i in range(1,50):
    s='http://www.computing.dcu.ie/~hamilton/teaching/CA448/testcases/test'+str(i)+'.tig'
    download(s)
