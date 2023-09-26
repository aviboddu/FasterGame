#include "frictionArea.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/physics_server3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_direct_body_state3d.hpp>
#include <godot_cpp/classes/physics_shape_query_parameters3d.hpp>
#include <godot_cpp/classes/os.hpp>

using namespace godot;

constexpr uint_fast8_t SEG_UP{0b00000001};
constexpr uint_fast8_t SEG_DOWN{0b00000010};
constexpr uint_fast8_t SEG_LEFT{0b00000100};
constexpr uint_fast8_t SEG_RIGHT{0b00001000};
constexpr uint_fast8_t SEG_FRONT{0b00010000};
constexpr uint_fast8_t SEG_BACK{0b00100000};

void FrictionArea::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_upFrictionCoefficient", "coeff"), &FrictionArea::set_upFrictionCoefficient);
    ClassDB::bind_method(D_METHOD("get_upFrictionCoefficient"), &FrictionArea::get_upFrictionCoefficient);
    ClassDB::add_property("FrictionArea", PropertyInfo(Variant::VECTOR3, "upFrictionCoefficient"), "set_upFrictionCoefficient", "get_upFrictionCoefficient");

    ClassDB::bind_method(D_METHOD("set_downFrictionCoefficient", "coeff"), &FrictionArea::set_downFrictionCoefficient);
    ClassDB::bind_method(D_METHOD("get_downFrictionCoefficient"), &FrictionArea::get_downFrictionCoefficient);
    ClassDB::add_property("FrictionArea", PropertyInfo(Variant::VECTOR3, "downFrictionCoefficient"), "set_downFrictionCoefficient", "get_downFrictionCoefficient");

    ClassDB::bind_method(D_METHOD("set_leftFrictionCoefficient", "coeff"), &FrictionArea::set_leftFrictionCoefficient);
    ClassDB::bind_method(D_METHOD("get_leftFrictionCoefficient"), &FrictionArea::get_leftFrictionCoefficient);
    ClassDB::add_property("FrictionArea", PropertyInfo(Variant::VECTOR3, "leftFrictionCoefficient"), "set_leftFrictionCoefficient", "get_leftFrictionCoefficient");

    ClassDB::bind_method(D_METHOD("set_rightFrictionCoefficient", "coeff"), &FrictionArea::set_rightFrictionCoefficient);
    ClassDB::bind_method(D_METHOD("get_rightFrictionCoefficient"), &FrictionArea::get_rightFrictionCoefficient);
    ClassDB::add_property("FrictionArea", PropertyInfo(Variant::VECTOR3, "rightFrictionCoefficient"), "set_rightFrictionCoefficient", "get_rightFrictionCoefficient");

    ClassDB::bind_method(D_METHOD("set_frontFrictionCoefficient", "coeff"), &FrictionArea::set_frontFrictionCoefficient);
    ClassDB::bind_method(D_METHOD("get_frontFrictionCoefficient"), &FrictionArea::get_frontFrictionCoefficient);
    ClassDB::add_property("FrictionArea", PropertyInfo(Variant::VECTOR3, "frontFrictionCoefficient"), "set_frontFrictionCoefficient", "get_frontFrictionCoefficient");

    ClassDB::bind_method(D_METHOD("set_backFrictionCoefficient", "coeff"), &FrictionArea::set_backFrictionCoefficient);
    ClassDB::bind_method(D_METHOD("get_backFrictionCoefficient"), &FrictionArea::get_backFrictionCoefficient);
    ClassDB::add_property("FrictionArea", PropertyInfo(Variant::VECTOR3, "backFrictionCoefficient"), "set_backFrictionCoefficient", "get_backFrictionCoefficient");

    ClassDB::bind_method(D_METHOD("get_frictionCoefficient"), &FrictionArea::get_frictionCoefficient);

    ClassDB::bind_method(D_METHOD("is_colliding_up"), &FrictionArea::is_colliding_up);
    ClassDB::bind_method(D_METHOD("is_colliding_down"), &FrictionArea::is_colliding_down);
    ClassDB::bind_method(D_METHOD("is_colliding_left"), &FrictionArea::is_colliding_left);
    ClassDB::bind_method(D_METHOD("is_colliding_right"), &FrictionArea::is_colliding_right);
    ClassDB::bind_method(D_METHOD("is_colliding_front"), &FrictionArea::is_colliding_front);
    ClassDB::bind_method(D_METHOD("is_colliding_back"), &FrictionArea::is_colliding_back);
}

FrictionArea::FrictionArea() {
    frictionCoefficient = Vector3(1.0, 1.0, 1.0);
    overlappingSegments = 0x0;
    upFrictionCoefficient = Vector3(0.9, 1.0, 0.9);
    downFrictionCoefficient = Vector3(0.9, 1.0, 0.9);
    leftFrictionCoefficient = Vector3(1.0, 0.8, 0.9);
    rightFrictionCoefficient = Vector3(1.0, 0.8, 0.9);
    frontFrictionCoefficient = Vector3(0.9, 0.8, 1.0);
    backFrictionCoefficient = Vector3(0.9, 0.8, 1.0);
}

FrictionArea::~FrictionArea() { }

