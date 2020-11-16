## Lighting and Shading
### <a href='https://web.microsoftstream.com/channel/b84051cb-1dba-4cb4-a271-60cc6635f92f' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction
 <a href='01%20Introduction/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='01%20Introduction/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='01%20Introduction/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='01%20Introduction/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

In this workbook we explore some of the more advanced aspects of model rendering. In particular, we will focus on the use of lighting and shadow effects to make our rendering more realistic and aesthetically appealing.

In the slides and audio narration above, we introduce three distinct types of light as a convenient approximation to real-world lighting:

- Diffuse lighting (which can further be broken down into proximity and angle-of-incidence)
- Specular highlighting (to provide a sense of a shiny/reflective surface)
- Ambient lighting (as an approximation to global illumination)

To aid your understanding of this topic, the animation above provides an illustration of the appearance and effect of each type of light. Ensure the you have an understanding of the overall concepts before moving on to the next task. In the following sections, we go on to explore the implementation of these different types of light in more detail.  


# 
### Task 2: Proximity Lighting
 <a href='02%20Proximity%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='02%20Proximity%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='02%20Proximity%20Lighting/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Proximity lighting takes into account the distance of a surface from a light source. Intuitively, the closer a surface is to a light, the brighter it will appear. Take a look at the slides, audio commentary and animation above to gain an appreciation of how we might go about calculating the effect of proximity lighting. Then add some code to your renderer that will adjust the brightness of each pixel to take into account proximity to the light source.  


**Hints & Tips:**  
Your brightness values should be within the range 0.0 (total darkness) to 1.0 (fully illuminated). These values can then be used as a multiplier to adjust the brightness of a pixel. Simply multiply each of the three colour channels (RGB) by the brightness float, before you pack them into the final 32 bit pixel colour.

To find the distance between the light and a point on a triangle, you might like to make use of the GLM `length` function - this will give your the magnitude of a `vec3` variable. Don't normalise your `vec3` otherwise you'll always get a length of 1 !

Your algorithm should be based on the 1/4&pi;r<sup>2</sup> approach outlined in the slides, however feel free to experiment with multipliers in order to produce an effect that you think looks appropriate (this is were aesthetics rule over science !)

To help you implement and debug your code, you may wish to add some additional key event handlers that allow you to shift the position of your light source around the model.  


# 
### Task 3: Angle of Incidence Lighting
 <a href='03%20Angle%20of%20Incidence%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='03%20Angle%20of%20Incidence%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='03%20Angle%20of%20Incidence%20Lighting/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a> <a href='03%20Angle%20of%20Incidence%20Lighting/animation/segment-2.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a> <a href='03%20Angle%20of%20Incidence%20Lighting/animation/segment-3.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Your next task is to implement "Angle-of-Incidence" lighting. This first requires you to calculate a surface normal for each triangle in the model (as shown in the slides and audio narration above). To help achieve this, you might like to use the GLM `cross` function to calculate the cross product of two edges of a triangle.

It is worth noting that the `ModelTriangle` class has a `normal` attribute that can be used to store the calculated normal for each triangle. You can calculate these normals when you first load in the OBJ geometry file - they aren't going to change during the render (unless you start changing the orientation of triangles during some kind of animation).

Once the surface normals have been calculated, you can then use these to calculate the angle of incidence of light falling onto the surfaces. This is done by taking the dot product of the normal and light direction vectors (as shown in the slides and audio narration above). To calculate dot products, you might like to use the `dot` function provided by GLM.

Once calculated, use these angles of incidence to adjust the brightness of each pixel (in a similar way to that which you did with proximity in the previous task). You should end up with a render that looks like the image shown below. If you have implemented event handlers to move the position of your light point, use this feature to test that your lighting looks realistic from a number of different positions.


  


![](03%20Angle%20of%20Incidence%20Lighting/images/diffuse-lighting.png)

**Hints & Tips:**  
The "winding" order used by the model will impact how you should calculate the surface normals. The best advice is to just to assume a particular order and try it. You will soon know if you were wrong (all your surfaces will be dark) so you can then just switch to using the other winding order.

