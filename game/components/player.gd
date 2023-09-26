extends CharacterBody3D

# TODO: Implement on focus/unfocus functions
# TODO: Should pull this from a Player Settings Singleton
const MOUSE_SENSITIVITY: float = 0.001
const CONTROLLER_SENSITIVITY: float = 2.0

# Get the gravity from the project settings to be synced with RigidBody nodes.
var gravity: float = ProjectSettings.get_setting("physics/3d/default_gravity")
var interactableInFocus = null
var can_jump: bool = false
var jump_buffer: int = 0
var was_on_floor: bool = false

@export_range(-90, 90, 0.01, "radians") var MAX_X_ROTATION: float = 0.4 * PI

@export_group("Movement")
## Acceleration in meters per second
@export_range(0.0, 30.0, 0.1, "or_greater", "suffix:m/s") var ACCELERATION: float = 40.0
## Proportion of acceleration when in the air ("air control")
@export_range(0.0, 1.0, 0.01) var AIR_ACCELERATION_MODIFIER: float = 0.1 # Air control
@export_subgroup("Jump Adjustments")
## Jump Velocity
@export_range(0.0, 10.0, 0.1, "or_greater") var JUMP_VELOCITY: float = 5.0
## Increases gravity on the way down
@export_range(0.0, 1.0, 0.05, "or_greater") var FALL_GRAVITY_MULT: float = 0.2
## Decreases gravity at the peak of a jump
@export_range(0.0, 1.0, 0.05) var PEAK_GRAVITY_MULT: float = 0.7
## Jump Buffer Timing
@export_range(0.0, 1.0, 0.01) var JUMP_BUFFER_TIME: float = 0.1
## Coyote Time
@export_range(0.0, 1.0, 0.01) var COYOTE_TIME: float = 0.2

@onready var cam: Camera3D = $Camera
@onready var ray: RayCast3D = $Camera/InteractionRay
@onready var friction: FrictionArea = $FrictionArea

func _ready() -> void:
	Input.mouse_mode = Input.MOUSE_MODE_CAPTURED

func _input(event: InputEvent) -> void:
	if event is InputEventMouseMotion:
		cam.rotation.x = clamp(cam.rotation.x - event.relative.y * MOUSE_SENSITIVITY, -MAX_X_ROTATION, MAX_X_ROTATION)
		cam.rotation.y += -event.relative.x * MOUSE_SENSITIVITY
	
	if event.is_action_pressed("interact"):
		_try_interact()
	
	if event.is_action_pressed("jump"):
		_jump()
	

func _physics_process(delta: float) -> void:
	# Add the gravity.
	if !is_on_floor():
		if was_on_floor:
			if velocity.y <= 0: # We didn't jump, but fell off
				create_tween().tween_callback(_disallow_jump).set_delay(COYOTE_TIME)
			else: # We jumped, so we should disallow jumping immediately
				_disallow_jump()
		if velocity.y < -0.1:
			velocity.y -= FALL_GRAVITY_MULT * gravity * delta # Fall faster than we rise
		elif velocity.y < 0.1:
			velocity.y += PEAK_GRAVITY_MULT * gravity * delta # Stay at the peak a bit longer
		velocity.y -= gravity * delta
	elif !can_jump: # If we couldn't jump and just landed, allow jumping again
		_allow_jump()
	was_on_floor = is_on_floor()
	
	# Get the input direction and handle the movement/deceleration.
	var movement_dir = Input.get_vector("left", "right", "forward", "back")
	var direction = (cam.transform.basis * Vector3(movement_dir.x, 0, movement_dir.y)).normalized()
	if direction:
		velocity.x += direction.x * ACCELERATION * delta * (1.0 if is_on_floor() else AIR_ACCELERATION_MODIFIER)
		velocity.z += direction.z * ACCELERATION * delta * (1.0 if is_on_floor() else AIR_ACCELERATION_MODIFIER)
	
	velocity *= friction.get_frictionCoefficient()
	
	_check_ray()
	move_and_slide()


func _check_ray() -> void:
	var newInteractableInFocus = ray.get_collider()
	if is_same(newInteractableInFocus, interactableInFocus):
		return
	
	if interactableInFocus != null:
		_on_unfocus()
	
	interactableInFocus = newInteractableInFocus
	if interactableInFocus == null:
		return
	
	_on_focus()


func _try_interact() -> void:
	if interactableInFocus == null:
		return
	interactableInFocus.onInteract()

func _on_focus() -> void:
	pass

func _on_unfocus() -> void:
	pass

func _jump() -> void:
	if can_jump:
		velocity.y = JUMP_VELOCITY
	else:
		jump_buffer += 1
		create_tween().tween_callback(_decrement_buffer).set_delay(JUMP_BUFFER_TIME)

# Should only be called on landing
func _allow_jump() -> void:
	can_jump = true
	if jump_buffer > 0:
		_jump()

func _decrement_buffer() -> void:
	if jump_buffer > 0:
		jump_buffer -= 1

func _disallow_jump() -> void:
	if !is_on_floor():
		can_jump = false
