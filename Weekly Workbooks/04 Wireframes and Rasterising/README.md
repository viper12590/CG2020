## Wireframes and Rasterising
### <a href='https://web.microsoftstream.com/channel/b84051cb-1dba-4cb4-a271-60cc6635f92f' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction
 <a href='01%20Introduction/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='01%20Introduction/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

In this workbook we start to work in 3 dimensions ! 
Our fundamental objective is to take a 3D model (specified as a bunch of vertices in 3 dimensional space) and attempt to render them as a 2 dimensional image. In order to achieve this, we will make good use of the 2D drawing functions that you have written previously.

Rendering a 3D model onto a 2D image plane requires us to take a particular perspective/viewpoint on that 3D model. To aid in this discussion, we introduce the concept of a _camera_ - take a look at the slides and audio narration above for an explanation of this concept.

To help you in completing this week's tasks, you have been provided with a `ModelTriangle` class (found in the libs/sdw folder). This class represents a triangular plane in 3D space and contains variables to hold the positions of its three vertices, as well as the colour of the triangle's surface. As usual, this class overrides the `<<` operator so that you can direct it to `cout` for debugging purposes.

For this workbook, you will need to make use of the <a href="https://en.wikibooks.org/wiki/GLSL_Programming/Vector_and_Matrix_Operations" target="_blank">vec3 and vec4 classes</a> from the GLM library - it might be an idea to become familiar with these now !  


**Hints & Tips:**  
**Important**: Make sure you read the "Hints and Tips" at the end of each section before attempting that task. This might save you a lot of time and anguish !  


# 
### Task 2: Reading OBJ Geometry Files
 <a href='02%20Reading%20OBJ%20Geometry%20Files/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='02%20Reading%20OBJ%20Geometry%20Files/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

The first implementation task in this practical is to write a low-level file reader that can parse in 3D models from flat data files. Although the focus of this unit is not on grammars and parsing, understanding how models can be stored in data files provides us with valuable insight into the essential characteristics of 3D geometry and material properties. Check out the slides and audio narration above to find out about the OBJ file format.

Once you are happy with the concepts above, write a function that reads in 
<a href="models/cornell-box.obj" target="_blank">this sample OBJ geometry file</a>
and use the data to populate a vector of `ModelTriangles`. At this stage, you should only focus on reading in vertices (lines that being with a `v`) and faces/facets (lines that being with an `f`). We will deal with the other types of line later on in this workbook.

Once a file has been loaded, loop through all of the triangles in the vector and print them out, (you should be able to send `ModelTriangles` to `cout` !) just to make sure they have all been successfully loaded.

Add a scaling factor (float) parameter to the OBJ loading function that scales the position of all vertices at the point at which they are read in from the file. This is done in order to adjust the size of the model when it is loaded (different models will be created to different scales !). This is to avoid the "giant bunny" situation illustrated in the image below, where two models have been loaded in from separate files, but the model of the rabbit just happens to be of a particular large size.  


![](02%20Reading%20OBJ%20Geometry%20Files/images/giant-bunny.png)

**Hints & Tips:**  
To read in the data from the geometry file, you may like to make use of an `ifstream` and the `getline` function. A `split` function has also been provided (see the `Utils` class in libs/sdw) to make tokenising of lines easier. But it is really up to you how you parse in the files.

Be careful - remember that faces in OBJ files are indexed from 1 (whereas vectors are indexed from 0).

When loading in the Cornell Box model, we suggest you use a vertex scaling factor of 0.17 since this will bring the all dimensions of the vertices within the range of -1.0 to 1.0 (which seems sensible and logical).

You should keep your parser as simple as possible - remember this unit is about graphical rendering, not parsing data. Only implement the aspects of the OBJ file format that you need to complete the task at hand. Your parser will not be marked - it is just a means to an end (to allow you to incorporate complex models and to provide you with familiarity of geometry storage and retrieval).  


# 
### Task 3: Reading OBJ Material Files
 <a href='03%20Reading%20OBJ%20Material%20Files/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='03%20Reading%20OBJ%20Material%20Files/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

