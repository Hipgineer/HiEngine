
#define EPSILON_SCENE_HELPER 0.00000001

bool checkGlobalVariable()
{
    if (glm::length(g_buffer->m_commonParam.AnalysisBox.maxPoint - g_buffer->m_commonParam.AnalysisBox.minPoint) < EPSILON_SCENE_HELPER) return false;
    if (g_buffer->m_commonParam.radius < EPSILON_SCENE_HELPER) return false;
}

bool isInsideOfBox(glm::vec3 pos, boxPoint box)
{
    if ((pos.x < box.minPoint.x) || (pos.x > box.maxPoint.x)) return false;
    if ((pos.y < box.minPoint.y) || (pos.y > box.maxPoint.y)) return false;
    if ((pos.z < box.minPoint.z) || (pos.z > box.maxPoint.z)) return false;
    return true;
}

void createParticleGrid(boxPoint particleBox, glm::vec3 velocity, int32_t phaseID)
{
    if (!checkGlobalVariable()) SPDLOG_ERROR("failed to create particle grid.");
    glm::vec3 AnalysisBoxMinPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.minPoint + g_buffer->m_commonParam.radius;
    glm::vec3 AnalysisBoxMaxPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.maxPoint - g_buffer->m_commonParam.radius;
    boxPoint AnalysisBoxConsideringRadius = boxPoint(AnalysisBoxMinPointConsideringRadius, AnalysisBoxMaxPointConsideringRadius);

    int32_t xNum = static_cast<int32_t>((particleBox.maxPoint.x - particleBox.minPoint.x) / (2.0f * g_buffer->m_commonParam.radius));
    int32_t yNum = static_cast<int32_t>((particleBox.maxPoint.y - particleBox.minPoint.y) / (2.0f * g_buffer->m_commonParam.radius));
    int32_t zNum = static_cast<int32_t>((particleBox.maxPoint.z - particleBox.minPoint.z) / (2.0f * g_buffer->m_commonParam.radius));
    int32_t totalParticleNumber = xNum*yNum*zNum + g_buffer->GetNumParticles();

    g_buffer->m_positions.reserve(totalParticleNumber);
    g_buffer->m_velocities.reserve(totalParticleNumber);
    g_buffer->m_phases.reserve(totalParticleNumber);
    g_buffer->m_colorValues.reserve(totalParticleNumber);
    
    for (int32_t ii = 0 ; ii < xNum ; ++ii)
    {
        for (int32_t jj = 0 ; jj < yNum ; ++jj)
        {
            for (int32_t kk = 0 ; kk < zNum ; ++kk)
            {
                glm::vec3 tmp_position = glm::vec3(particleBox.minPoint.x + g_buffer->m_commonParam.radius + ii*2.0f*g_buffer->m_commonParam.radius,
                                                    particleBox.minPoint.y + g_buffer->m_commonParam.radius + jj*2.0f*g_buffer->m_commonParam.radius,
                                                    particleBox.minPoint.z + g_buffer->m_commonParam.radius + kk*2.0f*g_buffer->m_commonParam.radius);
                if (isInsideOfBox(tmp_position, AnalysisBoxConsideringRadius))
                {
                    g_buffer->m_positions.push_back(tmp_position);
                    g_buffer->m_velocities.push_back(velocity);
                    g_buffer->m_phases.push_back(phaseID);
                    g_buffer->m_colorValues.push_back(static_cast<float>(jj));
                }
            }
        }
    }

    g_buffer->m_positions.shrink_to_fit();
    g_buffer->m_velocities.shrink_to_fit();
    g_buffer->m_phases.shrink_to_fit();
    g_buffer->m_colorValues.shrink_to_fit();
}

