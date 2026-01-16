# TODO

* Simplify ui element configs. Maybe even try to get the functions to only a few parameters and not even use config structs
* More UIScaling cleanup
    * Define a window resolution. Try to never even check windowWidth or height in menu.c.
    * Calculate the scale fastor once per frame in the main drawing loop of the canvas.
* Stop embedding assets. Will become problematic once more are added
    * combine files into a .pak and use that instead.
* Implement SDL_image
* Should you still use sdl_ttf considering the warning shown in the console during compilation?
* Notification ui element
    * Do this one first bc the whole timed fade in / slide animation thing might require some new stuff that can be applied to other ui elements.
* Popup ui element
* Slider UIElement
* Dropdown ui element
* Board UI
* Turn time limits
* Improve win check efficiency (see WINCHECKCRITIQUE.md)
* Improve UI cleanliness once code is done
    * Maybe read that ui book xsku sent you
* Computer player with different difficulty levels
* Animated background
* Background music
* Improve sound design
* Haptics?
* Online Multiplayer?