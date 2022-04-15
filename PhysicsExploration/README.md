# Point-Spot-Lights-OpenGL
Computer Graphics (COMP 371) assignment 3
* Note: change perspective projection to orthographic for the shadow

> GLEW, GLFW and GLM libraries needed:
####
> ..\glm\gtx;..\glm\gtc;..\glm\detail;..\glm;..\glfw;..\glew;%(AdditionalIncludeDirectories)

## variables:

Pressing F1 adds 4 point lights.
####
Pressing F2 adds a spot light and implements the two-pass shadow mapping algorithm using
render-to-texture. 
####
For the point lights, they all have their positions and colors defined in an array.
####
### Ambient/diffuse/specular attributes and other light computations are handled by:
####
```vec3 ComputePointLight(PointLight light, vec3 norm, vec3 fragment_pos, vec3 view_dir, vec3 object_col);```

### For the spot light, the light computations are handled by:

```vec3 ComputeSpotLight(SpotLight light, vec3 norm, vec3 fragment_pos, vec3 view_dir, vec3 object_col);```

## Shadows have two passes:

### First pass:

Shadow projection, view (lightspace) and model matrices are passed to the shadow_vertex.shader (for the position) and shadow_fragment.shader (which does nothing).

### Second pass:

ShadowMap is set to 0.
####
Projection, view, model and lightspace matrices are passed to the normal vertex and fragment shaders.
####
Vertex shader provides "fragment_position_lightspace" to the fragment shader that will be used to process to shadows handled by:
####
```float shadow_calc(vec4 frag_pos_light, vec3 light_dir);```

# ASSIGNMENT 1 INFO
## For the camera:
`cam_pos`, `cam_dir`, `cam_up` are used as the basic settings of the camera.
####
That is, its position in the world, the direction it is pointing at and the orientation (the top of the camera is going up on the z axis).

`temp_cam_dir` is used as an arbitrary vector towards the y axis to be used when doing cross products.

## For the model:
`model` is a matrix variable used to apply scalor and rotational transformations. Model transformations are used in the game loop.
####
`modl_move` is a vector variable that is used for translation transformations. It is used to map the vector changes in the `translator` matrix variable used in the game loop. 

## methods:

## ADDITIONAL Triggers/Controls:
### key_callback method to handle keyboard inputs:

* Esc: closes window by setting `glfwSetWindowShouldClose(window, GL_TRUE)` to true.
* W: moves camera forward by increasing its `cam_pos` in the z-axis.
* S: moves camera backward by decreasing its `cam_pos` in the z-axis.
* D: moves camera right by decreasing its `cam_pos` in the x-axis (done using cross product between y and z vectors, which are cam_up and cam_dir).
* A: moves camera left by increasing its `cam_pos` in the x-axis (same way as D).
* I: moves model along the +Y axis by incrementing the y coordinate of the vector by 1.
* K: moves model along the -Y axis by decrementing the y coordinate of the vector by 1.
* J: moves model along the +X axis by incrementing the x coordinate of the vector by 1.
* L: moves model along the -X axis by decrementing the x coordinate of the vector by 1.
* Pg Up: moves model along the +Z axis by incrementing the z coordinate of the vector by 1.
* Pg Dn: moves model along the -Z axis by decrementing the z coordinate of the vector by 1.
* B: rotates `model` matrix variable around x using glm::rotate(model, glm::radians(5.f), glm::vec3(1, 0, 0)).
* N: rotates `model` matrix variable around y using glm::rotate(model, glm::radians(5.f), glm::vec3(0, 1, 0)).
* E: rotates `model` matrix variable around z using glm::rotate(model, glm::radians(5.f), glm::vec3(0, 0, 1)).
* O: scales `model` matrix variable up 10% by using glm::scale(model, glm::vec3(1.1f)) (1 + 0.1 = 1.1).
* P: scales `model` matrix variable down -10% by using glm::scale(model, glm::vec3(0.9f)) (1 - 0.1 = 0.9).
* up/down: rotate camera around its own y axis.
* left/right arrows: rotate camera around its own z axis.

### cursor_pos_callback method to handle mouse movement:

`last_y_pos` is a variable used to calculate whether or not the mouse has gone more up or down from the point that the mouse was pressed.

#### 
Has a boolean `lbutton_pressed` to check whether or not the left mouse button is pressed.
#### 
Uses `glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)` to check if it is pressed and `glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)` for when it is released.
####
When `lbutton_pressed` is true, then we can move the mouse to do stuff.
####
When the mouse goes up (calculated using the difference between the "new" cursor position of the mouse and the y position it was in when the left button was clicked), the camera zooms out.
####
When the mouse goes down (calculated in a simialr way as before), the camera sooms in.
####
The "new" cursor position (`last_y_pos`) becomes the new cursor y position when it changes position, which allows you to drag the mouse up and down until you get your desired picture.

## Game loop

I have my translator matrix variable in the game loop as so:
####
```glm::mat4 translator = glm::translate(glm::mat4(1.0f), modl_move);```
####
Where `modl_move` vector is modified in the `key_callback` method.
#### 
The rotation and scalar tranformations are applied directly to the `model` matrix variable in the `key_callback` method.
#### 
Transformations are applied in `modl_matrix` as so:
####
```modl_matrix = translator*model;```
#### 
Camera transformations are applied in the game loop with the `lookAt` method.
####
```glm::mat4 view_matrix = glm::lookAt(cam_pos, cam_pos+cam_dir, cam_up);```
```glUniformMatrix4fv(vm_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));```

