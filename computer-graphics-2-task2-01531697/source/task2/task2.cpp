#include "task2.h"

int flattenIndex(int x, int y, int size_x) {
    // This is a utility function that maps a 2D index into a 1D index
    // for our flat vector structures (normals, positions, masses...)
    return x + y * size_x;
}

void calcNormals(std::vector<float3>& normals,
                 const std::vector<float3>& positions, int grid_x, int grid_y) {
    // TODO: calculate the normal for each particle vertex
    //
    // To compute the particle normal consider the particles connected via
    // structural springs. Use flattenIndex(int x, int y, int size_x) to convert
    // from 2D coordinates to a 1D index for vectors normals and positions.
    //

    //todo ukoliko dojdje do segfaulta znam staj

    //todo ukoliko dojdje do segfaulta znam staj

    int flatten = 0;
    float3 vec1 = {0, 0, 0};
    float3 vec2 = {0, 0, 0};
    float3 vec3 = {0, 0, 0};
    float3 vec4 = {0, 0, 0};

//todo: -1
    for (int x = 0; x < grid_x; ++x) {
        for (int y = 0; y < grid_y; ++y) {

            flatten = flattenIndex(y, x, grid_y);

            if (x == 0 && y == 0) {
                vec1 = positions[flatten + 1] - positions[flatten];
                vec2 = positions[flatten + grid_y] - positions[flatten];
                normals[flatten] = normalize(normalize(cross(vec1, vec2)));
                continue;
            }
            if (x == 0 && y == grid_y - 1) {
                vec1 = positions[flatten - 1] - positions[flatten];
                vec2 = positions[flatten + grid_y] - positions[flatten];
                normals[flatten] = normalize(
                        normalize(cross(vec2, vec1)));
                continue;
            }
            if (x == grid_x - 1 && y == 0) {
                vec1 = positions[flatten - grid_y] - positions[flatten];
                vec2 = positions[flatten + 1] - positions[flatten];
                normals[flatten] = normalize(
                        normalize(cross(vec1, vec2)));
                continue;
            }

            if (x == grid_x - 1 && y == grid_y - 1) {
                vec1 = positions[flatten - 1] - positions[flatten];
                vec2 = positions[flatten - grid_y] - positions[flatten];
                normals[flatten] = normalize(normalize(cross(vec1, vec2)));
                continue;

            }


///////////////////// x is edges

            if (x == 0 && y > 0 && y < grid_y - 1) {
                vec1 = positions[flatten - 1] - positions[flatten];
                vec2 = positions[flatten + grid_y] - positions[flatten];
                vec3 = positions[flatten + 1] - positions[flatten];

                normals[flatten] = normalize(normalize(cross(vec3, vec2)) + normalize(cross(vec2, vec1)));
                continue;
            }

            if (x == grid_x - 1 && y > 0 && y < grid_y - 1) {

                vec1 = positions[flatten - 1] - positions[flatten];
                vec2 = positions[flatten - grid_y] - positions[flatten];
                vec3 = positions[flatten + 1] - positions[flatten];

                normals[flatten] = normalize(normalize(cross(vec1, vec2)) + normalize(cross(vec2, vec3)));

                continue;

            }

            ///////////////////// y is edges todo provjeriti forumle
            if (y == 0 && x > 0 && x < grid_x - 1) {
                vec1 = positions[flatten - grid_y] - positions[flatten];
                vec2 = positions[flatten + 1] - positions[flatten];
                vec3 = positions[flatten + grid_y] - positions[flatten];

                normals[flatten] = normalize(normalize(cross(vec1, vec2)) + normalize(cross(vec2, vec3)));

                continue;
            }

            if (y == grid_y - 1 && x > 0 && x < grid_x - 1) {

                vec1 = positions[flatten - grid_y] - positions[flatten];
                vec2 = positions[flatten - 1] - positions[flatten];
                vec3 = positions[flatten + grid_y] - positions[flatten];

                normals[flatten] = normalize(normalize(cross(vec3, vec2)) + normalize(cross(vec2, vec1)));


                continue;
            }

            ///////////////////// unutra is edges

            if (x > 0 && y > 0 && x < grid_x - 1 && y < grid_y - 1) {
                vec1 = positions[flatten - grid_y] - positions[flatten];
                vec2 = positions[flatten + 1] - positions[flatten];
                vec3 = positions[flatten + grid_y] - positions[flatten];
                vec4 = positions[flatten - 1] - positions[flatten];

                normals[flatten] = normalize(normalize(cross(vec1, vec2)) + normalize(cross(vec2, vec3))
                                             + normalize(cross(vec3, vec4)) + normalize(cross(vec4, vec1)));
                continue;
            }
        }

        // Parameters:
        // normals: resulting normals
        // positions: particle positions
        // grid_x, grid_y: particle-grid dimensions
    }
}

