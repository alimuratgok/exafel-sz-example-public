# This script runs peak finding on a crystallography experiment and generates 2 masks: ROI and RONI
import sys
import os
import h5py
import numpy as np
import psana
from psalgos.pypsalgos import PyAlgos
import matplotlib.pyplot as plt

if len(sys.argv) != 5 :
  print ""
  print "ERROR: Wrong number of arguments."
  print "Usage: python roi_SZ_write.py Experiment Run peakSize OutputRoot"
  print "Reads specified Run of the Experiment and generates data, ROI and RONI at the {OutputRoot}/{Experiment}/r{Run}/p{peakSize}"
  print "Example: python roi_SZ_write_p17.py cxic0415 90 17 /experimentData/"
  print "Generates data.bin, roi.bin and roni.bin at: /experimentData/cxic0415/r90/p17"
  print ""
  sys.exit(1)
  
expStr=sys.argv[1]
runStr=sys.argv[2]
peakSize=sys.argv[3]
outRoot=sys.argv[4]

dataFolder=outRoot+"/"+expStr+"/r"+runStr
# roiFolder=outRoot+"/"+expStr+"/r"+runStr+"/p"+peakSize

print "Configuration :"
print "Experiment       : ",expStr
print "Run              : ",runStr
print "Peak Size        : ",peakSize
print "Output Folder    : ",dataFolder
# print "Output ROI Folder    : ",roiFolder

# sys.exit(0)

os.makedirs(dataFolder)

# try:  
    # os.makedirs(roiFolder)
# except OSError:  
    # print "WARNING : ROI / RONI directory already exits. "
# #    print "ERROR : Creation of the ROI / RONI directory failed : "
# #    print roiFolder
# #    print "If that folder exists, please move or remove it to prevent accidental overwrites."
# #    print "TERMINATING"
# #    sys.exit(1)
# else:  
    # print "Successfully created the ROI / RONI directory : "
    # print roiFolder

dataFilename=dataFolder+"/data.bin"
peaksFilename=dataFolder+"/peaks.bin"
# roiFilename=roiFolder+"/roi.bin"
# roniFilename=roiFolder+"/roni.bin"

if os.path.exists(dataFilename):
  os.remove(dataFilename)
if os.path.exists(peaksFilename):
  os.remove(peaksFilename)
# if os.path.exists(roiFilename):
  # os.remove(roiFilename)
# if os.path.exists(roniFilename):
  # os.remove(roniFilename)

dataF=open(dataFilename,"ab")
peaksF=open(peaksFilename,"ab")
# roiF=open(roiFilename,"ab")
# roniF=open(roniFilename,"ab")

DataSourceStr="exp="+expStr+":run="+runStr+":idx"
print "Psana data source : ",DataSourceStr

hdr = '\nSeg  Row  Col  Npix    Amptot'
fmt = '%3d %4d %4d  %4d  %8.1f'

#ds = psana.DataSource('exp=cxitut13:run=10:idx')
ds = psana.DataSource(DataSourceStr)
run = ds.runs().next()
times = run.times()
env = ds.env()
#det = psana.Detector('DscCsPad')
det = psana.Detector('CxiDs2.0:Cspad.0')

#ROI = det.mask(run, calib=False, status=False, edges=False, central=False, unbond=True, unbondnbrs=True, unbondnbrs8=False)
#roni = det.mask(run, calib=True, status=True, edges=True, central=True, unbond=False, unbondnbrs=False, unbondnbrs8=False)
peakMask = det.mask(run, calib=True, status=True, edges=True, central=True, unbond=True, unbondnbrs=True, unbondnbrs8=False).astype(np.uint16)

alg = PyAlgos()
alg.set_peak_selection_pars(npix_min=2, npix_max=30, amax_thr=300, atot_thr=600, son_min=10)

rank = 3
r0 = 3
dr = 2
winR = r0+dr+rank
gSeg = 0
for t in range(len(times)): # loop through all event in a run
    # if (t % 10) == 0 :
      # print t
    if (t==100) :
      break
    print "Event=",t
    evt = run.event(times[t])
    calib = det.calib(evt)
    #print calib.shape
    peaks = alg.peak_finder_v3r3(calib, rank=3, r0=r0, dr=dr, nsigm=10, mask=peakMask)
    roi = ROI.copy() #np.ones_like(calib,dtype='uint8')  
    #print hdr
    # numPeaks=0
    # print type(len(peaks))
    peakLen=np.array([len(peaks),len(peaks)],np.int64)
    peaksF.write(bytearray(peakLen))
    print(peakLen[0])
    # print "Number of peaks:"+str(len(peaks))
    for i, peak in enumerate(peaks):
        # numPeaks=numPeaks+1
        _seg,_row,_col,_npix,_amax,_atot,_rcgrav,_ccgrav,_rsig,_csig,_rmin,_rmax,_cmin,_cmax,_bkgd,_noise,_son = peak
        # print fmt % (_seg, _row, _col, _npix, _atot)
        _seg = int(_seg)
        _row = int(_row)
        _col = int(_col)
        pk=np.array([_seg,_row,_col],np.int16)
        peaksF.write(bytearray(pk))
        # print type(_seg)
        # print type(_row)
        # print type(_col)
        # print hex(_seg),hex(_row),hex(_col)," in Decimal: ",_seg,_row,_col
        # pk=np.array([_seg,_row,_col],np.int16)
        # print pk[0]
        # print pk[1]
        # print pk[2]
        sr = _row-winR
        er = _row+winR+1
        sc = _col-winR
        ec = _col+winR+1
        # check out of bounds
        if sr < 0:
            sr = 0
        if sc < 0:
            sc = 0
        if er >= calib.shape[1]:
            er = calib.shape[1]
        if ec >= calib.shape[2]:
            ec = calib.shape[2]
        # add peaks found to ROI
        roi[_seg,sr:er,sc:ec] = 0
        gSeg = _seg
        #print er-sr
    # Look at ROI and RONI for this event
    # Remember ROI and RONI have 3D shape (32,185,388)
    # if (t % 1) == 0 :
    # if (t % 100) == 0 :
        # gSeg=4
        # plt.subplot(311)
        # plt.imshow(calib[gSeg,:,:],interpolation='none')
        # plt.title('Detector seg '+str(gSeg))
        # plt.subplot(312)
        # plt.imshow(roi[gSeg,:,:],interpolation='none')
        # plt.title('ROI seg '+str(gSeg))
        # plt.subplot(313)
        # plt.imshow(roni[gSeg,:,:],interpolation='none')
        # plt.title('RONI seg '+str(gSeg))
        # plt.show()
    # if numPeaks != lenPeaks:
      # print "ERROR: numPeaks = "+str(numPeaks)+" != "+str(len(peaks))+" = lenPeaks"
      # sys.exit(1)
    # else:
      # print "GOOD: numPeaks = "+str(numPeaks)+" == "+str(len(peaks))+" = lenPeaks"
    dataF.write(bytearray(calib))
    peaksF.flush()
    # roiF.write(bytearray(roi))
    # roniF.write(bytearray(roni))
      
dataF.close()
peaksF.close()
# roiF.close()
# roniF.close()    

