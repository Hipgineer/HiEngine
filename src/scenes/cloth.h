// Test Scene
// 
class Cloth : public Scene
{
public :
    Cloth(const char* name) : Scene(name) {}

	virtual void Init()
    {
        SPDLOG_INFO("Cloth Initializing");
        
        g_buffer->m_commonParam.radius  = 0.02f;    
        g_buffer->m_commonParam.diameter= g_buffer->m_commonParam.radius * 2.0f;    
        g_buffer->m_commonParam.H       = g_buffer->m_commonParam.diameter * 2.0f * 1.2f ; // 0.048f;      
        g_buffer->m_commonParam.dt      = 0.001f;   

        g_buffer->m_commonParam.iterationNumber = 1;
	    g_buffer->m_commonParam.relaxationParameter = powf(3.3f/g_buffer->m_commonParam.radius,2.0f);
	    g_buffer->m_commonParam.scorrK              = 0.00001f;
	    g_buffer->m_commonParam.scorrDq             = 0.3f;

        g_buffer->m_commonParam.gravity             = glm::vec3(0.0f, -9.81f, 0.0f);
        g_buffer->m_commonParam.AnalysisBox         = boxPoint(glm::vec3(-0.7f, 0.0f, -0.4f), glm::vec3(0.7f, 2.0f, 0.4f));

        PhaseParameters Water;
        Water.density = 1000.0f; 
        Water.color   = glm::vec3(1.0f, 0.0f, 0.0f);
        g_buffer->m_phaseParam.push_back(Water); // phase : 0
        
        // Generate Planes
        float size = 0.3;
        glm::vec3 initVel = glm::vec3(0.0, 0.0, 0.0);

        glm::vec3 centerPoint = glm::vec3(0, 1.0, 0.0);
        createParticlePlane(centerPoint, size, size, 1, initVel, 0);
        
        glm::vec3 centerPoint2 = glm::vec3(0, 0.5, 0.0);
        createParticlePlane(centerPoint2, size, size, 0, initVel, 0);
    }
};