When considering the angle that the light hits the surface...  
you **must** use: _the direction vector of the LIGHT from the POINT ON THE SURFACE_  
and **not**: _the direction vector of the POINT ON THE SURFACE from the LIGHT_  
If you use the wrong one, you will end up lighting the back surface of your triangles !
  


# 
### Task 4: Specular Lighting
 <a href='04%20Specular%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Specular%20Lighting/slides/segment-2.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Specular%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Specular%20Lighting/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

Your next task is to implement a "Specular" lighting effect (as illustrated in the introductory animation to this workbook). The slides and audio narration in this current section provide a description of specular light and how to calculate the brightness of specularly illuminated pixels.

This process involves calculating the outgoing angle of reflection `R` of an incoming beam from the light. This in itself is more complex than you might initially think. For this reason, we have included an additional slide and audio narration in order to help you calculate this particular element. View these now if you haven't done so already.

When you are ready, implement a specular lighting effect in your renderer. As with the previous tasks, you should again make use of the GLM `dot` function, as well as the surfaces normals that you previously calculated. The image below provides some examples of the kinds of specular highlight you might expect to see:  


![](04%20Specular%20Lighting/images/various-exponents.jpg)

**Hints & Tips:**  
You should use a relatively large exponent in the calculation of specular spread (for example 256). This will ensure that you get a tightly focused spot/beam of specular light which will be easier to see (compared to a specular highlight with a wider spread).

You may also need to move the light to an appropriate location in the scene in order to be able to see the specular highlight effect from the point-of-view of the camera.  


# 
### Task 5: Ambient Lighting
 <a href='05%20Ambient%20Lighting/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='05%20Ambient%20Lighting/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

In the real world, light bounces off objects in order to illuminate "hard to reach" areas of a model (even areas that are in shadow). Calculating such "indirect" lighting is computationally expensive, however it is possible implement a plausible approximation to such lighting with very little additional effort.

Two approaches to creating such ambient light level are commonly used:
- Minimum threshold: An IF statement (or call to the `max` function) is used to ensure that the brightness of any point never falls below a certain threshold level.
- Universal supplement: The brightness of a pixel is calculated, then a fixed amount of additional light is added (the same amount to each and every pixel).

Each approach has its own subtle advantages and disadvantages, yet both can be used to achieve the desired effect as illustrated in the image below. Compare the shadow regions of this image with those of the previous week's workbook (which included no ambient lighting).  


![](05%20Ambient%20Lighting/images/shadows-with-ambient.png)

# 
### Task 6: Gouraud Shading
 <a href='06%20Gouraud%20Shading/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='06%20Gouraud%20Shading/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='06%20Gouraud%20Shading/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='06%20Gouraud%20Shading/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Due to the fact that we represent models as a series of triangles, any artefacts within that model will have a finite resolution. When rendering a model, unless we have a huge number of triangles, the surfaces may well appear "blocky" and "low-res". But what if we could shade triangles more "intelligently" and blend their edges together ? That would smooth off the sharp corners and provide an apparently higher resolution final rendering ! _Gouraud shading_ in one such intelligent approach to surface shading.

View the slides and audio narration above to gain an understanding of Gouraud shading. Using this knowledge, implement Gouraud shading in you renderer. In order to achieve this, you will need to make use of _Barycentric coordinates_ to interpolate the brightnesses across the face of each triangle. The operation of Barycentric coordinates is illustrated in the animation at the top of this section.  


**Hints & Tips:**  
The Cornell Box isn't the most useful model for testing out Gouraud shading (it's hard to smooth off the edges of a cube !). For this reason, we have provided a very simple <a href="models/sphere.obj" target="_blank">additional model</a> for you to use.

  


# 
### Task 7: Phong Shading
 <a href='07%20Phong%20Shading/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='07%20Phong%20Shading/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

Gouraud shading in fine, but it only goes so far in creating smoothly rendered model surfaces. _Phong shading_ is a more sophisticated approach that takes shading one step further (although it can only achieve this at the cost of additional computational overhead !). Take a look at the slides and audio narration above to gain an understanding of Phong Shading. Adapt your renderer to make use of this more sophisticated approach.

Note that the audio narration makes reference to the concept of "bump maps" (which you may wish to explore in the assignment). We mention them here in order to link this concept into the discussion on Gouraud shading.  


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