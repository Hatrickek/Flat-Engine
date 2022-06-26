#pragma once
#include "core/gbuffer.h"
#include "core/ssao.h"
#include "core/framebuffer.h"
extern std::shared_ptr<FlatEngine::SSAO> m_ssao;
extern std::shared_ptr<FlatEngine::GBuffer> m_gBuffer;
extern std::shared_ptr<FlatEngine::FBuffer> m_fBuffer;