float3 calcWindAcc(const float3& normal, const float3& wind_force,
                   float inv_mass) {
    // TODO: calculate the particle-dependent acceleration due to wind
    //
    // The wind strength should depend on the cosine (dot product) between the
    // particle normal and the wind direction (note that the wind can hit the
    // particle from the front or back). The resulting particle acceleration
    // caused by the wind is the above scaling times the inverse mass of the
    // particle times the wind force.
    //
    // Parameters:
    // normal: particle normal
    // wind_force: wind force in XYZ direction
    // inv_mass: inverse mass of the particle
    float effec = 0;
    float3 wind = {0.f,0.f,0.f};

    if(wind_force.x == 0.0f && wind_force.y == 0.0f && wind_force.z == 0.f)
    {
        return  wind;
    }
    else
    {
        effec = std::fabs(dot(normal, normalize(wind_force)));
        wind = effec * inv_mass * wind_force;
        return wind;
    }

}

void verletIntegration(std::vector<float3>& p_next,
                       const std::vector<float3>& p_cur,
                       const std::vector<float3>& p_prev,
                       const std::vector<float3>& normals,
                       const std::vector<float>& inv_masses, float damp,
                       const float3& gravity, const float3& wind_force,
                       float dt) {
    // TODO: for each particle perform the numerical integration
    //
    // Use the formula given in the assignment sheet to perform the Verlet
    // Integration. Do NOT add spring forces, as they are taken care of by the
    // fixup-step! Make sure that fixed particles (inverse mass = 0) do not change
    // their position! Bonus: The particle-dependent accelerations should also
    // consider the wind direction and strength => Use your implementation of
    // calcWindAcc()
    //

    // Parameters:
    // p_next: updated particle positions
    // p_cur: current paricle positions
    // p_prev: previous particle positions
    // normals: particle normals
    // inv_masses: inverse particle masses
    // damp: damping factor
    // gravity: gravity vector
    // wind_force: wind force
    // dt: timestep

    for (int i = 0; i < p_next.size(); ++i)
    {
        if(inv_masses[i] != 0)
        {
            p_next[i] = (1 + damp) * p_cur[i] - damp * p_prev[i] + (gravity + calcWindAcc(normals[i],wind_force,inv_masses[i])) * pow(dt,2) ;
        }
        else
        {
            p_next[i] = p_cur[i];
        }
        //repo update
    }

}

