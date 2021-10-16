#pragma once

#include <memory>
#include <rendering/model.hpp>

namespace lve
{
struct Transform2dComponent
{
    glm::vec2 translation{};
    glm::vec2 scale{1.f, 1.f};
    float rotation;

    glm::mat2 mat2()
    {
        float s                = glm::sin(rotation);
        float c                = glm::cos(rotation);
        glm::mat2 rotation_mat = {{c, s}, {-s, c}};
        glm::mat2 scale_mat{{scale.x, 0.f}, {0.f, scale.y}};
        return rotation_mat * scale_mat;
    }
};

class LveGameObject
{
  public:
    using id_t = unsigned int;

    static LveGameObject create_game_object()
    {
        static id_t current_id = 0;
        return LveGameObject{current_id++};
    }

    id_t get_id()
    {
        return id_;
    };

    LveGameObject(const LveGameObject&) = delete;
    LveGameObject& operator=(const LveGameObject&) = delete;
    LveGameObject(LveGameObject&&)                 = default;
    LveGameObject& operator=(LveGameObject&&) = default;

    std::shared_ptr<LveModel> model{};
    glm::vec3 color{};
    Transform2dComponent transform2d;

  private:
    LveGameObject(id_t obj_id) : id_{obj_id}
    {
    }
    id_t id_;
};
} // namespace lve
