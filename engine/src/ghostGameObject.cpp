#include <Ghost/ghostGameObject.hpp>
#include <cmath>

namespace Ghost {
void GhostGameObject::update(float deltaTime) {
	m_accumulatedTime += deltaTime;

    transform.translation.z += cos(m_accumulatedTime) * deltaTime;
}
} // namespace Ghost
