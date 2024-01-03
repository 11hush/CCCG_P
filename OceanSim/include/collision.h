#ifndef _COLLISION_H_
#define _COLLISION_H_
#include "plane.h"
#include "ship.h"
#include <vector>

#define COLLID_ON_SHIP  1
#define COLLID_ON_TOW   2

int collision_detect(Plane& plane, Ship& ship, glm::vec3& collid_pos)
{
    glm::vec3 center = plane.Position;
    // std::vector<glm::vec3> plane_v = {
    //     center + plane.Front,
    //     center - plane.Front - 0.5f * plane.Right,
    //     center - plane.Front + 0.5f * plane.Right
    // };
    std::vector<glm::vec3> plane_v = {
        (center + 0.0f * plane.Right + plane.Front) * 1.0f,
        (center - 0.5f * plane.Right - plane.Front) * 1.0f,
        (center + 0.5f * plane.Right - plane.Front) * 1.0f
    };

    int is_collision = 0;
    // plane and ship
    std::vector<aabb_box> boxes = {ship.b_ship, ship.b_tow1, ship.b_tow2};

    // std::cout << "------------------------------------------------------" << std::endl;
    // std::cout << "center: " << center.x << "," << center.y << "," << center.z << std::endl;
    // std::cout << "ship: " << boxes[0].ld_point.x << "," << boxes[0].ld_point.y << "," << boxes[0].ld_point.z << std::endl;

    for(auto v : plane_v)
    {
        for(int i = 0; i < boxes.size(); i++)
        {
            aabb_box b = boxes[i];
            // std::cout << "x: " << v.x << " [" << b.ld_point.x << "," << b.edges.x + b.ld_point.x << "]" << "\t";
            // std::cout << "y: " << v.y << " [" << b.ld_point.y << "," << b.edges.y + b.ld_point.y << "]" << "\t";
            // std::cout << "z: " << v.z << " [" << b.ld_point.z << "," << b.edges.z + b.ld_point.z << "]" << std::endl;
            if( v.x * 1.0f - b.ld_point.x <= b.edges.x && 0 <= v.x * 1.0f - b.ld_point.x &&
                v.y * 1.0f - b.ld_point.y <= b.edges.y && 0 <= v.y * 1.0f - b.ld_point.y &&
                v.z * 1.0f - b.ld_point.z <= b.edges.z && 0 <= v.z * 1.0f - b.ld_point.z     )
            {
                collid_pos = v;
                for(int j = 0; j < 3; j++)
                {
                    if(center[j] > b.ld_point[j] + b.edges[j])
                        collid_pos[j] = b.ld_point[j] + b.edges[j];
                    else if(center[j] < b.ld_point[j])
                        collid_pos[j] = b.ld_point[j];
                }

                // collid_pos = center + 0.3f * (v - center);
                if(0 == i) 
                {
                    is_collision = COLLID_ON_SHIP;
                    // if( (   plane_v[1].x - b.ld_point.x <= b.edges.x && 0 <= plane_v[1].x - b.ld_point.x &&
                    //         plane_v[1].y - b.ld_point.y <= b.edges.y && 0 <= plane_v[1].y - b.ld_point.y &&
                    //         plane_v[1].z - b.ld_point.z <= b.edges.z && 0 <= plane_v[1].z - b.ld_point.z) ||
                    //     (   plane_v[2].x - b.ld_point.x <= b.edges.x && 0 <= plane_v[2].x - b.ld_point.x &&
                    //         plane_v[2].y - b.ld_point.y <= b.edges.y && 0 <= plane_v[2].y - b.ld_point.y &&
                    //         plane_v[2].z - b.ld_point.z <= b.edges.z && 0 <= plane_v[2].z - b.ld_point.z)   )
                    //         if(center.y == b.ld_point.y + b.edges.y)    
                    //             collid_pos.y += 0.05f;

                    // std::cout << "collid on the ship, center: ";
                    // std::cout << center.x << "," << center.y << "," << center.z;
                    // std::cout << ", point: ";
                    // std::cout << collid_pos.x << "," << collid_pos.y << "," << collid_pos.z << std::endl;
                }
                else    
                {
                    is_collision = COLLID_ON_TOW;
                    // std::cout << "collid on the tow, center: ";
                    // std::cout << center.x << "," << center.y << "," << center.z;
                    // std::cout << "\tv: ";
                    // std::cout << v.x << "," << v.y << "," << v.z;
                    // std::cout << "\tpoint: ";
                    // std::cout << collid_pos.x << "," << collid_pos.y << "," << collid_pos.z << std::endl;
                }
                
                if(is_collision)
                    break;
            }
            if(is_collision)
                break;
        }
        // std::cout << std::endl;
    }
    // if(is_collision)    std::cout << "collision detected!" << std::endl;
    return is_collision;
}

#endif