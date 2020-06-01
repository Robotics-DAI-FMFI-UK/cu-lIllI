# Humanoid Robot Lilli
You will follow development on a page: [kempelen.dai.fmph.uniba.sk/lilli/index.php/Building_lIllI](https://kempelen.dai.fmph.uniba.sk/lilli/index.php/Building_lIllI)

## Goals (ENG)
 Robot Lilli has been presented at Maker Faire 2018 in Vienna by its author Per R. Ø. Salkowitsch. It is a humanoid robot with 25 degrees of freedom constructed of laser-cut plywood pieces. There is no control sotware for the robot available at the moment. The aim of the diploma thesis is to study and implement algorithms that will allow the robot to move in its environment including inverse kinematics and utilizing the Machine Learning algorithms. It is expected that the student will develop a simulated model of the robot and verifies the algoithms both in simulation and on the real robot. 

## Goals (SVK)
Robot Lilli predstavil na viedenskom podujatí Maker Faire 2018 jeho autor Per R. Ø. Salkowitsch. Ide o humanoidného robota s 25 stupnami voľnosti vytvoreného z dielov vyrezaných z preglejky laserom. K robotu zatiaľ neexistuje obslužný softvér. Cieľom diplmovej práce bude preskúmať a implementovať algoritmy, pomocou ktorých sa robot bude vedieť pohybovať vo svojom prostredí, vrátane inverznej kinematiky a využitia algoritmov strojového učenia. Predpokladá sa vytvorenie modelu robota pre simuláciu a otestovanie algoritmov v simulácii i na reálnom robotovi.

## Folder Guide

- documentation: you can find the master thesis here
- hardware/autocad/lilli_dwg_parts: contains extruded dwg autocad parts<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/autocad/lilli_dxf_parts: contains extruded dxf autocad parts<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/autocad/lilli_stl_parts: contains stl files for all parts<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/solidworks/assemblies: contains modeled assemblies of Lilli<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/solidworks/new_parts: contains modeled Solidwork PART for all the robot parts<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/solidowrks/servos: contains the MG996R, DS3225  and MG90S servo models and horns<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/URDF/LilliHumanoid/meshes: contains the visual mesh files<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/URDF/LilliHumanoid/urdf: contains the 3D model described in URDF<br />
- software/LillyRemoteAPI: contains the Java client with 3 Java classes to control the robot simulation<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/LilliArmMovement.ttt: coppeliaSim .ttt file. Open this in coppeliaSim and run the simulation. After it change the sphere position via mouse translation (Click to object/item shift icon). The kinematic chain will follow the sphere and the joint variables are computed by inverse kinematics calculations.<br />
<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/LilliArmWarm-up.ttt: Open this saved scene in CopppeliaSim and run the simulation. After this open LilliRemoteApi project in some Java IDE and run the moveArm() function in the Initializer.java init function. The robot will do a warmp-up with an arm which tests the model correction.<br />
<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/LilliLegWarm-up.ttt: Open the scene in CoppeliaSim and run the simulation. The joints are configured. Run in Java IDE the moveLeg() function in the Initializer.java init function. The robot will do a warmp-up with a leg.
<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/LilliLegWarmp-upWithChildScript.ttt: Open the scene in CoppeliaSim and after that start the simulation with play button. The robot was control in velocity. The robot will make a leg warm-up. 
<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/LilliArmWarmp-upWithChildScript.ttt: Open the scene in CoppeliaSim and after that start the simulation with play button. The robot was control in velocity. The robot will make an arm warm-up.
<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/LilliLegWarmp-upControlViaPosition.ttt: Open the scene in CoppeliaSim and after that start the simulation with play button. The robot was control in position. The robot will make legs warm-up.
<br />
&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&ensp;&nbsp;/LilliArmWarmp-upControlViaPosition.ttt: Open the scene in CoppeliaSim and after that start the simulation with play button. The robot was control in position. The robot will make arms warm-up.