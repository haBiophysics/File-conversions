# A simple script to visualise ab1 chromatogram files.
# This is the file format for most Sanger sequencing facilities.
# See https://biopython.org/wiki/ABI_traces

from Bio import SeqIO
import matplotlib.pyplot as plt
import numpy as np

record = SeqIO.read('10b.ab1', 'abi')

record.annotations.keys()
record.annotations['abif_raw'].keys()
channels = ['DATA9', 'DATA10', 'DATA11', 'DATA12']

from collections import defaultdict
trace = defaultdict(list)
for c in channels:
	trace[c] = record.annotations['abif_raw'][c]

A = trace['DATA9']
T = trace['DATA10']
C = trace['DATA11']
G = trace['DATA12']

baseStartPos = 2000
baseEndPos = 8000
A = np.array( A[baseStartPos:baseEndPos] )
C = np.array( C[baseStartPos:baseEndPos] )
G = np.array( G[baseStartPos:baseEndPos] )
T = np.array( T[baseStartPos:baseEndPos] )

total = A + C + G + T
pA = 1e-8 + A / total.astype(np.float)
pC = 1e-8 + C / total.astype(np.float)
pG = 1e-8 + G / total.astype(np.float)
pT = 1e-8 + T / total.astype(np.float)

#plt.plot(total, 'black')
plt.plot(pA, color='blue')
plt.plot(pC, color='red')
plt.plot(pG, color='yellow')
plt.plot(pT, color='green')


# Find peak positions in the chromatogram
totalPeaks = np.logical_and( total > np.roll(total,1), total > np.roll(total,-1) )
APeaks = np.logical_and( pA > np.roll(pA,1), pA > np.roll(pA,-1) )
CPeaks = np.logical_and( pC > np.roll(pC,1), pC > np.roll(pC,-1) )
GPeaks = np.logical_and( pG > np.roll(pG,1), pG > np.roll(pG,-1) )
TPeaks = np.logical_and( pT > np.roll(pT,1), pT > np.roll(pT,-1) )

S = -( pA*np.log(pA) + pC*np.log(pC) + pG*np.log(pG) + pT*np.log(pT) )
print np.mean(S[totalPeaks])
print np.mean(total[totalPeaks])

plt.plot(pA*APeaks, 'ro')
plt.plot(pC*CPeaks, 'ro')
plt.plot(pG*GPeaks, 'ro')
plt.plot(pT*TPeaks, 'ro')
plt.show()
