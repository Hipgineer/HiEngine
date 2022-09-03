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

        PhaseParameters Sweater;
        Sweater.phaseType = StateOfMatter::CLOTH;
        Sweater.density = 1000.0f; 
        Sweater.color   = glm::vec3(0.0f, 0.0f, 0.0f);
        g_buffer->m_phaseParam.push_back(Sweater);

        // Generate Planes
        float size = 0.3;
        glm::vec3 initVel = glm::vec3(0.0, 0.0, 0.0);

        glm::vec3 centerPoint = glm::vec3(0, 1.0, 0.0);
        createParticleCloth(centerPoint, size, size, 1, initVel, 0);
    }
};