Once you have read in the geometry data, your next job is to write a function to read in
<a href="models/cornell-box.mtl" target="_blank">this sample OBJ material file</a>
and populate a "palette" of colours. You can use the previously encountered `Colour` class for this purpose (this is where the `name` attribute now becomes useful).

You could just use a vector to store the palette of colours or, if you want something a bit fancier, use a hashmap/hashtable for more efficient colour lookup (using the name of the colour as a key).

Once you have successfully loaded the materials file, add some additional code to your geometry parsing function (from the previous task) so that it accesses your colour palette when it encounters a reference to a material (lines that begin with `usemtl` in the OBJ material file). Note that `ModelTriangle` objects have a `colour` attribute that can be used to store a reference to the colour of that triangle.  


**Hints & Tips:**  
You should keep your parser as simple as possible - remember this unit is about graphical rendering, not parsing data. Only implement the aspects of the OBJ file format that you need to complete the task at hand. Your parser will not be marked - it is just a means to an end (to allow you to incorporate complex models and to provide you with familiarity of geometry storage and retrieval).  


# 
### Task 4: Projecting onto the Image Plane
 <a href='04%20Projecting%20onto%20the%20Image%20Plane/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Projecting%20onto%20the%20Image%20Plane/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Projecting%20onto%20the%20Image%20Plane/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Now that we have geometry and material data loaded, we can attempt to render a graphical representation of this data onto the image plane (i.e. the SDL drawing window). Projecting a vertex onto the image plane is based around the notion of similar triangles. See the slides, narrated audio and animation above for an explanation of this.

Clearly, calculating the projected position of a vertex on the image plane is going to involve the _(x,y,z)_ position of the vertex in 3D space, as well as the distance between the camera and image plane. There is an additional complexity in that the origin of the Cornell Box model is in the centre of the room, whereas the origin of the image plane is at the top left of the drawing window.

When rendering a scene on the 2D image plane, what we really want is for the centre of the room to appear in the _centre_ of the image plane. To achieve this, we just need to shift the 2D position of each vertex before we draw it onto the screen. If we shift everything along by half the width of the image plane and down by half the height of the image plane, the render will appear centred in the middle of the plane. If we didn't do this, the centre of the room would appear in the top left corner of the drawing window !

The consequence of all of the above discussion is that, for a particular vertex _i_ that has a position in 3D space of _(x<sup>i</sup>, y<sup>i</sup>, z<sup>i</sup>)_ we can calculate its position on the image plane _(u<sup>i</sup>, v<sup>i</sup>)_ using the two formulae provide below (where _f_ is the distance from the camera to the image plane - often referred to as the "focal length"). Note that these formulae assume the vertices are in the _camera coordinate system_ (i.e. the camera is the origin and x, y and z are distances relative to the camera position). The vertices in the OBJ model are however all in the _model coordinate system_ (i.e. relative to the centre of the room). You will need to do a simple transposition in order to convert them from one coordinate system to the other. You will get used to such transpositions in this unit, there are numerous origins we have to cope with !

Use these formulae to project each 3D vertex from the Cornell Box model down onto the 2D image plane. If you draw a single white pixel at each projected point, you should end up with a point-cloud render that looks like the image shown below the formulae. Don't forget to read the "Hints & Tips" section below before attempting this !  


![](04%20Projecting%20onto%20the%20Image%20Plane/images/01-transpose-to-canvas.png)

![](04%20Projecting%20onto%20the%20Image%20Plane/images/02-point-cloud.png)

**Hints & Tips:**  
In the interests of consistency, you should adopt the convention that positive z is "out of the screen" (towards you) and negative z is "into the screen" (away from you).

When implementing your renderer, you will find it useful to store the position of the camera in a `vec3` variable. Start by setting your camera position centred in the x and y dimensions, but "stepped back" a bit in the z (by 4.0). Your initial camera position should therefore be `(0.0, 0.0, 4.0)`

You will also need to create a float variable to store the distance of the camera from the image plane (this is the "focal length" mentioned above). This you should initially set to `2.0` - which makes sense as this would position the image plane halfway between the camera and the model (anywhere else would be a bit crazy really !).