void fixupStep(std::vector<float3>& p_out, const std::vector<float3>& p_in,
               int grid_x, int grid_y, float cloth_x, float cloth_y,
               const std::vector<float>& inv_masses, float fixup_percent,
               const std::vector<Sphere>& obstacles,
               bool apply_structural_fixup, bool apply_shear_fixup,
               bool apply_flexion_fixup) {
    // TODO: implement the fixup-step
    //
    // For each particle iterate over all particles connected via springs and
    // compute the movement required to restore each spring into its resting
    // position. Based on the mass ratio between the particle pairs compute the
    // part of the movement that would have to be carried out by the current
    // particle. Sum these movements up and update the particle position with a
    // fraction of that movement(fixup_percent of the full movement). Iterate over
    // all obstacles and resolve the collisions.
    //
    // *Only* apply the relevant fixup (structural/shear/flexion) if the
    // corresponding flag is set (apply_*_fixup)!
    //
    // Parameters:
    // p_out: new particle positions
    // p_in: current particle positions

    // grid_x, grid_y: particle-grid dimensions

    // cloth_x, cloth_y: cloth size

    // inv_masses: inverse particle masses
    // fix_percent: fixup percent [0..1]

    // obstacles: obstacle vector

    // apply_structural_fixup: only structural fixup if this is true
    // apply_shear_fixup: only apply shear fixup if this is true
    // apply_flexion_fixup: only apply flexion fixup if this is true


    float rest_struct_x = cloth_x/(grid_x - 1);
    float rest_struct_y = cloth_y/(grid_y - 1);

    float rest_flex_x = (cloth_x/(grid_x - 1)) * 2;
    float rest_flex_y = (cloth_y/(grid_y - 1)) * 2;

    float shear = sqrtf(rest_struct_x * rest_struct_x  + rest_struct_y * rest_struct_y);


    for (int y = 0; y < grid_y ; ++y) {

        for (int x = 0; x < grid_x ; ++x) {

            int FI = flattenIndex(x,y,grid_x);

            p_out.at(FI) = p_in.at(FI);

            if(inv_masses.at(flattenIndex(x,y,grid_x)) == 0)
            {
                continue;
            }


            if (apply_structural_fixup) {

                float3 left = {0.f,0.f,0.f};
                float3 right = {0.f,0.f,0.f};
                float3 up = {0.f,0.f,0.f};
                float3 down = {0.f,0.f,0.f};

                float massLeft = 0.f;
                float massRight = 0.f;
                float massUp = 0.f;
                float massDown = 0.f;

                float lenLeft = 0.f;
                float lenRight = 0.f;
                float lenUp = 0.f;
                float lenDown = 0.f;

                float3 directionLeft = {0.f,0.f,0.f};
                float3 directionRight = {0.f,0.f,0.f};
                float3 directionUp = {0.f,0.f,0.f};
                float3 directionDown = {0.f,0.f,0.f};


                if(x != 0)
                {
                    left = p_in.at(FI - 1) - p_in.at(FI);
                    massLeft = inv_masses.at(FI) / (inv_masses.at(FI - 1) + inv_masses.at(FI));
                    lenLeft = length(left);
                    directionLeft = normalize(left);
                }
                if ( x != grid_x - 1)
                {
                    right = p_in.at(FI + 1) - p_in.at(FI);
                    massRight = inv_masses.at(FI) / (inv_masses.at(FI + 1) + inv_masses.at(FI));
                    lenRight = length(right);
                    directionRight = normalize(right);

                }
                if ( y != 0)
                {
                    up = p_in.at(FI - grid_x) - p_in.at(FI);
                    massUp = inv_masses.at(FI) / (inv_masses.at(FI - grid_x) + inv_masses.at(FI));
                    lenUp = length(up);
                    directionUp = normalize(up);

                }
                if (y != grid_y - 1)
                {
                    down = p_in.at(FI + grid_x) - p_in.at(FI);
                    massDown = inv_masses.at(FI) / (inv_masses.at(FI + grid_x) + inv_masses.at(FI));
                    lenDown = length(down);
                    directionDown = normalize(down);
                }

                float leftOffset = lenLeft - rest_struct_x;
                float rightOffset = lenRight - rest_struct_x;
                float upOffset = lenUp - rest_struct_y;
                float downOffset = lenDown - rest_struct_y;

                float3 fall1 = directionLeft * leftOffset * massLeft;
                float3 fall2 = directionRight * rightOffset * massRight;
                float3 fall3 = directionUp * upOffset * massUp;
                float3 fall4 = directionDown * downOffset * massDown;

                p_out.at(FI) += (fall1 + fall2 + fall3 + fall4) * fixup_percent;
            }

            if (apply_shear_fixup)
            {

                float3 leftUp = {0.f,0.f,0.f};
                float3 leftDown = {0.f,0.f,0.f};
                float3 rightUp = {0.f,0.f,0.f};
                float3 rightDown = {0.f,0.f,0.f};


                float massLeftUp = 0.f;
                float massLeftDown = 0.f;
                float massRightUp = 0.f;
                float massRightDown = 0.f;

                float lenLeftUp = 0.f;
                float lenLeftDown = 0.f;
                float lenRightDown = 0.f;
                float lenRightUp = 0.f;

                float3 directionLeftUp = {0.f,0.f,0.f};
                float3 directionLeftDown = {0.f,0.f,0.f};
                float3 directionRightUp = {0.f,0.f,0.f};
                float3 directionRightDown = {0.f,0.f,0.f};


                if(x != 0 && y != 0)
                {
                    leftUp = p_in.at(FI - 1 - grid_x) - p_in.at(FI);;
                    massLeftUp = inv_masses.at(FI) / (inv_masses.at(FI - 1 - grid_x) + inv_masses.at(FI));
                    lenLeftUp = length(leftUp);
                    directionLeftUp = normalize(leftUp);
                }

                if(x != 0 && y != grid_y -1)
                {
                    leftDown = p_in.at(FI - 1 + grid_x) - p_in.at(FI);;
                    massLeftDown = inv_masses.at(FI) / (inv_masses.at(FI - 1 + grid_x) + inv_masses.at(FI));
                    lenLeftDown = length(leftDown);
                    directionLeftDown = normalize(leftDown);
                }

                if(x != grid_x - 1 && y != 0)
                {
                    rightUp = p_in.at(FI + 1 - grid_x) - p_in.at(FI);;
                    massRightUp = inv_masses.at(FI) / (inv_masses.at(FI + 1 - grid_x) + inv_masses.at(FI));
                    lenRightUp = length(rightUp);
                    directionRightUp = normalize(rightUp);
                }

                if(x != grid_x - 1 && y != grid_y - 1)
                {
                    rightDown = p_in.at(FI + 1 + grid_x) - p_in.at(FI);;
                    massRightDown = inv_masses.at(FI) / (inv_masses.at(FI + 1 + grid_x) + inv_masses.at(FI));
                    lenRightDown = length(rightDown);
                    directionRightDown = normalize(rightDown);
                }

                float luo = lenLeftUp - shear;
                float ldo = lenLeftDown - shear;
                float rdo = lenRightDown - shear;
                float ruo = lenRightUp - shear;

                float3 fall1 = directionLeftUp * luo * massLeftUp;
                float3 fall2 = directionLeftDown * ldo * massLeftDown;
                float3 fall3 = directionRightDown * rdo * massRightDown;
                float3 fall4 = directionRightUp * ruo * massRightUp;

                p_out.at(FI) += (fall1 + fall2 + fall3 + fall4 ) * fixup_percent;


            }

            if(apply_flexion_fixup)
            {

                float3 leftFlex = {0.f,0.f,0.f};
                float3 rightFlex  = {0.f,0.f,0.f};
                float3 upFlex  = {0.f,0.f,0.f};
                float3 downFlex  = {0.f,0.f,0.f};

                float massUpFlex = 0.f;
                float massLeftFlex  = 0.f;
                float massRightFlex  = 0.f;
                float massDownFlex  = 0.f;

                float lenLeftFlex  = 0.f;
                float lenDownFlex  = 0.f;
                float lenRightFlex  = 0.f;
                float lenUpFlex  = 0.f;

                float3 directionLeftFlex  = {0.f,0.f,0.f};
                float3 directionRightFlex  = {0.f,0.f,0.f};
                float3 directionUpFlex  = {0.f,0.f,0.f};
                float3 directionDownFlex  = {0.f,0.f,0.f};


                if(x > 1)
                {
                    leftFlex = p_in.at(FI - 2) - p_in.at(FI);
                    massLeftFlex = inv_masses.at(FI) / (inv_masses.at(FI - 2) + inv_masses.at(FI));
                    lenLeftFlex= length(leftFlex);
                    directionLeftFlex = normalize(leftFlex);

                }
                if ( x < grid_x - 2)
                {
                    rightFlex = p_in.at(FI + 2) - p_in.at(FI);
                    massRightFlex = inv_masses.at(FI) / (inv_masses.at(FI + 2) + inv_masses.at(FI));
                    lenRightFlex = length(rightFlex);
                    directionRightFlex = normalize(rightFlex);

                }
                if ( y > 1)
                {
                    upFlex = p_in.at(FI - 2*grid_x) - p_in.at(FI);
                    massUpFlex = inv_masses.at(FI) / (inv_masses.at(FI - 2*grid_x) + inv_masses.at(FI));
                    lenUpFlex = length(upFlex);
                    directionUpFlex = normalize(upFlex);

                }
                if (y < grid_y - 2)
                {
                    downFlex = p_in.at(FI + 2*grid_x) - p_in.at(FI);
                    massDownFlex = inv_masses.at(FI) / (inv_masses.at(FI + 2*grid_x) + inv_masses.at(FI));
                    lenDownFlex = length(downFlex);
                    directionDownFlex = normalize(downFlex);
                }


                float leftOffsetFlex = lenLeftFlex - rest_flex_x;
                float rightOffsetFlex  = lenRightFlex - rest_flex_x;
                float upOffsetFlex  = lenUpFlex - rest_flex_y;
                float downOffsetFlex  = lenDownFlex - rest_flex_y;

                float3 fall1 = directionLeftFlex * leftOffsetFlex * massLeftFlex;
                float3 fall2 = directionRightFlex * rightOffsetFlex * massRightFlex;
                float3 fall3 = directionUpFlex * upOffsetFlex * massUpFlex;
                float3 fall4 = directionDownFlex * downOffsetFlex * massDownFlex;

                p_out.at(FI) += (fall1 + fall2 + fall3 + fall4) * fixup_percent;

            }

            for(int obstacle = 0; obstacle < obstacles.size(); ++obstacle)
            {
                fixCollision(p_out.at(FI), obstacles.at(obstacle));
            }

        }
    }
}

