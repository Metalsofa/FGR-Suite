#Glimmer - List of standard commands

##File managemant
 Command | Arguments	| description | shortcuts | example
 ------- | ---------	| ----------- | --------- | -------
map | <Keys> <Simulated keys> | Map a key or sequence of keystrokes to a set of keystrokes to simulate. This can include using the console with ':'. | none | `:map b :tool 7<CR>` |
q[uit] | none	| Exits the program. Use 'q!' to force quit. | q | `:q` |
warpcursor | <X-pox> <Y-pox> | Force the mouse to warp to the specified pixel on the screen. | warp | `:warp 600 350` |
click | none | Simulate a click at the current location of the mouse | none | `:click` |
w[rite] | <Filename(optional) | Write the file currently edited, or specify a new file name. | w | `:w my_art.fgr` |
source | <Filename> | Provide a filename for the console to interpret as a set of commands. | none | `:source .glimrc` |
e[dit] | <Filename> | Provide a filename and open it in the current editor |
home | none | Reset all perspective transformations (pan, zoom, rotation) | h | `:home` |
fit | none | Set the pan and zoom to just fit the current artwork within the viewport | none | `:fit` | 
mode | <GLModeName/GLModeNum> | Set the current shape/glyph's GL rendering mode | none | `:mode GL_QUAD_STRIP` |
clear | none | Clear all vertices from the current glyph/shape. | none | `:clear` |
iterations | <IterationCount> | If in the experimental fractal mode, set the number of iterations this way | none | `:iteration 5` |
shapen | none | Move to edit the next shape | none | `:shapen` |
shapep | none | Move to edit the previous shape | none | `:shapep` |
nshape | none | Create a new shape after this one | none | `:nshape` |
zen | none | Toggle *zen mode* | none | `:zen` |
c[olor] | 
linewidth
v[ertex]
pointsize
tool
convert | <Target file type> | Convert the current art into another type | none | `:convert fgr` | 
