# zephyr-server-mmo

1. The platform that you hope to develop can be adopted.

2. Proof: a good program is not how complicated, but how simple

3. I hope to bring new ideas to online games

TO PROGRAMMER:
The entire platform consists of 2 cores:
Doid distributed object address, which is the basis for the operation of the platform, a new addressing method
Pipe handles the basis of all thread and memory problems. In this thread, data is transmitted through the pipeline in one thread, and one production consumes one without lock.

Design ideas:
Message processing: Divided into 5 modules according to the OSI7 layer structure (Net->Comm->Skeleton->MsgHandler->App)

Logical layer: Vertically split, divided into multiple services according to function.

After the platform is finished, you need to build a deployment tool Service, which can automatically deploy the SerivceContainer and load the new Service.
Then provide files, databases, network connection services, map broadcast services (that is, developers as long as the relationship value processing logic), login verification services, billing services -> constitute a basic cloud platform.

Allows game operators to share server hardware across multiple games, while making game development easier to test and seamlessly integrate with carriers.
In this way, game developers can focus more on game content development without concern for the operator.


Developer: look at the interface first, the interface is in Public/Interface/Platform/include. This is the platform implementation.
Public/Interface/App/include This is the platform callback
Most interfaces have test code Test/

