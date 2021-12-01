


#include <limits>

#include "Scene.h"

#include "task1b.h"


constexpr float epsilon = 0.0001f;

const triangle_t* findClosestHit(
        const float3& p, const float3& d,
        const triangle_t* triangles, std::size_t num_triangles,
        const float3* vertices,
        float& t, float& lambda_1, float& lambda_2)
{
    // TODO: implement intersection test between a ray and a set of triangles.
    // This function should find the CLOSEST intersection with a triangle along the ray.
    // The ray is given by its start point p and direction d.
    // A triangle is represented as an array of three vertex indices.
    // The position of each vertex can be looked up from the vertex array via the vertex index.
    // triangles points to the first element of an array of num_triangles triangles.
    // If an intersection is found, set t to the ray parameter and
    // lambda_1 and lambda_2 to the barycentric coordinates corresponding to the
    // closest point of intersection, and return a pointer to the hit triangle.
    // If no intersection is found, return nullptr.


    float3 p1;
    float3 p2;
    float3 p3;
    float3 q;
    float l1;
    float l2;
    int triangle_index = -1;
    float temp_t;

    t = -1;

    for (int i = 0; i < num_triangles; ++i) {
        std::array<int, 3> o = triangles[i];

        p1 = vertices[o.at(0)];
        p2 = vertices[o.at(1)];
        p3 = vertices[o.at(2)];

        q = p - p1;
        float3 e1 = p2 - p1;
        float3 e2 = p3 - p1;

        if (dot(cross(d, e2), e1) != 0.0000f) {
            temp_t = dot(cross(q, e1), e2) / dot(cross(d, e2), e1);
            l1 = dot(cross(d, e2), q) / dot(cross(d, e2), e1);
            l2 = dot(cross(q, e1), d) / dot(cross(d, e2), e1);
            if(temp_t - t < epsilon || t < 0)
            {
                if (l1 >= 0 && l2 >= 0.0000f && (l1 + l2 <= 1.0000f + epsilon))
                {
                    t = temp_t;
                    lambda_1 = l1;
                    lambda_2 = l2;
                    triangle_index = i;
                }
            }
        }
    }
    if(triangle_index != -1)
    {
        return &triangles[triangle_index];
    }
    return nullptr;

}


bool intersectsRay(
        const float3& p, const float3& d,
        const triangle_t* triangles, std::size_t num_triangles,
        const float3* vertices,
        float t_min, float t_max) {
    // TODO: implement intersection test between a ray and a set of triangles.
    // This method only has to detect whether there is an intersection with ANY triangle
    // along the given subset of the ray.
    // The ray is given by its start point p and direction d.
    // A triangle is represented as an array of three vertex indices.
    // The position of each vertex can be looked up from the vertex array via the vertex index.
    // triangles points to an array of num_triangles.
    // If an intersection is found that falls on a point on the ray between
    // t_min and t_max, return true.
    // Otherwise, return false.


    float3 p1;
    float3 p2;
    float3 p3;
    float3 q;
    triangle_t hit_tri;
    float t ;
    float lambda_1, lambda_2 = 0.0000f;
    float teara = std::numeric_limits<float>::infinity();

    for (int i = 0; i < num_triangles; ++i) {
        std::array<int, 3> o = triangles[i];

        p1 = vertices[o.at(0)];
        p2 = vertices[o.at(1)];
        p3 = vertices[o.at(2)];

        q = p - p1;
        float3 e1 = p2 - p1;
        float3 e2 = p3 - p1;


        if (dot(cross(d, e2), e1) > 0.0000f)
        {

            t = dot(cross(q, e1), e2) / dot(cross(d, e2), e1);
            lambda_1 = dot(cross(d, e2), q) / dot(cross(d, e2), e1);
            lambda_2 = dot(cross(q, e1), d) / dot(cross(d, e2), e1);

            if (lambda_1 >= 0.0000f && lambda_2 >= 0.0000f && (lambda_1 + lambda_2 <= 1.0000f))
            {
                if (t > t_min && t < t_max)
                {
                    return true;
                }
            }
        }
    }
    return false;

}


