## Triangles and Textures
### <a href='https://web.microsoftstream.com/channel/b84051cb-1dba-4cb4-a271-60cc6635f92f' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction


As usual, make sure you watch the weekly briefing above before attempting to complete this week's workbook tasks.

Although we are still working primarily in 2 dimensions, the aim of this week's workbook is to build up a collection of useful drawing functions that can be used later on in the unit. Much of the later work will be in 3D however, as we shall soon see, we will still need to do a lot of 2D drawing. After all - displays are by their very nature two dimensional !

**Important:** Make sure you read the "Hints and Tips" at the end of each section before attempting that task. This might save you a lot of time and anguish !  


# 
### Task 2: Line Drawing
 <a href='02%20Line%20Drawing/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='02%20Line%20Drawing/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

We start this week by writing one of the core building blocks of any graphics framework - a function to draw straight lines. Although this might seem simple and straight-forward, there are some technical implementation issues that provide a useful learning opportunity !

View the slides and audio narration linked to above and then, using the knowledge gained, add a function to your code to draw a simple line from one point to another. 

A `CanvasPoint` class has been provided in the libs/sdw folder to help you. This class represents a particular point on the drawing window (what is often called the drawing "canvas"). This class contains the x and y coordinates of the point, as well as a number of other attributes that we will use in subsequent workbooks (just ignore these for the time being). Add `#include <CanvasPoint.h>` to your code in order to import this class.

You should allow a colour to be passed into your function so that different coloured lines can be drawn. A `Colour` class has been provided in the libs/sdw folder to help you - this class contains values for red, green and blue channels as well as an optional name for the colour (don't worry about naming your colours in for this workbook - we will use colour names in a later exercise). Add `#include <Colour.h>` to your code in order to import this class.

Test your code by drawing some lines to and from known locations (e.g. top-left corner of the window to the centre of the window, a vertical line in middle of the screen etc.).  


**Hints & Tips:**  
Use the knowledge of interpolation you gained from the last practical sessions to help !

The `CanvasPoint` and `Colour` classes override the `<<` operator so that you can direct them straight to `cout` for debugging purposes.  


# 
### Task 3: Stroked Triangles


Add a new function to your code that draws “stroked” (unfilled) triangles. This function should draw the three outer edge lines of a triangle passed in as parameters. You should also allow a colour to be passed in so that different coloured triangles can be drawn. 

A `CanvasTriangle` class has been provided in the libs/sdw folder to help you. This class represents a 2D triangle for drawing onto the image plane (canvas). This class contains an array of three `CanvasPoints` to store the vertices of the triangle. Add `#include <CanvasTriangle.h>` to your code in order to import this class.

In order to make your application interactive, add to the event handling function so that when the `u` key is pressed, the triangle drawing function is called with a `CanvasTriangle` of 3 randomly generated vertices. You should also randomly generate a colour for the triangle as well !  

Test out your code by pressing the `u` key (lots of times !!!).  
You should end up with a screen that looks something like the one below.  


![](03%20Stroked%20Triangles/images/stroked-triangles.jpg)

**Hints & Tips:**  
Use the standard C <a href="http://www.cplusplus.com/reference/cstdlib/rand/" target="_blank">rand</a> function to help (e.g. `rand()%256` gives a random number between 0-255).


The `CanvasTriangle` class overrides the `<<` operator so that you can direct them straight to `cout` for debugging purposes.  


# 
### Task 4: Filled Triangles
 <a href='04%20Filled%20Triangles/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Filled%20Triangles/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Filled%20Triangles/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Filled%20Triangles/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a> <a href='04%20Filled%20Triangles/animation/segment-2.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Drawing filled triangles is more difficult than you might think ! Take a look at the media linked to by the buttons above (you will notice that there are a number of them - reflecting the complexity of this task). Note that you do NOT need to implement the interactive features demonstrated in the above animations - these are just for illustration purposes. You should however write a new function that takes a `CanvasTriangle` and a `Colour` as parameters and draws a suitable _filled_ triangle. 

As well as drawing a filled triangle of the specified colour, your function should also draw a white _stroked_ triangle over the top of it (this is to make sure that your "rasterising" is correct - both filled and stroked triangles should line up !)  

Make your application interactive, so that pressing the `f` key will cause a random filled triangle to be drawn (with a random colour). You should end up with a screen that looks something like the one below.
  


![](04%20Filled%20Triangles/images/filled-triangles.jpg)

![](04%20Filled%20Triangles/images/skipped-line.jpg)

**Hints & Tips:**  
Watch out for "skipped" lines, as with the orange triangle illustrated above. This is due to truncating floats when converting them into integer screen coordinates - you should really be rounding them to the nearest whole number !

You may like to use the standard C `std::swap` function if you need to do any sorting ;o)  


