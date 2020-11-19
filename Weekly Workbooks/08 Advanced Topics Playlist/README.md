## Advanced Topics Playlist
### <a href='https://web.microsoftstream.com/channel/b84051cb-1dba-4cb4-a271-60cc6635f92f' target='_blank'> Weekly Briefing ![](../../resources/icons/briefing.png) </a>
### Task 1: Introduction


This workbook does not contain any explicit practical activities for you to complete. Rather, it provides a "playlist" of slides, audio commentary and animations of advanced rendering topics. Some of these techniques you may wish to implement in your rendering engine, others you may wish only to gain an appreciation of.

If you are taking the coursework assessment variant of this unit, implementing a selection of these in your final assignment should result in a higher grade. If you are taking the examination variant of this unit, you will not be assessed on these topics - however you may wish to explore them for your own interest.  


# 
### Task 2: Perspective Correction
 <a href='02%20Perspective%20Correction/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='02%20Perspective%20Correction/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='02%20Perspective%20Correction/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

There is an issue with the basic texture mapping approach that we used in the rasteriser workbook. The problem is that it doesn't take into account the effect of perspective. Review the slides and audio narration to gain an understanding of the nature of the problem and how we might go about solving it.  


# 
### Task 3: Soft Shadow
 <a href='03%20Soft%20Shadow/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='03%20Soft%20Shadow/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='03%20Soft%20Shadow/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='03%20Soft%20Shadow/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Up until this point we have taken a simplistic approach to shadows. By only considering single point light sources we can only produce naive and simplistic "hard" shadows. If we want to implement more realistic "soft" shadows we are going to need a more sophisticated approach using multi-point light sources.  


# 
### Task 4: Reflective Materials
 <a href='04%20Reflective%20Materials/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='04%20Reflective%20Materials/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Reflective%20Materials/audio/segment-2.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='04%20Reflective%20Materials/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Rather than rendering just opaque, matt surfaces, it would be nice if we could render more complex materials. The slides, audio narration and animation above provide details of how to render triangles as reflective surfaces - in order to represent materials such as metals or other mirrored surfaces.  


# 
### Task 5: Environment Maps
 <a href='05%20Environment%20Maps/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='05%20Environment%20Maps/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a>

The problem with rendering mirrored surfaces is that they reflect everything that surrounds them ! This can make rendering complex scenes particularly costly. We can use flat images in the form of "Environment Maps" to capture a view of the surrounding area from the perspective of a 3D object. Because these maps are flat images (rather than 3D models in their own right) this can make the computation of reflections much more efficient.
  


# 
### Task 6: Refractive Materials
 <a href='06%20Refractive%20Materials/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='06%20Refractive%20Materials/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='06%20Refractive%20Materials/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a> <a href='06%20Refractive%20Materials/animation/segment-2.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Rather than rendering just opaque, matt surfaces, it would be nice if we could render more complex materials. The slides, audio narration and animations above provide details of how to render triangles as refractive surfaces - in order to represent materials such as glass or water.  


# 
### Task 7: Bump Maps
 <a href='07%20Bump%20Maps/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='07%20Bump%20Maps/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='07%20Bump%20Maps/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Finding the closest intersection between a ray and a set of triangles is a costly activity (especially with complex models containing many triangles). But what if we were able to create "fake" geometry using just "light & shadows" (and only a minimal number of actual triangles). Sounds like magic ? But bump maps allow us to do just that.  


# 
### Task 8: Photon Maps
 <a href='08%20Photon%20Maps/slides/segment-1.pdf' target='_blank'> ![](../../resources/icons/slides.png) </a> <a href='08%20Photon%20Maps/audio/segment-1.mp4' target='_blank'> ![](../../resources/icons/audio.png) </a> <a href='08%20Photon%20Maps/animation/segment-1.mp4' target='_blank'> ![](../../resources/icons/animation.png) </a>

Previously we used ambient lighting as an approximation to global illumination. However, this is a very big approximation, that prevents us from rendering certain types of lighting effect. _Photon maps_ offer a more sophisticated approach that allow us to precompute patterns of light dispersal in order to (relatively) efficiently render more realistic indirect illumination.  


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