void createParticleSphere(glm::vec3 centerPoint, float sphereRadius, glm::vec3 velocity, int32_t phaseID)
{
    if (!checkGlobalVariable()) SPDLOG_ERROR("failed to create particle sphere.");
    glm::vec3 AnalysisBoxMinPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.minPoint + g_buffer->m_commonParam.radius;
    glm::vec3 AnalysisBoxMaxPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.maxPoint - g_buffer->m_commonParam.radius;
    boxPoint AnalysisBoxConsideringRadius = boxPoint(AnalysisBoxMinPointConsideringRadius, AnalysisBoxMaxPointConsideringRadius);

    int32_t rNum = static_cast<int32_t>((sphereRadius) / (2.0f * g_buffer->m_commonParam.radius));
    int32_t totalParticleNumber = (2*rNum+1)*(2*rNum+1)*(2*rNum+1) + g_buffer->GetNumParticles();

    g_buffer->m_positions.reserve(totalParticleNumber);
    g_buffer->m_velocities.reserve(totalParticleNumber);
    g_buffer->m_phases.reserve(totalParticleNumber);
    g_buffer->m_colorValues.reserve(totalParticleNumber);
    
    for (int32_t ii = -rNum ; ii < rNum+1 ; ++ii)
    {
        for (int32_t jj = -rNum; jj < rNum + 1; ++jj)
        {
            for (int32_t kk = -rNum; kk < rNum + 1; ++kk)
            {
                glm::vec3 tmp_position = glm::vec3(centerPoint.x + ii*2.0f*g_buffer->m_commonParam.radius,
                                                   centerPoint.y + jj*2.0f*g_buffer->m_commonParam.radius,
                                                   centerPoint.z + kk*2.0f*g_buffer->m_commonParam.radius);

                if (isInsideOfBox(tmp_position, AnalysisBoxConsideringRadius) && (length(tmp_position-centerPoint) < sphereRadius-g_buffer->m_commonParam.radius))
                {
                    g_buffer->m_positions.push_back(tmp_position);
                    g_buffer->m_velocities.push_back(velocity);
                    g_buffer->m_phases.push_back(phaseID);
                    g_buffer->m_colorValues.push_back(static_cast<float>(jj));
                }
            }
        }
    }

    g_buffer->m_positions.shrink_to_fit();
    g_buffer->m_velocities.shrink_to_fit();
    g_buffer->m_phases.shrink_to_fit();
    g_buffer->m_colorValues.shrink_to_fit();
}

void createParticlePlane(glm::vec3 center, float size1, float size2, int32_t axis, glm::vec3 velocity,  int32_t phaseID)
{
    if (!checkGlobalVariable()) SPDLOG_ERROR("failed to create particle plane.");
    glm::vec3 AnalysisBoxMinPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.minPoint + g_buffer->m_commonParam.radius;
    glm::vec3 AnalysisBoxMaxPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.maxPoint - g_buffer->m_commonParam.radius;
    boxPoint AnalysisBoxConsideringRadius = boxPoint(AnalysisBoxMinPointConsideringRadius, AnalysisBoxMaxPointConsideringRadius);

    int32_t num1 = static_cast<int32_t>( size1 / (2.0f * g_buffer->m_commonParam.radius));
    int32_t num2 = static_cast<int32_t>( size2 / (2.0f * g_buffer->m_commonParam.radius));
    int32_t totalParticleNumber = num1*num2 + g_buffer->GetNumParticles();

    g_buffer->m_positions.reserve(totalParticleNumber);
    g_buffer->m_velocities.reserve(totalParticleNumber);
    g_buffer->m_phases.reserve(totalParticleNumber);
    g_buffer->m_colorValues.reserve(totalParticleNumber);
    
    glm::vec3 tmp_position;
    for (int32_t ii = 0 ; ii < num1 ; ++ii)
    {
        for (int32_t jj = 0 ; jj < num2 ; ++jj)
        {   
            if ( axis == 0 )
                tmp_position = center + g_buffer->m_commonParam.radius + glm::vec3(0.0, ii*2.0f*g_buffer->m_commonParam.radius, jj*2.0f*g_buffer->m_commonParam.radius);
            else if ( axis == 1 )
                tmp_position = center + g_buffer->m_commonParam.radius + glm::vec3(ii*2.0f*g_buffer->m_commonParam.radius, 0.0, jj*2.0f*g_buffer->m_commonParam.radius);
            else if ( axis == 2 )
                tmp_position = center + g_buffer->m_commonParam.radius + glm::vec3(ii*2.0f*g_buffer->m_commonParam.radius, jj*2.0f*g_buffer->m_commonParam.radius, 0.0);

            if (isInsideOfBox(tmp_position, AnalysisBoxConsideringRadius))
            {
                g_buffer->m_positions.push_back(tmp_position);
                g_buffer->m_velocities.push_back(velocity);
                g_buffer->m_phases.push_back(phaseID);
                g_buffer->m_colorValues.push_back(static_cast<float>(jj));
            }
        }
    }

    g_buffer->m_positions.shrink_to_fit();
    g_buffer->m_velocities.shrink_to_fit();
    g_buffer->m_phases.shrink_to_fit();
    g_buffer->m_colorValues.shrink_to_fit();

    SPDLOG_INFO("a plane generated");
}

