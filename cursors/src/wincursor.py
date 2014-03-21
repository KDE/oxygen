import sys
import os

configs = ["left_ptr.in", "help.in", "progress.in", "wait.in", "cross.in", "text.in", "pencil.in", "circle.in", "size_ver.in", "size_hor.in", "size_fdiag.in", "size_bdiag.in", "fleur.in", "up_arrow.in", "pointer.in"]

if len(sys.argv) < 4:
    print "too few arguments for wincursor"
    sys.exit(1)

pngdir = sys.argv[1]
conf = sys.argv[2]
outdir = sys.argv[3]
dummy = sys.argv[4]

f = open(dummy, "w")
f.close()

if not configs.count(conf[conf.rfind("/")+1:]):
    sys.exit(0)
else:
    conffile = conf[conf.rfind("/")+1:]

f = open(conf)
r = f.read()
f.close()

ls = r.split("\n")
imglist = []

for l in ls:
    if l and not l.startswith("#"):
        v = l.split(" ")
        if len(v) == 4:
            imglist += [(v[3], v[1], v[2])]
        elif len(v) == 5:
            imglist += [(v[3], v[1], v[2], v[4])]

if len(imglist) > 1:
    pngls = [os.path.join(pngdir, i[0]) for i in imglist]
    pngstr = ""
    for p in pngls:
        pngstr += " %s" % p
    
    img = imglist[0]
    jiffie = int((float(img[3]) / 1000) * 60)
            
    print os.popen4("png2ico %s --hotspotx %s --hotspoty %s --framerate %s %s" % (os.path.join(outdir, conffile.replace(".in", ".ani")), img[1], img[2], jiffie, pngstr))[1].read()
elif len(imglist) == 1:
    img = imglist[0]
    print os.popen4("png2ico %s --hotspotx %s --hotspoty %s %s" % (os.path.join(outdir, conffile.replace(".in", ".cur")), img[1], img[2], os.path.join(pngdir, img[0])))[1].read()
