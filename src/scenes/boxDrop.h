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
 
        g_buffer->m_commonParam.radius  = 0.01f;    
        g_buffer->m_commonParam.diameter= g_buffer->m_commonParam.radius * 2.0f;    
        g_buffer->m_commonParam.H       = g_buffer->m_commonParam.diameter * 2.0f * 1.2f ; // 0.048f;      
        g_buffer->m_commonParam.dt      = 0.001f;   

        g_buffer->m_commonParam.iterationNumber = 1;
	    g_buffer->m_commonParam.relaxationParameter = powf(3.3f/g_buffer->m_commonParam.radius,2.0f);
	    g_buffer->m_commonParam.scorrK              = 0.00001f;
	    g_buffer->m_commonParam.scorrDq             = 0.3f;
        g_buffer->m_commonParam.gravity             = glm::vec3(0.0f, -9.81f, 0.0f);
        g_buffer->m_commonParam.AnalysisBox         = boxPoint(glm::vec3(0.0f, -0.4f, -0.2f), glm::vec3(1.0f, 1.0f, 0.2f));

        PhaseParameters Water;
        Water.density = 1000.0f; 
        Water.color   = glm::vec3(1.0f, 0.0f, 0.0f);
        g_buffer->m_phaseParam.push_back(Water); // phase : 0
        

        //Box Generate
        boxPoint WaterBox = boxPoint(glm::vec3(0.0f, -0.4f, -0.2f), glm::vec3(0.4f, 0.4f, 0.2f));

        int32_t xNum = (WaterBox.maxPoint.x - WaterBox.minPoint.x) / (2.0f * g_buffer->m_commonParam.radius);
        int32_t yNum = (WaterBox.maxPoint.y - WaterBox.minPoint.y) / (2.0f * g_buffer->m_commonParam.radius);
        int32_t zNum = (WaterBox.maxPoint.z - WaterBox.minPoint.z) / (2.0f * g_buffer->m_commonParam.radius);
        int32_t const initParticleNumber = xNum*yNum*zNum;

        g_buffer->m_positions.reserve(initParticleNumber);
        g_buffer->m_velocities.reserve(initParticleNumber);
        g_buffer->m_phases.reserve(initParticleNumber);
        g_buffer->m_colorValues.reserve(initParticleNumber);
        
        for (int32_t ii = 0 ; ii < xNum ; ++ii)
        {
            for (int32_t jj = 0 ; jj < yNum ; ++jj)
            {
                for (int32_t kk = 0 ; kk < zNum ; ++kk)
                {
                    glm::vec3 tmp_position = glm::vec3(WaterBox.minPoint.x + g_buffer->m_commonParam.radius + ii*2.0f*g_buffer->m_commonParam.radius,
                                                       WaterBox.minPoint.y + g_buffer->m_commonParam.radius + jj*2.0f*g_buffer->m_commonParam.radius,
                                                       WaterBox.minPoint.z + g_buffer->m_commonParam.radius + kk*2.0f*g_buffer->m_commonParam.radius);
                    g_buffer->m_positions.push_back(tmp_position);
                    g_buffer->m_velocities.push_back(glm::vec3(0.0f, 0.0f,0.0f));
                    g_buffer->m_phases.push_back(0);
                    g_buffer->m_colorValues.push_back(static_cast<float>(jj));
                }
            }
        }
    }
};