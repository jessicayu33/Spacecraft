# 3260 project
CODE
- Objects rendering + texture * all-the-time transformation
	- wonder star
		* self-rotating
	- earth
		* self-rotating
	- energy rings(>3)
		* self-rotating
	- spacecraft
	- asteroid ring cloud
		* >=200 random flaoting rocks around wonder star
		* move around wonder star
- !skybox
- light source
	- basic lights (ambient, diffuse, specular) (on ALL objects)
- control
	- mouse
		- spacescraft: rotate left/right
	- keyboard
		- up cursor: move spacecraft forward
		- down cursor: move spacecraft backward
		- left cursor: move spacecraft to the left
		- right cursor: move spacecraft to the right
		- "": tune lighting
- viewpoint
	- stick behind spacecraft
- change texture
	- spacecraft & energy ring change to green texture when pass thru it

BONUS
- add another light source
- normal mapping for earth
- collition detection => make one of them disappear (exclude spacecraft x energy ring)

REPORT
- screenshots
	- overall scene
	- close look on each kind of object, with basic light
	- spacecraft passing thru a ring
	- bonus features
- brief implementation details

SUBMISSION
- groupXX.zip
	- .h, .cpp, shaders
	- report
