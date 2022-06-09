class BoxDrop : public Scene
{
public :
    BoxDrop(const char* name) : Scene(name) {}

	virtual void Init()
    {
        SPDLOG_INFO("BoxDrop Initializing");
        g_buffer->m_positions.push_back(glm::vec3(0.0, 0.0, 0.0));
        g_buffer->m_positions.push_back(glm::vec3(0.1, 0.0, 0.0));
        g_buffer->m_positions.push_back(glm::vec3(0.2, 0.0, 0.0));

        g_buffer->m_velocities.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
        g_buffer->m_velocities.push_back(glm::vec3(0.1f, 0.1f, 0.0f));
        g_buffer->m_velocities.push_back(glm::vec3(0.1f, 0.2f, 0.0f));
    }
};