void createParticleCloth(glm::vec3 center, float size1, float size2, int32_t axis, glm::vec3 velocity,  int32_t phaseID)
{
    if (!checkGlobalVariable()) SPDLOG_ERROR("failed to create particle plane.");
    glm::vec3 AnalysisBoxMinPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.minPoint + g_buffer->m_commonParam.radius;
    glm::vec3 AnalysisBoxMaxPointConsideringRadius = g_buffer->m_commonParam.AnalysisBox.maxPoint - g_buffer->m_commonParam.radius;
    boxPoint AnalysisBoxConsideringRadius = boxPoint(AnalysisBoxMinPointConsideringRadius, AnalysisBoxMaxPointConsideringRadius);

    if ((axis == 0)&&(isInsideOfBox(center + glm::vec3(0, 0.5*size1, 0.5*size2), AnalysisBoxConsideringRadius))) SPDLOG_ERROR("failed to create particle plane.");
    if ((axis == 1)&&(isInsideOfBox(center + glm::vec3(0.5*size1, 0, 0.5*size2), AnalysisBoxConsideringRadius))) SPDLOG_ERROR("failed to create particle plane.");
    if ((axis == 2)&&(isInsideOfBox(center + glm::vec3(0.5*size1, 0.5*size2, 0), AnalysisBoxConsideringRadius))) SPDLOG_ERROR("failed to create particle plane.");
    if ((axis == 0)&&(isInsideOfBox(center - glm::vec3(0, 0.5*size1, 0.5*size2), AnalysisBoxConsideringRadius))) SPDLOG_ERROR("failed to create particle plane.");
    if ((axis == 1)&&(isInsideOfBox(center - glm::vec3(0.5*size1, 0, 0.5*size2), AnalysisBoxConsideringRadius))) SPDLOG_ERROR("failed to create particle plane.");
    if ((axis == 2)&&(isInsideOfBox(center - glm::vec3(0.5*size1, 0.5*size2, 0), AnalysisBoxConsideringRadius))) SPDLOG_ERROR("failed to create particle plane.");

    int32_t num1 = static_cast<int32_t>( size1 / (2.0f * g_buffer->m_commonParam.radius)) - 1;
    int32_t num2 = static_cast<int32_t>( size2 / (2.0f * g_buffer->m_commonParam.radius)) - 1;
    int32_t totalParticleNumber = num1*num2 + g_buffer->GetNumParticles();

    g_buffer->m_positions.reserve(totalParticleNumber);
    g_buffer->m_velocities.reserve(totalParticleNumber);
    g_buffer->m_phases.reserve(totalParticleNumber);
    g_buffer->m_colorValues.reserve(totalParticleNumber);

    int32_t nStretchLines = (num1-1) * (num2) + (num1) * (num2-1);
    int32_t nBendLines    = (num1-2) * (num2) + (num1) * (num2-2) ;
    int32_t nShearLines   = 2 * (num1-1) * (num2-1);
    int32_t nTriangles    = 2 * (num1-1) * (num2-1);

    g_buffer->m_stretchID.reserve(2 * (nStretchLines + g_buffer->GetNumStretchLines()) );
    g_buffer->m_bendID.reserve(2 * (nBendLines  + g_buffer->GetNumBendLines()) );       
    g_buffer->m_shearID.reserve(2 * (nShearLines + g_buffer->GetNumShearLines()) );                   
    g_buffer->m_triangleID.reserve(3 * (nTriangles + g_buffer->GetNumTriangles()) );       
    
    glm::vec3 tmp_position;
    for (int32_t ii = 0 ; ii < num1 ; ++ii)
    {
        for (int32_t jj = 0 ; jj < num2 ; ++jj)
        {   
            if ( axis == 0 )
                tmp_position = center + g_buffer->m_commonParam.radius + glm::vec3(0.0, ii*2.0f*g_buffer->m_commonParam.radius, jj*2.0f*g_buffer->m_commonParam.radius);
            else if ( axis == 1 )
                tmp_position = center + g_buffer->m_commonParam.radius + glm::vec3(ii*2.0f*g_buffer->m_commonParam.radius, 0.0, jj*2.0f*g_buffer->m_commonParam.radius);
            else if ( axis == 2 )
                tmp_position = center + g_buffer->m_commonParam.radius + glm::vec3(ii*2.0f*g_buffer->m_commonParam.radius, jj*2.0f*g_buffer->m_commonParam.radius, 0.0);

            if (isInsideOfBox(tmp_position, AnalysisBoxConsideringRadius))
            {
                g_buffer->m_positions.push_back(tmp_position);
                g_buffer->m_velocities.push_back(velocity);
                g_buffer->m_phases.push_back(phaseID);
                g_buffer->m_colorValues.push_back(static_cast<float>(jj));
            }
        }
    }

    // stretchIDs
    int32_t tmpN = 0;
    for (int32_t ii = 0 ; ii < num1 ; ++ii)
    {
        for (int32_t jj = 0 ; jj < num2 ; ++jj)
        {   
            if (jj < num2 - 1)
            {
                g_buffer->m_stretchID.push_back(ii*num2 + jj);
                g_buffer->m_stretchID.push_back(ii*num2 + jj + 1);
                ++tmpN;
            }
            if (ii < num1 - 1)
            {
                g_buffer->m_stretchID.push_back(ii*num2 + jj);
                g_buffer->m_stretchID.push_back((ii+1)*num2 + jj);
                ++tmpN;
            }
        }
    }
    if (tmpN != 2*nStretchLines) SPDLOG_ERROR("failed to create particle plane.");

    // BendIDs
    tmpN = 0;
    for (int32_t ii = 0 ; ii < num1 ; ++ii)
    {
        for (int32_t jj = 0 ; jj < num2 ; ++jj)
        {   
            if (jj < num2 - 2)
            {
                g_buffer->m_stretchID.push_back(ii*num2 + jj);
                g_buffer->m_stretchID.push_back(ii*num2 + jj + 2);
                ++tmpN;
            }
            if (ii < num1 - 2)
            {
                g_buffer->m_stretchID.push_back(ii*num2 + jj);
                g_buffer->m_stretchID.push_back((ii+2)*num2 + jj);
                ++tmpN;
            }
        }
    }
    if (tmpN != 2*nBendLines) SPDLOG_ERROR("failed to create particle plane.");


    // shearIDs
    tmpN = 0;
    for (int32_t ii = 0 ; ii < num1-1 ; ++ii)
    {
        for (int32_t jj = 0 ; jj < num2-1 ; ++jj)
        {   
            g_buffer->m_stretchID.push_back(ii*num2 + jj);
            g_buffer->m_stretchID.push_back((ii+1)*num2 + jj + 1);
            ++tmpN;
            
            g_buffer->m_stretchID.push_back((ii+1)*num2 + jj);
            g_buffer->m_stretchID.push_back(ii*num2 + jj + 1);
            ++tmpN;
        }
    }
    if (tmpN != 2*nShearLines) SPDLOG_ERROR("failed to create particle plane.");


    // triIDs
    tmpN = 0;
    for (int32_t ii = 0 ; ii < num1-1 ; ++ii)
    {
        for (int32_t jj = 0 ; jj < num2-1 ; ++jj)
        {   
            g_buffer->m_triangleID.push_back(ii*num2 + jj);
            g_buffer->m_triangleID.push_back((ii+1)*num2 + jj + 1);
            g_buffer->m_triangleID.push_back(ii*num2 + jj + 1);
            ++tmpN;
            
            g_buffer->m_triangleID.push_back(ii*num2 + jj);
            g_buffer->m_triangleID.push_back((ii+1)*num2 + jj + 1);
            g_buffer->m_triangleID.push_back((ii+1)*num2 + jj);
            ++tmpN;
        }
    }
    if (tmpN != 3*nTriangles) SPDLOG_ERROR("failed to create particle plane.");


    g_buffer->m_positions.shrink_to_fit();
    g_buffer->m_velocities.shrink_to_fit();
    g_buffer->m_phases.shrink_to_fit();
    g_buffer->m_colorValues.shrink_to_fit();

    g_buffer->m_stretchID.shrink_to_fit();
    g_buffer->m_bendID.shrink_to_fit();  
    g_buffer->m_shearID.shrink_to_fit();              
    g_buffer->m_triangleID.shrink_to_fit();

    SPDLOG_INFO("a plane generated");
}