void FrictionArea::_process(double delta) {
    uint_fast8_t oldSegments = overlappingSegments;
    overlappingSegments = 0;
    TypedArray<Node3D> overlappingBodies = get_overlapping_bodies();
    PhysicsServer3D* physicsServer = PhysicsServer3D::get_singleton();
    PhysicsDirectSpaceState3D* areaState = physicsServer->space_get_direct_state(physicsServer->area_get_space(get_rid()));
    Vector3 global_pos = get_global_position();
    RID parent_rid = cast_to<CollisionObject3D>(get_parent())->get_rid();
    for (int64_t i = 0; i < overlappingBodies.size(); i++) {
        if (cast_to<CollisionObject3D>(overlappingBodies[i])->get_rid() == parent_rid) {
            continue;
        }
        for (int32_t j = 0; j < physicsServer->body_get_shape_count(overlappingBodies[i]); j++) {
            PhysicsShapeQueryParameters3D query = PhysicsShapeQueryParameters3D();
            query.set_collide_with_bodies(true);
            query.set_collide_with_areas(false);
            query.set_collision_mask(this->get_collision_mask());
            query.set_shape_rid(physicsServer->body_get_shape(overlappingBodies[i], j));
            TypedArray<Vector3> collisionPoints = areaState->collide_shape(&query);
            for (int64_t k = 0; k < collisionPoints.size(); k++) {
                Vector3 collisionPoint = collisionPoints[i];
                collisionPoint = to_local(collisionPoint);
                Vector3 absVector = collisionPoint.abs();
                if (absVector[1] > 0.8) { // Y AXIS
                    if (collisionPoint[1] > 0)
                        overlappingSegments |= SEG_UP;
                    else
                        overlappingSegments |= SEG_DOWN;
                } else if (absVector[0] > absVector[2]) {// X AXIS
                    if (collisionPoint[0] > 0)
                        overlappingSegments |= SEG_RIGHT;
                    else
                        overlappingSegments |= SEG_LEFT;
                } else { // Z AXIS
                    if (collisionPoint[2] > 0)
                        overlappingSegments |= SEG_BACK;
                    else
                        overlappingSegments |= SEG_FRONT;
                }
                if (overlappingSegments == 0b111111)
                    break;
            }
            if (overlappingSegments == 0b111111)
                break;
        }
        if (overlappingSegments == 0b111111)
          break;
    }
    if (overlappingSegments != oldSegments)
        frictionCoefficient = calculate_frictionCoefficient();
}

Vector3 FrictionArea::calculate_frictionCoefficient() const {
    Vector3 frictionCoeff = Vector3(1.0, 1.0, 1.0);
    if (overlappingSegments & SEG_UP) [[unlikely]]
        frictionCoeff *= upFrictionCoefficient;
    if (overlappingSegments & SEG_DOWN) [[likely]]
        frictionCoeff *= downFrictionCoefficient;
    if (overlappingSegments & SEG_LEFT)
        frictionCoeff *= leftFrictionCoefficient;
    if (overlappingSegments & SEG_RIGHT)
        frictionCoeff *= rightFrictionCoefficient;
    if (overlappingSegments & SEG_FRONT) [[unlikely]]
        frictionCoeff *= frontFrictionCoefficient;
    if (overlappingSegments & SEG_BACK) [[unlikely]]
        frictionCoeff *= backFrictionCoefficient;
    return frictionCoeff;
}

Vector3 FrictionArea::get_frictionCoefficient() const {
    return frictionCoefficient;
}

void FrictionArea::set_upFrictionCoefficient(const Vector3 coeff) {
    upFrictionCoefficient = coeff;
}

Vector3 FrictionArea::get_upFrictionCoefficient() const {
    return upFrictionCoefficient;
}

void FrictionArea::set_downFrictionCoefficient(const Vector3 coeff) {
    downFrictionCoefficient = coeff;
}

Vector3 FrictionArea::get_downFrictionCoefficient() const {
    return downFrictionCoefficient;
}

void FrictionArea::set_leftFrictionCoefficient(const Vector3 coeff) {
    leftFrictionCoefficient = coeff;
}

Vector3 FrictionArea::get_leftFrictionCoefficient() const {
    return leftFrictionCoefficient;
}

void FrictionArea::set_rightFrictionCoefficient(const Vector3 coeff) {
    rightFrictionCoefficient = coeff;
}

Vector3 FrictionArea::get_rightFrictionCoefficient() const {
    return rightFrictionCoefficient;
}

void FrictionArea::set_frontFrictionCoefficient(const Vector3 coeff) {
    frontFrictionCoefficient = coeff;
}

Vector3 FrictionArea::get_frontFrictionCoefficient() const {
    return frontFrictionCoefficient;
}

void FrictionArea::set_backFrictionCoefficient(const Vector3 coeff) {
    backFrictionCoefficient = coeff;
}

Vector3 FrictionArea::get_backFrictionCoefficient() const {
    return backFrictionCoefficient;
}

bool FrictionArea::is_colliding_up() const {
    return overlappingSegments & SEG_UP != 0;
}

bool FrictionArea::is_colliding_down() const {
    return overlappingSegments & SEG_DOWN != 0;
}

bool FrictionArea::is_colliding_left() const {
    return overlappingSegments & SEG_LEFT != 0;
}

bool FrictionArea::is_colliding_right() const {
    return overlappingSegments & SEG_RIGHT != 0;
}

bool FrictionArea::is_colliding_front() const {
    return overlappingSegments & SEG_FRONT != 0;
}

bool FrictionArea::is_colliding_back() const {
    return overlappingSegments & SEG_BACK != 0;
}