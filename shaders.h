#pragma once
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include "FastNoise.h"
#include "uniforms.h"
#include "fragment.h"
#include "noise.h"
#include "print.h"

static int frame = 0;

Vertex vertexShader(const Vertex& vertex, const Uniforms& uniforms) {
    // Apply transformations to the input vertex using the matrices from the uniforms
    glm::vec4 clipSpaceVertex = uniforms.projection * uniforms.view * uniforms.model * glm::vec4(vertex.position, 1.0f);

    // Perspective divide
    glm::vec3 ndcVertex = glm::vec3(clipSpaceVertex) / clipSpaceVertex.w;

    // Apply the viewport transform
    glm::vec4 screenVertex = uniforms.viewport * glm::vec4(ndcVertex, 1.0f);
    
    // Transform the normal
    glm::vec3 transformedNormal = glm::mat3(uniforms.model) * vertex.normal;
    transformedNormal = glm::normalize(transformedNormal);

    glm::vec3 transformedWorldPosition = glm::vec3(uniforms.model * glm::vec4(vertex.position, 1.0f));

    // Return the transformed vertex as a vec3
    return Vertex{
        glm::vec3(screenVertex),
        transformedNormal,
        vertex.tex,
        transformedWorldPosition,
        vertex.position
    };
}

Fragment fragmentShaderStripes(Fragment& fragment) {
        // Define the base color for Mars

    // Create some noise based on the fragment position
    /* float noise = glm::fract(sin(glm::dot(glm::vec2(fragment.x, fragment.y), glm::vec2(12.9898f, 78.233f))) * 43758.5453f); */

    /* float noise = glm::fract(sin(glm::dot(fragment.worldPos, glm::vec3(12.9898f, 78.233f, 56.789f))) * 43758.5453f); */
    // Slightly adjust the color based on the noise
    /* Color color = marsColor + Color(static_cast<int>(noise * 25), static_cast<int>(noise * 10), static_cast<int>(noise * 5)); */
    /* glm::vec3 wc = fragment.worldPos * 1.0f + 0.5f; */
    /* Color color = Color(wc.r, wc.g, wc.b); */

    // Define a color for Jupiter. You may want to adjust this to better match Jupiter's appearance.
    Color baseColor = Color(125, 67, 37);  // This color is a kind of dark orange

    // Create stripes based on the fragment's Y position in world space. The frequency of the stripes 
    // can be controlled by scaling the Y coordinate. 
    // The number 10.0f determines the frequency of the stripes, adjust it to get the desired effect.
    float stripePattern = glm::abs(glm::cos(fragment.originalPos.y * 20.0f));

    // Interpolate between the base color and a darker version of the base color based on the stripe pattern.
    // This will create dark stripes on the sphere.
    Color stripeColor = baseColor * (0.5f + 0.5f * stripePattern);

    // Apply lighting intensity
    stripeColor = stripeColor * fragment.intensity;

    // Set the fragment color
    Color color = stripeColor;



    // Define the direction to the center of the circle in world space
    // Apply lighting intensity
    color = color * fragment.intensity;

    // Set the fragment color
    fragment.color = color;

    return fragment;
}

Fragment fragmentShaderNorthPole(Fragment& fragment) {
    // Define a color for the ocean and the continent
    glm::vec3 oceanColor = glm::vec3(0, 0, 128);  // Dark blue
    glm::vec3 continentColor = glm::vec3(34, 139, 34);  // Forest green

    // Create a simple "continent" in the northern hemisphere by making a region where the Y coordinate is 
    // above a certain value look different
    float continentPattern = glm::smoothstep(0.4f, 1.0f, fragment.originalPos.y);

    // Interpolate between the ocean color and the continent color based on the continent pattern
    glm::vec3 c = glm::mix(oceanColor, continentColor, continentPattern);
    Color color = Color(c.x, c.y, c.z);
    // Apply lighting intensity
    color = color * fragment.intensity;

    // Set the fragment color
    fragment.color = color;

    return fragment;
}

Fragment fragmentShaderGreeN(Fragment& fragment) {
    // Define the color for the ocean and the continent using integer values
    Color oceanColor = Color(0, 0, 128);  // Dark blue
    Color continentColor = Color(34, 139, 34);  // Forest green

    // Create a simple "continent" using a spot on the side of the planet.
    // The spot is defined as a region where the x coordinate is within a certain range.
    float continentPattern = glm::smoothstep(-0.2f, 0.2f, fragment.originalPos.x);

    // Interpolate between the ocean color and the continent color based on the continent pattern
    Color c = oceanColor * (1.0f - continentPattern) + continentColor * continentPattern;

    // Apply lighting intensity
    c = c * fragment.intensity;

    // Set the fragment color
    fragment.color = c;

    return fragment;
}

