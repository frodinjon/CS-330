# CS-330
Comp Graphics and Visualization
\

NOTE: My project file said it is 191MB and GitHub only allows 25MB uploads (this was a zipped folder already). I have instead uploaded my source.cpp and screenshots of the project in action.\
How do I approach designing software?\
    What new design skills has your work on the project helped you to craft?
    This project taught me a ton about frames, polygons, rendering, and how items are positioned on the screen. This will really help when, in the future, I need to create visually engaging programs. Even if a project uses something like DirectX instead, it will be helpful to understand positioning, scaling, and placement in order to really get a feeling for how programs work with displays. For example, one thing I didn't understand when first doing early website work with CSS and HTML was the z-index. It seems to only work sometimes, but at least now the concept is abundantly clear to me, the position on the z axis relative to other objects. This has been fascinating to explore.
    What design process did you follow for your project work?
    In my project, I first broke down the target scene into core components. This ended up being many cubes and cylinders, and then some shapes that, from the beginning, I knew would have to be custom. Once I had the rough estimate, I decided to see how abstract I could make the rendering. This ended up being extremely helpful later on. I didn't want to code in the same squares again and again but have something like the translation slightly different. So, I made a rough draft of what would end up beign my renderCube() etc. methods. These expanded or contracted in parameters depending on what the program called for. Once I had the scene skeleton ready, I worked on the texturing and the lighting. Once that was in place, I added the fish and then the animation of the fish to give the scene life and to explore the concept of animation.
    How could tactics from your design approach be applied in future work?
    The design approach I took here could have a dramatic impact on future projects. Some projects in the past had little planning, especially in school with limited time. This one really highlighted the need for a detailed plan from the get-go. I need to expand that and bring it in to all of my projects in the future. I also need to abstract more, which I did not do so much on earlier projects. This project really helped me see the power of methods and parameters to make a scene dynamic. The callbacks were also fascinating, and this gave me a lot to think about with bringing in more functionality with keys and mouse inputs.
    
How do I approach developing programs?
    What new development strategies did you use while working on your 3D scene?
    I used interrupts and recursive calls in the development of this program. I did't end up with recursion in my final version, but I did experiment with it when rendering the water. I ended up only thinking of the solution to make it work after already submitting the project. The recursion is definitely something I want to explore futher because I think it can be incredibly efficient. I also used interrupts in this program, though. That was interesting. For example, I had to turn off the depth test in order to only draw one piece of the scene and then turn it back on.
    How did iteration factor into your development?
    Iteration was a huge factor in my development. The first part was just a simple cube, I believe. Then the second was a cube and a cylinder. This helped me gain a lot of knowledge about how the shapes worked, and more importantly, how to manipulate the shapes in the scene. It was a crawl getting from that first cube all the way to the fish tank scene in the final project, but very worthwhile and provided so many learning experiences.
    How has your approach to developing code evolved throughout the milestones, which led you to the project’s completion?
    My approach was initially to hardcore things. Then it became much more abstract. After starting to abstract the various render functions, I learned a ton about how those actually work and how the shapes can be manipulated. This ended up helping me with lighting and other aspects of the scene like animation as well.

How can computer science help me in reaching my goals?
    How do computational graphics and visualizations give you new knowledge and skills that can be applied in your future educational pathway?
    Visualizations may have totally changed my future educational pathway. Before this class I thought I had no interest in graphics. Then, workign through these milestones, I developed a bit of a passion for it. This is fascinating. I am now considering further studies in graphics in a master's program after I'm finished with this bachelor's degree. I think it is really an incredible field of study and had no idea how involved and complicated it waws.

