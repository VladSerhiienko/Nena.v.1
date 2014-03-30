# Nena
## Demo/ - source files for demo applications (see Bin/ for binaries)
#### There are some projects you might be interested in:

Folders:
  - **SimpleSkin/, SimpleAnimation/** - shows how to use MayaReader and to load *.ma.xml files, then find meshes, transforms, clusters and curves you need, how to perform skinning on the CPU and then animate skeleton (resolving both approximate (for simple hermite curve segment - for baked animation curves - FBX SDK has such option when exporting) and exact curve values (requires finding solution for cubic equation fast) for any given time within the animation curve time range (out of bound attemps return the very left or right curve values))
  - **SmallTalk/** - the Microsoft Excel 2010 (or later) add-in, that shows, how to load data from .xls(x) file, how to use Excel API, use built-in functionality (resolve Students t-distribution values, Chi-squared distribution, Fisher distribution and others), how to perform Grubbs test for outliers, ANOVA (analysis of variance), check normalization hypothesis and normalize sample, how to resolve cental and raw moments and coefficients (variance, covariance, asymmetry, kurtosis, mean, std deviation and others), find coefficients for any polynomial regression and some advanced checks for linear model, correlation analysis; it also contains classes for integration, solving LES (gauss solver), generating combinations, and also a set of advanced math functions (ExtendedMath.cs) I took in the internet (but, didn`t used, because I found Excel API had everything I needed to complete labwork task); it also shows, how to add charts to you document
  - **Extra/PolyAreaCalculation/, Extra/PolyPointIntersection** - line-line intersection, segment-segment intersection, polygon-point intersection
  - **Fire/, ShadowMapping/, SoftShadowMapping/** - basically, those projects are Rastertek tutorials, implemented using my classes (SoftShadowMapping uses compute shaders with cached and separable gaussians for blurring render target)
  - **SimpleLight/, SpecularLight/** - simple NdotL lighting algorithms implemented
  - **SimpleRaycast/** - finds out whether camera ray intersects a sphere in the scene
  - **SimpleText/** - Direct2D (DirectWrite actually) interoperation with Direct3D sample
  - **Isoclus/** - ISOCLUS clusterization algorithm, implemented with C#
  - **Pathfinding/** - implements A* and D* pathfinding algoritms (one project has two algorithms implemented)
  - **MayaWriter/** - parses *.ma files to *.ma.xml to xml format, that can be imported with MayaReader (see Lib/MayaReader)
  - **GestureWinemit/** - 
  - **Projectiles/** - simulates projectiles trajectories with initial simulation parameters defined
  - **Satellites/** - simulates satellites trajectories in space
  - **Puzzles/** - heuristic solver for 3x3 puzzle game
  - **Speech2Text/, Text2Speech/** - using windows api (one should have just ideal pronunciation)