You may find your initial vertex point cloud is a little small - feel free to apply an image plane multiplier to scale the coordinates up so that they fit nicely into the drawing window. Make sure you scale x and y by the same amount (in order to maintain the aspect ratio of the point cloud).

You might find (depending on how you write your code) that your render is upside-down. Any ideas why this might be ?  
Any thoughts about how to fix this problem ?    


# 
### Task 5: Rendering Wireframes


Now that you know how to project individual vertices from a 3D model on the image plane, the next step is to produce a "wireframe" render of the Cornell Box model. The basic principle is to create an equivalent 2D `CanvasTriangle` for each 3D `ModelTriangle` found in the model. You can then reuse your 2D stroked triangle drawing function that you wrote in the previous workbook to create the final "wireframe" render. If everything works out OK, you should get a wireframe render that looks something like the image below.  


![](05%20Rendering%20Wireframes/images/wireframe.png)

# 
### Task 6: Rendering Filled Triangles


After you have successfully rendered a wireframe of the model, the next step is to fill those triangles ! This is going to be easier than you might think !!! The image panel is 2D and you have already written a 2D "filled triangle" rasterising function in a previous workbook. Make sure you use the correct colours to fill the triangles (namely the colours that you read in from the materials file and stored in your palette). The end product of drawing the filled triangles of the Cornell Box should look something like the image below.  


![](06%20Rendering%20Filled%20Triangles/images/overlap.png)

# 
### Task 7: Implementing a Depth Buffer
 <a href='07%20Implementing%20a%20Depth%20Buffer/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='07%20Implementing%20a%20Depth%20Buffer/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='07%20Implementing%20a%20Depth%20Buffer/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

You might have noticed a problem with the above image ? Depending on the order in which the triangles appear in an OBJ file, rendered triangles may sometimes overlap each other inappropriately. For example, in the image above, the blue box is shown overlapping the red one - which is not the case in the true model. View the slides and narrated audio above to understand what causes this problem and what we can do to solve it.

In your code, create a 2D array of floats (the dimensions of the array being the same width and height as the image plane). You are going to use this array to keep track of the z depth of the model element drawn for each pixel. The data stored in this "depth buffer" will allow us to decide what colour to paint a pixel when there is contention. It is important that we store the _**inverse**_ of the z depth of the model element represented. We will explore the reason for this in a future workbook - for the time being you will have to take this on trust (or you could take a look at <a href="https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation" target="_blank">this chapter from scratch-a-pixel</a> if you _really_ can't wait).

When filling the depth buffer, you will need to calculate the z depth of specific points on the surface of a triangle. It is worth noting that at this point in time you do not actually _have_ depth information for all these points. These depths can however be calculated by interpolating from the depth of known points (i.e. the vertices). Watch the animation linked to at the top of this section for help on achieving this.

Use your depth buffer in your drawing functions to help decide what colour to set a particular image plane pixel. In some cases the first colour you draw a pixel will be the correct one, in other cases this colour will need to be overwritten by a subsequent colour. If your depth buffer is operating correctly, you should see something like the image shown below. Don't forget to read the "Hints & Tips" section below before attempting this !  


![](07%20Implementing%20a%20Depth%20Buffer/images/overlap-fixed.png)

![](07%20Implementing%20a%20Depth%20Buffer/images/z-depth.png)

**Hints & Tips:**  
Note that the `CanvasPoint` class has a `depth` attribute that you might like to use to help you keep track of the z depth of each vertex of a `CanvasTriangle`.

When you create the depth buffer array you should first initialise every element to zero. This signifies that at the start (before rendering the scene takes place) that there is no model element represented in any pixel position. Another way to think of this is that this value represents 1/Z where Z is infinity (i.e. the distance to the element that has been drawn is infinite - because there isn't one !).

Because we are using the inverse of the z depth, we must remember that the colour a particular pixel should be drawn must be the model element with the _largest_ 1/Z. Consider the diagram above for example - the pixel on the image plane through which the dotted line passes should clearly be drawn red (since the red triangle is the closest to the camera/eye). However 1/Z for the red triangle (0.5) is _greater_ than that of the blue triangle (0.25). The _largest_ 1/Z always wins !  


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