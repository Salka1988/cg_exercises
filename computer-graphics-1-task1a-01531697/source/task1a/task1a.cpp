

#include <iostream>


#include "Scene.h"
#include "task1a.h"


bool intersectRayPlane(const float3& p, const float3& d,
                       const float4& plane, float& t)
{
    float3 n = { plane.x, plane.y , plane.z };

    float dn = dot(d, n);

    if (dn == 0.0f)
        return false;

    t = (plane.w - dot(p, n)) / dn;

    if (t < 0.0f)
        return false;

    return true;
}

bool intersectRaySphere(const float3& p, const float3& d,
                        const float3& c, float r, float& t)
{
    float3 p_ = p;
    float3 c_ = c;
    float3 d_ = d;
    // center is in positive 0.0.1*Z
    // and radius + Z > 0)
    //is it inside or outside t
    float in_out;

    float manijak = c.z + r;

    in_out = c.z - r;

    float3 q_ = {p_.x - c_.x, p_.y - c_.y, p_.z - c_.z};

    if(in_out < 0)
    {
        float delta = dot(d_,q_)*dot(d_,q_) -
                      dot(d_,d_)*(dot(q_,q_) - (r*r));

        float t1 = (float)(-1*dot(d_,q_) +
                           (float)sqrt(delta))/(float)dot(d_,d_);
        float t2 = (float)(-1*dot(d_,q_) -
                           (float)sqrt(delta))/(float)dot(d_,d_);
        if(delta >= 0.0f)
        {
            if (manijak > 0.0f)
            {
                t = t2;
                if(t1 > t2)
                {
                    t = t1;
                }
                return true;
            }
            t = t1;
            if(t1 > t2)
            {
                t = t2;
            }
            return true;
        }
    }
    return false;
//    // TODO: implement intersection test between a ray and a sphere.
//    // If the ray given by start point p and direction
//    // d intersects the sphere given by its center c and radius r,
//    // assign the ray parameter that corresponds to the first point
//    // of intersection to t and return true.
//    // Otherwise, return false.
//    //
}

bool intersectRayBox(const float3& p, const float3& d, const float3& min, const float3& max, float& t)
{

    float epsi = 0.0001;

//    fabs(f1 – f2) <= epsilon;


    if(min.z < 0.0f)
    {
        float xmin = ((min.x - p.x) / d.x);
        float xmax = ((max.x - p.x) / d.x);
        float ymin = ((min.y - p.y) / d.y);
        float ymax = ((max.y - p.y) / d.y);
        float zmin = ((min.z - p.z) / d.z);
        float zmax = ((max.z - p.z) / d.z);

        float tmp = 0.0f;

//        if(abs(xmin - xmax) < epsi && fabs(xmin - xmax) > -epsi)
//            std::cout << fabs(xmin - xmax) << "----prva----\n";
//
//        if(fabs(ymin - ymax) < epsi && fabs(ymin - ymax) > -epsi)
//            std::cout << fabs(ymin - ymax) << "----druga----\n";
//
//        if(fabs(ymin - xmin) < epsi && fabs(ymin - xmin) > -epsi)
//            std::cout << fabs(ymin - xmin) << "----treca----\n";
//
//        if(fabs(ymax - xmax) < epsi && fabs(ymax - xmax) > -epsi)
//            std::cout << fabs(ymax - xmax) << "----cetvrta----\n";
//
//        if(fabs(zmin - xmin) < epsi && fabs(zmin - xmax) > -epsi)
//            std::cout << fabs(zmin - xmin) << "----peta----\n";


        if (xmin > xmax)
        {
            tmp = xmax;
            xmax = xmin;
            xmin = tmp;
        }

        if (ymin > ymax)
        {
            tmp = ymax;
            ymax = ymin;
            ymin = tmp;
        }

        if ((xmin > ymax) || (ymin > xmax))
        {
            return false;
        }

        (ymin > xmin)? xmin = ymin : 0;
        (ymax < xmax)? xmax = ymax : 0;

        if (zmin > zmax)
        {
            tmp = zmax;
            zmax = zmin;
            zmin = tmp;
        }

        if ((xmin > zmax) || (zmin > xmax))
        {
            return false;
        }
        
        (zmin > xmin)? xmin = zmin : 0;
        (zmax < xmax)? xmax = zmax : 0;

        t = xmin;

        if(t < 0)
        {
            t = xmax;
        }
        return true;
    }


    // TODO: implement intersection test between a ray and a box.
    // If the ray given by start point p and direction d intersects
    // the box given by
    // its miniumum and maximum
    // coordinates min and max, assign the ray parameter that corresponds to the first
    // point of intersection to t and return true.
    // Otherwise, return false.
    //


    return false;
}

void render(image2D<float4>& framebuffer, const Scene& scene,
            float w_s, float f, const float4& background_color)
{
    // TODO: implement ray casting
    size_t wi = width(framebuffer);
    size_t hi = height(framebuffer);

    float aspect_ratio = static_cast<float>(wi)/static_cast<float>(hi);
    float h_s = w_s / aspect_ratio;
    float3 p_e = {0,0,0}; // e in the beginning of the coordinate system
    float x_;
    float y_;

    for (int y = 0; y < height(framebuffer); ++y)
    {
        for (int x = 0; x < width(framebuffer); ++x)
        {
            y_ = (h_s/2.0f - h_s*(y + 0.5f)/hi);
            x_ = (w_s*(x+0.5f)/wi - w_s/2.0f);

            float3 target = { x_,y_,-f};
            float t;
            bool hit;
            hit = scene.findClosestHit(p_e,target,t);

            float s = length(t*target);

            float4 grey = float4(1.0f/s, 1.0f/s, 1.0f/s, 1.0f);

            if(hit)
            {
//                float3 dest = {t*target.x,t*target.y,t*target.z };
                framebuffer(x,y) = grey;
            }
            else{
                framebuffer(x,y) = background_color;
            }
        }
    }

}


