#ifndef _COLLISION_H_
#define _COLLISION_H_
#include "plane.h"
#include "ship.h"
#include <vector>

bool collision_detect(Plane& plane, Ship& ship, glm::vec3& collid_pos)
{
    glm::vec3 center = plane.Position;
    std::vector<glm::vec3> plane_v = {
        center + plane.Front,
        center - plane.Front - 0.5f * plane.Right,
        center - plane.Front + 0.5f * plane.Right
    };

    bool is_collision = false;
    // plane and ship
    std::vector<aabb_box> boxes = {ship.b_ship, ship.b_tow1, ship.b_tow2};
    aabb_box ship_box = ship.b_ship;
    for(auto v : plane_v)
    {
        for(auto b : boxes)
        {
            if( v.x - ship_box.ld_point.x <= ship_box.edges.x && 0 <= v.x - ship_box.ld_point.x &&
                v.y - ship_box.ld_point.y <= ship_box.edges.y && 0 <= v.y - ship_box.ld_point.y &&
                v.z - ship_box.ld_point.z <= ship_box.edges.z && 0 <= v.z - ship_box.ld_point.z     )
            {
                is_collision = true;
                collid_pos = v;
                break;
            }
        }
    }
    return is_collision;
}

#endif