Fragment fragmentShaderEarth(Fragment& fragment) {
    Color color;

    glm::vec3 groundColor = glm::vec3(0.44f, 0.51f, 0.33f);
    glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);

    float x = fragment.originalPos.x;
    float y = fragment.originalPos.y;
    float z = fragment.originalPos.z;
    /* glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y); */
    float radius = sqrt(x*x + y*y + z*z);

    /* glm::vec2 uv = glm::vec2( */
    /*     atan2(x, z), */
    /*     acos(y/sqrt(x*x + y*y + z*z)) */
    /* ); */

    glm::vec3 uv = glm::vec3(
        atan2(x, z),
        acos(y / radius),
        radius
    );

    glm::vec3 uv2 = glm::vec3(
        atan2(x + 10, z),
        acos(y / radius),
        radius
    );


    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    /* noiseGenerator.SetRotationType3D(FastNoiseLite::RotationType3D_ImproveXYPlanes); */
    /* noiseGenerator.DomainWarp(uv.x, uv.y, uv.z); */

    float ox = 1200.0f;
    float oy = 3000.0f;
    float zoom = 100.0f;

    /* float noiseValue1 = noiseGenerator.GetNoise(uv.x * zoom, uv.y * zoom); */
    /* float noiseValue2 = noiseGenerator.GetNoise(uv.y * zoom + 1000.0f, uv.x * zoom + 1000.0f); */
    /* float noiseValue = (noiseValue1 + noiseValue2) * 0.5f; */

    float noiseValue1 = noiseGenerator.GetNoise(uv.x * zoom, uv.y * zoom, uv.z * zoom);
    float noiseValue2 = noiseGenerator.GetNoise(uv2.x * zoom + ox, uv2.y * zoom, uv2.z * zoom + oy);
    float noiseValue = (noiseValue1 + noiseValue2) * 0.5f;



    glm::vec3 tmpColor = (noiseValue < 0.2f) ? oceanColor : groundColor;

    float oxc = 5500.0f;
    float oyc = 6900.0f;
    float zoomc = 300.0f;

    float noiseValueC = noiseGenerator.GetNoise((uv.x + oxc) * zoomc, (uv.y + oyc) * zoomc);
    
    if (noiseValueC > 0.5f) {
        /* tmpColor = cloudColor; */
    }


    color = Color(tmpColor.x, tmpColor.y, tmpColor.z);

    

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment fragmentShader(Fragment& fragment) {

    Color color;

    glm::vec3 brightColor = glm::vec3(1.0f, 0.6f, 0.0f);
    glm::vec3 darkColor = glm::vec3(0.8f, 0.2f, 0.0f); 

    float x = fragment.originalPos.x;
    float y = fragment.originalPos.y;
    float z = fragment.originalPos.z;
    float radius = sqrt(x*x + y*y + z*z);

    glm::vec3 uv = glm::vec3(
        atan2(x, z),
        acos(y/radius),
        radius
    );

    FastNoiseLite noiseGenerator;


    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    noiseGenerator.SetFrequency(0.02 + (10 - abs((static_cast<int>(frame/10.0f) % (2 * 10)) - 10))/2000.0f);

    float zoom = 1000.0f;


    float lat = uv.y;  // Latitude
    float lon = uv.x;  // Longitude

    // Normalize the direction vector
    glm::vec3 dir = glm::normalize(glm::vec3(sin(lat) * cos(lon), sin(lat) * sin(lon), cos(lat)));

    float noiseValue1 = noiseGenerator.GetNoise(uv.x * zoom, uv.y * zoom, uv.z * zoom);
    float noiseValue2 = noiseGenerator.GetNoise(uv.x * zoom + 1000.0f, uv.y * zoom + 1000.0f);
    float noiseValue = (noiseValue1 + noiseValue2) * 0.5f;

    float edgeFactorY = sin(dir.y);  // Value is 0 at the poles and 1 at the equator
    float edgeFactorX = sin(dir.x);  // Value is 0 at leftmost and rightmost points, 1 at front and back
    float edgeFactor = edgeFactorX * edgeFactorY;
    float alpha = 1.0f - glm::mix(1.0f, 0.0f, edgeFactor);  // Alpha is 1 at the poles and 0 at the equator

    glm::vec3 tmpColor = mix(brightColor, darkColor, noiseValue);

    color = Color(tmpColor.x, tmpColor.y, tmpColor.z, alpha);

    
    fragment.color = color * fragment.intensity;

    return fragment;
}

