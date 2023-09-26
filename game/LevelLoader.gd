extends Node

const LEVEL_PATH = "levels/%s.tscn"

@export var current_scene: String = "playground"
var loading: bool
var current_level: Node3D

@onready var player: CharacterBody3D = $"../player"

func _ready() -> void:
	load_level(current_scene)

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(_delta: float) -> void:
	if loading and ResourceLoader.load_threaded_get_status(LEVEL_PATH % current_scene) == ResourceLoader.THREAD_LOAD_LOADED:
		loading = false
		current_level = ResourceLoader.load_threaded_get(LEVEL_PATH % current_scene).instantiate()
		current_level.player = player
		get_tree().get_root().add_child(current_level)

func load_level(level_name) -> bool:
	match typeof(level_name):
		TYPE_INT: level_name = "level%d" % level_name
		TYPE_STRING: pass
		_: return false
	
	if loading:
		return false
	if !ResourceLoader.exists(LEVEL_PATH % level_name):
		return false
	
	loading = true
	
	if current_level != null:
		remove_child(current_level)
		current_level.queue_free()
	
	current_scene = level_name
	ResourceLoader.load_threaded_request(LEVEL_PATH % current_scene, "", true)
	return true
