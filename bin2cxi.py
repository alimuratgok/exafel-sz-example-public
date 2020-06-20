import numpy as np
import os
import h5py
import time
import sys
import glob

if len(sys.argv) != 3 :
  print ""
  print "ERROR: Wrong number of arguments."
  print "Usage: python bin2cxi.py inFile outFile"
  print ""
  sys.exit(0)

inFile=sys.argv[1] #bin file
outFile=sys.argv[2] #hdf5 file

print inFile
print outFile

dim0,dim1,dim2 = [32,185,388]

totSize = os.path.getsize(inFile)
nEvents = totSize / (dim0*dim1*dim2*4)

outF = h5py.File(outFile,"w")
outF.create_dataset('/eventNumber', data=range(nEvents), dtype='int')
dset_data = outF.create_dataset('/data/data', (nEvents, dim0, dim1, dim2), chunks=(1,dim0,dim1,dim2), dtype=np.float32)

with open(inFile,"rb") as inbf:
  data = inbf.read()
  dset_data[:,:,:,:] = np.fromstring(data,dtype=np.float32).reshape((nEvents, dim0, dim1, dim2))
  outF.flush()



outF.close()

