#include <Ghost/ghostGameObject.hpp>

namespace Ghost {
void GhostGameObject::update(float deltaTime) {
	m_accumulatedTime += deltaTime;
	const float rotationSpeed = 0.5f;
	glm::quat deltaRotation = glm::angleAxis(rotationSpeed * deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));

	transform.rotation = transform.rotation * deltaRotation;

	transform.rotation = glm::normalize(transform.rotation);
}
} // namespace Ghost
