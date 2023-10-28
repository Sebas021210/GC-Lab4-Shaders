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

Fragment sun(Fragment& fragment, float time) {
    Color color;

    // Get UV coordinates
    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float offsetX = 10000.0f;
    float offsetY = 10000.0f;
    float scale = 9000.0f;

    // Generate Perlin noise
    float noiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);

    // Map noise range to color
    // Color naranja con borde amarillo
    glm::vec3 sunColor = glm::vec3(1.0f, 0.5f, 0.0f) + noiseValue * 0.7f;

    // Set final fragment color
    color = Color(sunColor.r, sunColor.g, sunColor.b);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment earth(Fragment& fragment, float time) {
    Color color;

    // Get UV coordinates
    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    float offsetX = 10000.0f;
    float offsetY = 10000.0f;
    float scale = 900.0f;

    // Generate noise for the land and ocean
    float landNoiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);

    // Map noise to colors
    glm::vec3 landColor = glm::vec3(0.44f, 0.51f, 0.33f);
    glm::vec3 oceanColor = glm::vec3(0.12f, 0.38f, 0.57f);
    glm::vec3 polarColor = glm::vec3(1.0f, 1.0f, 1.0f);  // Color para los polos (blanco)
    glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);

    if (landNoiseValue < 0.3f) {
        color = Color(oceanColor.r, oceanColor.g, oceanColor.b);
    } else {
        color = Color(landColor.r, landColor.g, landColor.b);
    }

    // Agrega los colores de los polos
    if (uv.y < -0.41 || uv.y > 0.41) {
        color = Color(polarColor.r, polarColor.g, polarColor.b);
    }

    // Generate noise for clouds
    float cloudNoiseValue = noiseGenerator.GetNoise(uv.x * scale, uv.y * scale);

    // Añade las nubes transparentes
    if (cloudNoiseValue > 0.7f) {
        // Ajusta la opacidad de las nubes (0.3 para que sean más transparentes)
        glm::vec3 blendedColor = mix(glm::vec3(color.r, color.g, color.b), cloudColor, 0.3f);
        color = Color(blendedColor.r, blendedColor.g, blendedColor.b);
    }

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment jupiter(Fragment& fragment, float time) {
    Color color;

    // Get UV coordinates
    glm::vec2 uv = glm::vec2(fragment.originalPos.x, fragment.originalPos.y);

    // Definir un radio para Júpiter
    float jupiterRadius = 0.5; // Ajusta este valor según tus necesidades

    // Calcular la distancia desde el centro de Júpiter
    float distance = length(uv);

    // Ajustar el color de base de Júpiter
    glm::vec3 jupiterColor = glm::vec3(0.6, 0.3, 0.1); // Ajusta este valor según tus necesidades

    // Añadir bandas atmosféricas de Júpiter
    float bands = sin(15.0 * distance);

    // Ajusta la apariencia de las bandas
    float bandsIntensity = (0.5 + 0.5 * bands); // Ajusta los valores para controlar la apariencia de las bandas

    // Aplicar variaciones de color basadas en ruido
    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float offsetX = 10000.0f;
    float offsetY = 10000.0f;
    float scale = 2000.0f;

    // Generar ruido para las variaciones en el color
    float colorNoiseValue = noiseGenerator.GetNoise((uv.x + offsetX) * scale, (uv.y + offsetY) * scale);

    // Aplicar variaciones de color
    glm::vec3 finalColor = jupiterColor + glm::vec3(colorNoiseValue * 0.05); // Ajusta el factor de ruido según tus necesidades

    // Añadir las bandas atmosféricas a las variaciones de color
    finalColor *= bandsIntensity;

    // Aplicar el color final
    color = Color(finalColor.r, finalColor.g, finalColor.b);

    fragment.color = color * fragment.intensity;

    return fragment;
}

Fragment moon(Fragment& fragment, float time) {
    Color color;

    // No need for UV coordinates

    FastNoiseLite noiseGenerator;
    noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    float scale = 5000.0f;

    // Generate noise for the moon's surface
    float noiseValue = noiseGenerator.GetNoise(fragment.originalPos.x * scale, fragment.originalPos.y * scale);

    // Definir un umbral para identificar cráteres
    float craterThreshold = 0.01f;

    if (noiseValue < craterThreshold) {
        // Usar un color más claro para representar cráteres
        color = Color(0.9f, 0.9f, 0.9f);
    } else {
        // Usar un color uniforme para la superficie de la luna
        color = Color(0.8f, 0.8f, 0.8f);
    }

    fragment.color = color * fragment.intensity;

    return fragment;
}
