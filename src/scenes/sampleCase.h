// Test Scene
// 
class SampleCase : public Scene
{
public :
    SampleCase(const char* name) : Scene(name) {}

	virtual void Init()
    {
        SPDLOG_INFO("SampleCase Initializing");
        // TODO: read obj file
 
        g_buffer->m_commonParam.radius  = 0.005f;    
        g_buffer->m_commonParam.diameter= g_buffer->m_commonParam.radius * 2.0f;    
        g_buffer->m_commonParam.H       = g_buffer->m_commonParam.diameter * 2.0f * 1.2f ; // 0.048f;      
        g_buffer->m_commonParam.dt      = 0.0005f;   

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
        boxPoint WaterBox = boxPoint(glm::vec3(0.3f, -0.4f, -0.2f), glm::vec3(1.0f, -0.2f, 0.2f));
        glm::vec3 initVel = glm::vec3(0.0, 0.0, 0.0);
        createParticleGrid(WaterBox, initVel, 0);
    }
};