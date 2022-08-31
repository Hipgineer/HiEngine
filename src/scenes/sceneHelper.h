bool isInsideOfBox(glm::vec3 pos, boxPoint box)
{
    if ((pos.x < box.minPoint.x) || (pos.x > box.maxPoint.x)) return false;
    if ((pos.y < box.minPoint.y) || (pos.y > box.maxPoint.y)) return false;
    if ((pos.z < box.minPoint.z) || (pos.z > box.maxPoint.z)) return false;
    return true;
}

void createParticleGrid(boxPoint particleBox, glm::vec3 velocity, int32_t phaseID)
{
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