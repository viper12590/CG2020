## Assignment Overview

The aim of this unit has been to provide you with an understanding
of a variety of approaches to 3D rendering. The weekly workbooks have
focused on implementing some of the key concepts presented in the video
lectures that formed the main theoretical content of this unit.

The assignment for this unit provides an opportunity to demonstrate your
understanding of these concepts, as well as a chance to exercise the code
that you have been accumulating over the course of the unit.

Many of the approaches to rendering that we have explored can only be fully
appreciated by movement of the camera and/or the rendered model.
For this reason, your task is to create a short (approximately 10 second)
animated "ident" video. To provide a focus and purpose for this animation,
you should attempt to create an ident for use by
[the MVB hackspace](https://www.facebook.com/groups/363509640788223/)

For more insight into the concept of an "ident", you should view this
[compilation of BBC2 idents](https://www.youtube.com/watch?v=LDL0GZKvz6o).
Note that there are quite a wide variety of examples presented - some are
more relevant to this unit than others !

We have provided a [texture-mapped hackspace logo](logo.zip) which you may
like to use to help you on your way. This however is not mandatory and you
are free to use any alternative models you see fit.

## Assignment Details

You should refine and extend the codebase that you have developed during the
completion of the weekly workbooks in order to build a capable C++ renderer.
You should attempt to include some of the approaches introduced in the
["advanced topics" playlist](../Weekly%20Workbooks/08%20Advanced%20Topics%20Playlist/).

It is up to you which additional approaches you select, however you should
remember that there is a performance trade-off at work. The more complex
features you implement, the longer will be the rendering time for your
animation. You will need to find an appropriate compromise between renderer
complexity, algorithmic performance, animation aesthetics and rendering time.

You should use your code to render out individual frames of an animation (as
images) and then sequence these together into an mp4 using an external video
editing tool (guidance will be provided on suitable tools for this purpose).

You should ensure that your animation clearly demonstrates all the rendering
approaches that you have implemented - switching between different modes of
rendering as appropriate. You may find that some features can only be fully
demonstrated using certain types of render (wireframe/rasterised/raytraced) !

The expectation is that students will spend 3 full working weeks on their two
assignments. The effort spent on the assignment for each unit should be
approximately equal, being roughly equivalent to 1.5 working weeks each.

## Marking criteria

Your work will be assess on the extent to which you achieve the following objectives:

- Successful implementation of different approaches to 3D rendering
(wireframe, rasterising, ray tracing)

- Successful implementation of a range of different approaches to lighting and shading
(ambient/diffuse/specular lighting, shadows, Phong Shading, Gouraud shading)

- Effective use of animation of camera position, camera orientation and model elements

- Selection and implementation of advanced rendering features, which may include:
    - Generative geometry (e.g. landscapes, replication of model components etc.)
    - Non-opaque materials (e.g. glass, mirrors, metal)
    - Surface mapping (texture maps, bump maps, environment maps)
    - Advanced lighting approaches (e.g. photon maps)

An [indicative marking guide](indicative-guide.md) has been provided to illustrate the
marking grade boundaries that specific rendering features relate to.

## Submission

You should submit a single 480p (640×480) MP4 video file containing your ident onto
Blackboard. This MUST be the video file itself (i.e. not the URL of an online video).
You should make an effort to compress your video in order to reduce file size.
Be sure to include segments that clearly illustrate all rendering features you have
implemented (if we can't see it, it is hard to award marks for it !). 

You should also submit your code so that we may verify the correct implementation of
features. Include a Make file to build your rendered and make sure your code compiles
on the MVB lab machines (this is where your code will be tested and marked !).

Upon submission, you will be asked to fill out a questionnaire indicating which
approaches you have used to implement specific rendering features.

The deadline for submission of all optional unit assignments is 13:00 on Friday
11th of December. Students should submit all required materials to the "Assessment,
submission and feedback" section of Blackboard - it is essential that this is done
on the Blackboard page related to the "With Coursework" variant of the unit.

## Academic Offences

Academic offences (including submission of work that is not your own, falsification
of data/evidence or the use of materials without appropriate referencing) are all
taken very seriously by the University. Suspected offences will be dealt with in
accordance with the University’s policies and procedures. If an academic offence
is suspected in your work, you will be asked to attend an interview with senior
members of the school, where you will be given the opportunity to defend your work.
The plagiarism panel are able to apply a range of penalties, depending the severity
of the offence. These include: requirement to resubmit work, capping of grades and
the award of no mark for an element of assessment.

## Extenuating circumstances
If the completion of your assignment has been significantly disrupted by serious
health conditions, personal problems, periods of quarantine, or other similar issues,
you may be able to apply for consideration of extenuating circumstances (in accordance
with the normal university policy and processes). Students should apply for
consideration of extenuating circumstances as soon as possible when the problem occurs,
using the [online faculty form](https://apps.powerapps.com/play/3172b943-0956-4b88-bf3d-3f37871d1170?tenantId=b2e47f30-cd7d-4a4e-a5da-b18cf1a4151b)

You should note however that extensions are not possible for optional unit assignments.
If your application for extenuating circumstances is successful, it is most likely
that you will be required to retake the assessment of the unit at the next available
opportunity.

The UK Government will be instigating a “travel window” to allow students return home
from University once UK national restrictions have been lifted. This will take place
between the 3rd and 9th of December and as such, will occur during the optional unit
assignment period. Students whose work is significantly impacted by extended periods
of travel and quarantine during this period should apply for extenuating circumstances.
As discussed previously, extensions are not possible for optional unit assignments.
If your application for extenuating circumstances is successful, it is most likely
that you will be required to retake the assessment of the unit at the next available
opportunity.