float3 shade(
        const float3& p, const float3& d,
        const HitPoint& hit,
        const Scene& scene,
        const Pointlight* lights, std::size_t num_lights)
{
    // TODO: implement phong shading.
    // hit represents the surface point to be shaded.
    // hit.position, hit.normal, and hit.k_d and hit.k_s contain the position,
    // surface normal, and diffuse and specular reflection coefficients,
    // hit.m the specular power.
    // lights is a pointer to the first element of an array of num_lights
    // point light sources to consider.
    // Each light contains a member to give its position and color.
    // Return the shaded color.

    // To implement shadows, use scene.intersectsRay(p, d, t_min, t_max) to test
    // whether a ray given by start point p and direction d intersects any
    // object on the section between t_min and t_max.


    float3 v = normalize(p - hit.position);//camera.eye
    float3 n = normalize(hit.normal);//normala
    float3 r = float3(0.0f,0.0f,0.0f);
    float3 light_sum = float3(0.0f,0.0f,0.0f);
    float3 cs = float3(0.0f,0.0f,0.0f);
    float3 cd = float3(0.0f,0.0f,0.0f);


    for (int i = 0; i < num_lights; ++i)
    {
        float3 l = normalize(lights[i].position - hit.position);

        float cosTheta = dot(hit.normal, l);

        r = normalize(2*dot(n,l)*n-l);

        cd.x = hit.k_d.x * std::max(cosTheta,0.0000f);
        cd.y = hit.k_d.y * std::max(cosTheta,0.0000f);
        cd.z = hit.k_d.z * std::max(cosTheta,0.0000f);

        float ar = dot(r, v);

        bool intersected = scene.intersectsRay(hit.position + epsilon * hit.normal, l, epsilon,
                                               std::numeric_limits<float>::infinity());//std::numeric_limits<float>::max());.0f

        if(!intersected)
        {
            if(cosTheta > 0.0000f)
            {
                cs.x = hit.k_s.x * pow(std::max(ar,0.0000f), hit.m);
                cs.y = hit.k_s.y * pow(std::max(ar,0.0000f), hit.m);
                cs.z = hit.k_s.z * pow(std::max(ar,0.0000f), hit.m);
            }
            else
            {
                cs = {0.0000f,0.0000f,0.0000f};
            }

            light_sum.x += (cd.x + cs.x) * lights[i].color.x;
            light_sum.y += (cd.y + cs.y) * lights[i].color.y;
            light_sum.z += (cd.z + cs.z) * lights[i].color.z;
        }

    }
//
    return light_sum;
}

void render(
        image2D<float3>& framebuffer,
        int left, int top, int right, int bottom,
        const Scene& scene,
        const Camera& camera,
        const Pointlight* lights, std::size_t num_lights,
        const float3& background_color,
        int max_bounces)
{
    // TODO: implement raytracing, render the given portion of the framebuffer.
    // left, top, right, and bottom specify the part of the image to compute
    // (inclusive left, top and exclusive right and bottom).
    // camera.eye, camera.lookat, and camera.up specify the position and
    // orientation of the camera, camera.w_s the width of the image plane,
    // and camera.f the focal length to use.
    // Use scene.findClosestHit(p, d) to find the closest point where the ray
    // hits an object.
    // The method returns an std::optional<HitPoint>.
    // If an object is hit, call the function shade to compute the color value
    // of the HitPoint illuminated by the given array of lights.
    // If the ray does not hit an object, use background_color.

    // BONUS: extend your implementation to recursive ray tracing.
    // max_bounces specifies the maximum number of secondary rays to trace.


    //todo ispraviti coding

    size_t wi = width(framebuffer);
    size_t hi = height(framebuffer);
    float3 w = normalize(camera.eye - camera.lookat);
    float3 u = normalize(cross(camera.up, w));
    float3 v = cross(w,u);
    float3 fw = camera.f * w;

    float aspect_ratio = static_cast<float>(wi)/static_cast<float>(hi);
    float h_s = camera.w_s / aspect_ratio;
    float v_;
    float u_;


    for (int y = top; y < bottom; ++y)
    {
        for (int x = left; x < right; ++x)
        {
            v_ = (h_s/2.0f - h_s*(y + 0.5f)/hi);
            u_ = (camera.w_s*(x+0.5f)/wi - camera.w_s/2.0f);

            float3 target = normalize(-fw + v_* v  + u_ * u);
            auto hit = scene.findClosestHit(camera.eye, target);

            float3  pixel;

            if(hit)
            {
                HitPoint hiter = hit.value();
                pixel = shade(camera.eye,target,hiter,scene,lights,num_lights);
                framebuffer(x, y) = pixel;
            }
            else
            {
                framebuffer(x, y) = background_color;
            }

        }
    }
}
