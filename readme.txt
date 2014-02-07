Description of the software archive eSMCs Robotino
The eSMCs project, EU grant no 270212, esmcs.eu
Alexander Maye, University Medical Center Hamburg-Eppendorf
01/05/2014
______________________________________________________________

The archive contains the computational model of extended sensorimotor contingencies (eSMCs) that is described in [1]. The model controls the behavior of a Robotino robot (Festo didactic, Germany). eSMCs are conceived of as sequences of actions (movements) and sensory observations. These sequences are gathered during an initially random exploration and stored in a tree data structure. Each eSMC has an associated value that reflects the usefulness of this sensorimotor interaction for the robot. Collisions, for example, carry a low value, whereas straight movements have a high value. With sensorimotor experience accumulating, the value to be expected from future actions can be predicted, and actions can be selected accordingly. This results in a behavior that avoids collisions and favors straight movements.

The methods are implemented in C++ and rely on Qt4 for the GUI and the OpenRobotino-API for controlling the robot. QtCreator was used as the project management tool, but other IDEs can be used as well.
The UI is structured in three windows. The first offers direct interaction with the robot. It is used to position the robot manually by pushing buttons for the respective directions, and to connect the Robotino API to the robot via wifi. It allows to record raw data from the Robotino and play them back later to the model. It also shows a real-time image from the robot's camera and plots of the battery power and current consumption. This window is implemented in the files robotinowindow.*.
The second window visualizes the preprocessed sensor data. It is implemented in the files robotinofeaturewindow.*. The third window is used to interact with the computational model of eSMCs. It visualizes various properties of the current sensorimotor context and the action selection. The learned sensorimotor contingencies can be stored to disc and loaded back into memory. This window is implemented in the smcwindow.* files.
The core of the model is implemented in the class SMCThread (smcthread.*), that runs in a separate thread and is concerned with storing, updating, and matching eSMCs as well as selecting the next actions.
All other files implement helper classes for the various visualization gadgets or for interacting with the Robotino.

[1] Maye A, Engel AK (2012) Using Sensorimotor Contingencies for Prediction and Action Planning, In: Ziemke T,  Balkenius C, Hallam J (eds.) From Animals to Animats 12, Springer Berlin Heidelberg, 2012, 7426, 106-116 