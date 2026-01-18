# TODO

* Clean up the ui element declaration. It feels weird to have to do scaling calculationsin menu.c. Maybe it should all be handled in the other non screen files?
* Simplify ui element configs. Maybe even try to get the functions to only a few parameters and not even use config structs
* Stop embedding assets. Will become problematic once more are added
    * combine files into a .pak and use that instead.
* Implement SDL_image
* Notification ui element
    * Do this one first bc the whole timed fade in / slide animation thing might require some new stuff that can be applied to other ui elements.
* Popup ui element
* Slider UIElement
* Dropdown ui element
* Board UI
* Turn time limits
* Computer player with different difficulty levels
* Improve win check efficiency (see WINCHECKCRITIQUE.md)
* Improve UI cleanliness once code is done
    * Maybe read that ui book xsku sent you
* Animated background
* Background music
* Improve sound design
* Haptics?
* Online Multiplayer?