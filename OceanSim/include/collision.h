#ifndef _COLLISION_H_
#define _COLLISION_H_
#include "plane.h"
#include "ship.h"
#include <vector>

bool collision_detect(Plane& plane, Ship& ship, glm::vec3& collid_pos)
{
    glm::vec3 center = plane.Position;
    // std::vector<glm::vec3> plane_v = {
    //     center + plane.Front,
    //     center - plane.Front - 0.5f * plane.Right,
    //     center - plane.Front + 0.5f * plane.Right
    // };
    std::vector<glm::vec3> plane_v = {
        center - plane.Front,
        center - 0.5f * plane.Right + plane.Front,
        center + 0.5f * plane.Right + plane.Front
    };

    bool is_collision = false;
    // plane and ship
    std::vector<aabb_box> boxes = {ship.b_ship, ship.b_tow1, ship.b_tow2};

    // std::cout << "------------------------------------------------------" << std::endl;
    // std::cout << "center: " << center.x << "," << center.y << "," << center.z << std::endl;
    // std::cout << "ship: " << boxes[0].ld_point.x << "," << boxes[0].ld_point.y << "," << boxes[0].ld_point.z << std::endl;

    for(auto v : plane_v)
    {
        for(auto b : boxes)
        {
            // std::cout << "x: " << v.x << " [" << b.ld_point.x << "," << b.edges.x + b.ld_point.x << "]" << "\t";
            // std::cout << "y: " << v.y << " [" << b.ld_point.y << "," << b.edges.y + b.ld_point.y << "]" << "\t";
            // std::cout << "z: " << v.z << " [" << b.ld_point.z << "," << b.edges.z + b.ld_point.z << "]" << std::endl;
            if( v.x - b.ld_point.x <= b.edges.x && 0 <= v.x - b.ld_point.x &&
                v.y - b.ld_point.y <= b.edges.y && 0 <= v.y - b.ld_point.y &&
                v.z - b.ld_point.z <= b.edges.z && 0 <= v.z - b.ld_point.z     )
            {
                is_collision = true;
                collid_pos = center;
                break;
            }
        }
        // std::cout << std::endl;
    }
    // if(is_collision)    std::cout << "collision detected!" << std::endl;
    return is_collision;
}

#endif