#include "qt3dwidget.h"

#include <Qt3DCore/QEntity>

#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QRenderSurfaceSelector>

Qt3DWidget::Qt3DWidget(QWidget *parent)
{
    Qt3DExtras::Qt3DWindow *window = new Qt3DExtras::Qt3DWindow;

    container = createWindowContainer(window, this);
    container->setMinimumSize(QSize(800, 600));

    Qt3DRender::QRenderSurfaceSelector *surfaceSelector = new Qt3DRender::QRenderSurfaceSelector;
    surfaceSelector->setSurface(this);

    viewport = new Qt3DRender::QViewport(surfaceSelector);
    viewport->setNormalizedRect(QRectF(0, 0, 1.0, 1.0));

    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(viewport);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);
    clearBuffers->setClearColor(Qt::white);

    window->setActiveFrameGraph(surfaceSelector);
    Qt3DCore::QEntity *root = createScene();
    window->setRootEntity(root);
}

Qt3DCore::QEntity* Qt3DWidget::addSkybox()
{
    // TODO add custom skybox

    Qt3DCore::QEntity *skybox = new Qt3DCore::QEntity;
    /*
    Qt3DExtras::QSkyboxEntity* skybox = new Qt3DExtras::QSkyboxEntity;
    QString path = QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/skybox/park").toString();
    skybox->setBaseName(path);
    skybox->setExtension(".png");
    */
    return skybox;
}

void Qt3DWidget::fillLineList(std::shared_ptr<LineList> lineList, int j)
{
    for(int i = 1; i < 10; i++)
    {
        dms *ra = new dms(10*i);
        dms *dec = new dms(10*j);
        std::shared_ptr<SkyPoint>  point(new SkyPoint(*ra, *dec));
        lineList->append(point);
    }    
}

std::shared_ptr<LineListList> Qt3DWidget::fillLineListList()
{
    std::shared_ptr<LineListList> lineListList(new LineListList);

    for(int i = 1; i < 10; i++)
    {
        std::shared_ptr<LineList> lineList(new LineList);
        fillLineList(lineList, i);
        lineListList->append(lineList);
    }

    return lineListList;
}

Qt3DCore::QEntity* Qt3DWidget::addLine(SkyPoint *pLast, SkyPoint *pThis)
{
    // Root entity
    Qt3DCore::QEntity *lineEntity = new Qt3DCore::QEntity;

    QMatrix4x4 instTransform = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    QVector<QVector3D> pos;
    pos.append(QVector3D(pLast->ra().radians(), pLast->dec().radians(), 0));
    pos.append(QVector3D(pThis->ra().radians(), pThis->dec().radians(), 0));
    qDebug()<<pos;

    // Line Geometry
    LineGeometry *lineGeometry = new LineGeometry();
    lineGeometry->setPoints(pos);

    // line Geometry Renderer
    Qt3DRender::QGeometryRenderer *lineGeometryRenderer = new Qt3DRender::QGeometryRenderer;
    lineGeometryRenderer->setPrimitiveType( Qt3DRender::QGeometryRenderer::Lines);
    lineGeometryRenderer->setGeometry( lineGeometry );
    lineGeometryRenderer->setVertexCount( lineGeometry->count() );

    // line Material
    LineMaterial *lineMaterial = new LineMaterial();

    // line Transform
    Qt3DCore::QTransform *lineTransform = new Qt3DCore::QTransform();
    lineTransform->setTranslation(QVector3D(0.0f, 1.5f, 0.0f));

    // line Entity
    lineEntity->addComponent(lineMaterial);
    lineEntity->addComponent(lineGeometryRenderer);
    lineEntity->addComponent(lineTransform);
    lineEntity->setEnabled(true);

    return lineEntity;
}

Qt3DCore::QEntity* Qt3DWidget::addSkyPolyline(LineList *lineList)
{
    Qt3DCore::QEntity* skyPolyLine = new Qt3DCore::QEntity();

    SkyList *points = lineList->points();

    for (int j = 1; j < points->size(); j++)
    {
        SkyPoint *pLast = points->at(j++).get();
        SkyPoint *pThis = points->at(j).get();
        Qt3DCore::QEntity* line = addLine(pLast, pThis);
        line->setParent(skyPolyLine);
    }

    return skyPolyLine;
}

Qt3DCore::QEntity* Qt3DWidget::addLines()
{
    // Add lines on the celestial sphere
    Qt3DCore::QEntity* lines = new Qt3DCore::QEntity();

    std::shared_ptr<LineListList> lineListList = fillLineListList();

    for (int i = 0; i < lineListList->size(); i++)
    {
        std::shared_ptr<LineList> lineList = lineListList->at(i);

        Qt3DCore::QEntity* line = addSkyPolyline(lineList.get());
        line->setParent(lines);
    }
    
    return lines;
}

Qt3DCore::QEntity* Qt3DWidget::createScene()
{
    Qt3DCore::QEntity *root = new Qt3DCore::QEntity;

    // add skybox
    Qt3DCore::QEntity *skybox = addSkybox();
    skybox->setParent(root);

    // add polyline
    Qt3DCore::QEntity *lines = addLines();
    lines->setParent(root);

    return root;
}