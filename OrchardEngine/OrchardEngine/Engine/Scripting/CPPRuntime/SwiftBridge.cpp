#include "SwiftBridge.hpp"
#include "../../Core/Engine.hpp"
#include "../../ECS/World.hpp"
#include "../../ECS/Components/TransformComponent.hpp"

using namespace Orchard;

extern "C" {

void* engine_create() {
    return &Engine::Instance();
}

void engine_destroy(void* engine) {
}

bool engine_initialize(void* engine, const char* appName, uint32_t width, uint32_t height) {
    Engine* e = static_cast<Engine*>(engine);
    return e->Initialize(appName, width, height);
}

void engine_shutdown(void* engine) {
    Engine* e = static_cast<Engine*>(engine);
    e->Shutdown();
}

void engine_run(void* engine) {
    Engine* e = static_cast<Engine*>(engine);
    e->Run();
}

void engine_request_exit(void* engine) {
    Engine* e = static_cast<Engine*>(engine);
    e->RequestExit();
}

double engine_get_delta_time(void* engine) {
    Engine* e = static_cast<Engine*>(engine);
    return e->GetDeltaTime();
}

uint64_t engine_get_frame_count(void* engine) {
    Engine* e = static_cast<Engine*>(engine);
    return e->GetFrameCount();
}

void* world_create() {
    return new ECS::World();
}

void world_destroy(void* world) {
    delete static_cast<ECS::World*>(world);
}

CEntity world_create_entity(void* world) {
    ECS::World* w = static_cast<ECS::World*>(world);
    ECS::Entity entity = w->CreateEntity();
    return CEntity{ entity.id, entity.version };
}

void world_destroy_entity(void* world, CEntity entity) {
    ECS::World* w = static_cast<ECS::World*>(world);
    ECS::Entity e{ entity.id, entity.version };
    w->DestroyEntity(e);
}

void world_add_transform_component(void* world, CEntity entity, CTransform* transform) {
    ECS::World* w = static_cast<ECS::World*>(world);
    ECS::Entity e{ entity.id, entity.version };
    
    ECS::TransformComponent tc;
    tc.SetPosition(Math::Vector3(transform->positionX, transform->positionY, transform->positionZ));
    tc.SetRotation(Math::Quaternion(transform->rotationX, transform->rotationY, 
                                    transform->rotationZ, transform->rotationW));
    tc.SetScale(Math::Vector3(transform->scaleX, transform->scaleY, transform->scaleZ));
    
    w->AddComponent(e, tc);
}

void world_update(void* world, double deltaTime) {
    ECS::World* w = static_cast<ECS::World*>(world);
    w->Update(deltaTime);
}

}
