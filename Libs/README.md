#Nena
##Libs/...
- App/, Window/ (in __Application_vs2013.sln__)
  - Constains classes, designed for real-time applications, using Win32 API. Those classes are implemented w/o virtual functions but still are very flexible and easy to use and to understand. App events are designed so that one can handle very complex pipelines, combine those pipelines, make changes in runtime saving robustness, design different scenarios for his app, that will execute seamlessly and fasy
  - Contains utility functions for converting strings to numbers and vice versa
  - Contains thread-safe class for reading/writing/editing xml configuration file for applications
  - Contains classes that allow communication between instances through tcp sockets (implements some sort of server, that can accept multiple clients, and a client)
  - Constains a class for creating an ordinary or borderless (for games) window, switching to fullscreen and back
  - Contains classes, that imitate C# events and delegates (are fast but not enough for real-time games, especially, when are utilised for direct window events handling in core classes, and are not used within this library but are stable and tested in my previous demos; the original implementation was found on codeproject a year ago (I have improved it a bit)

- DirectX/, DirectXOverlay/, DirectXResources/ (in __Graphics_vs2013.sln__)
  - Contains classes that handle DirectX device, context and rendering resources are created correctly and are compatible with each other, reacts on screen rotations (I took code from DirectX samples as a base for those classes, extracted some core classes and interfaces, added Direct2D interoperability features to ensure both render targets are compatible)
  - Contains a class, that implements a step timer for simulations and animations (can be used with fixed step, has fixed timestep update logic for low fps applications - ensures they are running as smoothly as possible)
  - Contains a class for creating Direct2D device, context and rendering resources, that are compatible with Direct3D stuff
  - Contains utility classes and functions for loading and/or creating shaders, textures/images, buffers (contains asynchronous functions, using ppltasks.h)

- Recognizing/ (in __Recognizing_vs2013.sln__)
  - Perceptual_vs2013:
    - Contains classes for gesture recognition (requires Interactive Gesture Cam) and face tracking (any web camera device can be used) using Intel Perceptual Computing SDK 
    - Video stream location in ::Perc::Capture class is very similar to UtilCapture class, provided with SDK, but my class has a bit more straightforward usage pattern - there is ::Perc::Sample class that is used for sampling video streams (the original ...->QueryImage(IMAGE_DEPTH) drove me crazy); also, there are more classes, that use my capture class and, optionally, my sample class for capturing images from  streams and processing them
  - Luxand_vs2013:
    - Contains classes for live face recognition using Luxand SDK (you should get your own evaluation key, otherwise sdk activation will fail; you can insert you key in application configuration file - app.vars.xml - there is a reserved record - 'App/Video/Tracking/LuxandSDK/License'; there are some more reserved records with 'App/Video/Tracking/LuxandSDK/...', but they can stay unchanged or can be removed, unless you have several video device connected to PC and you want to use a particular one with desired settings like capture dimensions) 
