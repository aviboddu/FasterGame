#ifndef FRICTIONAREA_H
#define FRICTIONAREA_H

#include <godot_cpp/classes/area3d.hpp>

namespace godot {
    class FrictionArea: public Area3D {
        GDCLASS(FrictionArea, Area3D)

        private:
            uint_fast8_t overlappingSegments;
            Vector3 frictionCoefficient;
            Vector3 upFrictionCoefficient;
            Vector3 downFrictionCoefficient;
            Vector3 leftFrictionCoefficient;
            Vector3 rightFrictionCoefficient;
            Vector3 frontFrictionCoefficient;
            Vector3 backFrictionCoefficient;

            Vector3 calculate_frictionCoefficient() const;

        protected:
            static void _bind_methods();
        
        public:
            FrictionArea();
            ~FrictionArea();

            void _physics_process(double delta);

            Vector3 get_frictionCoefficient() const;

            void set_upFrictionCoefficient(const Vector3 coeff);
            Vector3 get_upFrictionCoefficient() const;
            void set_downFrictionCoefficient(const Vector3 coeff);
            Vector3 get_downFrictionCoefficient() const;
            void set_leftFrictionCoefficient(const Vector3 coeff);
            Vector3 get_leftFrictionCoefficient() const;
            void set_rightFrictionCoefficient(const Vector3 coeff);
            Vector3 get_rightFrictionCoefficient() const;
            void set_frontFrictionCoefficient(const Vector3 coeff);
            Vector3 get_frontFrictionCoefficient() const;
            void set_backFrictionCoefficient(const Vector3 coeff);
            Vector3 get_backFrictionCoefficient() const;

            bool is_colliding_up() const;
            bool is_colliding_down() const;
            bool is_colliding_left() const;
            bool is_colliding_right() const;
            bool is_colliding_front() const;
            bool is_colliding_back() const;
    };
}

#endif // FRICTIONAREA_H