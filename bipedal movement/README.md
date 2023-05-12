Lilli.ttt   -scene that can be open in CoppeliaSim (for example via command bellow)

./coppeliaSim.sh Lilli.ttt

model.ttm   -model of robot Lilli with legs next to each other, in init() function robot will move its upper body to the right

modelK10.ttm -model of robot with bended left knee and left leg in the front (modelK15.ttm and modelK20.ttm are other models with the same properties except with greater bending in left knee)

Child script is executed from dummy object Resizable_Floor_5_25, which can be opened with double clicking on the icon next to it
On line 86 in code and bellow are some function walking genes that can be viewed in simulation by uncommenting them 
