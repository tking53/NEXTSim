# NEXTSim Reaction Files

[Cory R. Thornsberry](cthornsb@vols.utk.edu)

Last updated: May 24, 2019

## Using a Reaction Source

Instead of using a particle beam or a source with a discrete energy spectrum,
it is possible to use a 2-body kinematics reaction instead. To do this, use
the macro command

```
/nDet/source/reaction pt.rxn
```

where pt.rxn is the filename of a tab-delimited ascii file, an example of which
is shown below

| pt.rxn      |            |               |                     |
|-------------|------------|---------------|---------------------|
| Beam Z      | Beam A     | Beam BE/A     |                     |
| Target Z    | Target A   | Target BE/A   |                     |
| Recoil Z    | Recoil A   | Recoil BE/A   | Recoil Excitation   |
| Ejectile Z  | Ejectile A | Ejectile BE/A | Ejectile Excitation |
| Beam Energy |            |               |                     |

Where Z and A are integer numbers and the binding energy, excitation energy, and
beam energy are specified in MeV. As an example, a reaction file for a p-t reaction
with 2.2 MeV protons is given below.

```
1	1	0
1	3	2.827265
2	3	2.57268	0
0	1	0	0
2.2
```

By default, the beam is defined along the X-axis but the source may be rotated
to any angle in 3d space using `/nDet/source/direction`. The energy of the outgoing 
particle varies as a function of the angle with respect to the beam axis but does 
not vary with azimuthal angle. The neutron energy distribution for such a source 
is shown below.

![PT reaction energy distribution](images/energySpectrum.png "PT reaction energy distribution")

### Using Target Energy Straggling

Occasionally, you may want to include target energy straggling in the simulation of reaction
products. Straggling is useful if, for example, you are simulating a very thick target
or one that is very dense. For such cases, the thickness of the target and the energy
loss of the beam particle in the target as a function of distance may be specified using
`/nDet/source/reaction`. For the p-t example above, assume that the 3H gas cell is *30 mm*
long. The energy loss of the *2.2 MeV* proton beam in such a target is approximately *0.0015 MeV/mm*.
To set the source to account for this we use

```
/nDet/source/reaction rxn/uk_pt.rxn 30 -0.0015
```

For each event, NEXTSim will uniformly select a random point in the thickness of the *30 mm* target
and modify the starting energy of the reaction according to the energy loss of *-0.0015 MeV/mm*. In
addition, the global start time of the event will be offset by the amount of time it would take the
beam particle to reach the random depth inside target assuming it is constantly slowing down.

Adding target energy straggling has three major effects on the reaction. All of which can significantly 
broaden photon time-of-arrival at your detector's PMTs. 
- The initial energy of the reaction now varies by event
- The time-of-arrival of particles at the detector now varies by both angle and by event
- The angle of the particle with respect to the beam varies due to the offset position in the target

