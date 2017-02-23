#include "ChartPainterSystem.h"

#include "Render/2D/Systems/RenderSystem2D.h"
#include "Render/rhi/dbg_Draw.h"

namespace OverdrawPerformanceTester
{

const DAVA::Vector2 ChartPainterSystem::chartOffset(0.1f, 0.1f);
const DAVA::Color ChartPainterSystem::gridColor(0.4f, 0.4f, 0.4f, 0.4f);
const DAVA::float32 ChartPainterSystem::chartLen = 0.8f;
const DAVA::float32 ChartPainterSystem::maxFrametime = 1.0f / 10.0f;
const DAVA::float32 ChartPainterSystem::minFrametime = 1.0f / 70.0f;
const DAVA::float32 ChartPainterSystem::frametimeAxisLen = maxFrametime - minFrametime;
const DAVA::float32 ChartPainterSystem::maxOverdraw = 1000.0f;
const DAVA::float32 ChartPainterSystem::overdrawStep = 100.0f;
const DAVA::float32 ChartPainterSystem::frametimeStep = 0.01f;
const DAVA::float32 ChartPainterSystem::overdrawStepCount = maxOverdraw / overdrawStep;
const DAVA::float32 ChartPainterSystem::frametimeStepCount = frametimeAxisLen / frametimeStep;

const DAVA::Array<DAVA::String, 6> ChartPainterSystem::legend =
{ {
    "0 tex",
    "1 tex",
    "2 tex",
    "3 tex",
    "4 tex",
    "dep r"
} };

const DAVA::Array<DAVA::Color, 6> ChartPainterSystem::chartColors =
{ {
    { 0.0f, 1.0f, 0.0f, 1.0f},
    { 1.0f, 1.0f, 0.0f, 1.0f},
    { 0.0f, 1.0f, 1.0f, 1.0f},
    { 0.0f, 0.0f, 1.0f, 1.0f},
    { 1.0f, 0.0f, 1.0f, 1.0f},
    { 1.0f, 0.0f, 0.0f, 1.0f}
} };

ChartPainterSystem::ChartPainterSystem(DAVA::Scene* scene, DAVA::Array<DAVA::Vector<ViewSceneScreen::FrameData>, 6>* preformanceData_)
    : DAVA::SceneSystem(scene), performanceData(preformanceData_), textColor(rhi::NativeColorRGBA(1.0f, 1.0f, 1.0f, 1.0f))
{
}

ChartPainterSystem::~ChartPainterSystem()
{

}

void ChartPainterSystem::Process(float32 timeElapsed)
{
    if (!shouldDrawGraph) return;

    VirtualCoordinatesSystem* vcs = DAVA::UIControlSystem::Instance()->vcs;

    int32 w = vcs->GetVirtualScreenSize().dx;
    int32 h = vcs->GetVirtualScreenSize().dy;

    DrawLegend(w, h);

    DrawGrid(w, h);
    DrawCharts(w, h);

    FlushDbgText();
}

void ChartPainterSystem::DrawGrid(int32 w, int32 h)
{
    Vector2 origin(chartOffset.x * w, (chartOffset.y + chartLen) * h);
    Vector2 xAxis((chartOffset.x + chartLen) * w, (chartOffset.y + chartLen) * h);
    Vector2 yAxis(chartOffset.x * w, chartOffset.y * h);

    Polygon2 p;
    p.AddPoint(origin);
    p.AddPoint(xAxis);
    RenderSystem2D::Instance()->DrawPolygon(p, false, Color::White);
    p.Clear();
    p.AddPoint(origin);
    p.AddPoint(yAxis);
    RenderSystem2D::Instance()->DrawPolygon(p, false, Color::White);

    int32 stepCount = static_cast<int>(frametimeStepCount + 1);
    for (int32 i = 1; i < stepCount + 1; i++)
    {
        p.Clear();
        float32 normalizedFps = (i * frametimeStep) / frametimeAxisLen;
        normalizedFps *= chartLen;
        float32 pointY = 1 - (normalizedFps + chartOffset.y);
        pointY *= h;
        p.AddPoint({ chartOffset.x * w, pointY });
        p.AddPoint({ (chartOffset.x + chartLen) * w, pointY });
        RenderSystem2D::Instance()->DrawPolygon(p, false, gridColor);

        DbgDraw::Text2D(static_cast<int32>(0.05f * w), static_cast<int32>(pointY), textColor, "%.2f", i * frametimeStep + minFrametime);
    }

    for (int32 i = 1; i < overdrawStepCount + 1; i++)
    {
        p.Clear();
        float32 normalizedOverdraw = (i * overdrawStep) / maxOverdraw;
        normalizedOverdraw *= chartLen;
        float32 pointX = normalizedOverdraw + chartOffset.x;
        pointX *= w;
        p.AddPoint({ pointX, chartOffset.y * h });
        p.AddPoint({ pointX, (chartOffset.y + chartLen) * h });
        RenderSystem2D::Instance()->DrawPolygon(p, false, gridColor);
        DbgDraw::Text2D(static_cast<int32>(pointX), static_cast<int32>((chartOffset.y + chartLen) * h), textColor, "%.2f", i * overdrawStep);
    }
}

void ChartPainterSystem::DrawCharts(int32 w, int32 h)
{
    for (int i = 0; i < 6; i++)
    {
        Polygon2 p;
        for (int j = 0; j < (*performanceData)[i].size(); j++)
        {
            float32 overdraw = (*performanceData)[i][j].Overdraw;
            float32 fps = static_cast<DAVA::float32>((*performanceData)[i][j].FPS) - minFrametime;

            float32 normalizedFps = fps / frametimeAxisLen;
            float32 normalizedOverdraw = overdraw / maxOverdraw;

            normalizedFps *= chartLen;
            normalizedOverdraw *= chartLen;

            float32 pointX = (normalizedOverdraw + chartOffset.x);
            float32 pointY = 1 - (normalizedFps + chartOffset.y);
            pointX *= w;
            pointY *= h;

            p.AddPoint({ pointX, pointY });
        }

        RenderSystem2D::Instance()->DrawPolygon(p, false, chartColors[i]);
    }
}

void ChartPainterSystem::FlushDbgText()
{
    rhi::RenderPassConfig passConfig;
    passConfig.colorBuffer[0].loadAction = rhi::LOADACTION_LOAD;
    passConfig.colorBuffer[0].storeAction = rhi::STOREACTION_STORE;
    passConfig.depthStencilBuffer.loadAction = rhi::LOADACTION_NONE;
    passConfig.depthStencilBuffer.storeAction = rhi::STOREACTION_NONE;
    passConfig.priority = PRIORITY_MAIN_2D - 10;
    passConfig.viewport.x = 0;
    passConfig.viewport.y = 0;
    passConfig.viewport.width = Renderer::GetFramebufferWidth();
    passConfig.viewport.height = Renderer::GetFramebufferHeight();

    DbgDraw::SetScreenSize(Renderer::GetFramebufferWidth(), Renderer::GetFramebufferHeight());

    rhi::HPacketList packetList;
    rhi::HRenderPass pass = rhi::AllocateRenderPass(passConfig, 1, &packetList);
    rhi::BeginRenderPass(pass);
    rhi::BeginPacketList(packetList);

    DbgDraw::FlushBatched(packetList);

    rhi::EndPacketList(packetList);
    rhi::EndRenderPass(pass);
}

void ChartPainterSystem::DrawLegend(int32 w, int32 h)
{
    float32 initialOffset = static_cast<float32>(w) / 14.0f;
    float32 step = static_cast<float32>(w) / 7.0f;
    int32 lineOffset = static_cast<int32>(static_cast<float32>(w) / 9.0f);
    int32 yPos = static_cast<int32>(0.05f * h);
    float32 yPosFloat = 0.05f * h;
    for (int i = 0; i < 6; i++)
    {
        int32 startX = static_cast<int32>(step * i + initialOffset);
        float32 startXFloat = step * i + initialOffset;
        Polygon2 p;
        p.AddPoint({ startXFloat, yPosFloat });
        p.AddPoint({ startXFloat + lineOffset, yPosFloat });
        RenderSystem2D::Instance()->DrawPolygon(p, false, chartColors[i]);
        DbgDraw::Text2D(startX, yPos, textColor, "%s", legend[i].c_str());
    }
}

}