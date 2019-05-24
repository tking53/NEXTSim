# NEXTSim Segmented PMTs

[Cory R. Thornsberry](cthornsb@vols.utk.edu)

Last updated: May 24, 2019

## Using a Segmented PMT

By default, optical photon sensitive surfaces (referred to as PMTs here) are
not segmented. In order to enable PMT segmentation, use the following commands

```
/nDet/detector/setPmtColumns 8
/nDet/detector/setPmtRows 8
```

These two commands will set the number of columns (along the X-axis or width 
of the detector) and the number of rows (along the Y-axis or thickness of the
detector) of anodes that a segmented PMT has. 

### Using an Anode Gain Matrix

When you set more than one PMT columns or rows, you have the ability to specify
the gain of each individual PMT anode.

The gain of a PMT anode is used by NEXTSim to set the amplitude of the
single photon response in the simulated PMT light response class. By default, 
the individual gains of the anodes of a segmented PSPmt are all equal to 100%. 
In some cases, it is necessary to set the gain of each anode individually to 
better represent a physical detector.

To load a NEXTSim anode gain matrix use the command

```
/nDet/detector/setGainMatrix gain.dat
```

where gain.dat is the filename of a tab-delimited ascii file, an example of which
is shown below. Note that the command `nDet/detector/update` **must** be called
before you attempt to read in a gain matrix otherwise the command will fail.

```
78	80	85	88	89	81	89	96
77	84	92	95	96	93	91	98
77	68	78	91	92	83	84	96
78	69	82	95	97	86	83	94
78	70	80	92	97	84	83	95
75	71	79	89	99	85	81	95
70	77	81	87	100	92	96	93
72	73	74	80	90	93	99	92
```

The gain matrix above is taken from the spec sheet of a Hamamatsu H12700A segmented
PMT with 8x8 anodes. All values are given as percentages relative to the gain of the
anode with the highest gain. There must be at least as many values in the file as there 
are anodes in the PSPmt otherwise the command will fail.
