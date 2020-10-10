## Pixels and Drawing
### <a href='https://web.microsoftstream.com/channel/b84051cb-1dba-4cb4-a271-60cc6635f92f' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction


Before commencing with this week's workbook, make sure you have first watched the "Weekly Briefing" linked to above.

This week we will take a look at the fundamentals of computer graphics: namely pixels and drawing. Everything that we cover this week will be in 2 dimensions. In future weeks we will move on to begin working in 3D, it is however important that we walk before we try to run !

You will hopefully find this workbook fairly straightforward and should be able to complete it relatively quickly. This is intended to be a gentle week in order to provide some flexibility in the workload - in case you are still having difficulties installing SDL2, compiling and running your code. Next week the workbooks start to get more challenging !  


# 
### Task 2: Single Element Numerical Interpolation


Interpolation (informally: "filling in the gaps between known values") is an essential operation in computer graphics. We will use it time and time again in this unit (for various different purposes). It is essential that we become familiar with the basic process - we are going to need to interpolate some complex constructs later on.

Let us start simply: create a duplicate of the _RedNoise_ template (giving the new project a suitable name) and then add a function called _interpolateSingleFloats_ that takes 3 parameters:
- from: a floating point number to start from
- to: a floating point number to go up to
- numberOfValues: the number of steps required from the start to the end

This function should return an **evenly spaced** list (as a vector) of size _numberOfValues_ that contains floating point numbers that start at _from_ and ends at _to_.

So, for example, calling: `interpolateSingleFloats(2.2, 8.5, 7)` should return a vector containing the following values: `2.2, 3.25, 4.3, 5.35, 6.4, 7.45, 8.5`

Test out your function to make sure it correctly prints out the above values by using the following code in your project's main function:

``` cpp
std::vector<float> result;
result = interpolateSingleFloats(2.2, 8.5, 7);
for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
std::cout << std::endl;
```

Note that `std::cout` refers to the character output object of the standard library and that `std::endl` is a flushed newline. It is also worth commenting that `size_t` is an unsigned integer type provided by C++ that is useful for comparing with the size of things (like vectors).  


# 
### Task 3: Single Dimension Greyscale Interpolation
 <a href='03%20Single%20Dimension%20Greyscale%20Interpolation/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='03%20Single%20Dimension%20Greyscale%20Interpolation/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

Interpolating numbers is fine, but this unit is supposed to be about graphics - so let's draw something ! Using your floating point interpolation function that you wrote in the previous task, draw a greyscale gradient across the _DisplayWindow_, from left to right (as shown in the image below). 

You will need to create a range of RGB values packed into `uint32_t` variables in order to set the colour of each pixel. In order to achieve this, you will need to take a look at the slides and audio narration for this section (the blue buttons above) to find out more about pixels and pixel colour packing.  


![](03%20Single%20Dimension%20Greyscale%20Interpolation/images/grey-scale.jpg)

**Hints & Tips:**  
Use the pixel looping code from the _RedNoise_ template to help get you started.  
Don't worry about the transparency/alpha of the pixels - just set it to 255 (fully opaque) as in the _RedNoise_ template.  


# 
### Task 4: Three Element Numerical Interpolation


Write a new function called _interpolateThreeElementValues_ that operates on three-element floating point values, rather than just single floats. These three-element floating point values should be instances of the <a href="https://en.wikibooks.org/wiki/GLSL_Programming/Vector_and_Matrix_Operations" target="_blank">vec3</a> class provided by GLM. If we have a function that can interpolate these, then we can deal with a wide range of entities including colours, 3D coordinates (in fact anything with 3 elements !)  

If we have the following two `vec3` variables:
``` cpp
glm::vec3 from(1, 4, 9.2);
glm::vec3 to(4, 1, 9.8);
```
And pass them into our new interpolation function (along with a _numberOfValues_ of 4), then we should get the following results back:

``` cpp
(1, 4, 9.2)
(2, 3, 9.4)
(3, 2, 9.6)
(4, 1, 9.8)
```

Write your function and then test it with the above values in order to check that it operates as intended.  


# 
### Task 5: Two Dimension Colour Interpolation
 <a href='05%20Two%20Dimension%20Colour%20Interpolation/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

In the earlier grayscale interpolation task, we were interpolating a single float value in just one dimension (the x axis). In this current task we will interpolate 3 float values (Red / Green / Blue) in two dimensions (x and y). Watch the animation button above to view a video showing more details about interpolating RGB colour values. Once you are confident with the concepts covered, implement the 2D RGB interpolation in order to produce the effect shown in the image below:
  


![](05%20Two%20Dimension%20Colour%20Interpolation/images/colour-spectrum.jpg)

**Hints & Tips:**  
A good way to tackle this problem is to start by creating variables for the four corners of the window and initialising them with the following primary colours:

``` cpp
glm::vec3 topLeft(255, 0, 0);        // red 
glm::vec3 topRight(0, 0, 255);       // blue 
glm::vec3 bottomRight(0, 255, 0);    // green 
glm::vec3 bottomLeft(255, 255, 0);   // yellow
```

Then, using your previously written `vec3` interpolation function to help:
1. Calculate the colour of all the pixels in the first (left-most) column of the window
2. Calculate the colour of all the pixels in the last (right-most) column of the window
3. For each row, calculate the colour of all the pixels in that row by interpolating between the first and last pixels  


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