# 
### Task 5: Texture Mapping
 <a href='05%20Texture%20Mapping/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='05%20Texture%20Mapping/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='05%20Texture%20Mapping/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a> <a href='05%20Texture%20Mapping/animation/segment-2.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

In this task, rather than drawing a filled triangle with a _single_ colour, you will load in a texture map from a file and apply it to the triangle. View the slides, audio narration and animations above to gain an understanding of how to do this. Again, the interactive features (i.e. dragging points around the screen) do NOT need to be implemented - this are just for demonstration purposes in the animations.

A `TextureMap` class has been provided for you in the libs/sdw folder. This class has a constructor that takes a single parameter (the filename of a PPM image file to load). A sample <a href="texture.ppm">PPM texture map</a> has been provided for you to use in this week's practical work. Each `TextureMap` object has publicly accessible `width` and `height` attributes.

`TextureMap` objects also have a `pixels` variable - a vector of pixel-packed RGB data. Note that this `pixels` variable is 1D (i.e. there is no representation of x or y positioning). The pixel data is just stored linearly, with the pixels from each row following directly on from those of the previous row. The vector contains pure RGB data - there is no concept of a "newline" or "end of row" marker. As a result, you will need to be very careful when indexing pixel values from this vector !

You might have noticed that the `CanvasPoint` class has a `TexturePoint` variable - this can be used to maintain the relationship between triangle vertices and positions on the texture map.

In order to test out your texture mapping code, you should attempt to texture map a canvas triangle that consists of the following three vertices:

| Canvas Point |
|:------------:|
|  (160, 10)   |
|  (300, 230)  |
|  (10, 150)   |


You should fill this triangle with pixels from the texture map - the mapping between canvas points and texture map points should be as follows:

| Canvas Point |   | Texture Point |
|:------------:|---|:-------------:|
|  (160, 10)   | → |   (195, 5)    |
|  (300, 230)  | → |   (395, 380)  |
|  (10, 150)   | → |   (65, 330)   |

If all goes to plan, you should end up with a textured triangle that looks like the one below:  


![](05%20Texture%20Mapping/images/textured-triangle.jpg)

**Hints & Tips:**  
Any new canvas points you create (e.g. when splitting triangles) must also be linked to a texture point (calculated by interpolating a suitable position within the texture image).

**Important**: Although the canvas triangles will be flat topped/bottomed, the equivalent texture triangles will NOT be.  

Be careful when calculating the texture map pixel index values - if some of your variables are floats, there is the danger that converting to integer values may lead to skewed texturing !
  


# 
### Progress Logbook

It is important to be able to monitor the progress of students as they progress through these workbooks.
This is to ensure that everyone is keeping up to date with the work and to identify students who are struggling.
For these reasons, it is **ESSENTIAL** that you keep a logbook of progress that you are making.
To minimise the overhead in keeping this logbook, you are only required to capture a single screenshot each week
(you only need to include the DrawingWindow, not the whole desktop).
Try to capture something that summarises the progress that you have made and upload this onto the
[logbook submission area on blackboard](https://www.ole.bris.ac.uk/webapps/assignment/uploadAssignment?content_id=_4837989_1&course_id=_240795_1&group_id=&mode=cpview).
(Note: you might have to log into blackboard _first_ before your click on this link !).
It is important that **EVERYBODY** uploads an image (so we can be sure that everyone is engaging with the unit).
If you don't keep your logbook up to date then you'll find us getting in touch to see if everything is OK !