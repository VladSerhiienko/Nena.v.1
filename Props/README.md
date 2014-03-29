# Nena
## Props/ 
#### (should be interesting for contributors, or those, who will use my classes within this solution folder 'Nena' with my libs and demos)

- When creating a demo (or library), you should first add Nn-build-checked to debug folder(s) (in visual studio property manager tab), or Nn-build-release for release folder(s) (otherwise, release versions of libraries will be linked to applications debug build (or files won`t be found at all), and you will get linker errors
- You have probably noticed some property sheets *d.props - they are here only for compability with with older demos, so that they can be loaded without errors, and will be removed in further releases
- After you have defined a build configuration (when added a nn-build-... props), you should add nn-bsc to all application configuration folders in property manager, optionally you can add nn-dpndc props, if you use 3rd party tools
- Then, in any order add props of features you gonna use:
  - nn-appwnd - adds headers and libs of Application and Window projects
  - nn-dx - adds DirectX, DirectXOverlay, DirectXResources to your project
  - nn-mare - links MayaReader library
  - nn-npx - links Nvidia PhysX library (ensure all the features you would like to use are linked, and a root directory is correct, paths to binaries and dlls are considered in PATH)
  - and others
- Finally, you can add nn-dfapp (in case its a demo) or nn-dflib (its a library) - it will configure output and intermediate directory for you project