void fixCollision(float3& particle_position, const Sphere& sphere) {
    // TODO: resolve particle/obstacle collision
    //
    // Check if the particle is inside the sphere.
    // If so, move the particle out of the sphere (along the sphere normal).
    //
    // Parameters:
    // particle_position: position of the particle
    // sphere: spherical obstacle containg the position and radius

    float dirVec = length(particle_position - sphere.position);
    if(dirVec < sphere.radius)
    {
        particle_position = sphere.position + normalize(particle_position - sphere.position)*sphere.radius;
    }
}

void simulationStep(std::array<std::vector<float3>, 3>& p_buffers, int& next,
                    int& current, int& previous, int grid_x, int grid_y,
                    float cloth_x, float cloth_y,
                    const std::vector<float>& inv_masses,
                    std::vector<float3>& normals, const float3& gravity,
                    const float3& wind_force, float damping,
                    float fixup_percent, int fixup_iterations,
                    const std::vector<Sphere>& obstacles, float dt,
                    bool apply_structural_fixup, bool apply_shear_fixup,
                    bool apply_flexion_fixup) {
    // TODO: implement the simulation step
    //
    // 1. Calculate particle normals  => calcNormals()
    // 2. Perform Verlet Integration  => verletIntegration()
    // 3. Perform fixup steps         => fixupStep()
    //



    // Verlet Integration:
    // You will need three position buffers, since the verlet integration needs
    // the current und previous positions. The integrated position shall be
    // written to the 'next' position buffer. Make sure to update the next,
    // current, previous indices! e.g. after the integration step: next ->
    // current, current -> last and last -> next
    //
    // Fixup:
    // Repeatedly execute the FixUp step after each integration
    // ('fixup_iterations' times). Make sure to update your position buffer
    // indices in the right way!
    //
    // Make sure that 'current' points to the right position buffer before you
    // return!
    //
    // Parameters:
    // p_buffers: 3 position buffers
    // next: index of next position buffer
    // current: index of current position buffer
    // previous: index of previous position buffer
    // grid_x, grid_y: particle-grid dimensions
    // cloth_x, cloth_y: cloth size
    // inv_masses: inverse particle masses
    // normals: particle normals
    // gravity: gravity vector
    // wind_force: wind force
    // damp: damping factor for verlet integration
    // fixup_percent: fixup percent [0..1]
    // fixup_iterations: how often the fixup step should be executed
    // obstacles: obstacle vector
    // dt: timestep
    //
    // apply_structural_fixup: apply structural fixup
    // (needs to be passed unmodified to fixupStep for automatic testing)
    //
    // apply_shear_fixup: apply shear fixup
    // (needs to be passed unmodified to fixupStep for automatic testing)
    //
    // apply_flexion_fixup: apply flexion fixup
    // (needs to be passed unmodified to fixupStep for automatic testing)
    //

    // Simply make cloth fall down. You will want to replace that ;)
    calcNormals(normals, p_buffers[current], grid_x, grid_y);
    verletIntegration(p_buffers[next], p_buffers[current], p_buffers[previous], normals, inv_masses, damping,
                      gravity, wind_force, dt);

    auto temp_ = previous;
    previous = current;
    current = next;
    next = temp_;

    for(size_t i = 0; i < fixup_iterations; ++i)
    {
        fixupStep(p_buffers[next], p_buffers[current], grid_x, grid_y, cloth_x, cloth_y, inv_masses, fixup_percent,obstacles, apply_structural_fixup, apply_shear_fixup, apply_flexion_fixup);
        std::swap(current, next);
    }

//    for (auto& p : p_buffers[current]) p += gravity * dt;
}
