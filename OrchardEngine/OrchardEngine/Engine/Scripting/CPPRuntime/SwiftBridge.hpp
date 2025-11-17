#pragma once

#include <cstdint>

extern "C" {

struct CEntity {
    uint64_t id;
    uint32_t version;
};

struct CTransform {
    float positionX, positionY, positionZ;
    float rotationX, rotationY, rotationZ, rotationW;
    float scaleX, scaleY, scaleZ;
};

void* engine_create();
void engine_destroy(void* engine);
bool engine_initialize(void* engine, const char* appName, uint32_t width, uint32_t height);
void engine_shutdown(void* engine);
void engine_run(void* engine);
void engine_request_exit(void* engine);
double engine_get_delta_time(void* engine);
uint64_t engine_get_frame_count(void* engine);

void* world_create();
void world_destroy(void* world);
CEntity world_create_entity(void* world);
void world_destroy_entity(void* world, CEntity entity);
void world_add_transform_component(void* world, CEntity entity, CTransform* transform);
void world_update(void* world, double deltaTime);

}
