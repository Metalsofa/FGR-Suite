# FGR-Suite (in development)
Open-source software for creating and editing FGR vector art.

## The FGR file format
The .fgr file format was developed for the purpose of rendering simple graphics efficiently using [freeglut](http://freeglut.sourceforge.net/) as well as other openGL toolkits. These graphics simply consist of some drawing parameters and a set of points. The primary purpose of this file format is for use in the (in development) RPG [Fourier](https://github.com/Metalsofa/Fourier), which uses freeglut for its graphics.

## Usage
There will be more info on usage when this application is actually usable.

## Contributing
This application is developed and maintained using Visual Studio, but contributors can obviously use whatever tools they want. It'll be easiest to work on, however, if you use Visual Studio.

At the moment, if you wish to work on Glimmer using Visual Studio, you'll have to update the include/library directories in the Visual Studio Project properties. In the future, when I figure out how to make project properties use relative include paths, this won't be neccessary. See the FREEGLUT readme for some additional information on that.
