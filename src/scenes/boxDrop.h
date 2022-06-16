// Test Scene
// 
class BoxDrop : public Scene
{
public :
    BoxDrop(const char* name) : Scene(name) {}

	virtual void Init()
    {
        SPDLOG_INFO("BoxDrop Initializing");
        // TODO: read obj file

        // TODO: Voxelize 3d object to make particles
        // ex) 
        // int64_t const initParticleNumber = voxelizer->CreateParticles(&g_buffer->m_positions);
 
        g_buffer->m_commonParam.radius  = 0.01;      
        g_buffer->m_commonParam.dt      = 0.001;      

        // Box Generate
        int32_t xNum = 10;
        int32_t yNum = 10;
        int32_t zNum = 10;
        int32_t const initParticleNumber = xNum*yNum*zNum;
        g_buffer->m_positions.reserve(initParticleNumber);
        g_buffer->m_velocities.reserve(initParticleNumber);

        for (int32_t ii = 0 ; ii < xNum ; ++ii)
            for (int32_t jj = 0 ; jj < yNum ; ++jj)
                for (int32_t kk = 0 ; kk < zNum ; ++kk)
                {
                    g_buffer->m_positions.push_back(glm::vec3(ii*g_buffer->m_commonParam.radius, jj*g_buffer->m_commonParam.radius, kk*g_buffer->m_commonParam.radius));
                    g_buffer->m_velocities.push_back(glm::vec3(0.01, 0.0, 0.0));
                }
    }
};