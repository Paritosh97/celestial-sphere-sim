#include <QColor>
#include <QDebug>
#include <QObject>
#include <QPropertyAnimation>
#include <QThread>
#include <QVector3D>
#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongAlphaMaterial>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/qorbitcameracontroller.h>
#include <Qt3DExtras/QSkyboxEntity>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QMaterial>
#include <Qt3DExtras/QFirstPersonCameraController>

#include "linegeometry.h"
#include "linematerial.h"

#include "skypoint.h"
#include "linelist.h"
#include "typedef.h"

class SkyPoint;

Qt3DCore::QEntity* addSkybox()
{
    Qt3DExtras::QSkyboxEntity* skybox = new Qt3DExtras::QSkyboxEntity;
    QString path = QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/skybox/park").toString();
    skybox->setBaseName(path);
    skybox->setExtension(".png");

    return skybox;
}

void fillLineList(std::shared_ptr<LineList> lineList, int j)
{
    for(int i = 1; i < 10; i++)
    {
        dms *ra = new dms(10*i);
        dms *dec = new dms(10*j);
        std::shared_ptr<SkyPoint>  point(new SkyPoint(*ra, *dec));
        lineList->append(point);
    }    
}

std::shared_ptr<LineListList> fillLineListList()
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

Qt3DCore::QEntity *addLine(SkyPoint *pLast, SkyPoint *pThis)
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

Qt3DCore::QEntity* addSkyPolyline(LineList *lineList)
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

Qt3DCore::QEntity* addLines()
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

Qt3DCore::QEntity* createTestScene()
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

int main(int argc, char *argv[])
{
    // Application
    QGuiApplication app(argc, argv);

    // Window
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *camera = view->camera();
    camera->lens()->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.1f, 100.0f);
    QVector3D originalPosition(0, 10.0f, 20.0f);
    camera->setPosition(originalPosition);
    QVector3D originalViewCenter(0, 0, 0);
    camera->setViewCenter(originalViewCenter);
    QVector3D upVector(0, 1.0, 0);
    camera->setUpVector(upVector);

    // Camera control
    Qt3DExtras::QFirstPersonCameraController *camController = new Qt3DExtras::QFirstPersonCameraController(rootEntity);
    camController->setCamera(camera);

    Qt3DCore::QEntity *scene = createTestScene();
    scene->setParent(rootEntity);

    view->setRootEntity(rootEntity);
    view->show();

    return app.exec();
}
