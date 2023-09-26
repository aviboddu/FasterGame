extends Node3D

@export var player: CharacterBody3D
@onready var spawn: Node3D = $spawn

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	player.transform = spawn.transform
