// Test Scene
// 
class SphereDrop : public Scene
{
public :
    SphereDrop(const char* name) : Scene(name) {}

	virtual void Init()
    {
        SPDLOG_INFO("SphereDrop Initializing");
        
        g_buffer->m_commonParam.radius  = 0.02f;    
        g_buffer->m_commonParam.diameter= g_buffer->m_commonParam.radius * 2.0f;    
        g_buffer->m_commonParam.H       = g_buffer->m_commonParam.diameter * 2.0f * 1.2f ; // 0.048f;      
        g_buffer->m_commonParam.dt      = 0.001f;   

        g_buffer->m_commonParam.iterationNumber = 1;
	    g_buffer->m_commonParam.relaxationParameter = powf(3.3f/g_buffer->m_commonParam.radius,2.0f);
	    g_buffer->m_commonParam.scorrK              = 0.00001f;
	    g_buffer->m_commonParam.scorrDq             = 0.3f;

        g_buffer->m_commonParam.gravity             = glm::vec3(0.0f, -9.81f, 0.0f);
        g_buffer->m_commonParam.AnalysisBox         = boxPoint(glm::vec3(-0.7f, 0.0f, -0.7f), glm::vec3(0.7f, 2.0f, 0.7f));

        PhaseParameters Water;
        Water.density = 1000.0f; 
        Water.color   = glm::vec3(1.0f, 0.0f, 0.0f);
        g_buffer->m_phaseParam.push_back(Water); // phase : 0

        PhaseParameters Oil;
        Oil.density = 2000.0f;
        Oil.color   = glm::vec3(0.0f, 1.0f, 0.0f);
        g_buffer->m_phaseParam.push_back(Oil);
        
        //Box Generate
        boxPoint fluidBox = boxPoint(glm::vec3(-0.7f, 0.0f, -0.7f), glm::vec3(0.7f, 0.4f, 0.7f));
        glm::vec3 initVel = glm::vec3(0.0, 0.0, 0.0);
        createParticleGrid(fluidBox, initVel, 0);

        //Sphere Generate
        glm::vec3 centerPoint = glm::vec3(0.0, 1.2, 0.0);
        float sphereRadius = 0.2;
        glm::vec3 initVel2 = glm::vec3(0.0, 0.0, 0.0);
        createParticleSphere(centerPoint, sphereRadius, initVel2, 0);
    }
};